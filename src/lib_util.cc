//------------------------------------------------------------------------
//  UTILITIES
//------------------------------------------------------------------------
//
//  Copyright (C) 2001-2018 Andrew Apted
//  Copyright (C) 1997-2003 André Majorel et al
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

#include "main.h"

#ifdef WIN32
#include <io.h>
#else  // UNIX or MACOSX
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef __APPLE__
#include <sys/param.h>
#endif

bool HasExtension(const char *filename)
{
	int A = (int)strlen(filename) - 1;

	if (A > 0 && filename[A] == '.')
		return false;

	for (; A >= 0 ; A--)
	{
		if (filename[A] == '.')
			return true;

		if (filename[A] == '/')
			break;

#ifdef WIN32
		if (filename[A] == '\\' || filename[A] == ':')
			break;
#endif
	}

	return false;
}

//
// MatchExtension
//
// When ext is NULL, checks if the file has no extension.
//
bool MatchExtension(const char *filename, const char *ext)
{
	if (! ext)
		return ! HasExtension(filename);

	int A = (int)strlen(filename) - 1;
	int B = (int)strlen(ext) - 1;

	for (; B >= 0 ; B--, A--)
	{
		if (A < 0)
			return false;

		if (toupper(filename[A]) != toupper(ext[B]))
			return false;
	}

	return (A >= 1) && (filename[A] == '.');
}


//
// ReplaceExtension
//
// When ext is NULL, any existing extension is removed.
//
// Returned string is a COPY.
//
char *ReplaceExtension(const char *filename, const char *ext)
{
	SYS_ASSERT(filename[0] != 0);

	size_t total_len = strlen(filename) + (ext ? strlen(ext) : 0);

	char *buffer = StringNew((int)total_len + 10);

	strcpy(buffer, filename);

	char *dot_pos = buffer + strlen(buffer) - 1;

	for (; dot_pos >= buffer && *dot_pos != '.' ; dot_pos--)
	{
		if (*dot_pos == '/')
			break;

#ifdef WIN32
		if (*dot_pos == '\\' || *dot_pos == ':')
			break;
#endif
	}

	if (dot_pos < buffer || *dot_pos != '.')
		dot_pos = NULL;

	if (! ext)
	{
		if (dot_pos)
			dot_pos[0] = 0;

		return buffer;
	}

	if (dot_pos)
		dot_pos[1] = 0;
	else
		strcat(buffer, ".");

	strcat(buffer, ext);

	return buffer;
}


const char *FindBaseName(const char *filename)
{
	// Find the base name of the file (i.e. without any path).
	// The result always points within the given string.
	//
	// Example:  "C:\Foo\Bar.wad"  ->  "Bar.wad"

	const char *pos = filename + strlen(filename) - 1;

	for (; pos >= filename ; pos--)
	{
		if (*pos == '/')
			return pos + 1;

#ifdef WIN32
		if (*pos == '\\' || *pos == ':')
			return pos + 1;
#endif
	}

	return filename;
}

//------------------------------------------------------------------------

bool FileExists(const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (fp)
	{
		fclose(fp);
		return true;
	}

	return false;
}


bool FileCopy(const char *src_name, const char *dest_name)
{
	char buffer[1024];

	FILE *src = fopen(src_name, "rb");
	if (! src)
		return false;

	FILE *dest = fopen(dest_name, "wb");
	if (! dest)
	{
		fclose(src);
		return false;
	}

	while (true)
	{
		size_t rlen = fread(buffer, 1, sizeof(buffer), src);
		if (rlen == 0)
			break;

		size_t wlen = fwrite(buffer, 1, rlen, dest);
		if (wlen != rlen)
			break;
	}

	bool was_OK = !ferror(src) && !ferror(dest);

	fclose(dest);
	fclose(src);

	return was_OK;
}


bool FileRename(const char *old_name, const char *new_name)
{
#ifdef WIN32
	return (::MoveFile(old_name, new_name) != 0);

#else // UNIX or MACOSX

	return (rename(old_name, new_name) == 0);
#endif
}


bool FileDelete(const char *filename)
{
#ifdef WIN32
	return (::DeleteFile(filename) != 0);

#else // UNIX or MACOSX

	return (remove(filename) == 0);
#endif
}

