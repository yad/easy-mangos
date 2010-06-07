#include <sstream>

#include "TileBuilder.h"

#include "IVMapManager.h"
#include "VMapManager2.h"
#include "MapTree.h"
#include "ModelInstance.h"
#include "G3D/Array.h"

using namespace VMAP;
using namespace MaNGOS;

namespace MMAP
{
    TileBuilder::TileBuilder(float maxWalkableAngle, bool hiRes, bool shred, IVMapManager* vmapManager) :
        m_hiResHeightMaps   (hiRes),
        m_shredHeightmaps   (shred),
        m_maxRadians        (maxWalkableAngle*G3D::pi()/180),
        m_groundNormal      (G3D::Plane(Vector3::unitY(), Vector3(0.f, 0.f, 0.f)).normal()),
        m_vmapManager       (vmapManager)
    {
        V9 = new float[V9_SIZE_SQ];
        V8 = new float[V8_SIZE_SQ];
        m_vertices = new G3D::Array<float>;
        m_triangles = new G3D::Array<int>;
    }

    TileBuilder::~TileBuilder()
    {
        delete [] V9;
        delete [] V8;
        m_vertices->clear();
        m_triangles->clear();
    }

    void TileBuilder::build(
        uint32 mapID,
        uint32 tileX,
        uint32 tileY,
        float* &vertices,
        uint32 &vertCount,
        int* &triangles,
        uint32 &triangleCount)
    {
        m_mapID =  mapID;
        m_tileX = tileX;
        m_tileY = tileY;

        char map[18];
        sprintf(map, "maps\\%03u%02u%02u.map", m_mapID, m_tileY, m_tileX);

        cleanup();
        loadHeightMap(map);
        intersect();

        char obj[20];
        sprintf(obj, "meshes\\%03u%02u%02u.obj", m_mapID, m_tileY, m_tileX);
        //generateObjFile(obj);

        // return vertices and triangles
        vertCount = m_vertices->size() / 3;
        triangleCount = m_triangles->size() / 3;
        vertices = m_vertices->getCArray();
        triangles = m_triangles->getCArray();
    }

    float TileBuilder::readHeightOffset(uint32 mapID, uint32 tileX, uint32 tileY)
    {
        char mapFileName[20];
        sprintf(mapFileName, "maps\\%03u%02u%02u.map", mapID, tileY, tileX);

        FILE* mapFile = fopen(mapFileName, "rb");

        if(!mapFile)
            return 0.f;

        map_fileheader fileHeader;
        fread(&fileHeader, sizeof(map_fileheader), 1, mapFile);

        fseek(mapFile, fileHeader.heightMapOffset, SEEK_SET);

        map_heightHeader heightHeader;
        fread(&heightHeader, sizeof(map_heightHeader), 1, mapFile);

        fclose(mapFile);

        return heightHeader.gridHeight;
    }

    bool TileBuilder::loadHeightMap(const char* mapFileName)
    {
        printf("[%02u,%02u]: Loading heightmap...       \r", m_tileY, m_tileX);

        FILE* mapFile = fopen(mapFileName, "rb");

        if(!mapFile)
            return false;

        map_fileheader header;
        fread(&header, sizeof(map_fileheader), 1, mapFile);

        readMapHeight(mapFile, header.heightMapOffset);

        fclose(mapFile);

        return true;
    }

