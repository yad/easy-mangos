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

#include <G3D/Vector3.h>
#include <G3D/Triangle.h>

#include "TileAssembler.h"
#include "CoordModelMapping.h"
#include "ModelContainer.h"

#include <limits.h>
#include <string.h>
#include <sstream>
#include <iomanip>

#ifdef _ASSEMBLER_DEBUG
FILE *g_df = NULL;
#endif

using G3D::Vector3;
using G3D::AABox;
using G3D::Array;
using G3D::AABSPTree;
using G3D::Triangle;
using G3D::inf;
using std::pair;

namespace VMAP
{

    void getBounds(const ModelSpawn* pSm, G3D::AABox& pAABox)
    {
        pAABox = pSm->iBound;//AABox(pSm->iBound.low()+pSm->iPos, pSm->iBound.high()+pSm->iPos);
    }

    template<class T>
    void countNodesAndElements(typename G3D::AABSPTree<T>::Node& pNode, int &pNNodes, int &pNElements)
    {
        ++pNNodes;
        pNElements += pNode.valueArray.size();

        if(pNode.child[0] != 0)
        {
            countNodesAndElements<T>(*pNode.child[0], pNNodes, pNElements);
        }
        if(pNode.child[1] != 0)
        {
            countNodesAndElements<T>(*pNode.child[1], pNNodes, pNElements);
        }
    }

    /* write G3D::KDTree in contiguous TreeNode array, more suitable for file storage, but static in structure */
    template<class T>
    void serializeTree(const typename AABSPTree<T>::Node& pNode, int &NodePos, int &ElementPos, Vector3& pLo, Vector3& pHi,
                       TreeNode *treeNodes, T *elements)
    {
        TreeNode treeNode = TreeNode(pNode.valueArray.size(), ElementPos);
        treeNode.setSplitAxis(pNode.splitAxis);
        treeNode.setSplitLocation(pNode.splitLocation);

        treeNodes[NodePos++] = treeNode;

        Vector3 lo = Vector3(inf(),inf(),inf());
        Vector3 hi = Vector3(-inf(),-inf(),-inf());

        for(int i=0;i<pNode.valueArray.size(); ++i)
        {
            G3D::_AABSPTree::Handle<T>* h= pNode.valueArray[i];
            lo = lo.min(h->bounds.low());
            hi = hi.max(h->bounds.high());

            elements[ElementPos++] = h->value;
        }

        if(pNode.child[0] != 0)
        {
            treeNode.setChildPos(0, NodePos);
            serializeTree<T>(*pNode.child[0], NodePos, ElementPos, lo, hi, treeNodes, elements);
        }
        if(pNode.child[1] != 0)
        {
            treeNode.setChildPos(1, NodePos);
            serializeTree<T>(*pNode.child[1], NodePos, ElementPos, lo, hi, treeNodes, elements);
        }

        treeNode.setBounds(lo,hi);

        // get absolute bounds
        pLo = pLo.min(lo);
        pHi = pHi.max(hi);
    }

    G3D::Vector3 fixCoords(const G3D::Vector3 &v){ return G3D::Vector3(v.z, v.x, v.y); }

    //=================================================================

    Vector3 ModelPosition::transform(const Vector3& pIn) const
    {
        Vector3 out = pIn * iScale;
        out = iRotation * out;
        return(out);
    }
    //=================================================================

    TileAssembler::TileAssembler(const std::string& pSrcDirName, const std::string& pDestDirName)
    {
        iCurrentUniqueNameId = 0;
        iFilterMethod = NULL;
        iSrcDir = pSrcDirName;
        iDestDir = pDestDirName;
        //mkdir(iDestDir);
        init();
    }

    //=================================================================

    TileAssembler::~TileAssembler()
    {
        delete iCoordModelMapping;
    }

    //=================================================================

    void TileAssembler::init()
    {
        iCoordModelMapping = new CoordModelMapping();
        addWorldAreaMapId(0);                               //Azeroth
        addWorldAreaMapId(1);                               //Kalimdor
        addWorldAreaMapId(530);                             //Expansion01
        addWorldAreaMapId(571);                             //Expansion02
    }
    //=================================================================

    std::string getModNameFromModPosName(const std::string& pModPosName)
    {
        size_t spos = pModPosName.find_first_of('#');
        std::string modelFileName = pModPosName.substr(0,spos);
        return(modelFileName);
    }

    //=================================================================

    unsigned int TileAssembler::getUniqueNameId(const std::string pName)
    {
        unsigned int result;

        if(!iUniqueNameIds.containsKey(pName))
        {
            ++iCurrentUniqueNameId;
            iUniqueNameIds.set(pName, iCurrentUniqueNameId);
        }
        result = iUniqueNameIds.get(pName);
        return result;
    }

    //=================================================================

