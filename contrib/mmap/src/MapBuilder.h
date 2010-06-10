#ifndef _MAP_BUILDER_2_H
#define _MAP_BUILDER_2_H

#include <vector>
#include <set>
#include <map>

#include "TileBuilder.h"
#include "IVMapManager.h"
#include "G3D/Array.h"

#include "ChunkyTriMesh.h"
#include "pathfinding/Recast/Recast.h"

using namespace std;
using namespace VMAP;
// G3D namespace typedefs conflicts with ACE typedefs

namespace MMAP
{
    typedef set<uint32> MapList;
    typedef map<uint32,set<uint32>*> TileList;

    typedef vector<vector<float>*> ModelVerts;
    typedef vector<vector<int>*> ModelTris;

    struct IntermediateValues
    {
        rcHeightfield* heightfield;
        unsigned char* triFlags;
        rcCompactHeightfield* compactHeightfield;
        rcContourSet* contours;
        rcPolyMesh* polyMesh;
        rcPolyMeshDetail* polyMeshDetail;
    };

    class MapBuilder
    {
        public:
            MapBuilder(float maxWalkableAngle   = 60.f,
                       bool skipContinents      = true,
                       bool skipJunkMaps        = true,
                       bool skipBattlegrounds   = true,
                       bool hiResHeightmaps     = false,
                       bool shredHeightmaps     = true,
                       bool debugOutput         = false);

            ~MapBuilder();

            /**
             Builds a mmap for the specifiec map id.
             * First, the entire vmap is loaded. Cannot do it tile-by-tile because vmap tiles only load models
               whose origin is in that tile.  Large models may span across tiles (stormwind, etc)

             * Second, iterates through the tiles and loads their heightmaps.
               These are processed so that steep inclines are removed.
               TODO: process liquid heightmap
               TODO: process heightmap and remove parts that block horizonal entrances

             * Third, the vmap model and heightmap data is aggregated

             * Fourth, data is sent off to recast for processing.  This optionally includes generating
               an obj file, for debugging with RecastDemo
               TODO: benchmark recast with and without steep inclines
               TODO: attempt to optimize rcBuildRegions
            */
            void build(uint32 mapID);

            // builds list of maps, then iterates through them calling build(uint32 mapID)
            void buildAll();

        private:
            /**
             lists .map and .vmap files to identify all maps that need processing for the specified map
            */
            void getMapAndTileList(uint32 mapID);

            /**
             lists .map and .vmap files to identify all maps that need processing for the specified map
            */
            void getMapAndTileLists();

            /**
             loads the entire vmap for the specified map, and applies tranforms to all of the models
             the models are stored in m_modelVertices/m_modelTriangles
            */
            void loadVMap(uint32 mapID);

            /**
             unloads the vmap
            */
            void unloadVMap(uint32 mapID);

            /**
             applies the specified scale, rotation, and translation to all vertices in the model
             results are copied to a new vector so that original data may be reused
            */
            void transform(vector<Vector3> original, vector<Vector3> &transformed, float scale, G3D::Matrix3 rotation, Vector3 position);

            /**
             appends (the data from source) to (dest)
            */
            void copyVertices(vector<Vector3> source, vector<float> &dest);

            /**
             appends (the data from source) to (dest), incrementing each entry by offset, and optionaly 
             inverts the order of each set of indices (M2 models are 'insideout')
            */
            void copyIndices(vector<MeshTriangle> source, vector<int> &dest, int offest, bool flip);
            void buildMoveMap(uint32 mapID);
            void initIntermediateValues(IntermediateValues &iv);
            void clearIntermediateValues(IntermediateValues &iv);

            void generateObjFile(uint32 mapID);
            void writeIV(uint32 mapID, uint32 tileX, uint32 tileY, IntermediateValues iv);
            void writeHeightfield(FILE* file, const rcHeightfield* hf);
            void writeSpan(FILE* file, const rcSpan* span);
            void writeCompactHeightfield(FILE* file, const rcCompactHeightfield* chf);
            void writeChunkyTriMesh(FILE* file, const rcChunkyTriMesh* mesh);
            void writePolyMesh(FILE* file, const rcPolyMesh* mesh);
            void writeDetailMesh(FILE* file, const rcPolyMeshDetail* mesh);

            void cleanup();
            float snapToGrid(const float coord);
            bool shouldSkipMap(uint32 mapID);
            bool isTransportMap(uint32 mapID);

            inline unsigned int nextPow2(unsigned int v)
            {
                v--;
                v |= v >> 1;
                v |= v >> 2;
                v |= v >> 4;
                v |= v >> 8;
                v |= v >> 16;
                v++;
                return v;
            }

            inline unsigned int ilog2(unsigned int v)
            {
                unsigned int r;
                unsigned int shift;
                r = (v > 0xffff) << 4; v >>= r;
                shift = (v > 0xff) << 3; v >>= shift; r |= shift;
                shift = (v > 0xf) << 2; v >>= shift; r |= shift;
                shift = (v > 0x3) << 1; v >>= shift; r |= shift;
                r |= (v >> 1);
                return r;
            }

            IVMapManager* m_vmapManager;
            TileBuilder* m_tileBuilder;

            MapList m_maps;
            TileList m_tiles;

            bool m_debugOutput;
            bool m_completeLists;

            bool m_skipContinents;
            bool m_skipJunkMaps;
            bool m_skipBattlegrounds;

            float m_maxWalkableAngle;

            ModelVerts m_modelVertices;
            ModelTris m_modelTriangles;
            G3D::Array<float> m_vertices;
            G3D::Array<int> m_triangles;
    };
}

#endif