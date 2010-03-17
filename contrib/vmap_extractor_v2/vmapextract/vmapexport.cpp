/*****************************************************************************/
/* StormLibTest.cpp                       Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* This module uses very brutal test methods for StormLib. It extracts all   */
/* files from the archive with Storm.dll and with stormlib and compares them,*/
/* then tries to build a copy of the entire archive, then removes a few files*/
/* from the archive and adds them back, then compares the two archives, ...  */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 25.03.03  1.00  Lad  The first version of StormLibTest.cpp                */
/*****************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#include <cstdio>
#include <iostream>
#include <vector>
#include <list>

#ifdef WIN32
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif

#undef min
#undef max

//#pragma warning(disable : 4505)
//#pragma comment(lib, "Winmm.lib")

#include <map>

//From Extractor
#include "adtfile.h"
#include "wdtfile.h"
#include "dbcfile.h"
#include "mpq_libmpq.h"
#include "wmo.h"

//------------------------------------------------------------------------------
// Defines

#define MPQ_BLOCK_SIZE 0x1000

//-----------------------------------------------------------------------------

extern ArchiveSet gOpenArchives;

typedef struct
{
    char name[64];
    unsigned int id;
}map_id;

map_id * map_ids;

std::map<uint64, uint32> wmoAreaTable;

uint16 * areas;
uint16 *areamax;
uint32 map_count;
char output_path[128]=".";
char input_path[1024]=".";
bool hasInputPathParam = false;
char tmp[512];
bool preciseVectorData = false;
//char gamepath[1024];

//Convert function
//bool ConvertADT(char*,char*);

// Constants

//static const char * szWorkDirMaps = ".\\Maps";
static const char * szWorkDirWmo = "./Buildings";

//static LPBYTE pbBuffer1 = NULL;
//static LPBYTE pbBuffer2 = NULL;

// Local testing functions

static void clreol()
{
    printf("\r                                                                              \r");
}

void strToLower(char* str)
{
    while(*str)
    {   
        *str=tolower(*str);
        ++str;
    }   
}

static const char * GetPlainName(const char * szFileName)
{
    const char * szTemp;

    if((szTemp = strrchr(szFileName, '\\')) != NULL)
        szFileName = szTemp + 1;
    return szFileName;
}

static void ShowProcessedFile(const char * szFileName)
{
/* not truncate file names in output
    char szLine[80];
    size_t nLength = strlen(szFileName);

    memset(szLine, 0x20, sizeof(szLine));
    szLine[sizeof(szLine)-1] = 0;

    if(nLength > sizeof(szLine)-1)
        nLength = sizeof(szLine)-1;
    memcpy(szLine, szFileName, nLength);
    printf("\r%s\n", szLine);
*/
    printf("\r%s\n", szFileName);
}

