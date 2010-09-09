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

#include "Object.h"
#include "Creature.h"
#include "PathFinder.h"
#include "Map.h"
#include "../recastnavigation/Detour/Include/DetourCommon.h"

////////////////// PathInfo //////////////////
PathInfo::PathInfo(const WorldObject* from, const float x, const float y, const float z) :
    m_navMesh(NULL), m_navMeshQuery(NULL),
    m_polyLength(0), m_pathPolyRefs(NULL), m_pointLength(0), m_pathPoints(NULL),
    m_pointPathPointer(0), m_sourceObject(from), m_type(PATHFIND_BLANK)
{
    setEndPosition(x, y, z);

    if(m_navMesh = m_sourceObject->GetMap()->GetNavMesh())
    {
        m_navMeshQuery = dtAllocNavMeshQuery();
        m_navMeshQuery->init(m_navMesh, MESH_MAX_NODES);

        BuildFreshPath();
    }
    else
    {
        setNextPosition(x, y, z);
        m_type = PATHFIND_SHORTCUT;
    }
}

PathInfo::~PathInfo()
{
    delete [] m_pathPolyRefs;
    delete [] m_pathPoints;

    if(m_navMesh && m_navMeshQuery)
        dtFreeNavMeshQuery(m_navMeshQuery);
}

dtPolyRef PathInfo::getPathPolyByPosition(float x, float y, float z)
{
    if(!m_navMesh)
        return 0;       // navmesh isn't loaded

    float distance;     // not used

    for(uint32 i = 0; i < m_polyLength; ++i)
    {
        const dtMeshTile* tile;
        const dtPoly* poly;
        if(!m_navMesh->getTileAndPolyByRef(m_pathPolyRefs[i], &tile, &poly))
            return 0;   // m_pathPolyRefs[i] is invalid

        if(!tile)
            continue;   // tile isn't loaded, no information about target polygon

        if(isPointInPolyBounds(x, y, z, distance, m_pathPolyRefs[i]))
            return m_pathPolyRefs[i];   // point is inside this polygon
    }

    return 0;   // point is not inside the path
}

bool PathInfo::isPointInPolyBounds(float x, float y, float z, float &distance, dtPolyRef polyRef)
{
    float point[VERTEX_SIZE] = {y, z, x};

    const dtMeshTile* tile;
    const dtPoly* poly;
    if(!m_navMesh->getTileAndPolyByRef(polyRef, &tile, &poly))
        return false;   // m_pathPolyRefs[i] is invalid

    float vertices[DT_VERTS_PER_POLYGON*VERTEX_SIZE];
    float ed[DT_VERTS_PER_POLYGON];             // distance^2 from edge to point
    float et[DT_VERTS_PER_POLYGON];             // describes where on edge is nearest point

    // Collect vertices.
    uint32 nv = 0;
    for (uint32 i = 0; i < poly->vertCount; ++i)
    {
        dtVcopy(&vertices[nv*VERTEX_SIZE], &tile->verts[poly->verts[i]*VERTEX_SIZE]);
        nv++;
    }

    bool isInsidePoly = dtDistancePtPolyEdgesSqr(point, vertices, nv, ed, et);

    if(!isInsidePoly)
    {
        // distance to nearest edge
        distance = FLT_MAX;
        for(uint32 i = 0; i < poly->vertCount; ++i)
            if(ed[i] < distance)
                distance = ed[i];
    }
    else
        distance = 0.0f;

    return isInsidePoly;
}

void PathInfo::BuildFreshPath()
{
    clear();

    float x, y, z;
    // set start and a default next position
    m_sourceObject->GetPosition(x, y, z);
    setStartPosition(x, y, z);

    float extents[VERTEX_SIZE] = {2.0f, 4.0f, 2.0f};      // defines bounds of box for search area
    dtQueryFilter filter = dtQueryFilter();     // use general filter so we know if we are near navmesh

    // get start and end positions
    float startPos[VERTEX_SIZE] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[VERTEX_SIZE] = {y, z, x};

    // find start and end poly
    dtPolyRef startPoly = m_navMeshQuery->findNearestPoly(startPos, extents, &filter, 0);
    dtPolyRef endPoly = m_navMeshQuery->findNearestPoly(endPos, extents, &filter, 0);

    BuildPath(startPoly, startPos, endPoly, endPos);
}

