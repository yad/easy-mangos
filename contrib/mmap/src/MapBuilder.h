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

#ifndef _MAP_BUILDER_H
#define _MAP_BUILDER_H

#include <vector>
#include <set>
#include <map>

#include "TerrainBuilder.h"

#include "IVMapManager.h"
#include "WorldModel.h"

#include "Recast.h"
#include "DetourNavMesh.h"

using namespace std;
using namespace VMAP;
// G3D namespace typedefs conflicts with ACE typedefs

namespace MMAP
{
    typedef map<uint32,set<uint32>*> TileList;

    struct IntermediateValues
    {
        rcHeightfield* heightfield;
        rcCompactHeightfield* compactHeightfield;
        rcContourSet* contours;
        rcPolyMesh* polyMesh;
        rcPolyMeshDetail* polyMeshDetail;

        IntermediateValues() :  compactHeightfield(NULL), heightfield(NULL),
                                contours(NULL), polyMesh(NULL), polyMeshDetail(NULL) {}
        ~IntermediateValues()
        {
            rcFreeCompactHeightfield(compactHeightfield);
            rcFreeHeightField(heightfield);
            rcFreeContourSet(contours);
            rcFreePolyMesh(polyMesh);
            rcFreePolyMeshDetail(polyMeshDetail);
        }

        void writeIV(uint32 mapID, uint32 tileX, uint32 tileY)
        {
            char fileName[255];
            char tileString[25];
            sprintf(tileString, "[%02u,%02u]: ", tileX, tileY);

            printf("%sWriting debug output...                       \r", tileString);

            string name("meshes/%03u%02i%02i.");

            #define DEBUG_WRITE(fileExtension,data) \
            do { \
                sprintf(fileName, (name + fileExtension).c_str(), mapID, tileY, tileX); \
                FILE* file = fopen(fileName, "wb"); \
                if (!file) \
                { \
                    char message[1024]; \
                    sprintf(message, "%sFailed to open %s for writing!\n",  tileString, fileName); \
                    perror(message); \
                } \
                else \
                    debugWrite(file, data); \
                if(file) fclose(file); \
                printf("%sWriting debug output...                       \r", tileString); \
            } while (false)

            if(heightfield)
                DEBUG_WRITE("hf", heightfield);
            if(compactHeightfield)
                DEBUG_WRITE("chf", compactHeightfield);
            if(contours)
                DEBUG_WRITE("cs", contours);
            if(polyMesh)
                DEBUG_WRITE("pmesh", polyMesh);
            if(polyMeshDetail)
                DEBUG_WRITE("dmesh", polyMeshDetail);

            #undef DEBUG_WRITE
        }

        void debugWrite(FILE* file, const rcHeightfield* mesh)
        {
            if (!file || !mesh)
                return;

            fwrite(&(mesh->cs), sizeof(float), 1, file);
            fwrite(&(mesh->ch), sizeof(float), 1, file);
            fwrite(&(mesh->width), sizeof(int), 1, file);
            fwrite(&(mesh->height), sizeof(int), 1, file);
            fwrite(mesh->bmin, sizeof(float), 3, file);
            fwrite(mesh->bmax, sizeof(float), 3, file);

            for (int y = 0; y < mesh->height; ++y)
                for (int x = 0; x < mesh->width; ++x)
                {
                    rcSpan* span = mesh->spans[x+y*mesh->width];

                    // first, count the number of spans
                    int spanCount = 0;
                    while (span)
                    {
                        spanCount++;
                        span = span->next;
                    }

                    // write the span count
                    fwrite(&spanCount, sizeof(int), 1, file);

                    // write the spans
                    span = mesh->spans[x+y*mesh->width];
                    while (span)
                    {
                        fwrite(span, sizeof(rcSpan), 1, file);
                        span = span->next;
                    }
                }
        }

