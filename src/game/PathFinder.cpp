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
#include "Map.h"
#include "../recastnavigation/Detour/Include/DetourCommon.h"

////////////////// PathInfo //////////////////
PathInfo::PathInfo(const WorldObject* from, const float destX, const float destY, const float destZ, bool useStraightPath) :
    m_pathPolyRefs(NULL), m_polyLength(0), m_type(PATHFIND_BLANK), m_useStraightPath(useStraightPath),
    m_sourceObject(from), m_navMesh(NULL), m_navMeshQuery(NULL)
{
    PathNode endPoint(destX, destY, destZ);
    setEndPosition(endPoint);

    float x,y,z;
    m_sourceObject->GetPosition(x, y, z);
    PathNode startPoint(x, y, z);
    setStartPosition(startPoint);

    PATH_DEBUG("++ PathInfo::PathInfo for %u \n", m_sourceObject->GetGUID());

    if(m_navMesh = m_sourceObject->GetMap()->GetNavMesh())
    {
        m_navMeshQuery = dtAllocNavMeshQuery();
        m_navMeshQuery->init(m_navMesh, MESH_MAX_NODES);

        BuildPolyPath(startPoint, endPoint);
    }
    else
    {
        BuildShortcut();
        m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
    }
}

PathInfo::~PathInfo()
{
    PATH_DEBUG("++ PathInfo::~PathInfo() for %u \n", m_sourceObject->GetGUID());

    if (m_pathPolyRefs)
        delete [] m_pathPolyRefs;

    if(m_navMesh && m_navMeshQuery)
        dtFreeNavMeshQuery(m_navMeshQuery);
}

bool PathInfo::Update(const float destX, const float destY, const float destZ, bool useStraightPath)
{
    PathNode newDest(destX, destY, destZ);
    PathNode oldDest = getEndPosition();
    setEndPosition(newDest);

    float x, y, z;
    m_sourceObject->GetPosition(x, y, z);
    PathNode newStart(x, y, z);
    PathNode oldStart = getStartPosition();
    setStartPosition(newStart);

    m_useStraightPath = useStraightPath;

    PATH_DEBUG("++ PathInfo::Update() for %u \n", m_sourceObject->GetGUID());

    // make sure navMesh works - we can run on map w/o mmap
    if(!m_navMesh)
    {
        BuildShortcut();
        m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
        return true;
    }

    float dist = m_sourceObject->GetObjectBoundingRadius();
    bool oldDestInRange = inRange(oldDest, newDest, dist, dist);

    // this can happen only if caller did a bad job calculating the need for path update
    if(oldDestInRange && inRange(newStart, oldStart, dist, dist))
        return false;

    // check if destination moved - if not we can optimize something here
    // we are following old, precalculated path?
    if(oldDestInRange && m_pathPoints.size() > 2)
    {
        // our target is not moving - we just coming closer
        // we are moving on precalculated path - enjoy the ride
        PATH_DEBUG("++ PathInfo::Update:: precalculated path\n");

        m_pathPoints.crop(1, 0);
        setNextPosition(m_pathPoints[1]);

        return false;
    }
    else
    {
        // target moved, so we need to update the poly path
        BuildPolyPath(newStart, newDest);
        return true;
    }
}

dtPolyRef PathInfo::getPathPolyByPosition(PathNode p, float &distance)
{
    distance = -1.0f; // set to invalid value

    if(!m_navMesh || !m_pathPolyRefs || !m_polyLength)
        return INVALID_POLYREF;

    float point[VERTEX_SIZE] = {p.y, p.z, p.x};
    dtPolyRef nearestPoly = INVALID_POLYREF;
    float minDist = FLT_MAX;

    for(uint32 i = 0; i < m_polyLength; ++i)
	{
		float closestPoint[VERTEX_SIZE];
		if (!m_navMeshQuery->closestPointOnPoly(m_pathPolyRefs[i], point, closestPoint))
			continue;

		float d = dtVdistSqr(point, closestPoint);
		if (d < minDist)
		{
			minDist = d;
			nearestPoly = m_pathPolyRefs[i];
		}

        if(minDist < 4.0f) // shortcut out - close enough for us
            break;
	}

    distance = dtSqrt(minDist);
    return (distance < 6.0f) ? nearestPoly : INVALID_POLYREF;
}