int ExtractWmo()
{
    char   szLocalFile[1024] = "";
    bool success=true;

    //const char* ParsArchiveNames[] = {"patch-2.MPQ", "patch.MPQ", "common.MPQ", "expansion.MPQ"};

    for (ArchiveSet::const_iterator ar_itr = gOpenArchives.begin(); ar_itr != gOpenArchives.end() && success; ++ar_itr)
    {
        vector<string> filelist;
        
        (*ar_itr)->GetFileListTo(filelist);
        for (vector<string>::iterator fname=filelist.begin(); fname != filelist.end() && success; ++fname)
        {
            bool file_ok=true;
            if (fname->find(".wmo") != string::npos)
            {
                // Copy files from archive
                std::cout << "found *.wmo file " << *fname << std::endl;
                sprintf(szLocalFile, "%s/%s", szWorkDirWmo, GetPlainName(fname->c_str()));
                fixnamen(szLocalFile,strlen(szLocalFile));
                FILE * n;
                if ((n = fopen(szLocalFile, "rb"))== NULL)
                {
                    int p = 0;
                    //Select root wmo files
                    const char * rchr = strrchr(GetPlainName(fname->c_str()),0x5f);
                    if(rchr != NULL)
                    {
                        char cpy[4];
                        strncpy((char*)cpy,rchr,4);
                        for (int i=0;i<4; ++i)
                        {
                            int m = cpy[i];
                            if(isdigit(m))
                                p++;
                        }
                    }
                    if(p != 3)
                    {
                        printf("RootWmo!\n");
                        WMORoot * froot = new WMORoot(*fname);
                        if(!froot->open())
                        {
                            printf("Not open RootWmo!!!\n");
                            continue;
                        }
                        FILE *output=fopen(szLocalFile,"wb");
                        if(!output)
                        {
                            printf("couldn't open %s for writing!\n", szLocalFile);
                            success=false;
                        }
                        froot->ConvertToVMAPRootWmo(output);
                        int Wmo_nVertices = 0;
                        printf("root has %d groups\n", froot->nGroups);
                        if(froot->nGroups !=0)
                        {
                            for (int i=0; i<froot->nGroups; ++i)
                            {
                                char temp[1024];
                                strcpy(temp, fname->c_str());
                                temp[fname->length()-4] = 0;
                                char groupFileName[1024];
                                sprintf(groupFileName,"%s_%03d.wmo",temp, i);
                                printf("Trying to open groupfile %s\n",groupFileName);
                                //printf("GroupWmo!\n");
                                string s = groupFileName;
                                WMOGroup * fgroup = new WMOGroup(s);
                                if(!fgroup->open())
                                {
                                    printf("Could not open all Group file for: %s\n",GetPlainName(fname->c_str()));
                                    file_ok=false;
                                    break;
                                }
                                // try to search specific area
                                uint64 key = uint64(froot->RootWMOID)<<32 | fgroup->groupWMOID;
                                std::map<uint64, uint32>::iterator itr = wmoAreaTable.find(key);

                                uint32 areaID;
                                if(itr == wmoAreaTable.end())
                                {
                                    // fallback, use root area
                                    key = uint64(froot->RootWMOID)<<32;
                                    itr = wmoAreaTable.find(key);
                                    areaID = itr == wmoAreaTable.end()?0:itr->second;
                                }
                                else
                                {
                                    areaID = itr->second;
                                }

                                Wmo_nVertices += fgroup->ConvertToVMAPGroupWmo(output, areaID, preciseVectorData);
                            }
                        }
                        fseek(output, 8, SEEK_SET); // store the correct no of vertices
                        fwrite(&Wmo_nVertices,sizeof(int),1,output);
                        fclose(output);
                    }
                }
                else
                {
                    fclose(n);
                }
            }
            // Delete the extracted file in the case of an error
            if(!file_ok)
                remove(szLocalFile);
        }
    }

    if(success)
        printf("\nExtract wmo complete (No (fatal) errors)\n");

    return success;
}

void ExtractMapsFromMpq()
{
}

void ParsMapFiles()
{
    char fn[512];
    char id_filename[64];
    char id[10];
    for (unsigned int i=0; i<map_count; ++i)
    {
        sprintf(id,"%03u",map_ids[i].id);
        sprintf(fn,"World\\Maps\\%s\\%s.wdt", map_ids[i].name, map_ids[i].name);
        WDTFile WDT(fn,map_ids[i].name);
        if(WDT.init(id))
        {
            for (int x=0; x<64; ++x)
            {
                for (int y=0; y<64; ++y)
                {
                    if (ADTFile*ADT = WDT.GetMap(x,y))
                    {
                        sprintf(id_filename,"%02u %02u %03u",x,y,map_ids[i].id);//!!!!!!!!!
                        ADT->init(id_filename);
                        delete ADT;
                    }
                }
            }
        }
    }
}
#if 0
void ParsMapFiles()
{
    char fn[512];
    for (unsigned int i=0; i<map_count; ++i)
    {
        sprintf(fn,"World\\Maps\\%s\\%s.wdt", map_ids[i].name, map_ids[i].name);
        WDTFile WDT(fn,map_ids[i].name);
        if(WDT.init())
        {
            for (int x=0; x<64; ++x)
            {
                for (int y=0; y<64; ++y)
                {
                    if (ADTFile*ADT = WDT.GetMap(x,y))
                    {
                        ADT->init();
                        delete ADT;
                    }
                }
            }
        }
    }
}
#endif

void getGamePath()
{
#ifdef _WIN32
    HKEY key;
    DWORD t,s;
    LONG l;
    s = sizeof(input_path);
    memset(input_path,0,s);
    l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Blizzard Entertainment\\World of Warcraft",0,KEY_QUERY_VALUE,&key);
    //l = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Blizzard Entertainment\\Burning Crusade Closed Beta",0,KEY_QUERY_VALUE,&key);
    l = RegQueryValueEx(key,"InstallPath",0,&t,(LPBYTE)input_path,&s);
    RegCloseKey(key);
    if (strlen(input_path) > 0)
    {
        if (input_path[strlen(input_path) - 1] != '\\') strcat(input_path, "\\");
    }
    strcat(input_path,"Data\\");
#else
    strcpy(input_path,"data/");
#endif
}

