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
project (MANGOS_VMAP_ASSEMB_IO)

set(CMAKE_VERBOSE_MAKEFILE true)

# uncomment next line to disable debug mode
ADD_DEFINITIONS("-DIOMAP_DEBUG")

ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-ggdb")

include_directories(../../src/shared/vmap/)
include_directories(../../dep/include/g3dlite/)
include_directories(../../dep/ACE_wrappers/)
include_directories(../../objdir/dep/ACE_wrappers)
include_directories(../../src/framework/)

add_library(g3dlite ../../dep/src/g3dlite/AABox.cpp
	../../dep/src/g3dlite/Box.cpp
	../../dep/src/g3dlite/Crypto.cpp
	../../dep/src/g3dlite/format.cpp
	../../dep/src/g3dlite/Matrix3.cpp
	../../dep/src/g3dlite/Plane.cpp
	../../dep/src/g3dlite/System.cpp
	../../dep/src/g3dlite/Triangle.cpp
	../../dep/src/g3dlite/Vector3.cpp
	../../dep/src/g3dlite/Vector4.cpp
	)

add_library(vmap
	../../src/shared/vmap/BaseModel.cpp
	../../src/shared/vmap/CoordModelMapping.cpp
#	../../src/shared/vmap/DebugCmdLogger.cpp
#	../../src/shared/vmap/ManagedModelContainer.cpp
	../../src/shared/vmap/ModelContainer.cpp
	../../src/shared/vmap/SubModel.cpp
	../../src/shared/vmap/TileAssembler.cpp
	../../src/shared/vmap/TreeNode.cpp
#	../../src/shared/vmap/VMapFactory.cpp
#	../../src/shared/vmap/VMapManager.cpp
	)

target_link_libraries(vmap g3dlite)

add_executable(vmap_assembler vmap_assembler.cpp)
target_link_libraries(vmap_assembler vmap)