void PathInfo::BuildPolyPath(PathNode startPos, PathNode endPos)
{
    // *** getting start/end poly logic ***

    float distToStartPoly, distToEndPoly;

    // first we check the current path
    // if the current path doesn't contain the current poly,
    // we need to use the expensive navMesh.findNearestPoly
    dtPolyRef startPoly = getPathPolyByPosition(startPos, distToStartPoly);
    dtPolyRef endPoly = getPathPolyByPosition(endPos, distToEndPoly);

    float startPoint[VERTEX_SIZE] = {startPos.y, startPos.z, startPos.x};
    float endPoint[VERTEX_SIZE] = {endPos.y, endPos.z, endPos.x};

    // we don't have it in our old path
    // try to get it by findNearestPoly()
    // use huge vertical range here
    if(startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
    {
        float extents[VERTEX_SIZE] = {3.f, 200.f, 3.f};    // bounds of poly search area
        dtQueryFilter filter = dtQueryFilter();            // filter for poly search
        float closestPoint[VERTEX_SIZE];

        if(startPoly == INVALID_POLYREF)
        {
            startPoly = m_navMeshQuery->findNearestPoly(startPoint, extents, &filter, closestPoint);
            distToStartPoly = dtVdist(closestPoint, startPoint);
        }

        if(endPoly == INVALID_POLYREF)
        {
            endPoly = m_navMeshQuery->findNearestPoly(endPoint, extents, &filter, closestPoint);
            distToEndPoly = dtVdist(closestPoint, endPoint);
        }
    }

    // we have a hole in our mesh
    // make shortcut path and mark it as NOPATH ( with flying exception )
    // its up to caller how he will use this info
    if(startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
    {
        PATH_DEBUG("++ BuildPolyPath :: (startPoly == 0 || endPoly == 0)\n");
        BuildShortcut();
        m_type = canFly() ? PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH) : PATHFIND_NOPATH;

        return;
    }

    // we may need a better number here
    bool farFromPoly = (distToStartPoly > 7.0f || distToEndPoly > 7.0f);
    if(farFromPoly)
    {
        // TODO: swimming case
        PATH_DEBUG("++ BuildPolyPath :: farFromPoly distToStartPoly=%.3f distToEndPoly=%.3f\n", distToStartPoly, distToEndPoly);
        if(canFly())
        {
            BuildShortcut();
            m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
            PATH_DEBUG("++ BuildPolyPath :: flying case (type %u)\n", m_type);
            return;
        }
        else
        {
            float closestPoint[VERTEX_SIZE];
            // we may want to use closestPointOnPolyBoundary instead
            if(m_navMeshQuery->closestPointOnPoly(endPoly, endPoint, closestPoint))
            {
                dtVcopy(endPoint, closestPoint);
                setActualEndPosition(PathNode(endPoint[2],endPoint[0],endPoint[1]));
            }

            m_type = PATHFIND_INCOMPLETE;
        }
    }

    // *** poly path generating logic ***

    // start and end are on same polygon
    // just need to move in straight line
    if(startPoly == endPoly)
    {
        PATH_DEBUG("++ BuildPolyPath :: (startPoly == endPoly)\n");

        BuildShortcut();

        m_pathPolyRefs = new dtPolyRef[1];
        m_pathPolyRefs[0] = startPoly;
        m_polyLength = 1;

        m_type = farFromPoly ? PATHFIND_INCOMPLETE : PATHFIND_NORMAL;
        PATH_DEBUG("++ BuildPolyPath :: path type %d\n", m_type);
        return;
    }

    // look for startPoly/endPoly in current path
    // TODO: we can merge it with getPathPolyByPosition() loop
    bool startPolyFound = false;
    bool endPolyFound = false;
    uint32 pathStartIndex, pathEndIndex;

    if(m_polyLength)
    {
        for(pathStartIndex = 0; pathStartIndex < m_polyLength; ++pathStartIndex)
            if(m_pathPolyRefs[pathStartIndex] == startPoly)
            {
                startPolyFound = true;
                break;
            }

        for(pathEndIndex = m_polyLength-1; pathEndIndex > pathStartIndex; --pathEndIndex)
            if(m_pathPolyRefs[pathEndIndex] == endPoly)
            {
                endPolyFound = true;
                break;
            }
    }

    if(startPolyFound && endPolyFound)
    {
        PATH_DEBUG("++ BuildPolyPath :: (startPolyFound && endPolyFound)\n");

        // we moved along the path and the target did not move out of our old poly-path
        // our path is a simple subpath case, we have all the data we need
        // just "cut" it out

        m_polyLength = pathEndIndex - pathStartIndex + 1;

        dtPolyRef* newPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(newPolyRefs, m_pathPolyRefs+pathStartIndex, m_polyLength*sizeof(dtPolyRef));

        delete [] m_pathPolyRefs;
        m_pathPolyRefs = newPolyRefs;
    }
    else if(startPolyFound && !endPolyFound)
    {
        PATH_DEBUG("++ BuildPolyPath :: (startPolyFound && !endPolyFound)\n");

        // we are moving on the old path but target moved out
        // so we have atleast part of poly-path ready

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

        // we need any point on our suffix start poly to generate poly-path, so we need last poly in prefix data
        float suffixEndPoint[VERTEX_SIZE];
        if(!m_navMeshQuery->closestPointOnPoly(suffixStartPoly, endPoint, suffixEndPoint))
        {
            // suffixStartPoly is invalid somehow, or the navmesh is broken => error state
            sLog.outError("%u's Path Build failed: invalid polyRef in path", m_sourceObject->GetGUID());

            BuildShortcut();
            m_type = PATHFIND_NOPATH;
            return;
        }

        // generate suffix
        dtQueryFilter filter = createFilter();
        dtPolyRef suffixPathPolys[MAX_PATH_LENGTH];

        uint32 suffixPolyLength = m_navMeshQuery->findPath(
                    suffixStartPoly,    // start polygon
                    endPoly,            // end polygon
                    suffixEndPoint,     // start position
                    endPoint,           // end position
                    &filter,            // polygon search filter
                    suffixPathPolys,    // [out] path
                    MAX_PATH_LENGTH);   // max number of polygons in output path

        if(suffixPolyLength == 0)
        {
            // this is probably an error state, but we'll leave it
            // and hopefully recover on the next Update
            // we still need to copy our preffix
            sLog.outError("%u's Path Build failed: 0 length path", m_sourceObject->GetGUID());
        }

        PATH_DEBUG("++  m_polyLength=%u prefixPolyLength=%u suffixPolyLength=%u \n",m_polyLength, prefixPolyLength, suffixPolyLength);

        // new path = prefix + suffix - overlap
        m_polyLength = prefixPolyLength + suffixPolyLength - 1;
        delete [] m_pathPolyRefs;
        m_pathPolyRefs = new dtPolyRef[m_polyLength];

        // copy the part of the old path we keep - prefix
        memcpy(m_pathPolyRefs, prefixPathPolys, prefixPolyLength*sizeof(dtPolyRef));

        // copy the newly created suffix - skip first poly, we have it at prefix end
        if(suffixPathPolys)
            memcpy(m_pathPolyRefs+prefixPolyLength, suffixPathPolys+1, (suffixPolyLength-1)*sizeof(dtPolyRef));
    }
    else
    {
        PATH_DEBUG("++ BuildPolyPath :: (!startPolyFound && !endPolyFound)\n");

        // either we have no path at all -> first run
        // or something went really wrong -> we aren't moving along the path to the target
        // just generate new path

        // free and invalidate old path data
        clear();

        dtQueryFilter filter = createFilter();      // use special filter so we use proper terrain types
        dtPolyRef pathPolys[MAX_PATH_LENGTH];

        m_polyLength = m_navMeshQuery->findPath(
                startPoly,          // start polygon
                endPoly,            // end polygon
                startPoint,         // start position
                endPoint,           // end position
                &filter,            // polygon search filter
                pathPolys,          // [out] path
                MAX_PATH_LENGTH);   // max number of polygons in output path

        if(m_polyLength == 0)
        {
            // only happens if we passed bad data to findPath(), or navmesh is messed up
            sLog.outError("%u's Path Build failed: 0 length path", m_sourceObject->GetGUID());
            BuildShortcut();
            m_type = PATHFIND_NOPATH;
            return;
        }

        m_pathPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(m_pathPolyRefs, pathPolys, m_polyLength*sizeof(dtPolyRef));
    }

    // by now we know what type of path we can get
    if(m_pathPolyRefs[m_polyLength - 1] == endPoly && !(m_type & PATHFIND_INCOMPLETE))
        m_type = PATHFIND_NORMAL;
    else
        m_type = PATHFIND_INCOMPLETE;

    // generate the point-path out of our up-to-date poly-path
    BuildPointPath(startPoint, endPoint);
}

void PathInfo::BuildPointPath(float *startPoint, float *endPoint)
{
    // get the actual reachable point on last poly in path
    float closestPoint[VERTEX_SIZE];
    if((m_type & PATHFIND_INCOMPLETE)
        && m_navMeshQuery->closestPointOnPoly(m_pathPolyRefs[m_polyLength-1], endPoint, closestPoint))
    {
        dtVcopy(endPoint, closestPoint);
        setActualEndPosition(PathNode(endPoint[2],endPoint[0],endPoint[1]));
    }

    float pathPoints[MAX_POINT_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = 0;
    if(m_useStraightPath)
    {
        pointCount = m_navMeshQuery->findStraightPath(
                startPoint,         // start position
                endPoint,           // end position
                m_pathPolyRefs,     // current path
                m_polyLength,       // lenth of current path
                pathPoints,         // [out] path corner points
                NULL,               // [out] flags
                NULL,               // [out] shortened path
                MAX_POINT_PATH_LENGTH);   // maximum number of points/polygons to use
    }
    else
    {
        pointCount = findSmoothPath(
                startPoint,         // start position
                endPoint,           // end position
                m_pathPolyRefs,     // current path
                m_polyLength,       // length of current path
                pathPoints,         // [out] path corner points
                MAX_POINT_PATH_LENGTH);    // maximum number of points
    }

    if(pointCount < 2)
    {
        // only happens if pass bad data to findStraightPath or navmesh is broken
        // single point paths can be generated here 
        // TODO : check the exact cases
        PATH_DEBUG("++ PathInfo::BuildPointPath FAILED! path sized %d returned\n", pointCount);
        BuildShortcut();
        m_type = PATHFIND_NOPATH;
        return;
    }

    m_pathPoints.resize(pointCount);
    for (uint32 i = 0; i < pointCount; ++i)
        m_pathPoints.set(i, PathNode(pathPoints[i*VERTEX_SIZE+2], pathPoints[i*VERTEX_SIZE], pathPoints[i*VERTEX_SIZE+1]));

    // first point is always our current location - we need the next one
    setNextPosition(m_pathPoints[1]);

    PATH_DEBUG("++ PathInfo::BuildPointPath path type %d size %d poly-size %d\n", m_type, pointCount, m_polyLength);
}

void PathInfo::BuildShortcut()
{
    clear();

    // make two point path, our curr pos is the start, and dest is the end
    m_pathPoints.resize(2);

    // set start and a default next position
    m_pathPoints.set(0, getStartPosition());
    m_pathPoints.set(1, getActualEndPosition());

    setNextPosition(getActualEndPosition());
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

    if(creature->CanWalk())
        filter.includeFlags |= NAV_GROUND;          // walk

    if(creature->CanSwim())
        filter.includeFlags |= NAV_WATER;           // swim

    // creatures don't take environmental damage
    if(creature->CanSwim())
        filter.includeFlags |= NAV_MAGMA | NAV_SLIME;

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
    return creature->CanFly();
}

bool PathInfo::canSwim()
{
    if(m_sourceObject->GetTypeId() != TYPEID_UNIT)
        return false;

    Creature* creature = (Creature*)m_sourceObject;
    return creature->CanSwim();
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
            return NAV_EMPTY;
    }
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
    uint32 nsmoothPath = 0;

    dtPolyRef polys[MAX_PATH_LENGTH];
    memcpy(polys, path, sizeof(dtPolyRef)*pathSize);
    uint32 npolys = pathSize;

    float iterPos[VERTEX_SIZE], targetPos[VERTEX_SIZE];
    m_navMeshQuery->closestPointOnPolyBoundary(polys[0], startPos, iterPos);
    m_navMeshQuery->closestPointOnPolyBoundary(polys[npolys-1], endPos, targetPos);

    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
    nsmoothPath++;

    // Move towards target a small advancement at a time until target reached or
    // when ran out of memory to store the path.
    while (npolys && nsmoothPath < maxSmoothPathSize)
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
            if (nsmoothPath < maxSmoothPathSize)
            {
                dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
                nsmoothPath++;
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
                if (nsmoothPath < maxSmoothPathSize)
                {
                    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], startPos);
                    nsmoothPath++;
                    // Hack to make the dotted path not visible during off-mesh connection.
                    if (nsmoothPath & 1)
                    {
                        dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], startPos);
                        nsmoothPath++;
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
        if (nsmoothPath < maxSmoothPathSize)
        {
            dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
            nsmoothPath++;
        }
    }

    return nsmoothPath;
}
