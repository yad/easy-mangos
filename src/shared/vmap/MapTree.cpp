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

#include "MapTree.h"
#include "VMapManager2.h"
#include "VMapDefinitions.h"

#include <string>
#include <sstream>
#include <iomanip>

using G3D::Vector3;

namespace VMAP
{

    class MapRayCallback
    {
        public:
            MapRayCallback(ModelInstance *val): prims(val) {}
            ModelInstance *prims;
            bool operator()(const G3D::Ray& ray, uint32 entry, float& distance, bool pStopAtFirstHit=true)
            {
                return prims[entry].intersectRay(ray, distance, pStopAtFirstHit);
                //std::cout << "trying to intersect '" << entity->name << "'\n";
            }
    };

    class AreaInfoCallback
    {
        public:
            AreaInfoCallback(ModelInstance *val): prims(val) {}
            void operator()(const Vector3& point, uint32 entry)
            {
#ifdef VMAP_DEBUG
                std::cout << "trying to intersect '" << prims[entry].name << "'\n";
#endif
                prims[entry].intersectPoint(point, aInfo);
            }

            ModelInstance *prims;
            AreaInfo aInfo;
    };


    //=========================================================

    std::string StaticMapTree::getTileFileName(uint32 mapID, uint32 tileX, uint32 tileY)
    {
        std::stringstream tilefilename;
        tilefilename.fill('0');
        tilefilename << std::setw(3) << mapID << "_";
        //tilefilename << std::setw(2) << tileX << "_" << std::setw(2) << tileY << ".vmtile";
        tilefilename << std::setw(2) << tileY << "_" << std::setw(2) << tileX << ".vmtile";
        return tilefilename.str();
    }

    bool StaticMapTree::getAreaInfo(Vector3 &pos, uint32 &flags, int32 &adtId, int32 &rootId, int32 &groupId) const
    {
        AreaInfoCallback intersectionCallBack(iTreeValues);
        iTree.intersectPoint(pos, intersectionCallBack);
        if (intersectionCallBack.aInfo.result)
        {
            flags = intersectionCallBack.aInfo.flags;
            adtId = intersectionCallBack.aInfo.adtId;
            rootId = intersectionCallBack.aInfo.rootId;
            groupId = intersectionCallBack.aInfo.groupId;
            pos.z = intersectionCallBack.aInfo.ground_Z;
            return true;
        }
        return false;
    }

    StaticMapTree::StaticMapTree(uint32 mapID, const std::string &basePath):
        iMapID(mapID), /* iTree(0), */ iTreeValues(0), iBasePath(basePath)
    {
        if(iBasePath.length() > 0 && (iBasePath[iBasePath.length()-1] != '/' || iBasePath[iBasePath.length()-1] != '\\'))
        {
            iBasePath.append("/");
        }
    }

    //=========================================================
    StaticMapTree::~StaticMapTree()
    {
        // TODO: release all acquired model references

        delete[] iTreeValues;
    }

    //=========================================================
    /**
    return dist to hit or inf() if no hit
    */

    float StaticMapTree::getIntersectionTime(const G3D::Ray& pRay, float pMaxDist, bool pStopAtFirstHit) const
    {
        float distance = pMaxDist;
        MapRayCallback intersectionCallBack(iTreeValues);
        iTree.intersectRay(pRay, intersectionCallBack, distance, pStopAtFirstHit);
        return distance;
    }
    //=========================================================

    bool StaticMapTree::isInLineOfSight(const Vector3& pos1, const Vector3& pos2) const
    {
        bool result = true;
        float maxDist = (pos2 - pos1).magnitude();
        // prevent NaN values which can cause BIH intersection to enter infinite loop
        if (maxDist < 1e-10f)
            return true;
        // direction with length of 1
        G3D::Ray ray = G3D::Ray::fromOriginAndDirection(pos1, (pos2 - pos1)/maxDist);
        float resultDist = getIntersectionTime(ray, maxDist, true);
        if(resultDist < maxDist)
        {
            result = false;
        }
        return result;
    }
    //=========================================================
    /**
    When moving from pos1 to pos2 check if we hit an object. Return true and the position if we hit one
    Return the hit pos or the original dest pos
    */

