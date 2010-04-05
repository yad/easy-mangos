#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "pathfinding/Detour/DetourNavMesh.h"

#define MAX_PATH_LENGTH 50

class WorldObject;

class PathInfo
{
    public:
        PathInfo() : Length(0), pathPolyRefs(0) { }
        ~PathInfo() { delete [] pathPolyRefs; }

        inline void setNextPosition(float x, float y, float z) { nextPosition[0] = x; nextPosition[1] = y; nextPosition[2] = z; }
        inline void setEndPosition(float x, float y, float z) { endPosition[0] = x; endPosition[1] = y; endPosition[2] = z; }
        
        dtPolyRef getPathPolyByPosition(float x, float y, float z);
        bool isPointInPolyBounds(float x, float y, float z, dtPolyRef polyRef);

        dtPolyRef   *pathPolyRefs;      // stores detour polygon references
        int         Length;             // Length 0 == unreachable location - updated automatically by WorldObject::UpdatePath(Path*)
        float       nextPosition[3];    // {x, y, z} of next location on the path - updated automatically by WorldObject::UpdatePath(Path*)
        float       endPosition[3];     // {x, y, z} of the target destination - updated automatically by WorldObject::UpdatePath(Path*)
        WorldObject *targetObject;      // the object we're moving toward, if any
        dtNavMesh   *navMesh;           // the nav mesh used to find the path

    private:
        void updateNextPosition();
};

inline bool areSamePositions(const float* position1, const float* position2)
{
    return (position1[0] == position2[0] && position1[1] == position2[1] && position1[2] == position2[2]) ? true : false;
}

#endif