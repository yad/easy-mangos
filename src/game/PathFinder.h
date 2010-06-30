#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "pathfinding/Detour/DetourNavMesh.h"

class WorldObject;

#define MAX_PATH_LENGTH 256

enum NavTerrain
{
    NAV_GROUND  = 0x01,

    NAV_MAGMA   = 0x02,
    NAV_SLIME   = 0x04,

    NAV_SHALLOW_WATER   = 0x08,
    NAV_AVERAGE_WATER   = 0x10,
    NAV_DEEP_WATER      = 0x20,
    NAV_SWIM_WATER      = 0x40,
    NAV_WATER           = NAV_SHALLOW_WATER | NAV_AVERAGE_WATER | NAV_DEEP_WATER | NAV_SWIM_WATER,

    NAV_UNSPECIFIED     = 0x80
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
        PathInfo(WorldObject* from, const float x, const float y, const float z);

        ~PathInfo() { delete [] m_pathPolyRefs; delete [] m_pathPoints; }

        inline void getStartPosition(float &x, float &y, float &z) { x = m_startPosition[0]; y = m_startPosition[1]; z = m_startPosition[2]; }
        inline void setStartPosition(float x, float y, float z) { m_startPosition[0] = x; m_startPosition[1] = y; m_startPosition[2] = z; }

        inline void getNextPosition(float &x, float &y, float &z) { x = m_nextPosition[0]; y = m_nextPosition[1]; z = m_nextPosition[2]; }
        inline void setNextPosition(float x, float y, float z) { m_nextPosition[0] = x; m_nextPosition[1] = y; m_nextPosition[2] = z; }

        inline void getEndPosition(float &x, float &y, float &z) { x = m_endPosition[0]; y = m_endPosition[1]; z = m_endPosition[2]; }
        inline void setEndPosition(float x, float y, float z) { m_endPosition[0] = x; m_endPosition[1] = y; m_endPosition[2] = z; }

        dtPolyRef getPathPolyByPosition(float x, float y, float z);
        bool isPointInPolyBounds(float x, float y, float z, float &distance, dtPolyRef polyRef);

        void Update(const float x, const float y, const float z);
        bool noPath();
        void GetLength();
        float* GetPoints();

        dtPolyRef   *   m_pathPolyRefs;     // array of detour polygon references
        float       *   m_pathPoints;       // array of float[3] for (x, y, z) coords
        int             m_currentNode;      // node of m_pathPoints that we are moving to
        int             m_length;           // number of polygons in the path
        float           m_startPosition[3]; // {x, y, z} of current location
        float           m_nextPosition[3];  // {x, y, z} of next location on the path
        float           m_endPosition[3];   // {x, y, z} of the destination
        WorldObject *   m_sourceObject;     // the object that is moving (safe pointer because PathInfo is only accessed from the mover?)
        dtNavMesh   *   m_navMesh;          // the nav mesh used to find the path
        PathType        m_type;             // tells what kind of path this is

    private:
        inline void clear()
        {
            delete [] m_pathPolyRefs; m_pathPolyRefs = 0;
            delete [] m_pathPoints; m_pathPoints = 0;
            m_length = 0;
        }

        void updateNextPosition();
        void Build();
        void Build(dtPolyRef startPoly, dtPolyRef endPoly);
        void trim(dtPolyRef startPoly, dtPolyRef endPoly);
        void shortcut();

        dtQueryFilter createFilter();

        bool canFly();
        bool canSwim();
        NavTerrain getNavTerrain(float x, float y, float z);

        inline void coreToRecast(float core[3], float recast[3]) {recast[0] = core[1]; recast[1] = core[2]; recast[2] = core[0];}
        inline void recastToCore(float recast[3], float core[3]) {core[0] = recast[2]; core[1] = recast[0]; core[2] = recast[1];}
};

inline bool isSamePoint(const float* point1, const float* point2)
{
    return (point1[0] == point2[0] && point1[1] == point2[1] && point1[2] == point2[2]);
}

inline bool isSamePoint(const float x1, const float y1, const float z1,
                        const float x2, const float y2, const float z2)
{
    return (x1 == x2 && y1 == y2 && z1 == z2);
}

#endif