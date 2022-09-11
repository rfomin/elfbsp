# AJBSP CMake Script
cmake_minimum_required(VERSION 3.2.2)

project(ajbsp)

set(AJBSP_SOURCES
	src/ajbsp.cc
	src/level.cc
	src/node.cc
	src/misc.cc
	src/utility.cc
	src/wad.cc
)

add_executable(ajbsp
	${AJBSP_SOURCES}
)
