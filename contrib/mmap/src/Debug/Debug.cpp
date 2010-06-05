
#include "Debug.h"

#include <vector>
#include "pathfinding/Detour/DetourNavMesh.h"
#include "pathfinding/Recast/Recast.h"
#include "MMapCommon.h"

void duReadNavMesh(int mapID, dtNavMesh* &navMesh)
{
    char fileName[25];
    FILE* file;

    sprintf(fileName, "mmaps\\%03i.mmap", mapID);
    file = fopen(fileName, "rb");

    if(!file)
        return;

    dtNavMeshParams params;
    fread(&params, sizeof(dtNavMeshParams), 1, file);
    fclose(file);

    navMesh = new dtNavMesh;
    navMesh->init(&params);

    sprintf(fileName, "%03i*.mmtile", mapID);

    vector<string> fileNames;
    MMAP::getDirContents(fileNames, "mmaps", fileName);

    for(int i = 0; i < fileNames.size(); ++i)
    {
        file = fopen(("mmaps\\" + fileNames[i]).c_str(), "rb");
        if(!file)
            continue;

        fseek(file, 0, SEEK_END);
        int length = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char* data = new unsigned char[length];
        fread(data, length, 1, file);
        fclose(file);

        navMesh->addTile(data, length, DT_TILE_FREE_DATA);
    }
}

void duReadPolyMesh(int mapID, rcPolyMesh* &mesh)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.pmesh", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    //if(!mesh)
    //    mesh = new rcPolyMesh;

    for(int i = 0; i < files.size(); ++i)
    {
        rcPolyMesh* newMesh = new rcPolyMesh;

        file = fopen(("meshes\\" + files[i]).c_str(), "rb");
        if(!file)
            continue;

        fread(&(newMesh->cs), sizeof(float), 1, file);
        fread(&(newMesh->ch), sizeof(float), 1, file);
        fread(&(newMesh->nvp), sizeof(int), 1, file);
        fread(newMesh->bmin, sizeof(float), 3, file);
        fread(newMesh->bmax, sizeof(float), 3, file);
        fread(&(newMesh->nverts), sizeof(int), 1, file);
        newMesh->verts = new unsigned short[newMesh->nverts*3];
        fread(newMesh->verts, sizeof(unsigned short), newMesh->nverts*3, file);
        fread(&(newMesh->npolys), sizeof(int), 1, file);
        newMesh->polys = new unsigned short[newMesh->npolys*newMesh->nvp*2];
        newMesh->flags = new unsigned short[newMesh->npolys];
        newMesh->areas = new unsigned char[newMesh->npolys];
        newMesh->regs = new unsigned short[newMesh->npolys];
        fread(newMesh->polys, sizeof(unsigned short), newMesh->npolys*newMesh->nvp*2, file);
        fread(newMesh->flags, sizeof(unsigned short), newMesh->npolys, file);
        fread(newMesh->areas, sizeof(unsigned char), newMesh->npolys, file);
        fread(newMesh->regs, sizeof(unsigned short), newMesh->npolys, file);
        fclose(file);

        if(i > 0)
        {
            rcPolyMesh* oldMesh = mesh;
            mesh = new rcPolyMesh;
            rcPolyMesh* meshes[2] = {oldMesh, newMesh};
            rcMergePolyMeshes(meshes, 2, *mesh);

            delete oldMesh;
            delete newMesh;
        }
        else
            mesh = newMesh;
    }
}

void duReadDetailMesh(int mapID, rcPolyMeshDetail* &mesh)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.dmesh", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    if(!mesh)
        mesh = new rcPolyMeshDetail;

    for(int i = 0; i < files.size(); ++i)
    {
        rcPolyMeshDetail* newMesh = new rcPolyMeshDetail;

        file = fopen(("meshes\\" + files[i]).c_str(), "rb");
        if(!file)
            continue;

        fread(&(newMesh->nverts), sizeof(int), 1, file);
        newMesh->verts = new float[newMesh->nverts*3];
        fread(newMesh->verts, sizeof(float), newMesh->nverts*3, file);

        fread(&(newMesh->ntris), sizeof(int), 1, file);
        newMesh->tris = new unsigned char[newMesh->ntris*4];
        fread(newMesh->tris, sizeof(char), newMesh->ntris*4, file);

        fread(&(newMesh->nmeshes), sizeof(int), 1, file);
        newMesh->meshes = new unsigned short[newMesh->nmeshes*4];
        fread(newMesh->meshes, sizeof(short), newMesh->nmeshes*4, file);

        fclose(file);

        if(i > 0)
        {
            rcPolyMeshDetail* oldMesh = mesh;
            mesh = new rcPolyMeshDetail;
            rcPolyMeshDetail* meshes[2] = {oldMesh, newMesh};
            rcMergePolyMeshDetails(meshes, 2, *mesh);

            delete oldMesh;
            delete newMesh;
        }
        else
            mesh = newMesh;
    }
}

