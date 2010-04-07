#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "pathfinding/Detour/DetourNavMesh.h"

#define MAX_PATH_LENGTH 50

enum PathType
{
    PATHFIND_BLANK      = 0,    // path not built yet
    PATHFIND_NORMAL     = 1,    // normal path
    PATHFIND_SHORTCUT   = 2     // insufficient info to build path, ignore terrain/obstacles
};

class WorldObject;

class PathInfo
{
    public:
        PathInfo()
            : Length(0), pathPolyRefs(0), sourceObject(0), targetObject(0), m_type(PATHFIND_BLANK)
        { Build(); }

        PathInfo(WorldObject* from, WorldObject* to)
            : Length(0), pathPolyRefs(0), sourceObject(from), targetObject(to), m_type(PATHFIND_BLANK)
        { Build(); }

        PathInfo(WorldObject* from, const float x, const float y, const float z)
            : Length(0), pathPolyRefs(0), sourceObject(from), m_type(PATHFIND_BLANK)
        { setEndPosition(x, y, z); Build(); }

        ~PathInfo() { delete [] pathPolyRefs; }

        inline void getStartPosition(float &x, float &y, float &z) { x = startPosition[0]; y = startPosition[1]; z = startPosition[2]; }
        inline void setStartPosition(float x, float y, float z) { startPosition[0] = x; startPosition[1] = y; startPosition[2] = z; }

        inline void getNextPosition(float &x, float &y, float &z) { x = nextPosition[0]; y = nextPosition[1]; z = nextPosition[2]; }
        inline void setNextPosition(float x, float y, float z) { nextPosition[0] = x; nextPosition[1] = y; nextPosition[2] = z; }
        
        inline void getEndPosition(float &x, float &y, float &z) { x = endPosition[0]; y = endPosition[1]; z = endPosition[2]; }
        inline void setEndPosition(float x, float y, float z) { endPosition[0] = x; endPosition[1] = y; endPosition[2] = z; }
        
        dtPolyRef getPathPolyByPosition(float x, float y, float z);
        bool isPointInPolyBounds(float x, float y, float z, dtPolyRef polyRef);

        void Update();

        dtPolyRef   *pathPolyRefs;      // array of detour polygon references
        int         Length;             // Length 0 == unreachable location
        float       startPosition[3];   // {x, y, z} of current location
        float       nextPosition[3];    // {x, y, z} of next location on the path
        float       endPosition[3];     // {x, y, z} of the destination
        WorldObject *sourceObject;      // the object that is moving
        WorldObject *targetObject;      // the object we're moving toward, if any
        dtNavMesh   *navMesh;           // the nav mesh used to find the path
        PathType    m_type;

    private:
        void updateNextPosition();
        void Build();
};

inline bool areSamePositions(const float* position1, const float* position2)
{
    return (position1[0] == position2[0] && position1[1] == position2[1] && position1[2] == position2[2]) ? true : false;
}

#endif