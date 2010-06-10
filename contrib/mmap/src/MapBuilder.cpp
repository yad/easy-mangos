
#include <set>
#include "MMapCommon.h"
#include "MapBuilder.h"
#include "VMapManager2.h"
#include "MapTree.h"
#include "ModelInstance.h"

#include "ChunkyTriMesh.h"
#include "pathfinding/Recast/Recast.h"
#include "pathfinding/Detour/DetourNavMeshBuilder.h"
#include "pathfinding/Detour/DetourNavMesh.h"

using namespace std;

namespace MMAP
{
    MapBuilder::MapBuilder(float maxWalkableAngle,
                           bool skipContinents, bool skipJunkMaps, bool skipBattlegrounds,
                           bool hiResHeightmaps, bool shredHeightmaps,
                           bool debugOutput) :
                           m_maxWalkableAngle  (maxWalkableAngle),
                           m_skipContinents    (skipContinents),
                           m_skipJunkMaps      (skipJunkMaps),
                           m_skipBattlegrounds (skipBattlegrounds),
                           m_debugOutput       (debugOutput),
                           m_completeLists     (false)
    {
        m_vmapManager = new VMapManager2();
        m_tileBuilder = new TileBuilder(m_maxWalkableAngle, hiResHeightmaps, shredHeightmaps, m_vmapManager);
    }

    MapBuilder::~MapBuilder()
    {
        m_maps.clear();

        for(TileList::iterator it = m_tiles.begin(); it != m_tiles.end(); ++it)
        {
            (*it).second->clear();
            delete (*it).second;
        }
        m_tiles.clear();

        delete m_tileBuilder;
        delete m_vmapManager;

        cleanup();
    }

    void MapBuilder::getMapAndTileList(uint32 mapID)
    {
        uint32 i;
        set<uint32>* tiles;
        vector<string> files;
        uint32 tileX, tileY, tileID;
        char filter[12];

        sprintf(filter, "%03u.vmtree", mapID);
        getDirContents(files, "vmaps", filter);
        for(i = 0; i < files.size(); ++i)
        {
            m_maps.insert(mapID);
        }

        files.clear();
        sprintf(filter, "%03u*.vmtile", mapID);
        getDirContents(files, "vmaps", filter);
        for(i = 0; i < files.size(); ++i)
        {
            tileX = uint32(atoi(files[i].substr(7,2).c_str()));
            tileY = uint32(atoi(files[i].substr(4,2).c_str()));
            tileID = StaticMapTree::packTileID(tileX, tileY);

            if(m_tiles.find(mapID) == m_tiles.end())
            {
                tiles = new set<uint32>;
                m_tiles.insert(pair<uint32,set<uint32>*>(mapID, tiles));
            }
            else
                tiles = m_tiles[mapID];

            tiles->insert(tileID);
        }

        sprintf(filter, "%03u*", mapID);
        files.clear();
        getDirContents(files, "maps");
        for(i = 0; i < files.size(); ++i)
        {
            mapID = uint32(atoi(files[i].substr(0,3).c_str()));
            tileX = uint32(atoi(files[i].substr(5,2).c_str()));
            tileY = uint32(atoi(files[i].substr(3,2).c_str()));
            tileID = StaticMapTree::packTileID(tileX, tileY);
            m_maps.insert(mapID);

            if(m_tiles.find(mapID) == m_tiles.end())
            {
                tiles = new set<uint32>;
                m_tiles.insert(pair<uint32,set<uint32>*>(mapID, tiles));
            }
            else
                tiles = m_tiles[mapID];

            tiles->insert(tileID);
        }
    }

    void MapBuilder::getMapAndTileLists()
    {
        uint32 i;
        set<uint32>* tiles;
        vector<string> files;
        uint32 mapID, tileX, tileY, tileID;

        getDirContents(files, "vmaps", "*.vmtree");
        for(i = 0; i < files.size(); ++i)
        {
            mapID = uint32(atoi(files[i].substr(0,3).c_str()));
            m_maps.insert(mapID);
        }

        files.clear();
        getDirContents(files, "vmaps", "*.vmtile");
        for(i = 0; i < files.size(); ++i)
        {
            mapID = uint32(atoi(files[i].substr(0,3).c_str()));
            tileX = uint32(atoi(files[i].substr(7,2).c_str()));
            tileY = uint32(atoi(files[i].substr(4,2).c_str()));
            tileID = StaticMapTree::packTileID(tileX, tileY);

            if(m_tiles.find(mapID) == m_tiles.end())
            {
                tiles = new set<uint32>;
                m_tiles.insert(pair<uint32,set<uint32>*>(mapID, tiles));
            }
            else
                tiles = m_tiles[mapID];

            tiles->insert(tileID);
        }

        files.clear();
        getDirContents(files, "maps");
        for(i = 0; i < files.size(); ++i)
        {
            mapID = uint32(atoi(files[i].substr(0,3).c_str()));
            tileX = uint32(atoi(files[i].substr(5,2).c_str()));
            tileY = uint32(atoi(files[i].substr(3,2).c_str()));
            tileID = StaticMapTree::packTileID(tileX, tileY);
            m_maps.insert(mapID);

            if(m_tiles.find(mapID) == m_tiles.end())
            {
                tiles = new set<uint32>;
                m_tiles.insert(pair<uint32,set<uint32>*>(mapID, tiles));
            }
            else
                tiles = m_tiles[mapID];

            tiles->insert(tileID);
        }

        m_completeLists = true;
    }

