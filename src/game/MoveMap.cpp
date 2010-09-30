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

#include "Map.h"
#include "Log.h"
#include "Utilities/UnorderedMapSet.h"
#include "World.h"

inline uint32 packTileID(uint32 tileX, uint32 tileY) { return tileX<<16 | tileY; }
inline void unpackTileID(uint32 ID, uint32 &tileX, uint32 &tileY) { tileX = ID>>16; tileY = ID&0xFF; }

void Map::LoadNavMesh(int gx, int gy)
{
    char fileName[512];
    FILE* file;

    if(!m_navMesh)
    {
        sprintf(fileName, "%smmaps/%03i.mmap", sWorld.GetDataPath().c_str(), i_id);
        file = fopen(fileName, "rb");

        if(!file)
        {
            sLog.outDebug("Error: Could not open mmap file '%s'", fileName);
            return;
        }

        dtNavMeshParams params;
        uint32 offset;
        fread(&params, sizeof(dtNavMeshParams), 1, file);
        fread(&offset, sizeof(uint32), 1, file);
        fclose(file);

        m_navMesh = dtAllocNavMesh();
        if(!m_navMesh->init(&params))
        {
            delete m_navMesh;
            m_navMesh = 0;
            sLog.outError("Error: Failed to initialize mmap %03u from file %s", i_id, fileName);
            return;
        }
    }

    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if(m_mmapTileMap.find(packedGridPos) != m_mmapTileMap.end())
        return;

    // mmaps/0000000.mmtile
    sprintf(fileName, "%smmaps/%03i%02i%02i.mmtile", sWorld.GetDataPath().c_str(), i_id, gx, gy);
    file = fopen(fileName, "rb");

    if(!file)
    {
        sLog.outDebug("Error: Could not open mmtile file '%s'", fileName);
        return;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data =  (unsigned char*)dtAlloc(length, DT_ALLOC_PERM);
    fread(data, length, 1, file);
    fclose(file);

    dtMeshHeader* header = (dtMeshHeader*)data;
    if (header->magic != DT_NAVMESH_MAGIC)
    {
        sLog.outError("Error: %03u%02i%02i.mmtile has an invalid header", i_id, gx, gy);
        dtFree(data);
        return;
    }
    if (header->version != DT_NAVMESH_VERSION)
    {
        sLog.outError("Error: %03u%02i%02i.mmtile was built with Detour v%i, expected v%i",
                              i_id, gx, gy,                 header->version, DT_NAVMESH_VERSION);
        dtFree(data);
        return;
    }

    if(!m_navMesh->addTile(data, length, DT_TILE_FREE_DATA))
    {
        sLog.outError("Error: could not load %03u%02i%02i.mmtile into navmesh", i_id, gx, gy);
        dtFree(data);
        return;
    }

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed

    uint32 packedTilePos = packTileID(uint32(header->x), uint32(header->y));
    m_mmapTileMap.insert(std::pair<uint32, uint32>(packedGridPos, packedTilePos));
    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i(%u)[%02i,%02i]", i_id, gx, gy, i_id, GetInstanceId(), header->x, header->y);
}

void Map::UnloadNavMesh(int gx, int gy)
{
    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if(m_mmapTileMap.find(packedGridPos) == m_mmapTileMap.end())
        return;

    uint32 packedTilePos = m_mmapTileMap[packedGridPos];
    uint32 tileX, tileY;
    unpackTileID(packedTilePos, tileX, tileY);

    // unload, and mark as non loaded
    m_navMesh->removeTile(m_navMesh->getTileRefAt(int(tileX), int(tileY)), 0, 0);
    m_mmapTileMap.erase(packedGridPos);

    sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i(%u)", i_id, gx, gy, i_id, GetInstanceId());
}

dtNavMesh* Map::GetNavMesh()
{
    return m_navMesh;
}

