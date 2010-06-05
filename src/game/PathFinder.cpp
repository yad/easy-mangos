#include "Object.h"
#include "PathFinder.h"
#include "ObjectGuid.h"
#include "Map.h"
#include "pathfinding/Detour/DetourNavMesh.h"
#include "pathfinding/Detour/DetourCommon.h"
#include "pathfinding/Recast/Recast.h"

////////////////// PathInfo //////////////////
PathInfo::PathInfo(WorldObject* from, const float x, const float y, const float z) :
    m_length(0), m_pathPolyRefs(0), m_pathPoints(0),
    m_sourceObject(from), m_type(PATHFIND_BLANK)
{
    clear();
    setEndPosition(x, y, z);
    Build();
}

dtPolyRef PathInfo::getPathPolyByPosition(float x, float y, float z)
{
    if(!m_navMesh)
        return 0;       // navmesh isn't loaded

    float distance;     // not used

    int i;
    for(i = 0; i < this->m_length; ++i)
    {
        const dtMeshTile* tile = this->m_navMesh->getTileByPolyRef(this->m_pathPolyRefs[i], 0);
        if(!tile)
            continue;   // tile isn't loaded, no information about target polygon

        if(isPointInPolyBounds(x, y, z, distance, m_pathPolyRefs[i]))
            return this->m_pathPolyRefs[i];   // point is inside this polygon
    }

    return 0;   // point is not inside the path
}

bool PathInfo::isPointInPolyBounds(float x, float y, float z, float &distance, dtPolyRef polyRef)
{
    float point[3] = {y, z, x};
    int polyindex;

    const dtMeshTile* tile = m_navMesh->getTileByPolyRef(polyRef, &polyindex);
    if(!tile)
        return false;

    const dtPoly poly = tile->polys[polyindex];

    float vertices[DT_VERTS_PER_POLYGON*3];
    float ed[DT_VERTS_PER_POLYGON];             // distance^2 from edge to point
    float et[DT_VERTS_PER_POLYGON];             // describes where on edge is nearest point

    // Collect vertices.
    int nv = 0;
    for (int i = 0; i < (int)poly.vertCount; ++i)
    {
        rcVcopy(&vertices[nv*3], &tile->verts[poly.verts[i]*3]);
        nv++;
    }

    bool isInsidePoly = dtDistancePtPolyEdgesSqr(point, vertices, nv, ed, et);

    if(!isInsidePoly)
    {
        // distance to nearest edge
        distance = FLT_MAX;
        for(int i = 0; i < (int)poly.vertCount; ++i)
            if(ed[i] < distance)
                distance = ed[i];
    }
    else
        distance = 0.0f;

    return isInsidePoly;
}

void PathInfo::Build()
{
    float x, y, z;
    // set start and a default next position
    m_sourceObject->GetPosition(x, y, z);
    setStartPosition(x, y, z);
    setNextPosition(x, y, z);

    // get nav mesh
    m_navMesh = m_sourceObject->GetMap()->GetNavMesh();

    if(!m_navMesh)
    {
        // ignore obstacles/terrain is better than giving up
        shortcut();
        return;
    }

    float extents[3] = {2.0f, 4.0f, 2.0f};      // defines bounds of box for search area
    dtQueryFilter filter = dtQueryFilter();     // search filter

    // get start and end positions
    getStartPosition(x, y, z);
    float startPos[3] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[3] = {y, z, x};

    // find start and end poly
    dtPolyRef startPoly = m_navMesh->findNearestPoly(startPos, extents, &filter, 0);
    dtPolyRef endPoly = m_navMesh->findNearestPoly(endPos, extents, &filter, 0);

    Build(startPoly, endPoly);
}

