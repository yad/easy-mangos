#include "Object.h"
#include "PathFinder.h"
#include "ObjectGuid.h"
#include "pathfinding/Detour/DetourNavMesh.h"
#include "pathfinding/Detour/DetourCommon.h"

////////////////// PathInfo //////////////////
dtPolyRef PathInfo::getPathPolyByPosition(float x, float y, float z)
{
    if(!navMesh)
        return 0;                           // navmesh isn't loaded

    int i;
    for(i = 0; i < this->Length; ++i)
    {
        const dtMeshTile* tile = this->navMesh->getTileByRef(this->pathPolyRefs[i], 0);
        if(!tile)
            continue;                       // tile isn't loaded, no information about target polygon

        if(isPointInPolyBounds(x, y, z, pathPolyRefs[i]))
            return this->pathPolyRefs[i];   // point is inside this polygon
    }

    return 0;                               // point is not inside the path
}

bool PathInfo::isPointInPolyBounds(float x, float y, float z, dtPolyRef polyRef)
{
    float point[3] = {y, z, x};

    const dtPoly* poly = this->navMesh->getPolyByRef(polyRef);
    const float* vertices = this->navMesh->getPolyVertsByRef(polyRef);
    float ed[DT_VERTS_PER_POLYGON];             // distance^2 from edge to point?
    float et[DT_VERTS_PER_POLYGON];             // ?

    return distancePtPolyEdgesSqr(point, vertices, poly->vertCount, ed, et);
}

void PathInfo::Build()
{
    // incase we are re-building
    delete [] pathPolyRefs;
    Length = 0;

    float x, y, z;

    if(targetObject)
    {
        targetObject->GetPosition(x, y, z);
        setEndPosition(x, y, z);
    }


    // set a default next position
    sourceObject->GetPosition(x, y, z);
    setNextPosition(x, y, z);
    navMesh = sourceObject->GetMap()->GetNavMesh(x, y);

    if(!navMesh)
    {
        sLog.outError("%u's Path Build failed: navMesh is null", sourceObject->GetGUID());
        // ignore obstacles/terrain is better than giving up
        Length = 1;
        getEndPosition(x, y, z);
        setNextPosition(x, y, z);
        m_type = PATHFIND_SHORTCUT;
        return;
    }

    float extents[3] = {2.0f, 4.0f, 2.0f};
    dtQueryFilter filter = dtQueryFilter();

    sourceObject->GetPosition(x, y, z);
    float startPos[3] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[3] = {y, z, x};

    dtPolyRef startPoly = navMesh->findNearestPoly(startPos, extents, &filter, 0);
    dtPolyRef endPoly = navMesh->findNearestPoly(endPos, extents, &filter, 0);

    if(startPoly == 0 || endPoly == 0)
    {
        // one or both positions not in navmesh?
        sLog.outError("%u's Path Build failed: invalid start or end polygon", sourceObject->GetGUID());
        Length = 1;
        getEndPosition(x, y, z);
        setNextPosition(x, y, z);
        m_type = PATHFIND_SHORTCUT;
        return;
    }

    bool startOffPoly, endOffPoly;
    //float startHeight, endHeight;

    startOffPoly = !isPointInPolyBounds(startPos[2], startPos[0], startPos[1], startPoly);
    endOffPoly = !isPointInPolyBounds(endPos[2], endPos[0], endPos[1], endPoly);
    
    //startHeight =
    //endHeight = 
    
    dtPolyRef pathPolys[MAX_PATH_LENGTH];
    Length = navMesh->findPath(startPoly, endPoly, startPos, endPosition, &filter, pathPolys, MAX_PATH_LENGTH);

    if(Length == 0)
    {
        // no path exists
        sLog.outError("%u's Path Build failed: 0 length path", sourceObject->GetGUID());
        Length = 1;
        getEndPosition(x, y, z);
        setNextPosition(x, y, z);
        m_type = PATHFIND_SHORTCUT;
        return;
    }

    pathPolyRefs = new dtPolyRef[Length];

    int i;
    for(i = 0; i < Length; ++i)
        pathPolyRefs[i] = pathPolys[i];

    m_type = PATHFIND_NORMAL;
}

