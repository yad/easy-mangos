/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Creature.h"
#include "PathFinder.h"
#include "../recastnavigation/Detour/Include/DetourCommon.h"

////////////////// PathInfo //////////////////
PathInfo::PathInfo(const WorldObject* from, const float x, const float y, const float z) :
    m_navMesh(NULL), m_navMeshQuery(NULL), m_polyLength(0), m_pathPolyRefs(NULL),
    m_pointPathPointer(0), m_sourceObject(from), m_type(PATHFIND_BLANK)
{
    setEndPosition(PathNode(x, y, z));

    if(m_navMesh = m_sourceObject->GetMap()->GetNavMesh())
    {
        m_navMeshQuery = dtAllocNavMeshQuery();
        m_navMeshQuery->init(m_navMesh, MESH_MAX_NODES);

        BuildFreshPath();
    }
    else
        shortcut();
}

PathInfo::~PathInfo()
{
    if (m_pathPolyRefs)
        delete [] m_pathPolyRefs;

    if(m_navMesh && m_navMeshQuery)
        dtFreeNavMeshQuery(m_navMeshQuery);
}

dtPolyRef PathInfo::getPathPolyByPosition(float x, float y, float z)
{
    if(!m_navMesh || !m_pathPolyRefs)
        return INVALID_POLYREF;

    float point[VERTEX_SIZE] = {y, z, x};
    for(uint32 i = 0; i < m_polyLength; ++i)
    {
        const dtMeshTile* tile;
        const dtPoly* poly;
        if(!m_navMesh->getTileAndPolyByRef(m_pathPolyRefs[i], &tile, &poly))
            return INVALID_POLYREF;   // m_pathPolyRefs[i] is invalid

        if(!tile)
            continue;   // tile isn't loaded, no information about target polygon

        if(dtPointInPolygon(point, tile->verts, poly->vertCount))
            return m_pathPolyRefs[i];   // point is inside this polygon
    }

    return INVALID_POLYREF;   // point is not inside the path
}

void PathInfo::BuildFreshPath()
{
    clear();

    float x, y, z;
    // set start and a default next position
    m_sourceObject->GetPosition(x, y, z);
    setStartPosition(PathNode(x, y, z));

    // get start and end positions
    float startPos[VERTEX_SIZE] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[VERTEX_SIZE] = {y, z, x};

    // find start and end poly
    float extents[VERTEX_SIZE] = {2.0f, 4.0f, 2.0f};      // defines bounds of box for search area
    dtQueryFilter filter = dtQueryFilter();     // use general filter so we know if we are near navmesh

    dtPolyRef startPoly = m_navMeshQuery->findNearestPoly(startPos, extents, &filter, NULL);
    dtPolyRef endPoly = m_navMeshQuery->findNearestPoly(endPos, extents, &filter, NULL);

    BuildPath(startPoly, startPos, endPoly, endPos);
}

