#include <map>
#include <set>

#include "VMapManager2.h"
#include "MMapCommon.h"
#include "MapBuilder.h"
#include "TileBuilder.h"
#include "MapTree.h"

using namespace std;
using namespace MMAP;

bool checkDirectories(bool debugOutput)
{
    vector<string> dirFiles;

    if(!getDirContents(dirFiles, "maps") || !dirFiles.size())
    {
        printf("'maps' directory is empty or does not exist\n");
        return false;
    }

    dirFiles.clear();
    if(!getDirContents(dirFiles, "vmaps", "*.vmtree") || !dirFiles.size())
    {
        printf("'vmaps' directory is empty or does not exist\n");
        return false;
    }

    dirFiles.clear();
    if(!getDirContents(dirFiles, "mmaps"))
    {
        printf("'mmaps' directory does not exist\n");
        return false;
    }

    dirFiles.clear();
    if(debugOutput)
        if(!getDirContents(dirFiles, "Meshes"))
        {
            printf("'Meshes' directory does not exist (no place to put debugOutput files)\n");
            return false;
        }

    return true;
}

void handleArgs(int argc, char** argv,
               int &mapnum,
               float &maxAngle,
               bool &skipContinents,
               bool &skipJunkMaps,
               bool &skipBattlegrounds,
               bool &hiResHeightmaps,
               bool &debugOutput,
               bool &invalidMapNum)
{
    int i;
    char* param;
    for(i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "--maxAngle") == 0)
        {
            param = argv[++i];
            float maxangle = atof(param);
            if(maxangle <= 90.f && maxangle >= 45.f)
                maxAngle = maxangle;
            else
                printf("invalid option for '--maxAngle', using default\n");
        }
        else if(strcmp(argv[i], "--skipContinents") == 0)
        {
            param = argv[++i];
            if(strcmp(param, "true") == 0)
                skipContinents = true;
            else if(strcmp(param, "false") == 0)
                skipContinents = false;
            else
                printf("invalid option for '--skipContinents', using default\n");
        }
        else if(strcmp(argv[i], "--skipJunkMaps") == 0)
        {
            param = argv[++i];
            if(strcmp(param, "true") == 0)
                skipJunkMaps = true;
            else if(strcmp(param, "false") == 0)
                skipJunkMaps = false;
            else
                printf("invalid option for '--skipJunkMaps', using default\n");
        }
        else if(strcmp(argv[i], "--skipBattlegrounds") == 0)
        {
            param = argv[++i];
            if(strcmp(param, "true") == 0)
                skipBattlegrounds = true;
            else if(strcmp(param, "false") == 0)
                skipBattlegrounds = false;
            else
                printf("invalid option for '--skipBattlegrounds', using default\n");
        }
        else if(strcmp(argv[i], "--hiResHeightmaps") == 0)
        {
            param = argv[++i];
            if(strcmp(param, "true") == 0)
                hiResHeightmaps = true;
            else if(strcmp(param, "false") == 0)
                hiResHeightmaps = false;
            else
                printf("invalid option for '--hiResHeightmaps', using default\n");
        }
        else if(strcmp(argv[i], "--debugOutput") == 0)
        {
            param = argv[++i];
            if(strcmp(param, "true") == 0)
                debugOutput = true;
            else if(strcmp(param, "false") == 0)
                debugOutput = false;
            else
                printf("invalid option for '--debugOutput', using default true\n");
        }
        else
        {
            int map = atoi(argv[i]);
            if(map > 0 || (map == 0 && (strcmp(argv[i], "0") == 0)))
                mapnum = map;
            else
            {
                printf("invalid map number specified\n");
                invalidMapNum = true;
            }
        }
    }
}

int finish(char* message, int returnValue)
{
    printf("%s", message);
    getchar();
    return returnValue;
}

int main(int argc, char** argv)
{
    int mapnum = -1;
    float maxAngle = 60.f;
    bool skipContinents = true,
         skipJunkMaps = true,
         skipBattlegrounds = true,
         hiResHeightmaps = false,
         debugOutput = false,
         invalidMapNum = false;

    handleArgs(argc,
              argv,
              mapnum,
              maxAngle,
              skipContinents,
              skipJunkMaps,
              skipBattlegrounds,
              hiResHeightmaps,
              debugOutput,
              invalidMapNum);

    if(invalidMapNum)
    {
        printf("Specify a valid (non-negative, integer) map number.");
        return -1;
    }

    if(mapnum == -1 && debugOutput)
    {
        printf("You have specifed debug output, but didn't specify a map to generate.\n");
        printf("This will generate debug output for ALL maps.\n");
        printf("Are you sure you want to continue? (y/n) ");
        if(getchar() != 'y')
            return 0;
    }

    if(!checkDirectories(debugOutput))
        return finish("Press any key to close...", -1);

    MapBuilder builder(maxAngle,
                       skipContinents,
                       skipJunkMaps,
                       skipBattlegrounds,
                       hiResHeightmaps,
                       debugOutput);

    if(mapnum >= 0)
        builder.build(uint32(mapnum));
    else
        builder.buildAll();

    return finish("Movemap build is complete!", 1);
}
