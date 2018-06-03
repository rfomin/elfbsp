# ajBSP CMake Script
cmake_minimum_required(VERSION 3.2.2)
project(ajBSP)

# Find zlib first since ajbsp requires it
find_package( ZLIB REQUIRED )
include_directories(${ZLIB_INCLUDE_DIR})

set(ajBSP_SOURCES
	src/bsp_level.cc
	src/bsp_node.cc
	src/bsp_util.cc
	src/lib_file.cc
	src/lib_util.cc
	src/main.cc
	src/w_wad.cc
)
add_executable(ajBSP
	${ajBSP_SOURCES}
)

target_link_libraries(ajBSP
${ZLIB_LIBRARY_RELEASE}
)