void PathInfo::BuildPath(dtPolyRef startPoly, float* startPos, dtPolyRef endPoly, float* endPos)
{
    if(startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
    {
        // source or dest not near navmesh polygons:
        // flying, falling, swimming, or navmesh has a hole

        // ignore obstacles/terrain is better than giving up
        // PATHFIND TODO: prevent walking/swimming mobs from flying into the air
        sLog.outError("%u's Path Build failed: invalid start or end polygon", m_sourceObject->GetGUID());
        //if(canFly())    // TODO
            shortcut();
        return;
    }

    // start and end are on same polygon
    // just need to move in straight line - nothing more to do here
    if(startPoly == endPoly)
    {
        //printf("++ PathInfo::BuildPath :: (startPoly == endPoly) %u\n",m_sourceObject->GetGUID());
        shortcut();

        m_pathPolyRefs = new dtPolyRef[1];
        m_pathPolyRefs[0] = startPoly;
        m_polyLength = 1;

        m_type = PATHFIND_NORMAL;
        return;
    }

    // look for startPoly/endPoly in current path
    bool startPolyFound = false;
    bool endPolyFound = false;
    for(uint32 i = 0; i < m_polyLength; ++i)
    {
        if(m_pathPolyRefs[i] == startPoly)
            startPolyFound = true;

        if(m_pathPolyRefs[i] == endPoly)
            endPolyFound = true;
    }

    if(startPolyFound && endPolyFound)
    {
        //printf("++ PathInfo::BuildPath :: (startPolyFound && endPolyFound) %u\n",m_sourceObject->GetGUID());

        // we moved along the path and the target did not move out of our old poly-path
        // our path is a simple subpath case, we have all the data we need
        // just "cut" it out

        uint32 pathStartIndex, pathEndIndex; // keep outside loops

        // find if start node is on the existing path
        for(pathStartIndex = 0; pathStartIndex < m_polyLength; ++pathStartIndex)
            if(m_pathPolyRefs[pathStartIndex] == startPoly)
                break;

        // find if end node is on the existing path - start from last
        for(pathEndIndex = m_polyLength-1; pathEndIndex > pathStartIndex; --pathEndIndex)
            if(m_pathPolyRefs[pathEndIndex] == endPoly)
                break;

        m_polyLength = pathEndIndex - pathStartIndex + 1;

        dtPolyRef* newPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(newPolyRefs, m_pathPolyRefs+pathStartIndex, m_polyLength*sizeof(dtPolyRef));

        delete [] m_pathPolyRefs;
        m_pathPolyRefs = newPolyRefs;
    }
    else if(startPolyFound && !endPolyFound)
    {
        //printf("++ PathInfo::BuildPath :: (startPolyFound && !endPolyFound) %u\n",m_sourceObject->GetGUID());

        // we are moving on the old path but target moved out
        // so we have atleast part of poly-path ready

        // find if start node is on the existing path
        uint32 pathStartIndex;
        for(pathStartIndex = 0; pathStartIndex < m_polyLength; ++pathStartIndex)
            if(m_pathPolyRefs[pathStartIndex] == startPoly)
                break;

        m_polyLength -= pathStartIndex;

        // try to adjust the suffix of the path instead of recalculating entire length
        // at given interval the target cannot get too far from its last location
        // thus we have less poly to cover
        // sub-path of optimal path is optimal

        // take ~80% of the original length
        // TODO : play with the values here
        uint32 prefixPolyLength = uint32(m_polyLength*0.8f + 0.5f);
        dtPolyRef prefixPathPolys[MAX_PATH_LENGTH];
        memcpy(prefixPathPolys, m_pathPolyRefs+pathStartIndex, prefixPolyLength*sizeof(dtPolyRef));

        dtPolyRef suffixStartPoly = prefixPathPolys[prefixPolyLength-1];
        dtPolyRef suffixPathPolys[MAX_PATH_LENGTH];
        uint32 suffixPolyLength = 0;

        // we need any point on our suffix start poly to generate poly-path, so we need last poly in prefix data
        const dtMeshTile* tile;
        const dtPoly* poly;
        if(m_navMesh->getTileAndPolyByRef(suffixStartPoly, &tile, &poly))
        {
            dtQueryFilter filter = createFilter();

            // generate suffix
            suffixPolyLength = m_navMeshQuery->findPath(
                    suffixStartPoly,     // start polygon
                    endPoly,            // end polygon
                    // we might need to get better point here
                    &tile->verts[poly->verts[0]], // start position
                    endPos,             // end position
                    &filter,            // polygon search filter
                    suffixPathPolys,     // [out] path
                    MAX_PATH_LENGTH);   // max number of polygons in output path

            if(suffixPolyLength)
            {
                // new path = prefix + suffix - overlap

                m_polyLength = prefixPolyLength + suffixPolyLength - 1;
                delete [] m_pathPolyRefs;
                m_pathPolyRefs = new dtPolyRef[m_polyLength];

                // copy the part of the old path we keep - prefix
                memcpy(m_pathPolyRefs, prefixPathPolys, prefixPolyLength*sizeof(dtPolyRef));

                // copy the newly created suffix - skip first poly, we have it at prefix end
                memcpy(m_pathPolyRefs+prefixPolyLength, suffixPathPolys+1, (suffixPolyLength-1)*sizeof(dtPolyRef));
            }
            else
            {
                // this is probably an error state, but we'll leave it
                // and hopefully recover on the next Update
                sLog.outError("%u's Path Build failed: 0 length path", m_sourceObject->GetGUID());
            }
        }
        else
        {
            // suffixStartPoly is invalid somehow, or the navmesh is broken => error state
            sLog.outError("%u's Path Build failed: invalid polyRef in path", m_sourceObject->GetGUID());

            // we need to get usable start/end polyRefs and recalculate path from scratch
            BuildFreshPath();
            return;
        }
    }
    else
    {
        //printf("++ PathInfo::BuildPath :: (!startPolyFound && !endPolyFound) %u\n",m_sourceObject->GetGUID());

        // either we have no path at all -> first run
        // or something went really wrong -> we aren't moving along the path to the target
        // just generate new path

        // free and invalidate old path data (particularly m_pointPathPointer)
        clear();

        dtQueryFilter filter = createFilter();      // use special filter so we use proper terrain types
        dtPolyRef pathPolys[MAX_PATH_LENGTH];

        m_polyLength = m_navMeshQuery->findPath(
                startPoly,          // start polygon
                endPoly,            // end polygon
                startPos,           // start position
                endPos,             // end position
                &filter,            // polygon search filter
                pathPolys,          // [out] path
                MAX_PATH_LENGTH);   // max number of polygons in output path

        if(m_polyLength == 0)
        {
            // only happens if we passed bad data to findPath(), or navmesh is messed up
            sLog.outError("%u's Path Build failed: 0 length path", m_sourceObject->GetGUID());
            shortcut();
            return;
        }

        m_pathPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(m_pathPolyRefs, pathPolys, m_polyLength*sizeof(dtPolyRef));
    }

    if(m_pathPolyRefs[m_polyLength - 1] == endPoly)
        m_type = PATHFIND_NORMAL;
    else if (m_polyLength < MAX_PATH_LENGTH)
        m_type = PATHFIND_NOPATH;
    else
        m_type = PATHFIND_INCOMPLETE;

    // generate the point-path out of our up-to-date poly-path
    updateNextPosition();
}

void PathInfo::Update(const float destX, const float destY, const float destZ)
{
    // make sure navMesh works - we can run on map w/o mmap
    // guard against target changing maps
    if(!m_navMesh || m_navMesh != m_sourceObject->GetMap()->GetNavMesh())
    {
        setEndPosition(PathNode(destX, destY, destZ));
        shortcut();
        return;
    }

    if(!m_pathPolyRefs)
    {
         // path was not built before, most likely because navmesh wasn't working
         // start from scratch, then return
        BuildFreshPath();
        return;
    }

    float x, y, z;

    m_sourceObject->GetPosition(x, y, z);
    setStartPosition(PathNode(x, y, z));

    // check if destination moved - if not we can optimize something here
    // we are following old, precalculated path?
    float dist = m_sourceObject->GetObjectBoundingRadius();

    PathNode oldDest = getEndPosition();
    PathNode newDest = PathNode(destX, destY, destZ);

    if(inRange(oldDest, newDest, dist, 2*dist) &&
       m_pathPoints.size() > 2 &&
       m_pointPathPointer + 1 < m_pathPoints.size())
        // our target is not moving - we just coming closer
        // increment pointPathPointer to the next point in the path
        m_pointPathPointer++;
    else
        // target moved, so we need to update the poly path
        m_pointPathPointer = 0;

    // update destination
    setEndPosition(newDest);

    if(!m_pointPathPointer)
    {
        float startPos[VERTEX_SIZE] = {y, z, x};
        float endPos[VERTEX_SIZE] = {destY, destZ, destX};

        // find start and end poly
        // first we check the current path
        // if the current path doesn't contain the current poly,
        // we need to use the expensive navMesh.findNearestPoly

        dtPolyRef startPoly = getPathPolyByPosition(x, y, z);
        dtPolyRef endPoly = getPathPolyByPosition(destX, destY, destZ);
        if(startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
        {
            // start or end is off the path or invalid, need to find the polygon
            float extents[VERTEX_SIZE] = {2.f, 4.f, 2.f};   // bounds of poly search area
            dtQueryFilter filter = dtQueryFilter();         // filter for poly search

            if(startPoly == INVALID_POLYREF)
                startPoly = m_navMeshQuery->findNearestPoly(startPos, extents, &filter, NULL);

            if(endPoly == INVALID_POLYREF)
                endPoly = m_navMeshQuery->findNearestPoly(endPos, extents, &filter, NULL);
        }

        BuildPath(startPoly, startPos, endPoly, endPos);
    }
    else
    {
        // we are moving on prefixed path - enjoy the ride
        // we already have all the data we need
        updateNextPosition();
    }
}

void PathInfo::updateNextPosition()
{
    // we are moving on precalculated path - enjoy the ride
    if(m_pointPathPointer)
    {
        //printf("++ PathInfo::updateNextPosition :: precalculated path %u\n",m_sourceObject->GetGUID());
        setNextPosition(m_pathPoints[m_pointPathPointer]);
        return;
    }

    float x, y, z;

    getStartPosition(x, y, z);
    float startPos[VERTEX_SIZE] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[VERTEX_SIZE] = {y, z, x};

#ifndef _USE_SMOOTH_PATH
    float pathPoints[MAX_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = m_navMeshQuery->findStraightPath(
        startPos,           // start position
        endPos,             // end position
        m_pathPolyRefs,     // current path
        m_polyLength,       // lenth of current path
        pathPoints,         // [out] path corner points
        NULL,               // [out] flags
        NULL,               // [out] shortened path  PATHFIND TODO: see if this is usable (IE, doesn't leave gaps in path)
        MAX_PATH_LENGTH);   // maximum number of points/polygons to use
#else
    float pathPoints[MAX_SMOOTH_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = findSmoothPath(
        startPos,           // start position
        endPos,             // end position
        m_pathPolyRefs,     // current path
        m_polyLength,       // length of current path
        pathPoints,         // [out] path corner points
        MAX_SMOOTH_PATH_LENGTH);    // maximum number of points
#endif

    if(pointCount < 2)
    {
        // only happens if pass bad data to findStraightPath or navmesh is broken
        sLog.outError("%u's UpdateNextPosition failed: %u length path", m_sourceObject->GetGUID(), pointCount);
        shortcut();
        return;
    }

    m_pathPoints.resize(pointCount);
    for (uint32 i = 0; i < pointCount; ++i)
        m_pathPoints.set(i, PathNode(pathPoints[i*VERTEX_SIZE+2], pathPoints[i*VERTEX_SIZE], pathPoints[i*VERTEX_SIZE+1]));

    // first point is always our current location - we need the next one
    setNextPosition(m_pathPoints[1]);
    m_pointPathPointer = 1;
}

void PathInfo::shortcut()
{
    clear();

    // make two point path, our curr pos is the start, and dest is the end
    m_pathPoints.resize(2);

    float x, y, z;

    // set start and a default next position
    m_sourceObject->GetPosition(x, y, z);
    m_pathPoints.set(0, PathNode(x, y, z));

    getEndPosition(x, y, z);
    m_pathPoints.set(1, PathNode(x, y, z));

    setNextPosition(m_pathPoints[1]);
    m_pointPathPointer = 1;

    m_type = PATHFIND_SHORTCUT;
}

dtQueryFilter PathInfo::createFilter()
{
    dtQueryFilter filter;

    if(m_sourceObject->GetTypeId() != TYPEID_UNIT)
        return filter;

    Creature* creature = (Creature*)m_sourceObject;
    filter.includeFlags = 0;
    filter.excludeFlags = 0;

    if(creature->canWalk())
        filter.includeFlags |= NAV_GROUND;          // walk

    if(creature->canSwim())
        filter.includeFlags |= NAV_WATER;           // swim

    // TODO: check for NAV_MAGMA
    //if(creature->IsImmunedToDamage(SPELL_SCHOOL_MASK_FIRE))     // immune to fire damage - valid?

    // TODO: check for NAV_SLIME

    // allow creatures to cheat and use different movement types if they are moved
    // forcefully into terrain they can't normally move in
    if(creature->IsInWater() || creature->IsUnderWater())
        filter.includeFlags |= getNavTerrain(creature->GetPositionX(),creature->GetPositionY(),creature->GetPositionZ());

    return filter;
}

bool PathInfo::canFly()
{
    if(m_sourceObject->GetTypeId() != TYPEID_UNIT)
        return false;

    Creature* creature = (Creature*)m_sourceObject;
    return creature->canFly();
}

bool PathInfo::canSwim()
{
    if(m_sourceObject->GetTypeId() != TYPEID_UNIT)
        return false;

    Creature* creature = (Creature*)m_sourceObject;
    return creature->canSwim();
}

NavTerrain PathInfo::getNavTerrain(float x, float y, float z)
{
    GridMapLiquidData data;
    m_sourceObject->GetMap()->getLiquidStatus(x, y, z, MAP_ALL_LIQUIDS, &data);

    switch(data.type)
    {
        case MAP_LIQUID_TYPE_WATER:
        case MAP_LIQUID_TYPE_OCEAN:
            return NAV_WATER;
        case MAP_LIQUID_TYPE_MAGMA:
            return NAV_MAGMA;
        case MAP_LIQUID_TYPE_SLIME:
            return NAV_SLIME;
        default:
            return NavTerrain(0);
    }
}

bool PathInfo::noPath()
{
    return m_type == PATHFIND_NOPATH;
}

bool PathInfo::incompletePath()
{
    return m_type == PATHFIND_INCOMPLETE;
}

uint32 PathInfo::fixupCorridor(dtPolyRef* path, const uint32 npath, const uint32 maxPath,
                               const dtPolyRef* visited, const uint32 nvisited)
{
    int32 furthestPath = -1;
    int32 furthestVisited = -1;

    // Find furthest common polygon.
    for (int32 i = npath-1; i >= 0; --i)
    {
        bool found = false;
        for (int32 j = nvisited-1; j >= 0; --j)
        {
            if (path[i] == visited[j])
            {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found)
            break;
    }

    // If no intersection found just return current path.
    if (furthestPath == -1 || furthestVisited == -1)
        return npath;

    // Concatenate paths.

    // Adjust beginning of the buffer to include the visited.
    uint32 req = nvisited - furthestVisited;
    uint32 orig = uint32(furthestPath+1) < npath ? furthestPath+1 : npath;
    uint32 size = npath-orig > 0 ? npath-orig : 0;
    if (req+size > maxPath)
        size = maxPath-req;

    if (size)
        memmove(path+req, path+orig, size*sizeof(dtPolyRef));

    // Store visited
    for (uint32 i = 0; i < req; ++i)
        path[i] = visited[(nvisited-1)-i];

    return req+size;
}

bool PathInfo::getSteerTarget(const float* startPos, const float* endPos,
                              const float minTargetDist, const dtPolyRef* path, const uint32 pathSize,
                              float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
                              float* outPoints, uint32* outPointCount)
{
    // Find steer target.
    static const uint32 MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS*VERTEX_SIZE];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    uint32 nsteerPath = m_navMeshQuery->findStraightPath(startPos, endPos, path, pathSize,
                                                steerPath, steerPathFlags, steerPathPolys, MAX_STEER_POINTS);
    if (!nsteerPath)
        return false;

    if (outPoints && outPointCount)
    {
        *outPointCount = nsteerPath;
        for (uint32 i = 0; i < nsteerPath; ++i)
            dtVcopy(&outPoints[i*VERTEX_SIZE], &steerPath[i*VERTEX_SIZE]);
    }

    // Find vertex far enough to steer to.
    uint32 ns = 0;
    while (ns < nsteerPath)
    {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !inRangeYZX(&steerPath[ns*VERTEX_SIZE], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath)
        return false;

    dtVcopy(steerPos, &steerPath[ns*VERTEX_SIZE]);
    steerPos[1] = startPos[1];
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}

uint32 PathInfo::findSmoothPath(const float* startPos, const float* endPos,
                                     const dtPolyRef* path, const uint32 pathSize,
                                     float* smoothPath, const uint32 maxSmoothPathSize)
{
    uint32 m_nsmoothPath = 0;

    dtPolyRef polys[MAX_PATH_LENGTH];
    memcpy(polys, path, sizeof(dtPolyRef)*pathSize);
    uint32 npolys = pathSize;

    float iterPos[VERTEX_SIZE], targetPos[VERTEX_SIZE];
    m_navMeshQuery->closestPointOnPolyBoundary(polys[0], startPos, iterPos);
    m_navMeshQuery->closestPointOnPolyBoundary(polys[npolys-1], endPos, targetPos);

    dtVcopy(&smoothPath[m_nsmoothPath*VERTEX_SIZE], iterPos);
    m_nsmoothPath++;

    // Move towards target a small advancement at a time until target reached or
    // when ran out of memory to store the path.
    while (npolys && m_nsmoothPath < maxSmoothPathSize)
    {
        // Find location to steer towards.
        float steerPos[VERTEX_SIZE];
        unsigned char steerPosFlag;
        dtPolyRef steerPosRef;

        if (!getSteerTarget(iterPos, targetPos, SMOOTH_PATH_SLOP, polys, npolys, steerPos, steerPosFlag, steerPosRef))
            break;

        bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END);
        bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION);

        // Find movement delta.
        float delta[VERTEX_SIZE], len = 0.0f;
        dtVsub(delta, steerPos, iterPos);
        len = dtSqrt(dtVdot(delta,delta));
        // If the steer target is end of path or off-mesh link, do not move past the location.
        if ((endOfPath || offMeshConnection) && len < SMOOTH_PATH_STEP_SIZE)
            len = 1.0f;
        else
            len = SMOOTH_PATH_STEP_SIZE / len;

        float moveTgt[VERTEX_SIZE];
        dtVmad(moveTgt, iterPos, delta, len);

        // Move
        float result[VERTEX_SIZE];
        dtPolyRef visited[16];
        dtQueryFilter filter = createFilter();

        uint32 nvisited = m_navMeshQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &filter, result, visited, 16);

        npolys = fixupCorridor(polys, npolys, MAX_PATH_LENGTH, visited, nvisited);
        float h = 0;
        m_navMeshQuery->getPolyHeight(polys[0], result, &h);
        result[1] = h;
        dtVcopy(iterPos, result);

        // Handle end of path and off-mesh links when close enough.
        if (endOfPath && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 2.0f))
        {
            // Reached end of path.
            dtVcopy(iterPos, targetPos);
            if (m_nsmoothPath < MAX_SMOOTH_PATH_LENGTH)
            {
                dtVcopy(&smoothPath[m_nsmoothPath*VERTEX_SIZE], iterPos);
                m_nsmoothPath++;
            }
            break;
        }
        else if (offMeshConnection && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 2.0f))
        {
            // Reached off-mesh connection.
            float startPos[VERTEX_SIZE], endPos[VERTEX_SIZE];

            // Advance the path up to and over the off-mesh connection.
            dtPolyRef prevRef = 0, polyRef = polys[0];
            uint32 npos = 0;
            while (npos < npolys && polyRef != steerPosRef)
            {
                prevRef = polyRef;
                polyRef = polys[npos];
                npos++;
            }

            for (uint32 i = npos; i < npolys; ++i)
                polys[i-npos] = polys[i];

            npolys -= npos;

            // Handle the connection.
            if (m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos))
            {
                if (m_nsmoothPath < maxSmoothPathSize)
                {
                    dtVcopy(&smoothPath[m_nsmoothPath*VERTEX_SIZE], startPos);
                    m_nsmoothPath++;
                    // Hack to make the dotted path not visible during off-mesh connection.
                    if (m_nsmoothPath & 1)
                    {
                        dtVcopy(&smoothPath[m_nsmoothPath*VERTEX_SIZE], startPos);
                        m_nsmoothPath++;
                    }
                }
                // Move position at the other side of the off-mesh link.
                dtVcopy(iterPos, endPos);
                float h;
                m_navMeshQuery->getPolyHeight(polys[0], iterPos, &h);
                iterPos[1] = h;
             }
        }

        // Store results.
        if (m_nsmoothPath < maxSmoothPathSize)
        {
            dtVcopy(&smoothPath[m_nsmoothPath*VERTEX_SIZE], iterPos);
            m_nsmoothPath++;
        }
    }

    return m_nsmoothPath;
}