bool scan_patches(char* scanmatch, std::vector<std::string>& pArchiveNames)
{
    int i;
    char path[512];

    for (i = 1; i <= 99; i++)
    {
        if (i != 1)
        {
            sprintf(path, "%s-%d.MPQ", scanmatch, i);
        }
        else
        {
            sprintf(path, "%s.MPQ", scanmatch);
        }
#ifdef __linux__
        if(FILE* h = fopen64(path, "rb"))
#else
        if(FILE* h = fopen(path, "rb"))
#endif
        {
            fclose(h);
            //matches.push_back(path);
            pArchiveNames.push_back(path);
        }
    }

    printf("\n");

    return(true);
}

bool fillArchiveNameVector(std::vector<std::string>& pArchiveNames)
{
    if(!hasInputPathParam)
        getGamePath();

    printf("\nGame path: %s\n", input_path);

    char path[512];
    string in_path(input_path);
    std::vector<std::string> locales;

    locales.push_back("enGB");
    locales.push_back("enUS");
    locales.push_back("deDE");
    locales.push_back("esES");
    locales.push_back("frFR");
    locales.push_back("koKR");
    locales.push_back("ruRU");

    printf("\n");

    // open locale expansion and common files
    printf("Opening data files from locale directories.\n");
    for (std::vector<std::string>::iterator i = locales.begin(); i != locales.end(); ++i)
    {
        pArchiveNames.push_back(in_path + *i + "/locale-" + *i + ".MPQ");
        pArchiveNames.push_back(in_path + *i + "/expansion-locale-" + *i + ".MPQ");
        pArchiveNames.push_back(in_path + *i + "/lichking-locale-" + *i + ".MPQ");
        printf("\n");
    }

    // open expansion and common files
    pArchiveNames.push_back(input_path + string("common.MPQ"));
    pArchiveNames.push_back(input_path + string("common-2.MPQ"));
    pArchiveNames.push_back(input_path + string("expansion.MPQ"));
    pArchiveNames.push_back(input_path + string("lichking.MPQ"));

    // now, scan for the patch levels in the core dir
    printf("Loading patch levels from data directory.\n");
    sprintf(path, "%spatch", input_path);
    if (!scan_patches(path, pArchiveNames))
        return(false);

    printf("\n");

    // now, scan for the patch levels in locale dirs
    printf("Loading patch levels from locale directories.\n");
    bool foundOne = false;
    for (std::vector<std::string>::iterator i = locales.begin(); i != locales.end(); ++i)
    {
        printf("Locale: %s\n", i->c_str());
        sprintf(path, "%s%s/patch-%s", input_path, i->c_str(), i->c_str());
        if(scan_patches(path, pArchiveNames))
            foundOne = true;
    }
    if(!foundOne)
    {
        printf("no locale found\n");
        return false;
    }

    return true;
}

bool processArgv(int argc, char ** argv, const char *versionString)
{
    bool result = true;
    hasInputPathParam = false;
    bool preciseVectorData = false;

    for(int i=1; i< argc; ++i)
    {
        if(strcmp("-s",argv[i]) == 0)
        {
            preciseVectorData = false;
        }
        else if(strcmp("-d",argv[i]) == 0)
        {
            if((i+1)<argc)
            {
                hasInputPathParam = true;
                strcpy(input_path, argv[i+1]);
                if (input_path[strlen(input_path) - 1] != '\\' || input_path[strlen(input_path) - 1] != '/')
                    strcat(input_path, "/");
                ++i;
            }
            else
            {
                result = false;
            }
        }
        else if(strcmp("-?",argv[1]) == 0)
        {
            result = false;
        }
        else if(strcmp("-l",argv[i]) == 0)
        {
            preciseVectorData = true;
        }
        else
        {
            result = false;
            break;
        }
    }
    if(!result)
    {
        printf("Extract %s.\n",versionString);
        printf("%s [-?][-s][-l][-d <path>]\n", argv[0]);
        printf("   -s : (default) small size (data size optimization), ~500MB less vmap data.\n");
        printf("   -l : large size, ~500MB more vmap data. (might contain more details)\n");
        printf("   -d <path>: Path to the vector data source folder.\n");
        printf("   -? : This message.\n");
    }
    return result;
}