    bool StaticMapTree::getObjectHitPos(const Vector3& pPos1, const Vector3& pPos2, Vector3& pResultHitPos, float pModifyDist) const
    {
        bool result=false;
        float maxDist = (pPos2 - pPos1).magnitude();
        // prevent NaN values which can cause BIH intersection to enter infinite loop
        if (maxDist < 1e-10f)
        {
            pResultHitPos = pPos2;
            return false;
        }
        Vector3 dir = (pPos2 - pPos1)/maxDist;              // direction with length of 1
        G3D::Ray ray(pPos1, dir);
        float dist = getIntersectionTime(ray, maxDist, false);
        if(dist < maxDist)
        {
            pResultHitPos = pPos1 + dir * dist;
            if(pModifyDist < 0)
            {
                if ((pResultHitPos - pPos1).magnitude() > -pModifyDist)
                {
                    pResultHitPos = pResultHitPos + dir*pModifyDist;
                }
                else
                {
                    pResultHitPos = pPos1;
                }
            }
            else
            {
                pResultHitPos = pResultHitPos + dir*pModifyDist;
            }
            result = true;
        }
        else
        {
            pResultHitPos = pPos2;
            result = false;
        }
        return result;
    }

    //=========================================================

    float StaticMapTree::getHeight(const Vector3& pPos) const
    {
        float height = G3D::inf();
        Vector3 dir = Vector3(0,0,-1);
        G3D::Ray ray(pPos, dir);   // direction with length of 1
        float maxDist = VMapDefinitions::getMaxCanFallDistance();
        float dist = getIntersectionTime(ray, maxDist, false);
        if(dist < maxDist)
        {
            height = pPos.z - dist;
        }
        return(height);
    }

    //=========================================================

    bool StaticMapTree::init(const std::string &fname, VMapManager2 *vm)
    {
        std::cout << "Initializing StaticMapTree '" << fname << "'\n";
        bool success=true;
        std::string fullname = iBasePath + fname;
        FILE *rf = fopen(fullname.c_str(), "rb");
        if(!rf)
            return false;
        else
        {
            char chunk[8];
            //general info
            char tiled;
            if (fread(&tiled, sizeof(char), 1, rf) != 1) success = false;
            iIsTiled = (bool(tiled));
            // Nodes
            if (success && !readChunk(rf, chunk, "NODE", 4)) success = false;
            if (success) success = iTree.readFromFile(rf);
            if (success)
            {
                iNTreeValues = iTree.primCount();
                iTreeValues = new ModelInstance[iNTreeValues];
            }

            if (success && !readChunk(rf, chunk, "GOBJ", 4)) success = false;
            // global model spawns
            // only non-tiled maps have them, and if so exactly one (so far at least...)
            ModelSpawn spawn;
#ifdef VMAP_DEBUG
            std::cout << "Map isTiled:" << bool(iIsTiled) << std::endl;
#endif
            if (!iIsTiled && ModelSpawn::readFromFile(rf, spawn))
            {
                WorldModel *model = vm->acquireModelInstance(iBasePath, spawn.name);
                std::cout << "StaticMapTree::init(): loading " << spawn.name << std::endl;
                if (model)
                {
                    // assume that global model always is the first and only tree value (could be improved...)
                    iTreeValues[0] = ModelInstance(spawn, model);
                    iLoadedSpawns[spawn.ID] = 1;
                }
                else
                {
                    success = false;
                    std::cout << "error: could not acquire WorldModel pointer!\n";
                }
            }

            fclose(rf);
        }
        return success;
    }
    //=========================================================