void PathInfo::Build(dtPolyRef startPoly, dtPolyRef endPoly)
{
    clear();

    float x, y, z;

    if(startPoly == 0 || endPoly == 0)
    {
        // polygon didn't touch the search box
        // could mean start or end has...
        //     (x,y) outside navmesh
        //     (z) above/below the navmesh
        sLog.outError("%u's Path Build failed: invalid start or end polygon", m_sourceObject->GetGUID());
        shortcut();
        return;
    }

    getStartPosition(x, y, z);
    float startPos[3] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[3] = {y, z, x};

    bool startOffPoly, endOffPoly;
    float distanceStart, distanceEnd;
    startOffPoly = !isPointInPolyBounds(startPos[2], startPos[0], startPos[1], distanceStart, startPoly);
    endOffPoly = !isPointInPolyBounds(endPos[2], endPos[0], endPos[1], distanceEnd, endPoly);

    // if startPos is really far from startPoly, should we give up?

    // if endPos is really far from endPoly, should we clamp endPos to the polygon?

    // PATHFIND TODO: find height above polygon
    //float startHeight, endHeight;
    //startHeight =
    //endHeight = 

    dtQueryFilter filter = dtQueryFilter();

    dtPolyRef pathPolys[MAX_PATH_LENGTH];
    m_length = m_navMesh->findPath(
        startPoly,          // start polygon
        endPoly,            // end polygon
        startPos,           // start position
        endPos,             // end position
        &filter,            // polygon search filter
        pathPolys,          // [out] path
        MAX_PATH_LENGTH);   // max number of polygons in output path

    if(m_length == 0)
    {
        // only happens if we passed bad data to findPath(), or navmesh is messed up
        sLog.outError("%u's Path Build failed: 0-length path", m_sourceObject->GetGUID());
        shortcut();
        return;
    }

    // pathPolyRefs was deleted when we entered Build(dtPolyRef, dtPolyRef)
    m_pathPolyRefs = new dtPolyRef[m_length];

    int i;
    for(i = 0; i < m_length; ++i)
        m_pathPolyRefs[i] = pathPolys[i];

    updateNextPosition();

    m_currentNode = 0;
}

void PathInfo::Update(const float destX, const float destY, const float destZ)
{
    float x, y, z;

    // update start and end
    m_sourceObject->GetPosition(x, y, z);
    setStartPosition(x, y, z);
    setEndPosition(destX, destY, destZ);

    // make sure navMesh works
    if(!m_navMesh)
    {
        m_sourceObject->GetPosition(x, y, z);
        m_navMesh = m_sourceObject->GetMap()->GetNavMesh();

        if(!m_navMesh)
        {
            // can't pathfind if navmesh doesn't exist
            shortcut();
            return;
        }
    }

    if(!m_pathPolyRefs)
    {
        // path was not built before, most likely because navmesh wasn't working
        // start from scratch, then return
        Build();
        return;
    }

    // should be safe to update path now

    bool startOffPath = false;
    bool endOffPath = false;

    // find start and end poly
    // navMesh.findNearestPoly is expensive, so first we check just the current path
    getStartPosition(x, y, z);
    dtPolyRef startPoly = getPathPolyByPosition(x, y, z);
    getEndPosition(x, y, z);
    dtPolyRef endPoly = getPathPolyByPosition(x, y, z);

    if(startPoly != 0 && endPoly != 0)
        trim(startPoly, endPoly);
    else
    {
        // start or end is off the path, need to find the polygon

        float extents[3] = {2.f, 4.f, 2.f};      // bounds of poly search area
        dtQueryFilter filter = dtQueryFilter();     // filter for poly search

        if(!startPoly)
        {
            getStartPosition(x, y, z);
            float startPos[3] = {y, z, x};
            startOffPath = true;
            startPoly = m_navMesh->findNearestPoly(startPos, extents, &filter, 0);
        }
        if(!endPoly)
        {
            getEndPosition(x, y, z);
            float endPos[3] = {y, z, x};
            endOffPath = true;
            endPoly = m_navMesh->findNearestPoly(endPos, extents, &filter, 0);
        }

        if(startPoly == 0 || endPoly == 0)
        {
            // source or dest not near navmesh polygons:
            // flying, falling, swimming, or navmesh has a hole

            // ignore obstacles/terrain is better than giving up
            // PATHFIND TODO: prevent walking/swimming mobs from flying into the air
            shortcut();
            return;
        }
    }

    if(startPoly == endPoly)
    {
        // start and end are on same polygon
        // just need to move in straight line

        // PATHFIND TODO: prevent walking/swimming mobs from flying into the air

        m_length = 1;
        getEndPosition(x, y, z);
        setNextPosition(x, y, z);
        m_type = PATHFIND_NORMAL;
        return;
    }

    if(startOffPath)
    {
        bool adjacent = false;
        int i;
        for(i = 0; i < DT_VERTS_PER_POLYGON; ++i)
            if(startPoly == m_navMesh->getPolyByRef(m_pathPolyRefs[0])->neis[i])
            {
                adjacent = true;
                break;
            }

        if(adjacent)
        {
            // startPoly is adjacent to the path, we can add it to the start of the path
            // 50th poly will fall off of path, shouldn't be an issue because most paths aren't that long
            m_length = m_length < MAX_PATH_LENGTH ? m_length + 1 : m_length;
            dtPolyRef* temp = new dtPolyRef[m_length];
            temp[0] = startPoly;

            for(i = 1; i < m_length; ++i)
                temp[i] = m_pathPolyRefs[i - 1];

            delete [] m_pathPolyRefs;
            m_pathPolyRefs = temp;
        }
        else
        {
            // waste of time to optimize, just find brand new path
            Build(startPoly, endPoly);
            return;
        }
    }

    if(endOffPath)
    {
        bool adjacent = false;
        int i;
        for(i = 0; i < DT_VERTS_PER_POLYGON; ++i)
            if(startPoly == m_navMesh->getPolyByRef(m_pathPolyRefs[0])->neis[i])
            {
                adjacent = true;
                break;
            }

        if(adjacent)
        {
            if(m_length < MAX_PATH_LENGTH)
            {
                // endPoly is adjacent to the path, and we have enough room to add it to the end
                dtPolyRef* temp = new dtPolyRef[m_length + 1];
                for(i = 0; i < m_length; ++i)
                    temp[i] = m_pathPolyRefs[i];

                temp[i] = endPoly;

                delete [] m_pathPolyRefs;
                m_pathPolyRefs = temp;
            }
            //else
            //    ;   // endPoly is adjacent to the path, we just don't have room to store it
        }
        else
        {
            // waste of time to optimize, just find brand new path
            Build(startPoly, endPoly);
            return;
        }
    }

    updateNextPosition();
}