    void MapBuilder::buildAll()
    {
        getMapAndTileLists();

        for(MapList::iterator it = m_maps.begin(); it != m_maps.end(); ++it)
            if(!shouldSkipMap((*it)))
                build((*it));
    }

    void MapBuilder::build(uint32 mapID)
    {
        printf("Building map %03u:\n", mapID);

        if(!m_completeLists)
            getMapAndTileList(mapID);

        set<uint32> tiles;
        if(m_tiles.find(mapID) != m_tiles.end())
            tiles = *m_tiles[mapID];

        loadVMap(mapID);

        float* verts;
        int* tris;
        int count;
        uint32 i, tileX, tileY, vertCount = 0, triCount = 0;
        for(set<uint32>::iterator it = tiles.begin(); it != tiles.end(); ++it)
        {
            StaticMapTree::unpackTileID((*it), tileX, tileY);

            m_tileBuilder->build(mapID, tileX, tileY, verts, vertCount, tris, triCount);

            count = m_vertices.size() / 3;

            for(i = 0; i < vertCount; ++i)
                m_vertices.append(verts[i*3], verts[i*3+1], verts[i*3+2]);

            for(i = 0; i < triCount; ++i)
                m_triangles.append(tris[i*3]+count, tris[i*3+1]+count, tris[i*3+2]+count);
        }

        uint32 j;
        for(i = 0; i < m_modelVertices.size(); ++i)
        {
            vector<float> verts = *m_modelVertices[i];
            vector<int> tris = *m_modelTriangles[i];

            count = m_vertices.size() / 3;

            for(j = 0; j < verts.size(); j++)
                m_vertices.append(verts[j]);

            for(j = 0; j < tris.size(); j++)
                m_triangles.append(tris[j]+count);
        }

        generateObjFile(mapID);
        buildMoveMap(mapID);
        cleanup();

        printf("Complete!                               \n\n", mapID);
    }

    void MapBuilder::loadVMap(uint32 mapID)
    {
        printf("Loading vmap...                         \r");

        // any loadMap call initializes the StaticMapTree, which loads
        // models stored in the WDT. Necessary for maps that don't have
        // a heightmap or ADTs
        cout.setstate(cout.badbit);
        m_vmapManager->loadMap("vmaps", mapID, 64, 64);
        cout.clear(cout.goodbit);

        set<uint32> tiles;
        if(m_tiles.find(mapID) != m_tiles.end())
            tiles = *m_tiles[mapID];

        uint32 tileX, tileY;
        for(set<uint32>::iterator it = tiles.begin(); it != tiles.end(); ++it)
        {
            StaticMapTree::unpackTileID((*it), tileX, tileY);

            cout.setstate(cout.badbit);
            int result = m_vmapManager->loadMap("vmaps", mapID, tileX, tileY);
            cout.clear(cout.goodbit);
        }

        ModelInstance* models = 0;
        uint32 count = 0;

        InstanceTreeMap instanceTrees;
        ((VMapManager2*)m_vmapManager)->getInstanceMapTree(instanceTrees);

        if(!instanceTrees[mapID])
            return;

        instanceTrees[mapID]->getModelInstances(models, count);

        if(!models || !count)
            return;

        uint32 i;
        for(i = 0; i < count; ++i)
        {
            ModelInstance instance = models[i];

            vector<float>* modelVertices = new vector<float>;
            vector<int>* modelTriangles = new vector<int>;

            // model instances exist in tree even though there are instances of that model in this tile
            WorldModel* worldModel = instance.getWorldModel();
            if(!worldModel)
                continue;

            vector<GroupModel> groupModels;
            worldModel->getGroupModels(groupModels);

            // all M2s need to have triangle indices reversed
            bool isM2 = instance.name.find(".m2") != instance.name.npos || instance.name.find(".M2") != instance.name.npos;

            // transform data
            float scale = models[i].iScale;
            G3D::Matrix3 rotation = G3D::Matrix3::fromEulerAnglesZYX(-1*G3D::pi()*instance.iRot.y/180.f, -1*G3D::pi()*instance.iRot.x/180.f, -1*G3D::pi()*instance.iRot.z/180.f);
            Vector3 position = instance.iPos;
            position.x -= 32*533.33333f;
            position.y -= 32*533.33333f;

            //float heightOffset = TileBuilder::readHeightOffset(mapID, uint32(ceil(32-position.x/GRID_SIZE)), uint32(ceil(32-position.y/GRID_SIZE)));
            //position.z += heightOffset*2.f;

            for(vector<GroupModel>::iterator it = groupModels.begin(); it != groupModels.end(); ++it)
            {
                vector<Vector3> tempVertices;
                vector<Vector3> transformedVertices;
                vector<MeshTriangle> tempTriangles;

                (*it).getMeshData(tempVertices, tempTriangles);

                transform(tempVertices, transformedVertices, scale, rotation, position);

                int offset = modelVertices->size() / 3;

                copyVertices(transformedVertices, (*modelVertices));
                copyIndices(tempTriangles, (*modelTriangles), offset, isM2);
            }

            m_modelVertices.push_back(modelVertices);
            m_modelTriangles.push_back(modelTriangles);
        }

        unloadVMap(mapID);
    }

