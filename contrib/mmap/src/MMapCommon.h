#ifndef _MMAP_COMMON_H
#define _MMAP_COMMON_H

#include <string>
#include <vector>

#include "platform/Define.h"

using namespace std;

namespace MMAP
{
    enum ListFilesResult : int
    {
        LISTFILE_DIRECTORY_NOT_FOUND = -1,
        LISTFILE_OK = 1
    };

    inline ListFilesResult getDirContents(vector<string> &fileList, string dirpath = ".", string filter = "*", bool includeSubDirs = false)
    {
    #ifdef WIN32
        HANDLE hFind;
        WIN32_FIND_DATA findFileInfo;
        string directory;

        directory = dirpath + "\\" + filter;

        hFind = FindFirstFile(directory.c_str(), &findFileInfo);

        if(hFind == INVALID_HANDLE_VALUE)
            return LISTFILE_DIRECTORY_NOT_FOUND;

        do
        {
            if(includeSubDirs || (findFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                fileList.push_back(findFileInfo.cFileName);
        }
        while (FindNextFile(hFind, &findFileInfo));
    #else
        // TODO: *nix file list
    #endif

        return LISTFILE_OK;
    }
}

#endif