void loadWMOAreaTable()
{
    DBCFile * dbc = new DBCFile("DBFilesClient\\WMOAreaTable.dbc");
    if(!dbc->open())
    {
        delete dbc;
        printf("FATAL ERROR: WMOAreaTable.dbc not found in data file.\n");
        exit(1);
    }
    uint32 wmo_area_count = dbc->getRecordCount();
    std::cout << "WMOAreaTable.dbc has " << wmo_area_count << " entries\n";
    for(uint32 x=0; x<wmo_area_count; ++x)
    {
        int32 rootId = dbc->getRecord(x).getInt(1);
        int32 groupId = dbc->getRecord(x).getInt(3);
        if(groupId == -1)
            groupId = 0;
        uint32 areaId = dbc->getRecord(x).getUInt(10);
        if(rootId== 5164) std::cout << "5164|" << groupId << " => " << areaId << endl;
        if(!areaId)
            continue;
        uint64 key = uint64(rootId)<<32 | groupId;
        wmoAreaTable.insert(std::pair<uint64, uint32>(key, areaId));
    }
    delete dbc;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// Main
// 
// The program must be run with two command line arguments
//
// Arg1 - The source MPQ name (for testing reading and file find)
// Arg2 - Listfile name
//

int main(int argc, char ** argv)
{
    //char tmp[512];
//    FILE* pDatei;
//    char tmp[512];
//    char tmp1[512];
    //char tmp2[512];
//    char tmp3[512];
//    char tmp4[512];
//    char szMpqName[MAX_PATH] = "";
//    char szListFile[MAX_PATH] = "";
    //int nError = ERROR_SUCCESS;
    bool success=true;
    const char *versionString = "V2.4 2007_07_12";

    // Use command line arguments, when some
    if(!processArgv(argc, argv, versionString))
        return 1;

    printf("Extract %s. Beginning work ....\n",versionString);
    //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    // Create the working directory
    //if(nError == ERROR_SUCCESS)
    //{
        //if(!CreateDirectory(szWorkDirMaps, NULL))
        // nError = GetLastError();
        if(mkdir(szWorkDirWmo
#ifdef __linux__
                    , 0711
#endif
                    ))
// TODO: replace GetLastError() call to catch already existing dir case
            success=false;
//            nError = GetLastError();
//        if(nError == ERROR_ALREADY_EXISTS)
//            nError = ERROR_SUCCESS;
    //}

    // prepare archive name list
    std::vector<std::string> archiveNames;
    fillArchiveNameVector(archiveNames);
    for (size_t i=0; i < archiveNames.size(); ++i)
    {
        MPQArchive *archive = new MPQArchive(archiveNames[i].c_str());
        if(gOpenArchives.front() != archive)
            delete archive;
    }

    if(gOpenArchives.empty())
    {
        printf("FATAL ERROR: None MPQ archive found by path '%s'. Use -d option with proper path.\n",input_path);
        return 1;
    }

    loadWMOAreaTable();
    // extract data
    if(success)
        success = ExtractWmo();

    //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    //map.dbc
    if(success)
    {
        DBCFile * dbc = new DBCFile("DBFilesClient\\Map.dbc");
        if(!dbc->open())
        {
            delete dbc;
            printf("FATAL ERROR: Map.dbc not found in data file.\n");
            return 1;
        }
        map_count=dbc->getRecordCount ();
        map_ids=new map_id[map_count];
        for(unsigned int x=0;x<map_count;++x)
        {
            map_ids[x].id=dbc->getRecord (x).getUInt(0);
            strcpy(map_ids[x].name,dbc->getRecord(x).getString(1));
            printf("Map - %s\n",map_ids[x].name);
        }


        delete dbc;
        ParsMapFiles();
        delete [] map_ids;
        //nError = ERROR_SUCCESS;
    }

    clreol();
    if(!success)
    {
        printf("ERROR: Extract %s. Work NOT complete.\n   Precise vector data=%d.\nPress any key.\n",versionString, preciseVectorData);
        getchar();
    }

    printf("Extract %s. Work complete. No errors.",versionString);
}