    std::string TileAssembler::getDirEntryNameFromModName(unsigned int pMapId, const std::string& pModPosName)
    {
        size_t spos;
        char buffer[20];

        std::string modelFileName = getModNameFromModPosName(pModPosName);
        //std::string fext = pModPosName.substr(modelFileName.length(),pModPosName.length());
        unsigned int fextId = getUniqueNameId(pModPosName);
        sprintf(buffer, "_%07d",fextId);
        std::string fext(buffer);
        spos = modelFileName.find_last_of('/');
        std::string fname = modelFileName.substr(spos+1, modelFileName.length());
        spos = fname.find_last_of('.');
        fname = fname.substr(0,spos);
        sprintf(buffer, "%03u", pMapId);
        std::string dirEntry(buffer);
        dirEntry.append("_");
        dirEntry.append(fname);
        dirEntry.append(fext);
        dirEntry.append(".vmap");
        return(dirEntry);
    }

    //=================================================================

    void emptyArray(Array<ModelContainer*>& mc)
    {
        int no=mc.size();
        while(no > 0)
        {
            --no;
            delete mc[no];
            mc.remove(no);
        }
    }

    //=================================================================

    bool TileAssembler::readMapSpawns()
    {
        std::string fname = iSrcDir + "/dir_bin";
        FILE *dirf = fopen(fname.c_str(), "rb");
        if (!dirf)
        {
            printf("Could not read dir_bin file!\n");
            return false;
        }
        printf("Read coordinate mapping...\n");
        uint32 mapID, tileX, tileY, nameLen, check=0;
        char nameBuff[500];
        G3D::Vector3 v1, v2;
        ModelSpawn spawn;
        while (!feof(dirf))
        {
            check = 0;
            // read mapID, tileX, tileY, Flags, ID, Pos, Rot, Scale, Bound_lo, Bound_hi, name
            check += fread(&mapID, sizeof(uint32), 1, dirf);
            if (check == 0) break;
            check += fread(&tileX, sizeof(uint32), 1, dirf);
            check += fread(&tileY, sizeof(uint32), 1, dirf);
            check += fread(&spawn.flags, sizeof(uint32), 1, dirf);
            check += fread(&spawn.ID, sizeof(uint32), 1, dirf);
            // client internal map coordinate system is y-up apparently, shuffle pos to match our representation:
            check += fread(&v1, sizeof(float), 3, dirf);
            spawn.iPos = fixCoords(v1);
            check += fread(&spawn.iRot, sizeof(float), 3, dirf);
            check += fread(&spawn.iScale, sizeof(float), 1, dirf);
            bool isWMO = (spawn.flags & MOD_M2) == 0; // TODO: change to MOD_HAS_BOUND check
            if(isWMO) // only WMOs have bound in MPQ
            {
                check += fread(&v1, sizeof(float), 3, dirf);
                check += fread(&v2, sizeof(float), 3, dirf);
                spawn.iBound = AABox(fixCoords(v1), fixCoords(v2));
            }
            check += fread(&nameLen, sizeof(uint32), 1, dirf);
            if(check != (isWMO ? 19 : 13)) { printf("Error reading dir_bin! (%d != 19)!\n", check); break; }
            if(nameLen>500) { printf("Error, too large file name!\n"); return false; }
            check = fread(nameBuff, sizeof(char), nameLen, dirf);
            if(check != nameLen) { printf("Error reading dir_bin (%d != %d)!\n", check, nameLen); break; }
            spawn.name = std::string(nameBuff, nameLen);
            
            MapSpawns *current;
            std::map<uint32, MapSpawns*>::iterator map_iter = mapData.find(mapID); // hooray for "compact" STL syntax...
            if(map_iter == mapData.end())
            {
                printf("spawning Map %d\n", mapID);
                mapData[mapID] = current = new MapSpawns();
            }
            else current = (*map_iter).second;
            current->UniqueEntries.insert(pair<uint32, ModelSpawn>(spawn.ID, spawn));
            current->TileEntries.insert(pair<uint64, uint32>(makeKey(tileX, tileY), spawn.ID));
        }
        bool success = (ferror(dirf) == 0);
        fclose(dirf);
        return success;
    }