void PathInfo::Update()
{
    float x, y, z;

    if(!navMesh)
    {
        sourceObject->GetPosition(x, y, z);
        navMesh = sourceObject->GetMap()->GetNavMesh(x, y);

        if(!navMesh)
        {
            sLog.outError("%u's UpdatePath failed: navMesh is null", sourceObject->GetGUID());
            // can't pathfind if navmesh doesn't exist
            // ignore obstacles/terrain is better than giving up
            if(targetObject)
                targetObject->GetPosition(x, y, z);
            else
            {
                x = endPosition[0];
                y = endPosition[1];
                z = endPosition[2];
            }

            Length = 1;
            setNextPosition(x, y, z);
            m_type = PATHFIND_SHORTCUT;
            return;
        }
    } // done checking navMesh

    // update end position (only if we're moving toward an object that can move)
    if(targetObject)
    {
        uint8 objectType = targetObject->GetTypeId();
        if((objectType == TYPEID_UNIT || objectType == TYPEID_PLAYER)
            && !((Unit*)targetObject)->hasUnitState(UNIT_STAT_NOT_MOVE))
        {
            targetObject->GetPosition(x, y, z);
            setEndPosition(x, y, z);
        }
    }

    // update start position
    sourceObject->GetPosition(x, y, z);
    float startPosition[3] = {y, z, x};

    // should be safe to update path now
    float extents[3] = {2.0f, 4.0f, 2.0f};
    dtQueryFilter filter = dtQueryFilter();
    bool startOffPath = false;
    bool endOffPath = false;

    // find start and end poly
    // navMesh.findNearestPoly is expensive, so first we check just the current path
    dtPolyRef startPoly = getPathPolyByPosition(startPosition[2], startPosition[0], startPosition[1]);
    if(!startPoly)
    {
        startOffPath = true;
        startPoly = navMesh->findNearestPoly(startPosition, extents, &filter, 0);
    }

    dtPolyRef endPoly = getPathPolyByPosition(endPosition[0],endPosition[1],endPosition[2]);
    if(!endPoly)
    {
        endOffPath = true;
        endPoly = navMesh->findNearestPoly(endPosition, extents, &filter, 0);
    }

    if(startPoly == 0 || endPoly == 0)
    {
        sLog.outError("%u's UpdatePath failed: invalid start or end polygon", sourceObject->GetGUID());
        // poly is invalid
        // ignore obstacles/terrain is better than giving up
        Length = 1;
        setNextPosition(endPosition[0], endPosition[1], endPosition[2]);
        m_type = PATHFIND_SHORTCUT;
        return;
    }

    if(startPoly == endPoly)
    {
        // start and end are on same polygon
        // just need to move in straight line
        Length = 1;
        setNextPosition(endPosition[0], endPosition[1], endPosition[2]);
        m_type = PATHFIND_NORMAL;
        return;
    }

    if(startOffPath)
    {
        bool adjacent = false;
        int i;
        for(i = 0; i < DT_VERTS_PER_POLYGON; ++i)
            if(startPoly == navMesh->getPolyByRef(pathPolyRefs[0])->neis[i])
            {
                adjacent = true;
                break;
            }

        if(adjacent)
        {
            // startPoly is adjacent to the path, we can add it to the start of the path
            // 50th poly will fall off of path, shouldn't be an issue because most paths aren't that long
            int newLength = Length < MAX_PATH_LENGTH ? Length + 1 : Length;
            dtPolyRef* temp = new dtPolyRef[newLength];
            temp[0] = startPoly;

            for(i = 1; i < newLength; ++i)
                temp[i] = pathPolyRefs[i - 1];

            delete [] pathPolyRefs;
            pathPolyRefs = temp;
        }
        else
        {
            // waste of time to optimize, just find brand new path
            Build();
        }
    }

    if(endOffPath)
    {
        ;
    }

    // what to do now?

    // clean up

    m_type = PATHFIND_NORMAL;
    return;
}

