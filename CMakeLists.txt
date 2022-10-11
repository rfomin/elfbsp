# AJBSP CMake Script
cmake_minimum_required(VERSION 3.0)

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
