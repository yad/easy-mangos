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

uint32 packTileID(uint32 tileX, uint32 tileY) { return tileX<<16 | tileY; }
void unpackTileID(uint32 ID, uint32 &tileX, uint32 &tileY) { tileX = ID>>16; tileY = ID&0xFF; }

void Map::LoadNavMesh(int gx, int gy)
{
    if (!m_navMesh)
    {
        uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i.mmap")+1;
        char *fileName = new char[pathLen];
        snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i.mmap").c_str(), i_id);

        FILE* file = fopen(fileName, "rb");
        if (!file)
        {
            sLog.outDebug("Error: Could not open mmap file '%s'", fileName);
            delete [] fileName;
            return;
        }

        dtNavMeshParams params;
        fread(&params, sizeof(dtNavMeshParams), 1, file);
        fclose(file);
        delete [] fileName;

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh->init(&params))
        {
            dtFreeNavMesh(m_navMesh);
            m_navMesh = NULL;
            sLog.outError("Failed to initialize mmap %03u from file %s", i_id, fileName);
            return;
        }
    }

    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if (m_mmapLoadedTiles.find(packedGridPos) != m_mmapLoadedTiles.end())
        return;

    // mmaps/0000000.mmtile
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile")+1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i%02i%02i.mmtile").c_str(), i_id, gx, gy);

    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        sLog.outDebug("Could not open mmtile file '%s'", fileName);
        delete [] fileName;
        return;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data =  (unsigned char*)dtAlloc(length, DT_ALLOC_PERM);
    MANGOS_ASSERT(data);

    fread(data, length, 1, file);
    fclose(file);
    delete [] fileName;

    dtMeshHeader* header = (dtMeshHeader*)data;
    if (header->magic != DT_NAVMESH_MAGIC)
    {
        sLog.outError("%03u%02i%02i.mmtile has an invalid header", i_id, gx, gy);
        dtFree(data);
        return;
    }
    if (header->version != DT_NAVMESH_VERSION)
    {
        sLog.outError("%03u%02i%02i.mmtile was built with Detour v%i, expected v%i",i_id, gx, gy, header->version, DT_NAVMESH_VERSION);
        dtFree(data);
        return;
    }

    if (DT_SUCCESS != m_navMesh->addTile(data, length, DT_TILE_FREE_DATA, 0, NULL))
    {
        sLog.outError("Could not load %03u%02i%02i.mmtile into navmesh", i_id, gx, gy);
        dtFree(data);
        return;
    }

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed

    uint32 packedTilePos = packTileID(uint32(header->x), uint32(header->y));
    m_mmapLoadedTiles.insert(std::pair<uint32, uint32>(packedGridPos, packedTilePos));
    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i(%u)[%02i,%02i]", i_id, gx, gy, i_id, GetInstanceId(), header->x, header->y);
}

void Map::UnloadNavMesh(int gx, int gy)
{
    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if (m_mmapLoadedTiles.find(packedGridPos) == m_mmapLoadedTiles.end())
        return;

    uint32 packedTilePos = m_mmapLoadedTiles[packedGridPos];
    uint32 tileX, tileY;
    unpackTileID(packedTilePos, tileX, tileY);

    // unload, and mark as non loaded
    if(DT_SUCCESS != m_navMesh->removeTile(m_navMesh->getTileRefAt(int(tileX), int(tileY)), NULL, NULL))
    {
        sLog.outError("Could not unload %03u%02i%02i.mmtile from navmesh", i_id, gx, gy);
    }
    else
    {
        m_mmapLoadedTiles.erase(packedGridPos);
        sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i(%u)", i_id, gx, gy, i_id, GetInstanceId());
    }
}

dtNavMesh* Map::GetNavMesh()
{
    return m_navMesh;
}

std::set<uint32> Map::s_mmapDisabledIds = std::set<uint32>();

void Map::preventPathfindingOnMaps(std::string ignoreMapIds)
{
    s_mmapDisabledIds.clear();

    char* mapList = new char[ignoreMapIds.length()+1];
    strcpy(mapList, ignoreMapIds.c_str());

    char* idstr = strtok(mapList, ",");
    while (idstr)
    {
        s_mmapDisabledIds.insert(uint32(atoi(idstr)));
        idstr = strtok(NULL, ",");
    }

    delete[] mapList;
}

bool Map::IsPathfindingEnabled() const
{
    return sWorld.getConfig(CONFIG_BOOL_MMAP_ENABLED) && s_mmapDisabledIds.find(i_id) == s_mmapDisabledIds.end();
}