    void TileBuilder::readMapHeight(FILE* mapFile, uint32 heightHeaderOffset)
    {
        fseek(mapFile, heightHeaderOffset, SEEK_SET);

        map_heightHeader header;
        fread(&header, sizeof(map_heightHeader), 1, mapFile);

        m_heightOffset = header.gridHeight;

        if(header.flags & MAP_HEIGHT_NO_HEIGHT)
            return;

        int i;
        float heightMultiplier;

        //if(header.flags & MAP_HEIGHT_NO_HEIGHT)
        //{
        //    for(i = 0; i < V9_SIZE_SQ; ++i)
        //        V9[i] = header.gridHeight;

        //    if(m_hiResHeightMaps)
        //        for(i = 0; i < V8_SIZE_SQ; ++i)
        //            V8[i] = header.gridHeight;
        //}
        //else
        if(header.flags & MAP_HEIGHT_AS_INT8)
        {
            uint8 v9[V9_SIZE_SQ];
            uint8 v8[V8_SIZE_SQ];
            fread(v9, sizeof(uint8), V9_SIZE_SQ, mapFile);
            fread(v8, sizeof(uint8), V8_SIZE_SQ, mapFile);
            heightMultiplier = (header.gridMaxHeight - header.gridHeight) / 255;

            for(i = 0; i < V9_SIZE_SQ; ++i)
                V9[i] = (float)v9[i]*heightMultiplier + header.gridHeight;

            if(m_hiResHeightMaps)
                for(i = 0; i < V8_SIZE_SQ; ++i)
                    V8[i] = (float)v8[i]*heightMultiplier + header.gridHeight;
        }
        else if(header.flags & MAP_HEIGHT_AS_INT16)
        {
            uint16 v9[V9_SIZE_SQ];
            uint16 v8[V8_SIZE_SQ];
            fread(v9, sizeof(uint16), V9_SIZE_SQ, mapFile);
            fread(v8, sizeof(uint16), V8_SIZE_SQ, mapFile);
            heightMultiplier = (header.gridMaxHeight - header.gridHeight) / 65535;

            for(i = 0; i < V9_SIZE_SQ; ++i)
                V9[i] = (float)v9[i]*heightMultiplier + header.gridHeight;

            if(m_hiResHeightMaps)
                for(i = 0; i < V8_SIZE_SQ; ++i)
                    V8[i] = (float)v8[i]*heightMultiplier + header.gridHeight;
        }
        else
        {
            fread(V9, sizeof(float), V9_SIZE_SQ, mapFile);
            if(m_hiResHeightMaps)
                fread(V8, sizeof(float), V8_SIZE_SQ, mapFile);
        }

        int count = m_vertices->size() / 3;
        float xoffset = (float(m_tileX)-32)*533.33333f;
        float yoffset = (float(m_tileY)-32)*533.33333f;

        float coord[3];

        for(i = 0; i < V9_SIZE_SQ; ++i)
        {
            getHeightCoord(i, GRID_V9, xoffset, yoffset, coord);
            m_vertices->append(coord[0]);
            m_vertices->append(coord[2]);
            m_vertices->append(coord[1]);
        }

        if(m_hiResHeightMaps)
            for(i = 0; i < V8_SIZE_SQ; ++i)
            {
                getHeightCoord(i, GRID_V8, xoffset, yoffset, coord);
                m_vertices->append(coord[0]);
                m_vertices->append(coord[2]);
                m_vertices->append(coord[1]);
            }

        int inc;
        if(m_hiResHeightMaps)
            inc = 1;
        else
            inc = BOTTOM-TOP;

        int j;
        int indices[3];
        for(i = V8_SIZE*0; i < V8_SIZE_SQ; ++i)
            for(j = TOP; j <= BOTTOM; j+=inc)
                if(getHeightTriangle(i, TriangleSpot(j), indices, count))
                {
                    m_triangles->append(indices[2] + count);
                    m_triangles->append(indices[1] + count);
                    m_triangles->append(indices[0] + count);
                }
    }

    inline void TileBuilder::getHeightCoord(int index, Grid grid, float xOffset, float yOffset, float* coord)
    {
        // wow coords: x, y, height
        // coord is mirroed about the horizontal axes
        switch(grid)
        {
            case GRID_V9:
                coord[0] = (xOffset + index%(V9_SIZE)*GRID_PART_SIZE) * -1.f;
                coord[1] = (yOffset + (int)(index/(V9_SIZE))*GRID_PART_SIZE) * -1.f;
                coord[2] = V9[index];
                break;
            case GRID_V8:
                coord[0] = (xOffset + index%(V8_SIZE)*GRID_PART_SIZE + GRID_PART_SIZE/2.f) * -1.f;
                coord[1] = (yOffset + (int)(index/(V8_SIZE))*GRID_PART_SIZE + GRID_PART_SIZE/2.f) * -1.f;
                coord[2] = V8[index];
                break;
        }
    }

