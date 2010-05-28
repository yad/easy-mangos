
#include "Debug.h"

#include <vector>
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

