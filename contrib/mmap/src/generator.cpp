#include <map>
#include <set>

#include "VMapManager2.h"
#include "MMapCommon.h"
#include "MapBuilder.h"
#include "TileBuilder.h"
#include "MapTree.h"

using namespace std;
using namespace MMAP;

bool checkDirectories()
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
    if(!getDirContents(dirFiles, "Meshes"))
    {
        printf("'Meshes' directory does not exist\n");
        return false;
    }

    return true;
}

int finish(char* message, int returnValue)
{
    printf("%s", message);
    getchar();
    return returnValue;
}

int main(int argc, char* argv)
{
    if(!checkDirectories())
        return finish("Press any key to close...", -1);

    MapBuilder builder(60.f);

    builder.buildAll();

    return finish("Movemap build is complete!", 1);
}