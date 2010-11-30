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

#include "MoveMap.h"
#include "GridMap.h"
#include "Log.h"
#include "Utilities/UnorderedMapSet.h"
#include "World.h"

uint32 packTileID(int x, int y) { return uint32(x << 16 | y); }

void TerrainInfo::LoadNavMesh(int gx, int gy)
{
    if (!m_navMesh)
    {
        uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i.mmap")+1;
        char *fileName = new char[pathLen];
        snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i.mmap").c_str(), m_mapId);

        FILE* file = fopen(fileName, "rb");
        if (!file)
        {
            sLog.outDebug("MMAP: Error: Could not open mmap file '%s'", fileName);
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
            sLog.outError("MMAP: Failed to initialize mmap %03u from file %s", m_mapId, fileName);
            return;
        }
    }

    // check if we already have this tile loaded
    uint32 packedGridPos = packTileID(gx, gy);
    if (m_mmapLoadedTiles.find(packedGridPos) != m_mmapLoadedTiles.end())
    {
        sLog.outError("MMAP: Asked to load already loaded navmesh tile. %03u%02i%02i.mmtile", m_mapId, gx, gy);
        return;
    }

    MmapTileReader reader(m_mapId, gx, gy);
    if (!reader.check())
    {
        sLog.outError("MMAP: Bad header in mmap %03u%02i%02i.mmtile", m_mapId, gx, gy);
        return;
    }

    tileRefList* newTiles = new tileRefList();
    unsigned char* data = NULL;
    uint32 length = 0;

    while (reader.read(data, length))
    {
        dtMeshHeader* header = (dtMeshHeader*)data;
        dtTileRef tileRef = 0;

        // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
        dtStatus dtResult = m_navMesh->addTile(data, length, DT_TILE_FREE_DATA, 0, &tileRef);
        if(dtResult == DT_SUCCESS)
             newTiles->push_back(tileRef);
        else
             dtFree(data);
    }

    // have we loaded everything we expected?
    if(reader.getTileCount() == newTiles->size())
    {
        sLog.outDetail("MMAP: Loaded mmtile %03i[%02i,%02i]", m_mapId, gx, gy);
        m_mmapLoadedTiles.insert(std::pair<uint32, tileRefList*>(packedGridPos, newTiles));
    }
    else
    {
        // one or more of the tiles failed
        // we cannot allow having partially loaded maps
        sLog.outError("MMAP: Could not load %03u%02i%02i.mmtile into navmesh", m_mapId, gx, gy);

        // unload everything we can
        for (tileRefList::iterator itr = newTiles->begin(); itr != newTiles->end(); )
        {
            if(DT_SUCCESS == m_navMesh->removeTile(*itr, NULL, NULL))
                itr = newTiles->erase(itr);
            else
                ++itr;
        }

        // if we got here with newTiles not empty its really bad - but again, we cannot recover
        MANGOS_ASSERT(newTiles->empty());
        delete newTiles;
    }
}

void TerrainInfo::UnloadNavMesh(int gx, int gy)
{
    // navMesh was not loaded for this map
    if (!m_navMesh)
        return;

    uint32 packedGridPos = packTileID(gx, gy);
    if (m_mmapLoadedTiles.find(packedGridPos) == m_mmapLoadedTiles.end())
    {
        // file may not exist, therefore not loaded
        sLog.outDebug("MMAP: Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", m_mapId, gx, gy);
        return;
    }

    tileRefList* tiles = m_mmapLoadedTiles[packedGridPos];
    for (tileRefList::iterator itr = tiles->begin(); itr != tiles->end(); )
    {
        // unload, and mark as non loaded
        if(DT_SUCCESS == m_navMesh->removeTile(*itr, NULL, NULL))
            itr = tiles->erase(itr);
        else
            ++itr;
    }

    if (tiles->empty())
        sLog.outDetail("MMAP: Unloaded mmtile %03i[%02i,%02i] from %03i", m_mapId, gx, gy, m_mapId);
    else
    {
        // because the Terrain unloads the grid, this is technically a memory leak
        // if the grid is later loaded, dtNavMesh::addTile will return errors for the dtTileRefs we were unable to unload
        // if we got here, something is really worng - we cannot recover anyway
        sLog.outError("MMAP: Could not unload %u tile(s) from navmesh (%03u%02i%02i.mmtile)", tiles->size(), m_mapId, gx, gy);
        MANGOS_ASSERT(false);
    }

    delete tiles;
    m_mmapLoadedTiles.erase(packedGridPos);
}

dtNavMesh const* TerrainInfo::GetNavMesh() const
{
    return m_navMesh;
}

std::set<uint32> TerrainInfo::s_mmapDisabledIds = std::set<uint32>();

void TerrainInfo::preventPathfindingOnMaps(std::string ignoreMapIds)
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

bool TerrainInfo::IsPathfindingEnabled() const
{
    return sWorld.getConfig(CONFIG_BOOL_MMAP_ENABLED) && s_mmapDisabledIds.find(m_mapId) == s_mmapDisabledIds.end();
}

MmapTileReader::MmapTileReader(uint32 mapId, int32 x, int32 y)
    : m_mmapTileFile(NULL), m_currentTile(0)
{
    // mmaps/0000000.mmtile
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile")+1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i%02i%02i.mmtile").c_str(), mapId, x, y);

    m_mmapTileFile = fopen(fileName, "rb");
    if (!m_mmapTileFile)
        return;

    uint32 length = fread(&m_header, 1, sizeof(mmapTileHeader), m_mmapTileFile);
    if (length != sizeof(mmapTileHeader))
        memset(&m_header, 0, sizeof(mmapTileHeader));

    delete [] fileName;
}

MmapTileReader::~MmapTileReader()
{
    if (m_mmapTileFile)
        fclose(m_mmapTileFile);
}

bool MmapTileReader::check()
{
    // TODO: log error/debug
    if (!m_mmapTileFile)
        return false;

    if (m_header.mmapMagic != MMAP_MAGIC)
        return false;

    if (m_header.dtVersion != DT_NAVMESH_VERSION)
        return false;

    if (m_header.tileCount == 0)
        return false;

    return true;
}

bool MmapTileReader::read(unsigned char* &data, uint32 &dataLength)
{
    if (!m_mmapTileFile || m_header.tileCount <= m_currentTile++)
        return false;

    uint32 length = fread(&dataLength, 1, sizeof(uint32), m_mmapTileFile);
    if (sizeof(uint32) != length)
        return false;

    data = (unsigned char*)dtAlloc(dataLength, DT_ALLOC_PERM);
    MANGOS_ASSERT(data);

    length = fread(data, 1, dataLength, m_mmapTileFile);
    if (length != dataLength)
    {
        dtFree(data);
        return false;
    }

    return true;
}
