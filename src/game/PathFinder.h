#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "pathfinding/Detour/DetourNavMesh.h"

class WorldObject;

#define MAX_PATH_LENGTH 50

enum PathType
{
    PATHFIND_BLANK      = 0,    // path not built yet
    PATHFIND_NORMAL     = 1,    // normal path
    PATHFIND_SHORTCUT   = 2,    // travel through obstacles, terrain, air, etc (old behavior)
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
        void GetLength();
        float* GetPoints();

        dtPolyRef   *   m_pathPolyRefs;     // array of detour polygon references
        float       *   m_pathPoints;       // array of float[3] for (x, y, z) coords
        int             m_currentNode;      // node of m_pathPoints that we are moving to
        int             m_length;           // number of polygons in the path
        float           m_startPosition[3]; // {x, y, z} of current location
        float           m_nextPosition[3];  // {x, y, z} of next location on the path
        float           m_endPosition[3];   // {x, y, z} of the destination
        WorldObject *   m_sourceObject;     // the object that is moving
        dtNavMesh   *   m_navMesh;          // the nav mesh used to find the path
        PathType        m_type;             // tells what kind of path this is

    private:
        void updateNextPosition();
        void Build();
        void Build(dtPolyRef startPoly, dtPolyRef endPoly);
        void trim(dtPolyRef startPoly, dtPolyRef endPoly);
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