        void debugWrite(FILE* file, const rcCompactHeightfield* chf)
        {
            if (!file | !chf)
                return;

            fwrite(&(chf->width), sizeof(chf->width), 1, file);
            fwrite(&(chf->height), sizeof(chf->height), 1, file);
            fwrite(&(chf->spanCount), sizeof(chf->spanCount), 1, file);

            fwrite(&(chf->walkableHeight), sizeof(chf->walkableHeight), 1, file);
            fwrite(&(chf->walkableClimb), sizeof(chf->walkableClimb), 1, file);

            fwrite(&(chf->maxDistance), sizeof(chf->maxDistance), 1, file);
            fwrite(&(chf->maxRegions), sizeof(chf->maxRegions), 1, file);

            fwrite(chf->bmin, sizeof(chf->bmin), 1, file);
            fwrite(chf->bmax, sizeof(chf->bmax), 1, file);

            fwrite(&(chf->cs), sizeof(chf->cs), 1, file);
            fwrite(&(chf->ch), sizeof(chf->ch), 1, file);

            int tmp = 0;
            if (chf->cells) tmp |= 1;
            if (chf->spans) tmp |= 2;
            if (chf->dist) tmp |= 4;
            if (chf->areas) tmp |= 8;

            fwrite(&tmp, sizeof(tmp), 1, file);

            if (chf->cells)
                fwrite(chf->cells, sizeof(rcCompactCell), chf->width*chf->height, file);
            if (chf->spans)
                fwrite(chf->spans, sizeof(rcCompactSpan), chf->spanCount, file);
            if (chf->dist)
                fwrite(chf->dist, sizeof(unsigned short), chf->spanCount, file);
            if (chf->areas)
                fwrite(chf->areas, sizeof(unsigned char), chf->spanCount, file);
        }

        void debugWrite(FILE* file, const rcContourSet* cs)
        {
            if (!file || !cs)
                return;

            fwrite(&(cs->cs), sizeof(float), 1, file);
            fwrite(&(cs->ch), sizeof(float), 1, file);
            fwrite(cs->bmin, sizeof(float), 3, file);
            fwrite(cs->bmax, sizeof(float), 3, file);
            fwrite(&(cs->nconts), sizeof(int), 1, file);
            for (int i = 0; i < cs->nconts; ++i)
            {
                fwrite(&cs->conts[i].area, sizeof(unsigned char), 1, file);
                fwrite(&cs->conts[i].reg, sizeof(unsigned short), 1, file);
                fwrite(&cs->conts[i].nverts, sizeof(int), 1, file);
                fwrite(cs->conts[i].verts, sizeof(int), cs->conts[i].nverts*4, file);
                fwrite(&cs->conts[i].nrverts, sizeof(int), 1, file);
                fwrite(cs->conts[i].rverts, sizeof(int), cs->conts[i].nrverts*4, file);
            }
        }

        void debugWrite(FILE* file, const rcPolyMesh* mesh)
        {
            if (!file || !mesh)
                return;

            fwrite(&(mesh->cs), sizeof(float), 1, file);
            fwrite(&(mesh->ch), sizeof(float), 1, file);
            fwrite(&(mesh->nvp), sizeof(int), 1, file);
            fwrite(mesh->bmin, sizeof(float), 3, file);
            fwrite(mesh->bmax, sizeof(float), 3, file);
            fwrite(&(mesh->nverts), sizeof(int), 1, file);
            fwrite(mesh->verts, sizeof(unsigned short), mesh->nverts*3, file);
            fwrite(&(mesh->npolys), sizeof(int), 1, file);
            fwrite(mesh->polys, sizeof(unsigned short), mesh->npolys*mesh->nvp*2, file);
            fwrite(mesh->flags, sizeof(unsigned short), mesh->npolys, file);
            fwrite(mesh->areas, sizeof(unsigned char), mesh->npolys, file);
            fwrite(mesh->regs, sizeof(unsigned short), mesh->npolys, file);
        }