    void MapBuilder::unloadVMap(uint32 mapID)
    {
        printf("Unloading vmap...                       \r");

        set<uint32> tiles;
        if(m_tiles.find(mapID) != m_tiles.end())
            tiles = *m_tiles[mapID];

        uint32 tileX, tileY;
        for(set<uint32>::iterator it = tiles.begin(); it != tiles.end(); ++it)
        {
            StaticMapTree::unpackTileID((*it), tileX, tileY);

            cout.setstate(cout.badbit);
            m_vmapManager->unloadMap(mapID, tileX, tileY);
            cout.clear(cout.goodbit);
        }
        
        cout.setstate(cout.badbit);
        m_vmapManager->unloadMap(mapID);
        cout.clear(cout.goodbit);
    }

    inline void MapBuilder::transform(vector<Vector3> source, vector<Vector3> &transformedVertices, float scale, G3D::Matrix3 rotation, Vector3 position)
    {
        for(vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
        {
            // apply tranform, then mirror along the horizontal axes
            Vector3 v((*it) * rotation * scale + position);
            v.x *= -1.f;
            v.y *= -1.f;
            transformedVertices.push_back(v);
        }
    }

    inline void MapBuilder::copyVertices(vector<Vector3> source, vector<float> &dest)
    {
        for(vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
        {
            dest.push_back((*it).y);
            dest.push_back((*it).z);
            dest.push_back((*it).x);
        }
    }

    inline void MapBuilder::copyIndices(vector<MeshTriangle> source, vector<int> &dest, int offset, bool flip)
    {
        // evaluate flip once, makes faster code (but larger)
        if(flip)
        {
            for(vector<MeshTriangle>::iterator it = source.begin(); it != source.end(); ++it)
            {
                dest.push_back((*it).idx2+offset);
                dest.push_back((*it).idx1+offset);
                dest.push_back((*it).idx0+offset);
            }
        }
        else
        {
            for(vector<MeshTriangle>::iterator it = source.begin(); it != source.end(); ++it)
            {
                dest.push_back((*it).idx0+offset);
                dest.push_back((*it).idx1+offset);
                dest.push_back((*it).idx2+offset);
            }
        }
    }

    void MapBuilder::buildMoveMap(uint32 mapID)
    {
        float cellSize = .55f;      // larger number => less voxels => faster build time
        float agentHeight = 2.2f;
        float agentRadius = .55f;
        float agentMaxClimb = 1.65f;

        IntermediateValues iv;
        initIntermediateValues(iv);

        char fileName[25];
        FILE* file = 0;

        float bmin[3] = {0.f, 0.f, 0.f};
        float bmax[3] = {0.f, 0.f, 0.f};
        float* verts = m_vertices.getCArray();
        int* tris = m_triangles.getCArray();
        int vertCount = m_vertices.size() / 3;
        int triCount = m_triangles.size() / 3;

        rcCalcBounds(verts, vertCount, bmin, bmax);
        float yLen = abs(snapToGrid(bmax[0] + GRID_SIZE - 1) - snapToGrid(bmin[0]));
        float xLen = abs(snapToGrid(bmax[2] + GRID_SIZE - 1) - snapToGrid(bmin[2]));

        // set common config
        rcConfig config;
        memset(&config, 0, sizeof(rcConfig));
        config.maxVertsPerPoly = (int)6;

        // these are WORLD UNIT based metrics
        config.cs = cellSize;
        config.ch = .3f;
        config.walkableSlopeAngle = m_maxWalkableAngle;
        rcVcopy(config.bmin, bmin);
        rcVcopy(config.bmax, bmax);

        // these are VOXEL-based metrics
        rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
        config.tileSize = (int)ceilf(config.width / (config.width * config.cs / GRID_SIZE));
        config.walkableRadius = (int)ceilf(agentRadius / config.cs);
        config.borderSize = config.walkableRadius + 3;
        config.maxEdgeLen = (int)(12.f / config.cs);
        config.walkableHeight = (int)ceilf(agentHeight / config.ch);
        config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
        config.minRegionSize = (int)rcSqr(50);
        config.mergeRegionSize = (int)rcSqr(20);

        // unknown metric
        config.maxSimplificationError = 1.3f;
        config.detailSampleDist = 6.f < 0.9f ? 0 : config.cs * 6.f;
        config.detailSampleMaxError = config.ch * 1.f;

        // navmesh tile dimensions
        const int yTileCount = ceil(yLen / GRID_SIZE);
        const int xTileCount = ceil(xLen / GRID_SIZE);

        // calculate number of bits needed to store tiles & polys
        int tileBits = rcMin((int)ilog2(nextPow2(yTileCount*xTileCount)), 14);
        if (tileBits > 14) tileBits = 14;
        if (tileBits < 1) tileBits = 1;     // need at least one bit!
        int polyBits = 22 - tileBits;
        int maxTiles = 1 << tileBits;
        int maxPolysPerTile = 1 << polyBits;

        // nav mesh creation params
        dtNavMeshParams navMeshParams;
        memset(&navMeshParams, 0, sizeof(dtNavMeshParams));
        navMeshParams.tileWidth = GRID_SIZE;
        navMeshParams.tileHeight = GRID_SIZE;
        rcVcopy(navMeshParams.orig, bmin);
        navMeshParams.maxTiles = maxTiles;
        navMeshParams.maxPolys = maxPolysPerTile;
        navMeshParams.maxNodes = 2048;

        dtNavMesh* navMesh = new dtNavMesh;
        printf("Creating navMesh...                     \r");
        if(!navMesh->init(&navMeshParams))
        {
            printf("Failed creating navmesh!                \n");
            return;
        }

        sprintf(fileName, "mmaps\\%03u.mmap", mapID);
        if(!(file = fopen(fileName, "wb")))
        {
            printf("Failed to open %s for writing!\n", fileName);
            return;
        }

        // now that we know navMesh params are valid, we can write them to file
        fwrite(&navMeshParams, sizeof(dtNavMeshParams), 1, file);

        int tileX, tileY;
        float yMin = snapToGrid(bmin[0]);
        float xMin = snapToGrid(bmin[2]);

        fclose(file);

        printf("Creating ChunkyTriMesh...               \r");
        rcChunkyTriMesh* chunkyMesh = new rcChunkyTriMesh;
        if(!rcCreateChunkyTriMesh(verts, tris, triCount, 256, chunkyMesh))
        {
            printf("Failed creating ChunkyTriMesh!          \n");
            return;
        }

        char tileString[10]; // "[xx,yy]: "

        for(int y = 0; y < yTileCount; ++y)
        {
            // set tile bounds
            bmin[0] = yMin + GRID_SIZE*y;
            bmax[0] = bmin[0] + GRID_SIZE;

            for(int x = 0; x < xTileCount; ++x)
            {
                // re-initialize
                // do it at the start of each iteration!!
                clearIntermediateValues(iv);

                // set tile bounds
                bmin[2] = xMin + GRID_SIZE*x;
                bmax[2] = bmin[2] + GRID_SIZE;
                rcVcopy(config.bmin, bmin);
                rcVcopy(config.bmax, bmax);

                // pad bounds with a border
                config.bmin[0] -= config.borderSize*config.cs;
                config.bmin[2] -= config.borderSize*config.cs;
                config.bmax[0] += config.borderSize*config.cs;
                config.bmax[2] += config.borderSize*config.cs;

                // this sets the dimensions of the heightfield - should maybe happen before border padding
                rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

                // set tile string
                tileY = 32-((bmin[0] + bmax[0]) / 2)/GRID_SIZE;
                tileX = 32-((bmin[2] + bmax[2]) / 2)/GRID_SIZE;
                sprintf(tileString, "[%02i,%02i]: ", tileX, tileY);

                // build heightfield
                printf("%sBuilding Recast Heightfield...          \r", tileString);
                iv.heightfield = new rcHeightfield;
                if(!rcCreateHeightfield(*iv.heightfield, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch))
                {
                    printf("%sFailed building heightfield!            \n", tileString);
                    continue;
                }

                // select proper triangles
                iv.triFlags = new unsigned char[chunkyMesh->maxTrisPerChunk];
                float tbmin[2], tbmax[2];
                tbmin[0] = config.bmin[0];
                tbmin[1] = config.bmin[2];
                tbmax[0] = config.bmax[0];
                tbmax[1] = config.bmax[2];
                const static int chunksPerTile = 4096;
                int cid[chunksPerTile];
                const int ncid = rcGetChunksInRect(chunkyMesh, tbmin, tbmax, cid, chunksPerTile);
                if(!ncid)
                {
                    // tiles with MAP_HEIGHT_NO_HEIGHT are usually empty, they can be skipped
                    continue;
                }
                if(ncid > chunksPerTile)
                    printf("%sHave %i chunks, but we can only use %i!\n", ncid, chunksPerTile);

                // rasterize triangles
                printf("%sRasterizing triangles...                \r", tileString);
                int tileTriCount = 0;
                for(int i = 0; i < ncid; ++i)
                {
                    const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
                    const int* nodeTris = &chunkyMesh->tris[node.i*3];
                    const int nNodeTris = node.n;

                    tileTriCount += nNodeTris;

                    memset(iv.triFlags, 0, nNodeTris*sizeof(unsigned char));
                    rcMarkWalkableTriangles(config.walkableSlopeAngle, verts, vertCount, nodeTris, nNodeTris, iv.triFlags);
                    rcRasterizeTriangles(verts, vertCount, nodeTris, iv.triFlags, nNodeTris, *iv.heightfield, config.walkableClimb);
                }

                delete [] iv.triFlags;
                iv.triFlags = 0;

                // filter out unusable rasterization data (order of calls matters)
                printf("%sFiltering low obstacles...              \r", tileString);
                rcFilterLowHangingWalkableObstacles(config.walkableClimb, *iv.heightfield);

                printf("%sFiltering edges...                      \r", tileString);
                rcFilterLedgeSpans(config.walkableHeight, config.walkableClimb, *iv.heightfield);

                printf("%sFiltering low-clearance areas...        \r", tileString);
                rcFilterWalkableLowHeightSpans(config.walkableHeight, *iv.heightfield);

                printf("%sCompacting heightfield...               \r", tileString);
                iv.compactHeightfield = new rcCompactHeightfield;
                if(!rcBuildCompactHeightfield(config.walkableHeight, config.walkableClimb, RC_WALKABLE, *iv.heightfield, *iv.compactHeightfield))
                {
                    printf("%sFailed compacting heightfield!            \n", tileString);
                    continue;
                }

                if(!m_debugOutput)
                {
                    delete iv.heightfield;
                    iv.heightfield = 0;
                }

                // build polymesh intermediates
                printf("%sEroding walkable area width...          \r", tileString);
                if(!rcErodeArea(RC_WALKABLE_AREA, config.walkableRadius, *iv.compactHeightfield))
                {
                    printf("%sFailed eroding area!                    \n", tileString);
                    continue;
                }

                printf("%sBuilding distance field...              \r", tileString);
                if(!rcBuildDistanceField(*iv.compactHeightfield))
                {
                    printf("%sFailed building distance field!         \n", tileString);
                    continue;
                }

                // bottleneck is here
                printf("%sBuilding regions...                     \r", tileString);
                if(!rcBuildRegions(*iv.compactHeightfield, config.borderSize, config.minRegionSize, config.mergeRegionSize))
                {
                    printf("%sFailed building regions!                \n", tileString);
                    continue;
                }

                printf("%sBuilding contours...                    \r", tileString);
                iv.contours = new rcContourSet;
                if(!rcBuildContours(*iv.compactHeightfield, config.maxSimplificationError, config.maxEdgeLen, *iv.contours))
                {
                    printf("%sFailed building contours!               \n", tileString);
                    continue;
                }

                // build polymesh
                printf("%sBuilding polymesh...                    \r", tileString);
                iv.polyMesh = new rcPolyMesh;
                if(!rcBuildPolyMesh(*iv.contours, config.maxVertsPerPoly, *iv.polyMesh))
                {
                    printf("%sFailed building polymesh!               \n", tileString);
                    continue;
                }

                printf("%sBuilding polymesh detail...             \r", tileString);
                iv.polyMeshDetail = new rcPolyMeshDetail;
                if(!rcBuildPolyMeshDetail(*iv.polyMesh, *iv.compactHeightfield, config.detailSampleDist, config.detailSampleMaxError, *iv.polyMeshDetail))
                {
                    printf("%sFailed building polymesh detail!        \n", tileString);
                    continue;
                }

                if(!m_debugOutput)
                {
                    delete iv.compactHeightfield; iv.compactHeightfield = 0;
                    delete iv.contours; iv.contours = 0;
                }

                // this might be handled within Recast at some point
                printf("%sCleaning vertex padding...              \r", tileString);
                for(int i = 0; i < iv.polyMesh->nverts; ++i)
                {
                    unsigned short* v = &iv.polyMesh->verts[i*3];
                    v[0] -= (unsigned short)config.borderSize;
                    v[2] -= (unsigned short)config.borderSize;
                }

                // polymesh vertex indices are stored with ushorts in detour, can't have more than 65535
                if(iv.polyMesh->nverts >= 0xffff)
                {
                    printf("%sToo many vertices!                      \n", tileString);
                    continue;
                }

                // TODO: implement flags
                printf("%sSetting polys as walkable...            \r", tileString);
                for(int i = 0; i < iv.polyMesh->npolys; ++i)
                    if(iv.polyMesh->areas[i] == RC_WALKABLE_AREA)
                        //iv.polyMesh->areas[i] = 0;
                        iv.polyMesh->flags[i] = 1;

                dtNavMeshCreateParams params;
                memset(&params, 0, sizeof(params));
                params.verts = iv.polyMesh->verts;
                params.vertCount = iv.polyMesh->nverts;
                params.polys = iv.polyMesh->polys;
                params.polyAreas = iv.polyMesh->areas;
                params.polyFlags = iv.polyMesh->flags;
                params.polyCount = iv.polyMesh->npolys;
                params.nvp = iv.polyMesh->nvp;
                params.detailMeshes = iv.polyMeshDetail->meshes;
                params.detailVerts = iv.polyMeshDetail->verts;
                params.detailVertsCount = iv.polyMeshDetail->nverts;
                params.detailTris = iv.polyMeshDetail->tris;
                params.detailTriCount = iv.polyMeshDetail->ntris;
                params.walkableHeight = agentHeight;
                params.walkableRadius = agentRadius;
                params.walkableClimb = agentMaxClimb;
                params.tileX = (((bmin[0] + bmax[0]) / 2) - navMesh->getParams()->orig[0]) / GRID_SIZE;
                params.tileY = (((bmin[2] + bmax[2]) / 2) - navMesh->getParams()->orig[2]) / GRID_SIZE;
                rcVcopy(params.bmin, bmin);
                rcVcopy(params.bmax, bmax);
                params.cs = config.cs;
                params.ch = config.ch;
                params.tileSize = config.tileSize;

                // will hold final navmesh
                unsigned char* navData = 0;
                int navDataSize = 0;

                // these values are checked within dtCreateNavMeshData - handle them here
                // so we have a clear error message
                if (params.nvp > DT_VERTS_PER_POLYGON)
                {
                    printf("%sInvalid verts-per-polygon value!        \n", tileString);
                    continue;
                }
                if (params.vertCount >= 0xffff)
                {
                    printf("%sToo many vertices!                      \n", tileString);
                    continue;
                }
                if (!params.vertCount || !params.verts)
                {
                    //printf("%sNo vertices to build navMesh!           \n", tileString);
                    continue;
                }
                if (!params.polyCount || !params.polys)
                {
                    printf("%sNo polygons to build navMesh!           \n", tileString);
                    continue;
                }
                if (!params.detailMeshes || !params.detailVerts || !params.detailTris)
                {
                    printf("%sNo detail mesh to build navMesh!        \n", tileString);
                    continue;
                }

                printf("%sBuilding navmesh tile...                \r", tileString);
                if(!dtCreateNavMeshData(&params, &navData, &navDataSize))
                {
                    printf("%sFailed building navmesh tile!           \n", tileString);
                    delete [] navData;
                    continue;
                }

                dtTileRef tileRef = 0;
                printf("%Adding tile to navmesh...                \r", tileString);
                // DT_TILE_FREE_DATA tells detour to unallocate memory when the tile
                // is removed via removeTile()
                if(!(tileRef = navMesh->addTile(navData, navDataSize, DT_TILE_FREE_DATA)))
                {
                    printf("%Failed adding tile to navmesh!           \n", tileString);
                    delete [] navData;
                    continue;
                }

                sprintf(fileName, "mmaps\\%03u%02i%02i.mmtile", mapID, tileX, tileY);
                if(!(file = fopen(fileName, "wb")))
                {
                    printf("%sFailed to open %s for writing!\n",  tileString, fileName);
                    navMesh->removeTile(tileRef, 0, 0);
                    continue;
                }

                printf("%sWriting to file...                      \r", tileString);
                // should write navDataSize first... for now, just use ftell to find length when reading
                fwrite(navData, sizeof(unsigned char), navDataSize, file);
                fclose(file);

                if(m_debugOutput)
                    writeIV(mapID, tileX, tileY, iv);

                // now that tile is written to disk, we can unload it
                navMesh->removeTile(tileRef, 0, 0);
            }
        }

        // cleanup
        delete chunkyMesh;
        delete navMesh;
        clearIntermediateValues(iv);
    }

    void MapBuilder::initIntermediateValues(IntermediateValues &iv)
    {
        iv.compactHeightfield = 0;
        iv.heightfield = 0;
        iv.triFlags = 0;
        iv.contours = 0;
        iv.polyMesh = 0;
        iv.polyMeshDetail = 0;
    }

    void MapBuilder::clearIntermediateValues(IntermediateValues &iv)
    {
        if(iv.compactHeightfield) delete iv.compactHeightfield;
        if(iv.heightfield) delete iv.heightfield;
        if(iv.triFlags) delete [] iv.triFlags;
        if(iv.contours) delete iv.contours;
        if(iv.polyMesh) delete iv.polyMesh;
        if(iv.polyMeshDetail) delete iv.polyMeshDetail;

        initIntermediateValues(iv);
    }

    void MapBuilder::generateObjFile(uint32 mapID)
    {
        if(!m_debugOutput)
            return;

        printf("Generating obj file...                  \r");

        char objFileName[20];
        sprintf(objFileName, "meshes\\%03u.obj", mapID);

        FILE* objFile = fopen(objFileName, "wb");
        if(!objFile)
        {
            perror(objFileName);
            return;
        }

        int vertsSize = m_vertices.size() / 3;
        int trisSize = m_triangles.size() / 3;
        float* verts = m_vertices.getCArray();
        int* tris = m_triangles.getCArray();

        fwrite(&vertsSize, sizeof(int), 1, objFile);
        fwrite(verts, sizeof(float), vertsSize*3, objFile);
        fflush(objFile);

        fwrite(&trisSize, sizeof(int), 1, objFile);
        fwrite(tris, sizeof(int), trisSize*3, objFile);
        fflush(objFile);

        fclose(objFile);
    }

    void MapBuilder::writeIV(uint32 mapID, uint32 tileX, uint32 tileY, IntermediateValues iv)
    {
        char fileName[25];
        char tileString[25];
        FILE* file;

        sprintf(tileString, "[%02u,%02u]: ", tileX, tileY);

        printf("%sWriting debug output...                       \r", tileString);

        // heightfield
        sprintf(fileName, "meshes\\%03u%02i%02i.hf", mapID, tileX, tileY);
        if(!(file = fopen(fileName, "wb")))
            printf("%sFailed to open %s for writing!\n",  tileString, fileName);
        else
            writeHeightfield(file, iv.heightfield);
        if(file) fclose(file);

        printf("%sWriting debug output...                       \r", tileString);

        // compact heightfield
        sprintf(fileName, "meshes\\%03u%02i%02i.chf", mapID, tileX, tileY);
        if(!(file = fopen(fileName, "wb")))
            printf("%sFailed to open %s for writing!\n",  tileString, fileName);
        else
            writeCompactHeightfield(file, iv.compactHeightfield);
        if(file) fclose(file);

        printf("%sWriting debug output...                       \r", tileString);

        // poly mesh
        sprintf(fileName, "meshes\\%03u%02i%02i.pmesh", mapID, tileX, tileY);
        if(!(file = fopen(fileName, "wb")))
            printf("%sFailed to open %s for writing!\n",  tileString, fileName);
        else
            writePolyMesh(file, iv.polyMesh);
        if(file) fclose(file);

        printf("%sWriting debug output...                       \r", tileString);

        // detail mesh
        sprintf(fileName, "meshes\\%03u%02i%02i.dmesh", mapID, tileX, tileY);
        if(!(file = fopen(fileName, "wb")))
            printf("%sFailed to open %s for writing!\n",  tileString, fileName);
        else
            writeDetailMesh(file, iv.polyMeshDetail);
        if(file) fclose(file);
    }

    void MapBuilder::writeHeightfield(FILE* file, const rcHeightfield* mesh)
    {
        if(!file || !mesh)
            return;

        fwrite(&(mesh->cs), sizeof(float), 1, file);
        fwrite(&(mesh->ch), sizeof(float), 1, file);
        fwrite(&(mesh->width), sizeof(int), 1, file);
        fwrite(&(mesh->height), sizeof(int), 1, file);
        fwrite(mesh->bmin, sizeof(float), 3, file);
        fwrite(mesh->bmax, sizeof(float), 3, file);

        rcSpan blank;
        memset(&blank, 0, sizeof(rcSpan));
        // leave off the last member of spans, it's a pointer we don't need
        for(int i = 0; i < (mesh->width * mesh->height); ++i)
            if(mesh->spans[i])
                fwrite(mesh->spans[i], sizeof(rcSpan)-sizeof(rcSpan*), 1, file);
            else
                fwrite(&blank, sizeof(rcSpan)-sizeof(rcSpan*), 1, file);
    }

    void MapBuilder::writeCompactHeightfield(FILE* file, const rcCompactHeightfield* chf)
    {
        if(!file | !chf)
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

    void MapBuilder::writePolyMesh(FILE* file, const rcPolyMesh* mesh)
    {
        if(!file || !mesh)
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

    void MapBuilder::writeDetailMesh(FILE* file, const rcPolyMeshDetail* mesh)
    {
        if(!file || !mesh)
            return;

        fwrite(&(mesh->nverts), sizeof(int), 1, file);
        fwrite(mesh->verts, sizeof(float), mesh->nverts*3, file);
        fwrite(&(mesh->ntris), sizeof(int), 1, file);
        fwrite(mesh->tris, sizeof(char), mesh->ntris*4, file);
        fwrite(&(mesh->nmeshes), sizeof(int), 1, file);
        fwrite(mesh->meshes, sizeof(short), mesh->nmeshes*4, file);
    }

    void MapBuilder::cleanup()
    {
        uint32 i;
        for(i = 0; i < m_modelVertices.size(); ++i)
        {
            vector<float>* verts = m_modelVertices[i];
            verts->clear();
            delete verts;
        }
        m_modelVertices.clear();

        for(i = 0; i < m_modelTriangles.size(); ++i)
        {
            vector<int>* tris = m_modelTriangles[i];
            tris->clear();
            delete tris;
        }
        m_modelTriangles.clear();

        m_vertices.fastClear();
        m_triangles.fastClear();
    }

    float MapBuilder::snapToGrid(const float coord)
    {
        return floor(coord / GRID_SIZE) * GRID_SIZE;
    }

    bool MapBuilder::shouldSkipMap(uint32 mapID)
    {
        // debug
        //switch(mapID)
        //{
        //    case 309:
        //        return false;
        //    default:
        //        return true;
        //}

        if(m_skipContinents)
            switch(mapID)
            {
                case 0:
                case 1:
                case 530:
                case 571:
                    return true;
                default:
                    break;
            }

        if(m_skipJunkMaps)
            switch(mapID)
            {
                case 13:    // test.wdt
                case 25:    // ScottTest.wdt
                case 29:    // Test.wdt
                case 42:    // Colin.wdt
                case 169:   // EmeraldDream.wdt (unused, and very large)
                case 451:   // development.wdt
                case 573:   // ExteriorTest.wdt
                case 597:   // CraigTest.wdt
                case 605:   // development_nonweighted.wdt
                case 606:   // QA_DVD.wdt
                    return true;
                default:
                    if(isTransportMap(mapID))
                        return true;
                    break;
            }

        if(m_skipBattlegrounds)
            switch(mapID)
            {
                case 30:    // AV
                case 37:    // ?
                case 489:   // WSG
                case 529:   // AB
                case 566:   // EotS
                case 607:   // SotA
                case 628:   // IoC
                    return true;
                default:
                    break;
            }

        return false;
    }

    bool MapBuilder::isTransportMap(uint32 mapID)
    {
        switch(mapID)
        {
            // transport maps
            case 582:
            case 584:
            case 586:
            case 587:
            case 588:
            case 589:
            case 590:
            case 591:
            case 592:
            case 593:
            case 594:
            case 596:
            case 610:
            case 612:
            case 613:
            case 614:
            case 620:
            case 621:
            case 622:
            case 623:
            case 641:
            case 642:
            case 647:
            case 672:
            case 673:
            case 712:
            case 713:
            case 718:
                return true;
            default:
                return false;
        }
    }
}