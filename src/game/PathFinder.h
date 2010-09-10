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

#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "../recastnavigation/Detour/Include/DetourNavMesh.h"
#include "../recastnavigation/Detour/Include/DetourNavMeshQuery.h"

class WorldObject;

#define MESH_MAX_NODES 2048
#define MAX_PATH_LENGTH 256
#define VERTEX_SIZE 3

#define INVALID_POLYREF 0

// see contrib/mmap/src/TileBuilder.h
enum NavTerrain
{
    NAV_GROUND  = 0x01,
    NAV_MAGMA   = 0x02,
    NAV_SLIME   = 0x04,
    NAV_WATER   = 0x08,
    NAV_UNUSED1 = 0x10,
    NAV_UNUSED2 = 0x20
    // we only have 6 bits of a bitfield
};

enum PathType
{
    PATHFIND_BLANK      = 0x0001,   // path not built yet
    PATHFIND_NORMAL     = 0x0002,   // normal path
    PATHFIND_SHORTCUT   = 0x0004,   // travel through obstacles, terrain, air, etc (old behavior)
    PATHFIND_INCOMPLETE = 0x0008,   // path is too long, more will be calculated later
    PATHFIND_NOPATH     = 0x0010    // could not find a path
};

class PathInfo
{
    public:
        PathInfo(const WorldObject* from, const float x, const float y, const float z);
        ~PathInfo();

        inline void getStartPosition(float &x, float &y, float &z) { x = m_startPosition[0]; y = m_startPosition[1]; z = m_startPosition[2]; }
        inline void getNextPosition(float &x, float &y, float &z) { x = m_nextPosition[0]; y = m_nextPosition[1]; z = m_nextPosition[2]; }
        inline void getEndPosition(float &x, float &y, float &z) { x = m_endPosition[0]; y = m_endPosition[1]; z = m_endPosition[2]; }

        inline uint32 getFullPath(float **pathPoints) { *pathPoints = m_pathPoints; return m_pointLength; };

        void Update(const float x, const float y, const float z);
        bool noPath();

        // only for debug
        dtNavMesh *getMesh(){ return m_navMesh;}
        dtNavMeshQuery *getMeshQuery(){ return m_navMeshQuery;}
        dtPolyRef *getPathPolyRef(){ return m_pathPolyRefs;}
        uint32 getPolyLength(){ return m_polyLength; }

    private:

        dtPolyRef   *   m_pathPolyRefs;     // array of detour polygon references
        uint32          m_polyLength;       // number of polygons in the path

        float       *   m_pathPoints;       // array of float[3] for (x, y, z) coords
        uint32          m_pointLength;      // number of points in the path
        uint32          m_pointPathPointer; // points to current triple in m_pathPoints - used when dest do not change

        float           m_startPosition[VERTEX_SIZE]; // {x, y, z} of current location
        float           m_nextPosition[VERTEX_SIZE];  // {x, y, z} of next location on the path
        float           m_endPosition[VERTEX_SIZE];   // {x, y, z} of the destination

        const WorldObject *   m_sourceObject;     // the object that is moving (safe pointer because PathInfo is only accessed from the mover?)
        dtNavMesh   *   m_navMesh;          // the nav mesh
        dtNavMeshQuery* m_navMeshQuery;     // the nav mesh query used to find the path
        PathType        m_type;             // tells what kind of path this is

        inline void setNextPosition(float x, float y, float z) { m_nextPosition[0] = x; m_nextPosition[1] = y; m_nextPosition[2] = z; }
        inline void setStartPosition(float x, float y, float z) { m_startPosition[0] = x; m_startPosition[1] = y; m_startPosition[2] = z; }
        inline void setEndPosition(float x, float y, float z) { m_endPosition[0] = x; m_endPosition[1] = y; m_endPosition[2] = z; }

        inline void clear()
        {
            delete [] m_pathPolyRefs;
            m_pathPolyRefs = NULL;

            delete [] m_pathPoints;
            m_pathPoints = NULL;

            m_polyLength = 0;
            m_pointLength = 0;
            m_pointPathPointer = 0;
        }

        dtPolyRef getPathPolyByPosition(float x, float y, float z);
        bool isPointInPolyBounds(float x, float y, float z, float &distance, dtPolyRef polyRef);

        void BuildFreshPath();
        void BuildPath(dtPolyRef startPoly, float* startPos, dtPolyRef endPoly, float* endPos);
        // NODE: startPos, endPos is in Y,Z,X format!

        void updateNextPosition();
        void shortcut();

        dtQueryFilter createFilter();

        bool canFly();
        bool canSwim();
        NavTerrain getNavTerrain(float x, float y, float z);
};

// using == for two float numbers wont do us much good, use diff
inline bool isSamePoint(const float diff, const float* point1, const float* point2)
{
    return (abs(point1[0] - point2[0]) <= diff &&
            abs(point1[1] - point2[1]) <= diff &&
            abs(point1[2] - point2[2]) <= diff);
}

inline bool isSamePoint(const float diff, const float x1, const float y1, const float z1,
                                          const float x2, const float y2, const float z2)
{
    return (abs(x1 - x2) <= diff &&
            abs(y1 - y2) <= diff &&
            abs(z1 - z2) <= diff);
}

#endif
