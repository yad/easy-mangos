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

#ifndef _MAPTREE_H
#define _MAPTREE_H

#include <map>
#include "Platform/Define.h"
#include "Utilities/UnorderedMap.h"
#include "AABSPTree.h"

namespace VMAP
{
    class TreeNode;
    class ModelInstance;
    class VMapManager2;

    class StaticMapTree
    {
        typedef UNORDERED_MAP<uint32, bool> loadedTileMap;
        typedef UNORDERED_MAP<uint32, uint32> loadedSpawnMap;
        private:
            uint32 iMapID;
            TreeNode *iTree; // the KD-tree
            uint32 iNNodes;
            ModelInstance *iTreeValues; // the KD-tree entries
            uint32 iNTreeValues;
            // add UniqueEntryMap define?
            //std::map<uint32, ModelSpawn> mapSpawns;
            //uint32 iNLoadedTiles;

            // Key: filename, value ModelContainer
            // => VMapManager
            // G3D::Table<std::string, ManagedModelContainer *> iLoadedModelContainer;

            // Key: dir file name, value FilesInDir
            // => VMapManager
            // G3D::Table<std::string, FilesInDir> iLoadedDirFiles;

            // Store all the map tile idents that are loaded for that map
            // some maps are not splitted into tiles and we have to make sure, not removing the map before all tiles are removed
            // empty tiles have no tile file, hence map with bool instead of just a set (consistency check)
            loadedTileMap iLoadedTiles;
            // not strictly needed, stores model-ID reference count to invalidate tree nodes and be able to report errors
            loadedSpawnMap iLoadedSpawns;
            std::string iBasePath;

        private:
            float getIntersectionTime(const G3D::Ray& pRay, float pMaxDist, bool pStopAtFirstHit);
            // => VMapManager
            //bool isAlreadyLoaded(const std::string& pName) const { return(iLoadedModelContainer.containsKey(pName)); }
            //void setLoadedMapTile(unsigned int pTileIdent) { iLoadedMapTiles.set(pTileIdent, true); }
            //void removeLoadedMapTile(unsigned int pTileIdent) { iLoadedMapTiles.remove(pTileIdent); }
            // bool hasLoadedMapTiles() const { return iLoadedTiles.size() > 0; }
            //bool containsLoadedMapTile(unsigned int pTileIdent) const { return(iLoadedMapTiles.containsKey(pTileIdent)); }
        public:
            static std::string getTileFileName(uint32 mapID, uint32 tileX, uint32 tileY);
            static uint32 packTileID(uint32 tileX, uint32 tileY) { return tileX<<16 | tileY; }
            static void unpackTileID(uint32 ID, uint32 &tileX, uint32 &tileY) { tileX = ID>>16; tileY = ID&0xFF; }
            // => VMapManager
            // ManagedModelContainer *getModelContainer(const std::string& pName) { return(iLoadedModelContainer.get(pName)); }
            StaticMapTree(uint32 mapID, const std::string &basePath);
            ~StaticMapTree();

            bool isInLineOfSight(const G3D::Vector3& pos1, const G3D::Vector3& pos2);
            bool getObjectHitPos(const G3D::Vector3& pos1, const G3D::Vector3& pos2, G3D::Vector3& pResultHitPos, float pModifyDist);
            float getHeight(const G3D::Vector3& pPos);
            bool getAreaInfo(G3D::Vector3 pos, uint32 &areaID, uint32 &flags);

            //bool PrepareTree();
            bool init(const std::string &fname);
            bool loadMap(uint32 tileX, uint32 tileY, VMapManager2 *vm);
            void unloadMap(uint32 tileX, uint32 tileY, VMapManager2 *vm);
            uint32 numLoadedTiles() { return iLoadedTiles.size(); }
            // void addModelContainer(const std::string& pName, ManagedModelContainer *pMc);
    };

}                                                           // VMAP

#endif // _MAPTREE_H
