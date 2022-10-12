# AJBSP CMake Script
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(ajbsp)

add_executable(ajbsp
	src/ajbsp.cc
	src/level.cc
	src/node.cc
	src/misc.cc
	src/parse.cc
	src/utility.cc
	src/wad.cc
)