    inline bool TileBuilder::getHeightTriangle(int square, TriangleSpot triangle, int* indices, int offset)
    {
        int rowOffset = square/V8_SIZE;
        if(m_hiResHeightMaps)
            switch(triangle)
            {
                case TOP:
                    indices[0] = square+rowOffset;                      //           0-----1 .... 128
                    indices[1] = square+1+rowOffset;                    //           |\ T /|
                    indices[2] = (V9_SIZE_SQ)+square;              //           | \ / |
                    break;                                              //           |L 0 R| .. 127
                case LEFT:                                              //           | / \ |
                    indices[0] = square+rowOffset;                      //           |/ B \|
                    indices[1] = (V9_SIZE_SQ)+square;              //          129---130 ... 386
                    indices[2] = square+V9_SIZE+rowOffset;              //           |\   /|
                    break;                                              //           | \ / |
                case RIGHT:                                             //           | 128 | .. 255
                    indices[0] = square+1+rowOffset;                    //           | / \ |
                    indices[1] = square+V9_SIZE+1+rowOffset;            //           |/   \|
                    indices[2] = (V9_SIZE_SQ)+square;              //          258---259 ... 515
                    break;
                case BOTTOM:
                    indices[0] = (V9_SIZE_SQ)+square;
                    indices[1] = square+V9_SIZE+1+rowOffset;
                    indices[2] = square+V9_SIZE+rowOffset;
                    break;
            }
        else
            switch(triangle)
            {                                                           //           0-----1 .... 128
                case TOP:                                               //           |\    |
                    indices[0] = square+rowOffset;                      //           | \ T |
                    indices[1] = square+1+rowOffset;                    //           |  \  |
                    indices[2] = square+V9_SIZE+1+rowOffset;            //           | B \ |
                    break;                                              //           |    \|
                case BOTTOM:                                            //          129---130 ... 386
                    indices[0] = square+rowOffset;                      //           |\    |
                    indices[1] = square+V9_SIZE+1+rowOffset;            //           | \   |
                    indices[2] = square+V9_SIZE+rowOffset;              //           |  \  |
                    break;                                              //           |   \ |
            }                                                           //           |    \|
                                                                        //          258---259 ... 515

        if(m_shredHeightmaps)
        {
            // determine angle between triangle's plane and horizontal plane
            // this information is used to throw away triangles:
            // * that won't be used for paths because incline is too steep (speeds up recast's work)
            // * that would block cave entrances, doors, etc
            float* verts = m_vertices->getCArray();
            float* f1 = &verts[(indices[2]+offset)*3];
            float* f2 = &verts[(indices[1]+offset)*3];
            float* f3 = &verts[(indices[0]+offset)*3];
            Vector3 v1(f1[0], f1[1], f1[2]);
            Vector3 v2(f2[0], f2[1], f2[2]);
            Vector3 v3(f3[0], f3[1], f3[2]);
            float angle = G3D::aCos(double(G3D::Plane(v1, v2, v3).normal().dot(m_groundNormal)));

            return abs(angle) <= m_maxRadians;
        }

        return true;
    }

    void TileBuilder::loadModels()
    {
        printf("[%02u,%02u]: Loading vmap models...                 \r", m_mapID, m_tileX, m_tileY);

        cout.setstate(cout.badbit);
        int result = m_vmapManager->loadMap("vmaps", m_mapID, (int)m_tileX, (int)m_tileY);
        cout.clear(cout.goodbit);

        if(result == VMAP_LOAD_RESULT_ERROR)
            return;

        ModelInstance* models = 0;
        uint32 count = 0;

        InstanceTreeMap instanceTrees;
        ((VMapManager2*)m_vmapManager)->getInstanceMapTree(instanceTrees);

        if(!instanceTrees[m_mapID])
            return;

        instanceTrees[m_mapID]->getModelInstances(models, count);

        if(!models || !count)
            return;

        uint32 i;
        int percent = -1, oldPercent = -2;
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
            position.z += m_heightOffset;

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

            m_modelsVertices.push_back(modelVertices);
            m_modelsTriangles.push_back(modelTriangles);
        }
    }

    inline void TileBuilder::transform(vector<Vector3> source, vector<Vector3> &transformedVertices, float scale, G3D::Matrix3 rotation, Vector3 position)
    {
        for(vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
            transformedVertices.push_back((*it) * rotation * scale + position);
    }

    inline void TileBuilder::copyVertices(vector<Vector3> source, vector<float> dest)
    {
        for(vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
        {
            dest.push_back((*it).y);
            dest.push_back((*it).z);
            dest.push_back((*it).x);
        }
    }

    inline void TileBuilder::copyIndices(vector<MeshTriangle> source, vector<int> dest, int offset, bool flip)
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

    void TileBuilder::intersect()
    {
        printf("[%02u,%02u]: Intersecting meshes...     \r", m_tileY, m_tileX);
    }

    void TileBuilder::generateObjFile(const char* objFileName)
    {
        printf("[%02u,%02u]: Writing obj file           \r", m_tileX, m_tileY);

        G3D::Array<float> verts = *m_vertices;
        G3D::Array<int> tris = *m_triangles;

        if(verts.size() == 0 && tris.size() == 0)
            return;

        FILE* objFile = fopen(objFileName, "wb");

        int i;
        for(i = 0; i < verts.size() / 3; ++i)
            fprintf(objFile, "v %f %f %f\n", verts[i*3], verts[i*3+1], verts[i*3+2]);

        for(i = 0; i < tris.size() / 3; ++i)
            fprintf(objFile, "f %i %i %i\n", tris[i*3]+1, tris[i*3+1]+1, tris[i*3+2]+1);

        fclose(objFile);
    }

    void TileBuilder::cleanup()
    {
        cout.setstate(cout.badbit);
        m_vmapManager->unloadMap(m_mapID, m_tileX, m_tileY);
        m_vmapManager->unloadMap(m_mapID);
        cout.clear(cout.goodbit);

        m_vertices->fastClear();
        m_triangles->fastClear();
    }
}