    bool StaticMapTree::loadMap(uint32 tileX, uint32 tileY, VMapManager2 *vm)
    {
        if (!iIsTiled)
            return true;
        if (!iTreeValues)
        {
            std::cout << "Tree has not been initialized!\n";
            return false;
        }
        bool result = true;

        std::string tilefile = iBasePath + getTileFileName(iMapID, tileX, tileY);
        FILE* tf = fopen(tilefile.c_str(), "rb");
        if(tf)
        {
            while(result)
            {
                // read model spawns
                ModelSpawn spawn;
                result = ModelSpawn::readFromFile(tf, spawn);
                if(result)
                {
                    // acquire model instance
                    WorldModel *model = vm->acquireModelInstance(iBasePath, spawn.name);
                    if(!model) std::cout << "error: could not acquire WorldModel pointer!\n";

                    // update tree
                    uint32 nNodeVal=0, referencedVal;
                    fread(&nNodeVal, sizeof(uint32), 1, tf);
#ifdef VMAP_DEBUG
                    if(nNodeVal != 1)
                        std::cout << "unexpected amount of affected NodeVals! (" << nNodeVal << ")\n";
#endif
                    for (uint32 i=0; i<nNodeVal; ++i)
                    {
                        fread(&referencedVal, sizeof(uint32), 1, tf);
                        if (!iLoadedSpawns.count(spawn.ID))
                        {
#ifdef VMAP_DEBUG
                            if (referencedVal > iNTreeValues)
                            {
                                std::cout << "invalid tree element! (" << referencedVal << "/" << iNTreeValues << ")\n";
                                continue;
                            }
#endif
                            iTreeValues[referencedVal] = ModelInstance(spawn, model);
                            iLoadedSpawns[spawn.ID] = 1;
                        }
                        else
                        {
                            ++iLoadedSpawns[spawn.ID];
#ifdef VMAP_DEBUG
                            if (iTreeValues[referencedVal].ID != spawn.ID) std::cout << "error: trying to load wrong spawn in node!\n";
                            else if (iTreeValues[referencedVal].name != spawn.name) std::cout << "error: name collision on GUID="<< spawn.ID << "\n";
#endif
                        }
                    }
                }
            }
            iLoadedTiles[packTileID(tileX, tileY)] = true;
            fclose(tf);
        }
        else
            iLoadedTiles[packTileID(tileX, tileY)] = false;
        return result;
    }

    //=========================================================

    void StaticMapTree::unloadMap(uint32 tileX, uint32 tileY, VMapManager2 *vm)
    {
        if (!iIsTiled)
            return;
        uint32 tileID = packTileID(tileX, tileY);
        loadedTileMap::iterator tile = iLoadedTiles.find(tileID);
        if(tile == iLoadedTiles.end())
        {
            std::cout << "WARNING: trying to unload non-loaded tile. Map:" << iMapID << " X:" << tileX << " Y:" << tileY << std::endl;
            return;
        }
        if(tile->second) // file associated with tile
        {
            std::string tilefile = iBasePath + getTileFileName(iMapID, tileX, tileY);
            FILE* tf = fopen(tilefile.c_str(), "rb");
            if(tf)
            {
                bool result=true;
                while(result)
                {
                    // read model spawns
                    ModelSpawn spawn;
                    result = ModelSpawn::readFromFile(tf, spawn);
                    if(result)
                    {
//                        std::cout << "unloading '" << spawn.name << "'\n";

                        // release model instance
                        vm->releaseModelInstance(spawn.name);

                        // update tree
                        uint32 nNodeVal=0, referencedNode;
                        fread(&nNodeVal, sizeof(uint32), 1, tf);
                        for(uint32 i=0; i<nNodeVal; ++i)
                        {
                            fread(&referencedNode, sizeof(uint32), 1, tf);
                            if(!iLoadedSpawns.count(spawn.ID))
                            {
                                std::cout << "error! trying to unload non-referenced model '" << spawn.name << "' (ID:" << spawn.ID << ")\n";
                            }
                            else if(--iLoadedSpawns[spawn.ID] == 0)
                            {
                                //std::cout << "MapTree: removing '" << spawn.name << "' from tree\n";
                                iTreeValues[referencedNode].setUnloaded();
                                iLoadedSpawns.erase(spawn.ID);
                            }
                        }
                    }
                }
                fclose(tf);
            }
        }
        iLoadedTiles.erase(tile);
    }

}