void PathInfo::updateNextPosition()
{
    float x, y, z;

    getStartPosition(x, y, z);
    float startPos[3] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[3] = {y, z, x};

    float* pathPoints = new float[MAX_PATH_LENGTH*3];
    int pointCount = m_navMesh->findStraightPath(
        startPos,           // start position
        endPos,             // end position
        m_pathPolyRefs,     // current path
        m_length,           // lenth of current path
        pathPoints,         // [out] path corner points
        0,                  // [out] flags
        0,                  // [out] shortened path  PATHFIND TODO: see if this is usable (IE, doesn't leave gaps in path)
        MAX_PATH_LENGTH);   // maximum number of points/polygons to use

    // TODO: imitate PATHFIND_ITER code from RecastDemo so that terrain following is smoother

    if(pointCount == 0)
    {
        delete [] pathPoints;

        // only happens if pass bad data to findStraightPath or navmesh is broken
        sLog.outError("%u's UpdateNextPosition failed: 0 length path", m_sourceObject->GetGUID());
        shortcut();
        return;
    }

    int ix, iy, iz;
    if(pointCount == 1)
    {
        ix = 2; iy = 0; iz = 1;
    }
    else
    {
        ix = 5; iy = 3; iz = 4;
    }

    setNextPosition(pathPoints[ix], pathPoints[iy], pathPoints[iz]);

    // if startPosition == nextPosition, NPC will never advance to destination
    if(isSamePoint(m_startPosition, m_nextPosition) && !isSamePoint(m_nextPosition, m_endPosition))
        setNextPosition(pathPoints[ix + 3], pathPoints[iy + 3], pathPoints[iz + 3]);

    delete [] m_pathPoints;
    m_pathPoints = pathPoints;
    //delete [] pathPoints;

    m_type = PATHFIND_NORMAL;
}

void PathInfo::trim(dtPolyRef startPoly, dtPolyRef endPoly)
{
    int pathStartIndex, pathEndIndex, i, j;

    // find if start node is on the existing path
    for(pathStartIndex = 0; pathStartIndex < m_length; ++pathStartIndex)
        if(m_pathPolyRefs[pathStartIndex] == startPoly)
            break;
    
    // find if end node is on the existing path
    for(pathEndIndex = pathStartIndex; pathEndIndex < m_length; ++pathEndIndex)
        if(m_pathPolyRefs[pathEndIndex] == endPoly)
            break;

    pathEndIndex++;     // PATHFIND TODO: shouldn't need this increment?

    m_length = pathEndIndex - pathStartIndex;

    dtPolyRef* newPolyRefs = new dtPolyRef[m_length];

    for(i = pathStartIndex, j = 0; i < pathEndIndex; ++i, ++j)  // copy shortened path data
        newPolyRefs[j] = m_pathPolyRefs[i];
    
    delete [] m_pathPolyRefs;
    m_pathPolyRefs = newPolyRefs;
}

void PathInfo::shortcut()
{
    clear();

    float x, y, z;
    getEndPosition(x, y, z);
    setNextPosition(x, y, z);
    m_type = PATHFIND_SHORTCUT;
}
