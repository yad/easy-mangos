
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

    delete navMesh;
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

int duReadHeightfield(int mapID, rcHeightfield* &hf)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.hf", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    hf = new rcHeightfield[files.size()];

    for(int i = 0; i < files.size(); ++i)
    {
        file = fopen(("meshes/" + files[i]).c_str(), "rb");
        if(!file)
            continue;

        fread(&(hf[i].cs), sizeof(float), 1, file);
        fread(&(hf[i].ch), sizeof(float), 1, file);
        fread(&(hf[i].width), sizeof(int), 1, file);
        fread(&(hf[i].height), sizeof(int), 1, file);
        fread(hf[i].bmin, sizeof(float), 3, file);
        fread(hf[i].bmax, sizeof(float), 3, file);

        hf[i].spans = new rcSpan*[hf[i].width*hf[i].height + 1];

        for(int j = 0; j < hf[i].width*hf[i].height; ++j)
            hf[i].spans[j] = new rcSpan;
        hf[i].spans[hf[i].width*hf[i].height] = 0;

        for(int j = 0; j < hf[i].width*hf[i].height; ++j)
        {
            fread(hf[i].spans[j], sizeof(rcSpan)-sizeof(rcSpan*), 1, file);
            hf[i].spans[j]->next = hf[i].spans[j+1];
        }

        fclose(file);
    }

    return files.size();
}

int duReadCompactHeightfield(int mapID, rcCompactHeightfield* &chf)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.chf", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    chf = new rcCompactHeightfield[files.size()];
    for(int i = 0; i < files.size(); ++i)
    {
        file = fopen(("meshes/" + files[i]).c_str(), "rb");
        if(!file)
            continue;

        fread(&chf[i].width, sizeof(chf[i].width), 1, file);
        fread(&chf[i].height, sizeof(chf[i].height), 1, file);
        fread(&chf[i].spanCount, sizeof(chf[i].spanCount), 1, file);

        fread(&chf[i].walkableHeight, sizeof(chf[i].walkableHeight), 1, file);
        fread(&chf[i].walkableClimb, sizeof(chf[i].walkableClimb), 1, file);

        fread(&chf[i].maxDistance, sizeof(chf[i].maxDistance), 1, file);
        fread(&chf[i].maxRegions, sizeof(chf[i].maxRegions), 1, file);

        fread(chf[i].bmin, sizeof(chf[i].bmin), 1, file);
        fread(chf[i].bmax, sizeof(chf[i].bmax), 1, file);
    
        fread(&chf[i].cs, sizeof(chf[i].cs), 1, file);
        fread(&chf[i].ch, sizeof(chf[i].ch), 1, file);

        int tmp = 0;
        fread(&tmp, sizeof(tmp), 1, file);

        if (tmp & 1)
        {
            chf[i].cells = new rcCompactCell[chf[i].width*chf[i].height];
            fread(chf[i].cells, sizeof(rcCompactCell), chf[i].width*chf[i].height, file);
        }
        if (tmp & 2)
        {
            chf[i].spans = new rcCompactSpan[chf[i].spanCount];
            fread(chf[i].spans, sizeof(rcCompactSpan), chf[i].spanCount, file);
        }
        if (tmp & 4)
        {
            chf[i].dist = new unsigned short[chf[i].spanCount];
            fread(chf[i].dist, sizeof(unsigned short), chf[i].spanCount, file);
        }
        if (tmp & 8)
        {
            chf[i].areas = new unsigned char[chf[i].spanCount];
            fread(chf[i].areas, sizeof(unsigned char), chf[i].spanCount, file);
        }

        fclose(file);
    }

    return files.size();
}

int duReadPolyMesh(int mapID, rcPolyMesh* &mesh)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.pmesh", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    mesh = new rcPolyMesh[files.size()];

    for(int i = 0; i < files.size(); ++i)
    {
        rcPolyMesh* newMesh = &mesh[i];

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
    }

    return files.size();
}

int duReadDetailMesh(int mapID, rcPolyMeshDetail* &mesh)
{
    char fileName[25];
    FILE* file;

    vector<string> files;
    sprintf(fileName, "%03i*.dmesh", mapID);
    MMAP::getDirContents(files, "meshes", fileName);

    mesh = new rcPolyMeshDetail[files.size()];

    for(int i = 0; i < files.size(); ++i)
    {
        rcPolyMeshDetail* newMesh = &mesh[i];

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
    }

    return files.size();
}

myMeshLoaderObj::myMeshLoaderObj() :
	m_verts(0),
	m_tris(0),
	m_normals(0),
	m_vertCount(0),
	m_triCount(0)
{
}

myMeshLoaderObj::~myMeshLoaderObj()
{
	delete [] m_verts;
	delete [] m_normals;
	delete [] m_tris;
}

bool myMeshLoaderObj::load(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if(!file)
        return false;

    fread(&m_vertCount, sizeof(int), 1, file);
    m_verts = new float[m_vertCount*3];
    fread(m_verts, sizeof(float), m_vertCount*3, file);

    fread(&m_triCount, sizeof(int), 1, file);
    m_tris = new int[m_triCount*3];
    fread(m_tris, sizeof(int), m_triCount*3, file);

    fclose(file);

	m_normals = new float[m_triCount*3];
	for (int i = 0; i < m_triCount*3; i += 3)
	{
		const float* v0 = &m_verts[m_tris[i]*3];
		const float* v1 = &m_verts[m_tris[i+1]*3];
		const float* v2 = &m_verts[m_tris[i+2]*3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &m_normals[i];
		n[0] = e0[1]*e1[2] - e0[2]*e1[1];
		n[1] = e0[2]*e1[0] - e0[0]*e1[2];
		n[2] = e0[0]*e1[1] - e0[1]*e1[0];
		float d = sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
		if (d > 0)
		{
			d = 1.0f/d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}
	
	strncpy(m_filename, filename, sizeof(m_filename));
	m_filename[sizeof(m_filename)-1] = '\0';
	
	return true;
}