void PathInfo::BuildPath(dtPolyRef startPoly, float* startPos, dtPolyRef endPoly, float* endPos)
{
    if(startPoly == 0 || endPoly == 0)
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

        // PATHFIND TODO: prevent walking/swimming mobs from flying into the air
        clear();
        m_pathPolyRefs = new dtPolyRef[1];
        m_pathPolyRefs[0] = startPoly;
        m_polyLength = 1;

        float x,y,z;
        getEndPosition(x, y, z);
        setNextPosition(x, y, z);
        m_type = PathType(PATHFIND_NORMAL);
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

        // try to adjust the sufix of the path instead of recalculating entire length
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

        // we need any point on our sufix start poly to generate poly-path, so we need last poly in prefix data
        const dtMeshTile* tile;
        const dtPoly* poly;
        if(m_navMesh->getTileAndPolyByRef(suffixStartPoly, &tile, &poly))
        {
            dtQueryFilter filter = createFilter();

            // generate sufix
            suffixPolyLength = m_navMeshQuery->findPath(
                    suffixStartPoly,     // start polygon
                    endPoly,            // end polygon
                    // we might need to get better point here
                    &tile->verts[poly->verts[0]], // start position
                    endPos,             // end position
                    &filter,            // polygon search filter
                    suffixPathPolys,     // [out] path
                    MAX_PATH_LENGTH);   // max number of polygons in output path

            if(sufixPolyLenght == 0)
            {
                // only happens if we passed bad data to findPath(), or navmesh is messed up
                sLog.outError("%u's Path Build failed: 0 length path", m_sourceObject->GetGUID());
                // we have refix ready, use it - we'll recover later
            }
        }

        delete [] m_pathPolyRefs;

        // copy the part of the old path we keep - prefix
        // copy the newly created sufix - skip first poly, we have it at prefix end
        m_polyLength = prefixPolyLength + sufixPolyLenght - 1;
        m_pathPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(m_pathPolyRefs, prefixPathPolys, prefixPolyLength*sizeof(dtPolyRef));
        if(sufixPolyLenght)
            memcpy(m_pathPolyRefs+prefixPolyLength, sufixPathPolys+1, (sufixPolyLenght-1)*sizeof(dtPolyRef));
    }
    else
    {
        //printf("++ PathInfo::BuildPath :: (!startPolyFound && !endPolyFound) %u\n",m_sourceObject->GetGUID());

        // either we have no path at all -> first run
        // or something went really wrong -> we aren't moving along the path to the target

        // just generate new path
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

        delete [] m_pathPolyRefs;

        m_pathPolyRefs = new dtPolyRef[m_polyLength];
        memcpy(m_pathPolyRefs, pathPolys, m_polyLength*sizeof(dtPolyRef));
    }

    if(m_polyLength >= MAX_PATH_LENGTH)
         m_type = PathType(m_type | PATHFIND_INCOMPLETE);

    // generate the point-path out of our up-to-date poly-path
    updateNextPosition();
}

