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

#ifndef _MOVE_MAP_H
#define _MOVE_MAP_H

#include "Platform/Define.h"
#include "../../dep/recastnavigation/Detour/Include/DetourAlloc.h"
#include "../../dep/recastnavigation/Detour/Include/DetourNavMesh.h"

/*  memory management  */

inline void* dtCustomAlloc(int size, dtAllocHint hint)
{
    return (void*)new unsigned char[size];
}

inline void dtCustomFree(void* ptr)
{
    delete [] ptr;
}

/*  mmap tile structure  */

#define MMAP_MAGIC 0x4d4d4150   // 'MMAP'
#define MMAP_VERSION 1

struct mmapTileHeader
{
    uint32 mmapMagic;
    uint32 dtVersion;
    uint32 mmapVersion;
    uint32 tileCount;
    bool usesHiRes : 1;
    bool usesLiquids : 1;
};

class MmapTileReader
{
public:
    MmapTileReader(char* fileName);
    ~MmapTileReader();

    bool check();
    bool read(unsigned char* &data, uint32 &dataLength);

private:
    FILE* m_mmapTileFile;
    mmapTileHeader m_header;
    uint32 m_currentTile;
};

#endif  // _MOVE_MAP_H
