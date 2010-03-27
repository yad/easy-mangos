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

//#include <G3D/KDTree.h>
#include "VmapKDTree.h"

#include "WorldModel.h"
#include "TileAssembler.h"

using G3D::Vector3;
using G3D::AABox;
using G3D::Array;
using G3D::Triangle;
using G3D::inf;
using std::pair;

namespace VMAP
{
    bool readChunk(FILE *rf, char *dest, const char *compare, uint32 len)
    {
        if (fread(dest, sizeof(char), len, rf) != len) return false;
        return memcmp(dest, compare, len) == 0;
    }
    //=================================================================
    bool TileAssembler::readRawFile2(const std::string& pModelFilename,  ModelPosition& pModelPosition)
    {
        std::string filename = iSrcDir;
        if(filename.length() >0)
            filename.append("/");
        filename.append(pModelFilename);
        FILE *rf = fopen(filename.c_str(), "rb");

        if(!rf)
        {
            printf("ERROR: Can't open model file in form: %s",pModelFilename.c_str());
            printf("...                          or form: %s",filename.c_str() );
            return false;
        }

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
        uint32 tempNVectors;
        READ_OR_RETURN(&tempNVectors, sizeof(tempNVectors));

        uint32 groups;
        std::vector<GroupModelBound> boundsArray;
        char blockId[5];
        blockId[4] = 0;
        int blocksize;

        READ_OR_RETURN(&groups, sizeof(G3D::uint32));

        uint32 idxOffset=0;
        G3D::Array<SoloTriangle> triangles;
        G3D::Array<Vector3> vertexArray;

        for(int g=0;g<(int)groups;g++)
        {
            // group MUST NOT have more then 65536 indexes !! Array will have a problem with that !! (strange ...)
            Array<int> tempIndexArray;

            //AABSPTree<MeshTriangle> *gtree = new AABSPTree<MeshTriangle>();

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

            uint32 liquidflags;
            READ_OR_RETURN(&liquidflags, sizeof(G3D::uint32));

            uint32 branches;
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
            uint32 nindexes;
            READ_OR_RETURN(&nindexes, sizeof(uint32));
            if(nindexes >0)
            {
                uint16 *indexarray = new uint16[nindexes];
                READ_OR_RETURN(indexarray, nindexes*sizeof(uint16));
                for(uint32 i=0; i<nindexes; i+=3)
                {
                    triangles.append(SoloTriangle(indexarray[i]+idxOffset, indexarray[i+1]+idxOffset, indexarray[i+2]+idxOffset));
                }
                delete[] indexarray;
            }

            // ---- vectors
            READ_OR_RETURN(&blockId, 4);
            CMP_OR_RETURN(blockId, "VERT");
            READ_OR_RETURN(&blocksize, sizeof(int));
            unsigned int nvectors;
            READ_OR_RETURN(&nvectors, sizeof(int));

            if(nvectors >0)
            {
                float *vectorarray = new float[nvectors*3];
                READ_OR_RETURN(vectorarray, nvectors*sizeof(float)*3);
                for(uint32 i=0; i<nvectors; ++i)
                {
                    vertexArray.append( Vector3(vectorarray + 3*i) );
                }
                idxOffset += nvectors;
                delete vectorarray;
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

            // drop of temporary use defines
            #undef READ_OR_RETURN
            #undef CMP_OR_RETURN

        }
        fclose(rf);
        /* if(!boundsArray.empty())
        {
            tempModelExt.push_back(new WmoModelExt(boundsArray, pModelPosition.iPos, pModelPosition.iDir));
        } */
        for(int i=0; i<triangles.size(); ++i)
            triangles[i].setVertexData(vertexArray.getCArray());
        G3D::VmapKDTree<SoloTriangle> gtree;
        gtree.insert(triangles);

        gtree.balance();
        int nNodes, nElements;
        gtree.countNodesAndElements(nNodes, nElements);
        TreeNode *sTree = new TreeNode[nNodes];
        SoloTriangle *sTris = new SoloTriangle[nElements];
        Vector3 lo, hi;
        gtree.serializeTree(lo, hi, sTree, sTris);
        // TODO: add indoor/outdoor flags and areaid to submodel

        // write WorldModel
        WorldModel model(vertexArray.getCArray(), vertexArray.size(), sTris, nElements, sTree, nNodes);
        bool success = model.writeFile(iDestDir + "/" + pModelFilename + ".vmo");

        //std::cout << "readRawFile2: '" << pModelFilename << "' tris: " << nElements << " nodes: " << nNodes << std::endl;
        return success;
    }
}