//void WorldObject::UpdatePath(PathInfo* path)
//{
//    float x, y, z;
//
//    if(!path->navMesh)
//    {
//        path->navMesh = GetMap()->GetNavMesh(m_positionX, m_positionY);
//
//        if(!path->navMesh)
//        {
//            sLog.outError("%u's UpdatePath failed: navMesh is null", GetGUID());
//            // can't pathfind if navmesh doesn't exist
//            // ignore obstacles/terrain is better than giving up
//            path->targetObject->GetPosition(x, y, z);
//
//            path->Length = 1;
//            path->setNextPosition(x, y, z);
//            return;
//        }
//    }
//    
//    // update end position (only if we're moving toward an object that can move)
//    if(path->targetObject)
//    {
//        uint8 objectType = path->targetObject->GetTypeId();
//        if((objectType == TYPEID_UNIT || objectType == TYPEID_PLAYER) && !((Unit*)path->targetObject)->hasUnitState(UNIT_STAT_NOT_MOVE))
//        {
//            path->targetObject->GetPosition(x, y, z);
//            path->setEndPosition(x, y, z);
//        }
//    }
//    
//    // update start position
//    GetPosition(x, y, z);
//    float startPosition[3] = {y, z, x};
//
//    // should be safe to update path now
//    float extents[3] = {2.0f, 4.0f, 2.0f};
//    dtQueryFilter filter = dtQueryFilter();
//    bool startOffPath = false;
//    bool endOffPath = false;
//
//    // find start and end poly
//    // navMesh.findNearestPoly is expensive, so first we check just the current path
//    dtPolyRef startPoly = path->getPathPolyByPosition(startPosition[2], startPosition[0], startPosition[1]);
//    if(!startPoly)
//    {
//        startOffPath = true;
//        startPoly = path->navMesh->findNearestPoly(startPosition, extents, &filter, 0);
//    }
//
//    dtPolyRef endPoly = path->getPathPolyByPosition(path->endPosition[0],path->endPosition[1],path->endPosition[2]);
//    if(!endPoly)
//    {
//        endOffPath = true;
//        endPoly = path->navMesh->findNearestPoly(path->endPosition, extents, &filter, 0);
//    }
//
//    if(startPoly == 0 || endPoly == 0)
//    {
//        sLog.outError("%u's UpdatePath failed: invalid start or end polygon", GetGUID());
//        // poly is invalid
//        // ignore obstacles/terrain is better than giving up
//        path->Length = 1;
//        path->setNextPosition(path->endPosition[0], path->endPosition[1], path->endPosition[2]);
//        return;     
//    }
//
//    if(startPoly == endPoly)
//    {
//        // start and end are on same polygon
//        // just need to move in straight line
//        path->Length = 1;
//        path->setNextPosition(path->endPosition[0], path->endPosition[1], path->endPosition[2]);
//        return; 
//    }
//
//    if(startOffPath)
//    {
//        bool adjacent = false;
//        int i;
//        for(i = 0; i < DT_VERTS_PER_POLYGON; ++i)
//            if(startPoly == path->navMesh->getPolyByRef(path->pathPolyRefs[0])->neis[i])
//            {
//                adjacent = true;
//                break;
//            }
//
//        if(adjacent)
//        {
//            // startPoly is adjacent to the path, we can add it to the start of the path
//            // 50th poly will fall off of path, shouldn't be an issue
//            dtPolyRef temp[MAX_PATH_LENGTH];
//            temp[0] = startPoly;
//
//            for(i = 0; (i < path->Length) && (i < MAX_PATH_LENGTH - 1); ++i)
//                temp[i+1] = path->pathPolyRefs[i];
//
//            path->Length = path->Length < 50 ? path->Length + 1 : path->Length;
//
//            for(i = 0; i < path->Length; ++i)
//                path->pathPolyRefs[i] = temp[i];
//        }
//        else
//        {
//            // waste of time to optimize, just find brand new path
//            float endPos[3];
//            endPos[0] = path->endPosition[0];
//            endPos[1] = path->endPosition[1];
//            endPos[2] = path->endPosition[2];
//            WorldObject* targetObject = path->targetObject;
//
//            delete path;
//
//            path = GetPathTo(endPos[0], endPos[1], endPos[2]);
//            path->targetObject = targetObject;
//            startPoly = path->pathPolyRefs[0];
//            endPoly = path->pathPolyRefs[path->Length - 1];
//            endOffPath = false;
//        }
//    }
//
//    if(endOffPath)
//    {
//        ;
//    }
//
//    // what to do now?
//
//    // clean up
//}
//
//void WorldObject::GetNextPositionOnPath(PathInfo* path)
//{
//    if(!path->navMesh)
//    {
//        path->navMesh = GetMap()->GetNavMesh(m_positionX, m_positionY);
//
//        if(!path->navMesh)
//        {
//            sLog.outError("%u's GetNextPositionOnPath failed: navMesh is null", GetGUID());
//            // can't pathfind if navmesh doesn't exist
//            // ignore obstacles/terrain is better than giving up
//            path->Length = 1;
//            path->setNextPosition(path->endPosition[0], path->endPosition[1], path->endPosition[2]);
//            return;
//        }
//    }
//
//    if(!path->pathPolyRefs)
//    {
//        sLog.outError("%u's GetNextPositionOnPath failed: po path polygons provided", GetGUID());
//        // no path to follow
//        path->Length = 1;
//        path->setNextPosition(path->endPosition[0], path->endPosition[1], path->endPosition[2]);
//        return;
//    }
//
//    float startPos[3] = {m_positionY, m_positionZ, m_positionX};
//    float endPos[3] = {path->endPosition[1], path->endPosition[2], path->endPosition[0]};
//    float* pathPositions = new float[3*MAX_PATH_LENGTH];
//    dtPolyRef* straightPath = new dtPolyRef[MAX_PATH_LENGTH];
//
//    int positionCount = path->navMesh->findStraightPath(startPos, endPos, path->pathPolyRefs, path->Length, pathPositions, 0, straightPath, MAX_PATH_LENGTH);
//
//    if (positionCount <= 1)
//    {
//        sLog.outError("%u's GetNextPositionOnPath failed: no valid points found", GetGUID());
//        // only position is on the startPoly boundary
//        // not what we want
//        path->Length = 1;
//        path->setNextPosition(path->endPosition[0], path->endPosition[1], path->endPosition[2]);
//        return;
//    }
//    
//    //path->setNextPosition(
//}