//------------------------------------------------------------------------

//
// a case-insensitive strcmp()
//
int y_stricmp(const char *s1, const char *s2)
{
	for (;;)
	{
		if (tolower(*s1) != tolower(*s2))
			return (int)(unsigned char)(*s1) - (int)(unsigned char)(*s2);

		if (*s1 && *s2)
		{
			s1++;
			s2++;
			continue;
		}

		// both *s1 and *s2 must be zero
		return 0;
	}
}


//
// a case-insensitive strncmp()
//
int y_strnicmp(const char *s1, const char *s2, size_t len)
{
	SYS_ASSERT(len != 0);

	while (len-- > 0)
	{
		if (tolower(*s1) != tolower(*s2))
			return (int)(unsigned char)(*s1) - (int)(unsigned char)(*s2);

		if (*s1 && *s2)
		{
			s1++;
			s2++;
			continue;
		}

		// both *s1 and *s2 must be zero
		return 0;
	}

	return 0;
}


//
// upper-case a string (in situ)
//
void y_strupr(char *str)
{
	for ( ; *str ; str++)
	{
		*str = toupper(*str);
	}
}


//
// lower-case a string (in situ)
//
void y_strlowr(char *str)
{
	for ( ; *str ; str++)
	{
		*str = tolower(*str);
	}
}


char *StringNew(int length)
{
	// length does not include the trailing NUL.

	char *s = (char *) calloc(length + 1, 1);

	if (! s)
		FatalError("Out of memory (%d bytes for string)\n", length);

	return s;
}


char *StringDup(const char *orig, int limit)
{
	if (! orig)
		return NULL;

	if (limit < 0)
	{
		char *s = strdup(orig);

		if (! s)
			FatalError("Out of memory (copy string)\n");

		return s;
	}

	char * s = StringNew(limit+1);
	strncpy(s, orig, limit);
	s[limit] = 0;

	return s;
}


char *StringUpper(const char *name)
{
	char *copy = StringDup(name);

	for (char *p = copy; *p; p++)
		*p = toupper(*p);

	return copy;
}


char *StringPrintf(const char *str, ...)
{
	// Algorithm: keep doubling the allocated buffer size
	// until the output fits. Based on code by Darren Salt.

	char *buf = NULL;
	int buf_size = 128;

	for (;;)
	{
		va_list args;
		int out_len;

		buf_size *= 2;

		buf = (char*)realloc(buf, buf_size);
		if (!buf)
			FatalError("Out of memory (formatting string)\n");

		va_start(args, str);
		out_len = vsnprintf(buf, buf_size, str, args);
		va_end(args);

		// old versions of vsnprintf() simply return -1 when
		// the output doesn't fit.
		if (out_len < 0 || out_len >= buf_size)
			continue;

		return buf;
	}
}


void StringFree(const char *str)
{
	if (str)
	{
		free((void*) str);
	}
}


//
// sanity checks for the sizes and properties of certain types.
// useful when porting.
//

#define assert_size(type,size)            \
  do                  \
  {                 \
    if (sizeof (type) != size)            \
      FatalError("sizeof " #type " is %d (should be " #size ")\n",  \
  (int) sizeof (type));           \
  }                 \
  while (0)

#define assert_wrap(type,high,low)          \
  do                  \
  {                 \
    type n = high;              \
    if (++n != low)             \
      FatalError("Type " #type " wraps around to %lu (should be " #low ")\n",\
  (unsigned long) n);           \
  }                 \
  while (0)


void CheckTypeSizes()
{
	assert_size(u8_t,  1);
	assert_size(s8_t,  1);
	assert_size(u16_t, 2);
	assert_size(s16_t, 2);
	assert_size(u32_t, 4);
	assert_size(s32_t, 4);

	assert_size(raw_linedef_t, 14);
	assert_size(raw_sector_s,  26);
	assert_size(raw_sidedef_t, 30);
	assert_size(raw_thing_t,   10);
	assert_size(raw_vertex_t,   4);
}


//
// rounds the value _up_ to the nearest power of two.
//
int RoundPOW2(int x)
{
	if (x <= 2)
		return x;

	x--;

	for (int tmp = x >> 1 ; tmp ; tmp >>= 1)
		x |= tmp;

	return x + 1;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