void PathInfo::Update(const float destX, const float destY, const float destZ)
{
    // make sure navMesh works - we can run on map w/o mmap
    // can we switch map/get different mesh?
    if(!m_navMesh || m_navMesh != m_sourceObject->GetMap()->GetNavMesh())
    {
        setEndPosition(destX, destY, destZ);
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
    setStartPosition(x, y, z);

    // check if destination moved - if not we can optimize something here
    // we are following old, precalculated path?
    float dist = m_sourceObject->GetObjectBoundingRadius();
    float oldEnd_x, oldEnd_y, oldEnd_z;
    getEndPosition(oldEnd_x, oldEnd_y, oldEnd_z);
    if(isSamePoint(dist, oldEnd_x, oldEnd_y, oldEnd_z, destX, destY, destZ)
        && m_pointLength > 2 && m_pointPathPointer < m_pointLength)
    {
        // our target is not moving - we just coming closer
        if(!m_pointPathPointer)
        {
            // first in array is start point,
            // and next was set just after we got this path first time
            m_pointPathPointer = 2;
        }
        else
            m_pointPathPointer++;
    }
    else
    {
        // target moved, so we need to update the poly path
        m_pointPathPointer = 0;
        setEndPosition(destX, destY, destZ);
    }

    if(!m_pointPathPointer)
    {
        // find start and end poly
        // navMesh.findNearestPoly is expensive, so first we check just the current path
        float startPos[VERTEX_SIZE] = {y, z, x};
        float endPos[VERTEX_SIZE] = {destY, destZ, destX};
        dtPolyRef startPoly = getPathPolyByPosition(x, y, z);
        dtPolyRef endPoly = getPathPolyByPosition(destX, destY, destZ);
        if(startPoly == 0 || endPoly == 0)
        {
            // start or end is off the path, need to find the polygon
            float extents[VERTEX_SIZE] = {2.f, 4.f, 2.f};   // bounds of poly search area
            dtQueryFilter filter = dtQueryFilter();         // filter for poly search

            if(startPoly == 0)
                startPoly = m_navMeshQuery->findNearestPoly(startPos, extents, &filter, 0);

            if(endPoly == 0)
                endPoly = m_navMeshQuery->findNearestPoly(endPos, extents, &filter, 0);
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
        setNextPosition(m_pathPoints[m_pointPathPointer*VERTEX_SIZE+2],
                        m_pathPoints[m_pointPathPointer*VERTEX_SIZE],
                        m_pathPoints[m_pointPathPointer*VERTEX_SIZE+1]);
        return;
    }

    float x, y, z;

    getStartPosition(x, y, z);
    float startPos[VERTEX_SIZE] = {y, z, x};
    getEndPosition(x, y, z);
    float endPos[VERTEX_SIZE] = {y, z, x};

    float* pathPoints = new float[MAX_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = m_navMeshQuery->findStraightPath(
        startPos,           // start position
        endPos,             // end position
        m_pathPolyRefs,     // current path
        m_polyLength,       // lenth of current path
        pathPoints,         // [out] path corner points
        0,                  // [out] flags
        0,                  // [out] shortened path  PATHFIND TODO: see if this is usable (IE, doesn't leave gaps in path)
        MAX_PATH_LENGTH);   // maximum number of points/polygons to use

    // TODO: imitate PATHFIND_ITER code from RecastDemo so that terrain following is smoother
    // example in NavMeshTesterTool::recalc()
    // but it is pretty costy

    if(pointCount == 0)
    {
        delete [] pathPoints;

        // only happens if pass bad data to findStraightPath or navmesh is broken
        sLog.outError("%u's UpdateNextPosition failed: 0 length path", m_sourceObject->GetGUID());
        shortcut();
        return;
    }

    // first point is always our current location - we need the next one
    setNextPosition(pathPoints[VERTEX_SIZE+2], pathPoints[VERTEX_SIZE], pathPoints[VERTEX_SIZE+1]);

    delete [] m_pathPoints;
    m_pathPoints = pathPoints;
    m_pointLength = pointCount;
    m_pointPathPointer = 0;

    m_type = PathType(m_type | PATHFIND_NORMAL);

    // place we want to go, there is likely no complete path
    // findStraightPath() will always return the closest location
    // to our destination, if not rechable
    // we used shortcut() if no path at all found.
    endPos[0] = m_pathPoints[(m_pointLength-1)*VERTEX_SIZE+2];
    endPos[1] = m_pathPoints[(m_pointLength-1)*VERTEX_SIZE];
    endPos[2] = m_pathPoints[(m_pointLength-1)*VERTEX_SIZE+1];

    float dist = 2*m_sourceObject->GetObjectBoundingRadius();
    if(!isSamePoint(dist, endPos, m_endPosition))
        m_type = PathType(m_type | PATHFIND_INCOMPLETE);
}

void PathInfo::shortcut()
{
    clear();

    float x, y, z;
    getEndPosition(x, y, z);
    setNextPosition(x, y, z);
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
    m_sourceObject->GetMap()->getLiquidStatus(x,
                                              y,
                                              z,
                                              MAP_ALL_LIQUIDS,
                                              &data);

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
    // basically:
    // endOfPath != destination && completePath
    return (m_type & PATHFIND_NOPATH) && !(m_type & PATHFIND_INCOMPLETE);
}
