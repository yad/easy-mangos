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
            sLog.outError("Failed to initialize mmap %03u from file %s", m_mapId, fileName);
            return;
        }
    }

    // check if we already have this tile loaded
    uint32 packedGridPos = packTileID(gx, gy);
    if (m_mmapLoadedTiles.find(packedGridPos) != m_mmapLoadedTiles.end())
        sLog.outError("Asked to load already loaded navmesh tile. %03u%02i%02i.mmtile", m_mapId, gx, gy);

    // mmaps/0000000.mmtile
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile")+1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i%02i%02i.mmtile").c_str(), m_mapId, gx, gy);

    MmapTileReader reader(fileName);

    if (reader.check())
    {
        std::list<dtTileRef>* newTiles = new std::list<dtTileRef>;
        m_mmapLoadedTiles.insert(std::pair<uint32, std::list<dtTileRef>*>(packedGridPos, newTiles));

        unsigned char* data = NULL;
        uint32 length = 0;

        while (reader.read(data, length))
        {
            dtMeshHeader* header = (dtMeshHeader*)data;
            dtTileRef tileRef = 0;

            // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
            dtStatus dtResult = m_navMesh->addTile(data, length, DT_TILE_FREE_DATA, 0, &tileRef);
            switch(dtResult)
            {
                case DT_SUCCESS:
                {
                    newTiles->push_back(tileRef);
                    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i[%02i,%02i]", m_mapId, gx, gy, m_mapId, header->x, header->y);
                }
                break;
                case DT_FAILURE_DATA_MAGIC:
                {
                    sLog.outError("%03u%02i%02i.mmtile has an invalid tile header", m_mapId, gx, gy);
                    dtFree(data);
                }
                break;
                case DT_FAILURE_DATA_VERSION:
                {
                    sLog.outError("%03u%02i%02i.mmtile was built with Detour v%i, expected v%i",m_mapId, gx, gy, header->version, DT_NAVMESH_VERSION);
                    dtFree(data);
                }
                break;
                case DT_FAILURE_OUT_OF_MEMORY:
                case DT_FAILURE:
                default:
                {
                    sLog.outError("Could not load %03u%02i%02i.mmtile into navmesh", m_mapId, gx, gy);
                    dtFree(data);
                }
                break;
            }
        }
    }

    delete [] fileName;
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
        sLog.outDebug("Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", m_mapId, gx, gy);
        return;
    }

    std::list<dtTileRef>* tiles = m_mmapLoadedTiles[packedGridPos];
    std::list<dtTileRef> removedTiles;
    bool success = true;

    for (std::list<dtTileRef>::iterator it = tiles->begin(); it != tiles->end(); ++it)
    {
        // unload, and mark as non loaded
        if(DT_SUCCESS != m_navMesh->removeTile(*it, NULL, NULL))
            success = false;
        else
            removedTiles.push_back(*it);
    }

    if (success)
    {
        sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i", m_mapId, gx, gy, m_mapId);
        delete tiles;
        m_mmapLoadedTiles.erase(packedGridPos);
    }
    else
    {
        for (std::list<dtTileRef>::iterator it = removedTiles.begin(); it != removedTiles.end(); ++it)
            tiles->remove(*it);

        // because the Terrain unloads the grid, this is technically a memory leak
        // if the grid is later loaded, dtNavMesh::addTile will return errors for the dtTileRefs we were unable to unload
        sLog.outError("Could not unload tile from navmesh (%03u%02i%02i.mmtile)", m_mapId, gx, gy);
    }
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

MmapTileReader::MmapTileReader(char* fileName)
    : m_mmapTileFile(NULL), m_currentTile(0)
{
    m_mmapTileFile = fopen(fileName, "rb");

    if (!m_mmapTileFile)
        return;

    uint32 length;

    length = fread(&m_header, 1, sizeof(mmapTileHeader), m_mmapTileFile);
    if (length != sizeof(mmapTileHeader))
        memset(&m_header, 0, sizeof(mmapTileHeader));
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

    uint32 length;

    length = fread(&dataLength, 1, sizeof(dataLength), m_mmapTileFile);
    if (sizeof(dataLength) != length)
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
