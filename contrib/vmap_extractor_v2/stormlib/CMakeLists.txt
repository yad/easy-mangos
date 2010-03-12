# Copyright (C) 2005-2009 MaNGOS project <http://getmangos.com/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

cmake_minimum_required (VERSION 2.6)
project (MANGOS_VMAP_EXTRACT_IO)

add_library(storm
	./SCompression.cpp
	./SFileFindFile.cpp
	./SCommon.cpp
	./SListFile.cpp
	./SFileOpenArchive.cpp
	./SFileOpenFileEx.cpp
	./SFileCompactArchive.cpp
	./wave/wave.cpp
	./SFileReadFile.cpp
	./huffman/huff.cpp
	./SFileExtractFile.cpp
	./GfxDecode.cpp
	./StormPortLinux.cpp
	./SFileCreateArchiveEx.cpp
	./pklib/crc32.c
	./pklib/explode.c
	./pklib/implode.c
	./pklib/crc32_pk.c

	)

target_link_libraries(storm z bz2)
