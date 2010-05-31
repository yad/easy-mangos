
#ifndef _MMAP_DEBUG_H
#define _MMAP_DEBUG_H

#include <math.h>
#include "pathfinding/Detour/DetourNavMesh.h"
#include "pathfinding/Recast/Recast.h"

void duReadNavMesh(int mapID, dtNavMesh* &navMesh);
void duReadPolyMesh(int mapID, rcPolyMesh* &mesh);
void duReadDetailMesh(int mapID, rcPolyMeshDetail* &mesh);

#endif
