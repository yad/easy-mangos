## Process this file with automake to produce Makefile.in
## Sub-directories to parse
## CPP flags for includes, defines, etc.

AM_CPPFLAGS = $(MANGOS_INCLUDES) -I$(srcdir)/Include

## Build detour as convenience library.

noinst_LIBRARIES = libdetour.a

libdetour_a_SOURCES = \
  Source/DetourAlloc.cpp \
  Source/DetourCommon.cpp \
  Source/DetourNavMesh.cpp \
  Source/DetourNavMeshBuilder.cpp \
  Source/DetourNavMeshQuery.cpp \
  Source/DetourNode.cpp

## Additional files to include when running 'make dist'
#  Source and header files for detour.

EXTRA_DIST = \
  Include/DetourAlloc.h \
  Include/DetourAssert.h \
  Include/DetourCommon.h \
  Include/DetourNavMesh.h \
  Include/DetourNavMeshBuilder.h \
  Include/DetourNavMeshQuery.h \
  Include/DetourNode.h