    bool TileAssembler::calculateTransformedBound(ModelSpawn &spawn)
    {
        std::string modelFilename = iSrcDir + "/" + spawn.name;
        ModelPosition modelPosition;
        modelPosition.iDir = spawn.iRot;
        modelPosition.iScale = spawn.iScale;
        modelPosition.init();
        
        FILE *rf = fopen(modelFilename.c_str(), "rb");
        if(!rf)
        {
            printf("ERROR: Can't open model file: %s\n", modelFilename.c_str());
            return false;
        }

        AABox modelBound;
        bool boundEmpty=true;
        char ident[8];

        int readOperation = 1;

        // temporary use defines to simplify read/check code (close file and return at fail)
        #define READ_OR_RETURN(V,S) if(fread((V), (S), 1, rf) != 1) { \
                                        fclose(rf); printf("readfail, op = %i\n", readOperation); return(false); }readOperation++;
        #define CMP_OR_RETURN(V,S)  if(strcmp((V),(S)) != 0)        { \
                                        fclose(rf); printf("cmpfail, %s!=%s\n", V, S);return(false); }

        READ_OR_RETURN(&ident, 8);
        CMP_OR_RETURN(ident, "VMAP003");

        // we have to read one int. This is needed during the export and we have to skip it here
        G3D::uint32 tempNVectors;
        READ_OR_RETURN(&tempNVectors, sizeof(tempNVectors));

        G3D::uint32 groups;
        char blockId[5];
        blockId[4] = 0;
        int blocksize;
        float *vectorarray = 0;

        READ_OR_RETURN(&groups, sizeof(G3D::uint32));
        if(groups != 1) printf("Warning: '%s' does not seem to be a M2 model!\n", modelFilename.c_str());

        for(int g=0;g<(int)groups;g++) // should be only one for M2 files...
        {
            fseek(rf, 3*sizeof(uint32) + 6*sizeof(float), SEEK_CUR);

            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "GRP ");
            READ_OR_RETURN(&blocksize, sizeof(int));
            fseek(rf, blocksize, SEEK_CUR);

            // ---- indexes
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "INDX");
            READ_OR_RETURN(&blocksize, sizeof(int));
            fseek(rf, blocksize, SEEK_CUR);

            // ---- vectors
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "VERT");
            READ_OR_RETURN(&blocksize, sizeof(int));
            unsigned int nvectors;
            READ_OR_RETURN(&nvectors, sizeof(int));

            if(nvectors >0)
            {
                vectorarray = new float[nvectors*sizeof(float)*3];
                READ_OR_RETURN(vectorarray, nvectors*sizeof(float)*3);
            }

            for(unsigned int i=0, indexNo=0; indexNo<nvectors; indexNo++, i+=3)
            {
                Vector3 v = Vector3(vectorarray[i+0], vectorarray[i+1], vectorarray[i+2]);
                v = modelPosition.transform(v);

                if(boundEmpty)
                    modelBound = AABox(v, v), boundEmpty=false;
                else
                    modelBound = AABox( modelBound.low().min(v), modelBound.high().max(v) ); // modelBound.merge(v); in newer G3D
            }
            // drop of temporary use defines
            #undef READ_OR_RETURN
            #undef CMP_OR_RETURN
        }
        spawn.iBound = AABox(modelBound.low() + spawn.iPos, modelBound.high() + spawn.iPos); // = modelBound + spawn.iPos; in newer G3D
        spawn.flags |= MOD_HAS_BOUND;
        fclose(rf);
        return true;
    }

    bool TileAssembler::convertWorld2()
    {
        bool success = readMapSpawns();
        // export objects
        
        // export Map data
        for (MapData::iterator map_iter = mapData.begin(); map_iter != mapData.end(); ++map_iter)
        {
            uint32 __max_z_wmo=0,__max_z_m2=0;
            uint32 __max_bz_wmo=0,__max_bz_m2=0;
            // build global map tree
            AABSPTree<ModelSpawn*> pTree;
            UniqueEntryMap::iterator entry;
            for (entry = map_iter->second->UniqueEntries.begin(); entry != map_iter->second->UniqueEntries.end(); ++entry)
            {
                // M2 models don't have a bound set in WDT/ADT placement data, i still think they're not used for LoS at all on retail
                if(entry->second.flags & MOD_M2)
                {
                    if (!calculateTransformedBound(entry->second))
                        break;
                    Vector3 lo=entry->second.iBound.low(), hi=entry->second.iBound.high();
                    // __debug__
                    if(!__max_z_m2 || entry->second.iPos.z > map_iter->second->UniqueEntries[__max_z_m2].iPos.z)
                        __max_z_m2 = entry->first;
                    if(!__max_bz_m2 || entry->second.iBound.high().z > map_iter->second->UniqueEntries[__max_bz_m2].iBound.high().z)
                        __max_bz_m2 = entry->first;
                }
                // __debug__
                else
                {
                    if(!__max_z_wmo || entry->second.iPos.z > map_iter->second->UniqueEntries[__max_z_wmo].iPos.z)
                        __max_z_wmo = entry->first;
                    if(!__max_bz_wmo || entry->second.iBound.high().z > map_iter->second->UniqueEntries[__max_bz_wmo].iBound.high().z)
                        __max_bz_wmo = entry->first;
                }
                // /__debug__
                pTree.insert(&(entry->second));
            }
            // /__debug__
            ModelSpawn &__wmo = map_iter->second->UniqueEntries[__max_z_wmo];
            ModelSpawn &__m2 = map_iter->second->UniqueEntries[__max_z_m2];
            std::cout << "Highest WMO: " << __wmo.name << " z:" << __wmo.iPos.z <<std::endl;
            std::cout << "Highest M2:  " << __m2.name << " z:" << __m2.iPos.z <<std::endl;
            ModelSpawn &__bwmo = map_iter->second->UniqueEntries[__max_bz_wmo];
            ModelSpawn &__bm2 = map_iter->second->UniqueEntries[__max_bz_m2];
            std::cout << "Highest WMO: " << __bwmo.name << " z:" << __bwmo.iPos.z << " bz:" << __bwmo.iBound.high().z << std::endl;
            std::cout << "Highest M2:  " << __bm2.name << " z:" << __bm2.iPos.z << " bz:" << __bm2.iBound.high().z << std::endl;
            
            pTree.balance(3);
            // ===> possibly move this code to StaticMapTree class
            int nNodes=0, nElements=0;
            countNodesAndElements<ModelSpawn*>(*pTree.root, nNodes, nElements);
            printf("BSPTree: Nodes: %d, Elements: %d (%d)\n", nNodes, nElements, map_iter->second->UniqueEntries.size());
            // write .vmdir files to dynamically load/unload referenced geometry
            TreeNode *mapTree = new TreeNode[nNodes];
            ModelSpawn **treeElements = new ModelSpawn*[nElements];
            Vector3 lo = Vector3(inf(),inf(),inf());
            Vector3 hi = Vector3(-inf(),-inf(),-inf());
            int nodePos=0, elePos=0;
            serializeTree<ModelSpawn*>(*pTree.root, nodePos, elePos, lo, hi, mapTree, treeElements);
            printf("Serialized: %d nodes (expected:%d), %d elements (expected:%d)\n", nodePos, nNodes, elePos, nElements);
            printf("Overall tree bounds: %f, %f, %f | %f, %f, %f\n", lo.x, lo.y, lo.z, hi.x, hi.y, hi.z);
            // create (multi)map model ID => TreeNode indices
            std::multimap<uint32, uint32> modelNodeIdx;
            for(int i=0; i<nNodes; ++i)
                modelNodeIdx.insert(std::pair<uint32, uint32>(treeElements[i]->ID, i));
            printf("min GUID: %u, max GUID: %u\n", modelNodeIdx.begin()->first, modelNodeIdx.rbegin()->first);

            // write map tree file
            std::stringstream mapfilename;
            mapfilename << iDestDir << "/" << std::setfill('0') << std::setw(3) << map_iter->first << ".vmtree";
            FILE *mapfile = fopen(mapfilename.str().c_str(), "wb");
            if(!mapfile) success = false;

            //general info
            char isTiled=1; // TODO use actual info!
            if (fwrite(&isTiled, sizeof(char), 1, mapfile) != 1) success = false;
            // TODO: write world spawns
            // Nodes
            if (success && fwrite("NODE", 4, 1, mapfile) != 1) success = false;
            uint32 size = sizeof(uint32) + sizeof(TreeNode)*nNodes;
            if (success && fwrite(&size, 4, 1, mapfile) != 1) success = false;
            if (success && fwrite(&nNodes, sizeof(uint32), 1, mapfile) != 1) success = false;
            if (success && fwrite(mapTree, sizeof(TreeNode), nNodes, mapfile) != nNodes) success = false;
            // amount of tree elements
            if (success && fwrite(&nElements, sizeof(uint32), 1, mapfile) != 1) success = false;
            
            fclose(mapfile);

            // <====
            
            // write map tile files, similar to ADT files, only with extra BSP tree node info
            TileMap::iterator tile;
            for (tile = map_iter->second->TileEntries.begin(); tile != map_iter->second->TileEntries.end(); ++tile)
            {
                std::stringstream tilefilename;
                tilefilename.fill('0');
                tilefilename << iDestDir << "/" << std::setw(3) << map_iter->first << "_";
                uint32 x, y;
                unpackKey(tile->first, x, y);
                tilefilename << std::setw(2) << x << "_" << std::setw(2) << y << ".vmtile";
                FILE *tilefile = fopen(tilefilename.str().c_str(), "ab");
                success = ModelSpawn::writeToFile(tilefile, map_iter->second->UniqueEntries[tile->second]);
                fclose(tilefile);
            }
            
            
            break; // test...
        }
        return success;
    }

    // move to new file... (WorldModel.cpp?)
    bool ModelSpawn::readFromFile(FILE *rf, ModelSpawn &spawn)
    {
        uint32 check=0, nameLen;
        check += fread(&spawn.flags, sizeof(uint32), 1, rf);
        check += fread(&spawn.ID, sizeof(uint32), 1, rf);
        // client internal map coordinate system is y-up apparently, shuffle pos to match our representation:
        float vposarray[3];
        check += fread(vposarray, sizeof(float), 3, rf);
        spawn.iPos = Vector3(vposarray[2], vposarray[0], vposarray[1]);
        check += fread(&spawn.iRot, sizeof(float), 3, rf);
        check += fread(&spawn.iScale, sizeof(float), 1, rf);
        bool has_bound = (spawn.flags & MOD_HAS_BOUND);
        if(has_bound) // only WMOs have bound in MPQ, only available after computation
        {
            Vector3 bLow, bHigh;
            check += fread(&bLow, sizeof(float), 3, rf);
            check += fread(&bHigh, sizeof(float), 3, rf);
            spawn.iBound = AABox(bLow, bHigh);
        }
        check += fread(&nameLen, sizeof(uint32), 1, rf);
        if(check != (has_bound ? 16 : 10)) { printf("Error reading dir_bin! (%d != 19)!\n", check); return false; }
        char nameBuff[500];
        if(nameLen>500) { printf("Error, too large file name!\n"); return false; }
        check = fread(nameBuff, sizeof(char), nameLen, rf);
        if(check != nameLen) { printf("Error reading dir_bin (%d != %d)!\n", check, nameLen); return false; }
        spawn.name = std::string(nameBuff, nameLen);
        return true;
    }

    bool ModelSpawn::writeToFile(FILE *wf, const ModelSpawn &spawn)
    {
        uint32 check=0;
        check += fwrite(&spawn.flags, sizeof(uint32), 1, wf);
        check += fwrite(&spawn.ID, sizeof(uint32), 1, wf);
        // client internal map coordinate system is y-up apparently, shuffle pos to match our representation:
        Vector3 vposarray(spawn.iPos.y, spawn.iPos.z, spawn.iPos.x);
        check += fwrite(&vposarray, sizeof(float), 3, wf);
        check += fwrite(&spawn.iRot, sizeof(float), 3, wf);
        check += fwrite(&spawn.iScale, sizeof(float), 1, wf);
        bool has_bound = (spawn.flags & MOD_HAS_BOUND);
        if(has_bound) // only WMOs have bound in MPQ, only available after computation
        {
            check += fwrite(&spawn.iBound.low(), sizeof(float), 3, wf);
            check += fwrite(&spawn.iBound.high(), sizeof(float), 3, wf);
        }
        uint32 nameLen = spawn.name.length();
        check += fwrite(&nameLen, sizeof(uint32), 1, wf);
        if(check != (has_bound ? 16 : 10)) return false;
        check = fwrite(spawn.name.c_str(), sizeof(char), nameLen, wf);
        if(check != nameLen) return false;
        return true;
    }

    /* ========================================== /
    /       OLD CODE
    / ========================================== */

    bool TileAssembler::convertWorld()
    {
        #ifdef _ASSEMBLER_DEBUG
        #   ifdef _DEBUG
        ::g_df = fopen("../TileAssembler_debug.txt", "wb");
        #   else
        ::g_df = fopen("../TileAssembler_release.txt", "wb");
        #   endif
        #endif

        std::string fname = iSrcDir;
        fname.append("/");
        fname.append("dir");
        iCoordModelMapping->setModelNameFilterMethod(iFilterMethod);

        printf("Read coordinate mapping...\n");
        if(!iCoordModelMapping->readCoordinateMapping(fname))
            return false;

        Array<unsigned int> mapIds = iCoordModelMapping->getMaps();
        if(mapIds.size() == 0)
        {
            printf("Fatal error: empty map list!\n");
            return false;
        }

        for(int i=0; i<mapIds.size(); ++i)
        {
            unsigned int mapId = mapIds[i];

            #ifdef _ASSEMBLER_DEBUG
            if(mapId == 0)                                  // "Azeroth" just for debug
            {
                for(int x=28; x<29; ++x)                    //debug
                {
                    for(int y=28; y<29; ++y)
                    {
            #else
            // ignore DeeprunTram (369) it is too large for short vector and not important
            // ignore test (13), Test (29) , development (451)
            if(mapId != 369 && mapId != 13 && mapId != 29 && mapId != 451)
            {
                for(int x=0; x<66; ++x)
                {
                    for(int y=0; y<66; ++y)
                    {
            #endif
                        Array<ModelContainer*> mc;
                        std::string dirname;
                        char buffer[100];
                        if(iCoordModelMapping->isWorldAreaMap(mapId) && x<65 && y<65)
                        {
                            sprintf(buffer, "%03u_%d_%d",mapId,y,x); // Let's flip x and y here
                            dirname = std::string(buffer);
                            printf("%s...\n",dirname.c_str());
                        }
                        else
                        {
                            sprintf(buffer, "%03u",mapId);
                            dirname = std::string(buffer);

                            // prevent spam for small maps
                            if(x==0 && y==0)
                                printf("%s...\n",dirname.c_str());
                        }

                        bool result = fillModelContainerArray(dirname, mapId, x, y, mc);
                        emptyArray(mc);

                        if(!result)
                            return false;
                    }
                }
            }
        }
        #ifdef _ASSEMBLER_DEBUG
        if(::g_df) fclose(::g_df);
        #endif

        return true;
    }

    //=================================================================

    bool TileAssembler::fillModelContainerArray(const std::string& pDirFileName, unsigned int pMapId, int pXPos, int pYPos, Array<ModelContainer*>& pMC)
    {
        ModelContainer* modelContainer;

        NameCollection nameCollection = iCoordModelMapping->getFilenamesForCoordinate(pMapId, pXPos, pYPos);
        if(nameCollection.size() ==  0)
            return true;                                    // no data...

        char dirfilename[500];
        sprintf(dirfilename,"%s/%s.vmdir",iDestDir.c_str(),pDirFileName.c_str());
        FILE *dirfile = fopen(dirfilename, "ab");
        if(!dirfile)
        {
            printf("ERROR: Can't create file %s",dirfilename);
            return false;
        }

        char destnamebuffer[500];
        char fullnamedestnamebuffer[500];

        if(nameCollection.iMainFiles.size() >0)
        {
            sprintf(destnamebuffer,"%03u_%i_%i.vmap",pMapId, pYPos, pXPos); // flip it here too
            std::string checkDoubleStr = std::string(dirfilename);
            checkDoubleStr.append("##");
            checkDoubleStr.append(std::string(destnamebuffer));
            // Check, if same file already is in the same dir file
            if(!iCoordModelMapping->isAlreadyProcessedSingleFile(checkDoubleStr))
            {
                iCoordModelMapping->addAlreadyProcessedSingleFile(checkDoubleStr);
                fprintf(dirfile, "%s\n",destnamebuffer);
                sprintf(fullnamedestnamebuffer,"%s/%s",iDestDir.c_str(),destnamebuffer);
                modelContainer = processNames(nameCollection.iMainFiles, fullnamedestnamebuffer);
                if(modelContainer)
                    pMC.append(modelContainer);
                else
                    // TODO: fix this error
                    printf("warning: (if) problems in processing data for %s\n",destnamebuffer);
            }
        }
        // process the large singe files
        int pos = 0;
        while(pos < nameCollection.iSingeFiles.size())
        {
            std::string destFileName = iDestDir;
            destFileName.append("/");
            std::string dirEntryName = getDirEntryNameFromModName(pMapId,nameCollection.iSingeFiles[pos]);
            std::string checkDoubleStr = std::string(dirfilename);
            checkDoubleStr.append("##");
            checkDoubleStr.append(nameCollection.iSingeFiles[pos]);
            // Check, if same file already is in the same dir file
            if(!iCoordModelMapping->isAlreadyProcessedSingleFile(checkDoubleStr))
            {
                iCoordModelMapping->addAlreadyProcessedSingleFile(checkDoubleStr);
                fprintf(dirfile, "%s\n",dirEntryName.c_str());
                destFileName.append(dirEntryName);

                Array<std::string> positionarray;
                positionarray.append(nameCollection.iSingeFiles[pos]);

                if(!iCoordModelMapping->isAlreadyProcessedSingleFile(nameCollection.iSingeFiles[pos]))
                {
                    modelContainer = processNames(positionarray, destFileName.c_str());
                    iCoordModelMapping->addAlreadyProcessedSingleFile(nameCollection.iSingeFiles[pos]);
                    if(modelContainer)
                        pMC.append(modelContainer);
                    else
                        printf("warning: (while) problems in processing data for %s\n",destFileName.c_str());
                }
            }
            ++pos;
        }

        fclose(dirfile);
        return true;
    }

    //=================================================================

    void removeEntriesFromTree(AABSPTree<SubModel *>* pTree)
    {
        Array<SubModel *> submodelArray;
        pTree->getMembers(submodelArray);
        int no = submodelArray.size();
        while(no > 0)
        {
            --no;
            delete submodelArray[no];
        }
    }

    //=================================================================

    ModelContainer* TileAssembler::processNames(const Array<std::string>& pPositions, const char* pDestFileName)
    {
        ModelContainer *modelContainer = 0;

        Vector3 basepos = Vector3(0,0,0);
        AABSPTree<SubModel *>* mainTree = new AABSPTree<SubModel *>();

        int pos = 0;

        bool result = true;
        while(result && (pos < pPositions.size()))
        {
            std::string modelPosString = pPositions[pos];
            std::string modelFileName = getModNameFromModPosName(modelPosString);

            if(!fillModelIntoTree(mainTree, basepos, modelPosString, modelFileName))
            {
                result = false;
                break;
            }
            ++pos;
        }
        if(result && mainTree->size() > 0)
        {
            mainTree->balance();
            modelContainer = new ModelContainer(mainTree);
            modelContainer->writeFile(pDestFileName, tempModelExt);
        }
        removeEntriesFromTree(mainTree);
        for(size_t i=0; i<tempModelExt.size(); ++i)
            delete tempModelExt[i];
        tempModelExt.clear();

        delete mainTree;

        return(modelContainer);
    }

    //=================================================================
    bool TileAssembler::readRawFile(std::string& pModelFilename,  ModelPosition& pModelPosition, AABSPTree<SubModel *> *pMainTree)
    {
        std::string filename = iSrcDir;
        if(filename.length() >0)
            filename.append("/");
        filename.append(pModelFilename);
        FILE *rf = fopen(filename.c_str(), "rb");
        if(!rf)
        {
            // depending on the extractor version, the data could be located in the root dir
            std::string baseModelFilename = pModelFilename.substr((pModelFilename.find_first_of("/")+1),pModelFilename.length());
            filename = iSrcDir;
            if(filename.length() >0)
                filename.append("/");
            filename.append(baseModelFilename);
            rf = fopen(filename.c_str(), "rb");
        }

        if(!rf)
        {
            printf("ERROR: Can't open model file in form: %s",pModelFilename.c_str());
            printf("...                          or form: %s",filename.c_str() );
            return false;
        }

        char ident[8];

        int trianglecount =0;

        #ifdef _ASSEMBLER_DEBUG
        int startgroup = 0;                     //2;
        int endgroup = INT_MAX;                 //2;
        fprintf(::g_df,"-------------------------------------------------\n");
        fprintf(::g_df,"%s\n", pModelFilename.c_str());
        fprintf(::g_df,"-------------------------------------------------\n");
        #else
        int startgroup = 0;
        int endgroup = INT_MAX;
        #endif

        int readOperation = 1;

        // temporary use defines to simplify read/check code (close file and return at fail)
        #define READ_OR_RETURN(V,S) if(fread((V), (S), 1, rf) != 1) { \
                                        fclose(rf); printf("readfail, op = %i\n", readOperation); return(false); }readOperation++;
        #define CMP_OR_RETURN(V,S)  if(strcmp((V),(S)) != 0)        { \
                                        fclose(rf); printf("cmpfail, %s!=%s\n", V, S);return(false); }

        READ_OR_RETURN(&ident, 8);
        CMP_OR_RETURN(ident, "VMAP003");

        // we have to read one int. This is needed during the export and we have to skip it here
        G3D::uint32 tempNVectors;
        READ_OR_RETURN(&tempNVectors, sizeof(tempNVectors));

        G3D::uint32 groups;
        std::vector<GroupModelBound> boundsArray;
        char blockId[5];
        blockId[4] = 0;
        int blocksize;

        READ_OR_RETURN(&groups, sizeof(G3D::uint32));

        for(int g=0;g<(int)groups;g++)
        {
            // group MUST NOT have more then 65536 indexes !! Array will have a problem with that !! (strange ...)
            Array<int> tempIndexArray;
            Array<Vector3> tempVertexArray;

            AABSPTree<Triangle> *gtree = new AABSPTree<Triangle>();

            // add free gtree at fail
            #undef READ_OR_RETURN
            #undef CMP_OR_RETURN
            #define READ_OR_RETURN(V,S) if(fread((V), (S), 1, rf) != 1) { \
                                            fclose(rf);printf("readfail, op = %i\n", readOperation); delete gtree; return(false); }readOperation++;
            #define CMP_OR_RETURN(V,S)  if(strcmp((V),(S)) != 0)        { \
                                            fclose(rf);printf("cmpfail, %s!=%s\n", V, S); delete gtree; return(false); }


            G3D::uint32 mogpflags;
            READ_OR_RETURN(&mogpflags, sizeof(G3D::uint32));
            
            float bbox1[3], bbox2[3];
            READ_OR_RETURN(bbox1, sizeof(float)*3);
            READ_OR_RETURN(bbox2, sizeof(float)*3);

            G3D::uint32 areaID;
            READ_OR_RETURN(&areaID, sizeof(G3D::uint32));

            if(areaID != 0 || mogpflags != 0)
            {
                boundsArray.push_back(GroupModelBound(Vector3(bbox1), Vector3(bbox2), mogpflags, areaID));
                printf("Bbox: %f, %f, %f | %f, %f, %f  areaID=%d flags=%X\n", bbox1[0], bbox1[1], bbox1[2], bbox2[0], bbox2[1], bbox2[2], areaID, mogpflags);
            }

            G3D::uint32 liquidflags;
            READ_OR_RETURN(&liquidflags, sizeof(G3D::uint32));

            G3D::uint32 branches;
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "GRP ");
            READ_OR_RETURN(&blocksize, sizeof(int));
            READ_OR_RETURN(&branches, sizeof(G3D::uint32));
            for(int b=0;b<(int)branches; b++)
            {
                G3D::uint32 indexes;
                // indexes for each branch (not used jet)
                READ_OR_RETURN(&indexes, sizeof(G3D::uint32));
            }

            // ---- indexes
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "INDX");
            READ_OR_RETURN(&blocksize, sizeof(int));
            unsigned int nindexes;
            READ_OR_RETURN(&nindexes, sizeof(G3D::uint32));
            if(nindexes >0)
            {
                unsigned short *indexarray = new unsigned short[nindexes*sizeof(unsigned short)];
                READ_OR_RETURN(indexarray, nindexes*sizeof(unsigned short));
                for(int i=0;i<(int)nindexes; i++)
                {
                    unsigned short val = indexarray[i];
                    tempIndexArray.append(val);
                }
                delete[] indexarray;
            }

            // ---- vectors
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "VERT");
            READ_OR_RETURN(&blocksize, sizeof(int));
            unsigned int nvectors;
            READ_OR_RETURN(&nvectors, sizeof(int));

            float *vectorarray = 0;

            // add vectorarray free
            #undef READ_OR_RETURN
            #undef CMP_OR_RETURN
            #define READ_OR_RETURN(V,S) if(fread((V), (S), 1, rf) != 1) { \
                                            fclose(rf); delete gtree; delete[] vectorarray; return(false); }
            #define CMP_OR_RETURN(V,S)  if(strcmp((V),(S)) != 0)        { \
                                            fclose(rf); delete gtree; delete[] vectorarray; return(false); }

            if(nvectors >0)
            {
                vectorarray = new float[nvectors*sizeof(float)*3];
                READ_OR_RETURN(vectorarray, nvectors*sizeof(float)*3);
            }
            // ----- liquit
            if(liquidflags& 1)
            {
                // we have liquit -> not handled yet ... skip
                READ_OR_RETURN(&blockId, 4);
                CMP_OR_RETURN(blockId, "LIQU");
                READ_OR_RETURN(&blocksize, sizeof(int));
                fseek(rf, blocksize, SEEK_CUR);
            }


            for(unsigned int i=0, indexNo=0; indexNo<nvectors; indexNo++)
            {
                Vector3 v = Vector3(vectorarray[i+0], vectorarray[i+1], vectorarray[i+2]);
                i+=3;
                v = pModelPosition.transform(v);

                tempVertexArray.append(v);
            }

            // ---- calculate triangles
            int rest = nindexes%3;
            if(rest != 0)
            {
                nindexes -= rest;
            }

            for(unsigned int i=0;i<(nindexes);)
            {
                Triangle t = Triangle(tempVertexArray[tempIndexArray[i+2]], tempVertexArray[tempIndexArray[i+1]], tempVertexArray[tempIndexArray[i+0]] );
                i+=3;
                ++trianglecount;
                if(g>= startgroup && g <= endgroup)
                {
                    gtree->insert(t);
                }
            }

            // drop of temporary use defines
            #undef READ_OR_RETURN
            #undef CMP_OR_RETURN

            if(vectorarray != 0)
            {
                delete vectorarray;
            }

            if(gtree->size() >0)
            {
                gtree->balance();
                // gtree contains only triangles - is it possible to define "inside" and "outside" at all?
                // one submodel = one wmo group file
                // TODO: add indoor/outdoor flags and areaid to submodel
                SubModel *sm = new SubModel(gtree, mogpflags, areaID);
                #ifdef _ASSEMBLER_DEBUG
                if(::g_df) fprintf(::g_df,"group trianglies: %d, Tris: %d, Nodes: %d, gtree.triangles: %d\n", g, sm->getNTriangles(), sm->getNNodes(), gtree->memberTable.size());
                if(sm->getNTriangles() !=  gtree->memberTable.size())
                {
                    if(::g_df) fprintf(::g_df,"ERROR !!!! group trianglies: %d, Tris: %d, Nodes: %d, gtree.triangles: %d\n", g, sm->getNTriangles(), sm->getNNodes(), gtree->memberTable.size());
                }
                #endif
                sm->setBasePosition(pModelPosition.iPos);
                pMainTree->insert(sm);
            }
            delete gtree;
        }
        fclose(rf);
        if(!boundsArray.empty())
        {
            tempModelExt.push_back(new WmoModelExt(boundsArray, pModelPosition.iPos, pModelPosition.iDir));
        }
        return true;
    }

    //=================================================================

    bool TileAssembler::fillModelIntoTree(AABSPTree<SubModel *> *pMainTree, const Vector3& pBasePos, std::string& pPos, std::string& pModelFilename)
    {
        ModelPosition modelPosition;
        getModelPosition(pPos, modelPosition);
        // all should be relative to object base position
        modelPosition.moveToBasePos(pBasePos);

        modelPosition.init();

        if(readRawFile(pModelFilename,  modelPosition, pMainTree))
        {
            return true;
        }
        printf("Error while reading rawfile %s\n", pModelFilename.c_str());
        return false;
    }

    //=================================================================
    void TileAssembler::getModelPosition(std::string& pPosString, ModelPosition& pModelPosition)
    {
        float vposarray[3];
        float vdirarray[3];
        float scale;

        size_t spos = pPosString.find_first_of('#');
        std::string stripedPosString = pPosString.substr(spos+1,pPosString.length());

        sscanf(stripedPosString.c_str(), "%f,%f,%f_%f,%f,%f_%f",
            &vposarray[0],&vposarray[1],&vposarray[2],
            &vdirarray[0],&vdirarray[1],&vdirarray[2],
            &scale);

        // internal map coordinate system is y-up apparently, shuffle pos to mangos internal representation:
        pModelPosition.iPos = Vector3(vposarray[2], vposarray[0], vposarray[1]);
        pModelPosition.iDir = Vector3(vdirarray[0], vdirarray[1], vdirarray[2]);
        pModelPosition.iScale = scale;

    }
    //==========================================
}                                                           // VMAP
