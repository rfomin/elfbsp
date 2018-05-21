//------------------------------------------------------------------------
//  MAIN DEFINITIONS
//------------------------------------------------------------------------
//
//  AJ-BSP  Copyright (C) 2001-2018  Andrew Apted
//          Copyright (C) 1994-1998  Colin Reed
//          Copyright (C) 1997-1998  Lee Killough
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------

#ifndef __AJBSP_MAIN_H__
#define __AJBSP_MAIN_H__


#define AJBSP_TITLE  "AJBSP"

#define AJBSP_VERSION  "0.50"


/*
 *  Standard headers
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <vector>


/*
 *  Source headers
 */
#include "sys_type.h"
#include "sys_macro.h"
#include "sys_endian.h"

#include "lib_util.h"
#include "lib_file.h"
#include "w_rawdef.h"
#include "w_wad.h"

#include "bsp.h"


// !!!! FIXME !!!!

#define SYS_ASSERT(...)  do {} while (0)
#define LogPrintf(...)   do {} while (0)
#define DebugPrintf(...) do {} while (0)

#define AJ_PATH_MAX  4096
#define MSG_BUF_LEN  1024


/*
 *  Interfile global variables
 */

extern const char *install_dir;  // install dir (e.g. /usr/share/eureka)
extern const char *home_dir;     // home dir (e.g. $HOME/.eureka)
extern const char *cache_dir;    // for caches and backups, can be same as home_dir

extern const char *Game_name;   // Name of game "doom", "doom2", "heretic", ...
extern const char *Port_name;   // Name of source port "vanilla", "boom", ...
extern const char *Level_name;  // Name of map lump we are editing

extern map_format_e Level_format; // format of current map

extern const char *config_file; // Name of the configuration file, or NULL
extern const char *log_file;    // Name of log file, or NULL

extern const char *Iwad_name;   // Filename of the iwad
extern const char *Pwad_name;   // Filename of current wad, or NULL

extern std::vector< const char * > Pwad_list;


extern int   show_help;		// Print usage message and exit.
extern int   show_version;	// Print version info and exit.


/*
 *  Various global functions
 */

#ifdef __GNUC__
__attribute__((noreturn))
#endif
void FatalError(const char *fmt, ...);

#define BugError  FatalError


void Status_Set(const char *fmt, ...);
void Status_Clear();


#endif  /* __AJBSP_MAIN_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
