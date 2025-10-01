# ELFBSP CMake Script
cmake_minimum_required(VERSION 3.15)
project(elfbsp
        VERSION "1.7"
        LANGUAGES C CXX)

include(CheckIncludeFile)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

set(PROJECT_COMPANY "Guilherme Miranda, et al")
set(PROJECT_COPYRIGHT "Copyright (C) 1994-2025")
set(PROJECT_LICENSE "GNU General Public License, version 2")
set(PROJECT_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")

configure_file(config.h.in config.h)

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Wno-unused-parameter
    -O3
    -fno-exceptions
    -fno-rtti
    -fno-strict-aliasing
    -fwrapv
)

add_executable(elfbsp
    src/elfbsp.cpp
    src/level.cpp
    src/node.cpp
    src/misc.cpp
    src/parse.cpp
    src/utility.cpp
    src/wad.cpp
)

target_include_directories(elfbsp PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(elfbsp PRIVATE z)

if(WIN32)
    install(FILES LICENSE.txt DESTINATION .)
    install(FILES README.md DESTINATION .)
    install(TARGETS elfbsp RUNTIME DESTINATION .)
else()
    install(FILES LICENSE.txt DESTINATION "share/doc/${PROJECT_NAME}")
    install(FILES README.md DESTINATION "share/doc/${PROJECT_NAME}")
    install(FILES doc/elfbsp.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
    install(TARGETS elfbsp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