        void debugWrite(FILE* file, const rcPolyMeshDetail* mesh)
        {
            if (!file || !mesh)
                return;

            fwrite(&(mesh->nverts), sizeof(int), 1, file);
            fwrite(mesh->verts, sizeof(float), mesh->nverts*3, file);
            fwrite(&(mesh->ntris), sizeof(int), 1, file);
            fwrite(mesh->tris, sizeof(char), mesh->ntris*4, file);
            fwrite(&(mesh->nmeshes), sizeof(int), 1, file);
            fwrite(mesh->meshes, sizeof(int), mesh->nmeshes*4, file);
        }

    };

    struct Tile
    {
        Tile() : chf(NULL), solid(NULL), cset(NULL), pmesh(NULL), dmesh(NULL) {}
        ~Tile()
        {
            rcFreeCompactHeightfield(chf);
            rcFreeContourSet(cset);
            rcFreeHeightField(solid);
            rcFreePolyMesh(pmesh);
            rcFreePolyMeshDetail(dmesh);
        }
        rcCompactHeightfield* chf;
        rcHeightfield* solid;
        rcContourSet* cset;
        rcPolyMesh* pmesh;
        rcPolyMeshDetail* dmesh;
    };

    class MapBuilder
    {
        public:
            MapBuilder(float maxWalkableAngle   = 60.f,
                       bool skipLiquid          = false,
                       bool skipContinents      = false,
                       bool skipJunkMaps        = true,
                       bool skipBattlegrounds   = true,
                       bool debugOutput         = false,
                       bool bigBaseUnit         = false);

            ~MapBuilder();

            // builds all mmap tiles for the specified map id (ignores skip settings)
            void build(uint32 mapID);

            // builds an mmap tile for the specified map tile (ignores skip settings)
            void buildTile(uint32 mapID, uint32 tileX, uint32 tileY);

            // builds list of maps, then builds all of mmap tiles (based on the skip settings)
            void buildAll();

        private:
            // detect maps and tiles
            void discoverTiles();
            set<uint32>* getTileList(uint32 mapID);

            // load and unload models
            bool loadVMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData);
            void unloadVMap(uint32 mapID, uint32 tileX, uint32 tileY);

            // vert and triangle methods
            void transform(vector<Vector3> original, vector<Vector3> &transformed,
                           float scale, G3D::Matrix3 rotation, Vector3 position);
            void copyVertices(vector<Vector3> source, G3D::Array<float> &dest);
            void copyIndices(vector<MeshTriangle> source, G3D::Array<int> &dest, int offest, bool flip);
            void copyIndices(G3D::Array<int> &dest, G3D::Array<int> src, int offset);
            void cleanVertices(G3D::Array<float> &verts, G3D::Array<int> &tris);

            // move map building
            void buildMoveMap(uint32 mapID);
            void buildNavMesh(uint32 mapID, dtNavMesh* &navMesh);
            void buildMoveMapTile(uint32 mapID,
                                  uint32 tileX,
                                  uint32 tileY,
                                  MeshData meshData,
                                  float bmin[3],
                                  float bmax[3],
                                  dtNavMesh* navMesh);

            void getTileBounds(uint32 tileX, uint32 tileY,
                               float* verts, int vertCount,
                               float* bmin, float* bmax);

            bool shouldSkipMap(uint32 mapID);
            bool isTransportMap(uint32 mapID);
            bool shouldSkipTile(uint32 mapID, uint32 tileX, uint32 tileY);

            // debug output
            void generateObjFile(uint32 mapID, uint32 tileX, uint32 tileY, MeshData meshData);
            void generateRealObj(uint32 mapID, uint32 tileX, uint32 tileY, MeshData meshData);

            IVMapManager* m_vmapManager;
            TerrainBuilder* m_terrainBuilder;

            TileList m_tiles;

            bool m_debugOutput;

            bool m_skipContinents;
            bool m_skipJunkMaps;
            bool m_skipBattlegrounds;

            float m_maxWalkableAngle;
            bool m_bigBaseUnit;

            // build performance - not really used for now
            rcContext* m_rcContext;
    };
}

#endif
