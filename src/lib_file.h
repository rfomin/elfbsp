//------------------------------------------------------------------------
//  File Utilities
//------------------------------------------------------------------------
//
//  Copyright (C) 2006-2012 Andrew Apted
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

#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#ifdef WIN32
#define DIR_SEP_CH   '\\'
#define DIR_SEP_STR  "\\"
#else
#define DIR_SEP_CH   '/'
#define DIR_SEP_STR  "/"
#endif

// filename functions
bool HasExtension(const char *filename);
bool MatchExtension(const char *filename, const char *ext);
char *ReplaceExtension(const char *filename, const char *ext);
const char *FindBaseName(const char *filename);

// file utilities
bool FileExists(const char *filename);
bool FileCopy(const char *src_name, const char *dest_name);
bool FileRename(const char *old_name, const char *new_name);
bool FileDelete(const char *filename);


#endif /* __LIB_FILE_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
