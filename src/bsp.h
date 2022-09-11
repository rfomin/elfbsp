//------------------------------------------------------------------------
//
//  AJ-BSP  Copyright (C) 2000-2018  Andrew Apted, et al
//          Copyright (C) 1994-1998  Colin Reed
//          Copyright (C) 1997-1998  Lee Killough
//
//  Originally based on the program 'BSP', version 2.3.
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

#ifndef __AJBSP_BSP_H__
#define __AJBSP_BSP_H__

namespace ajbsp
{

#define AJBSP_VERSION  "1.02"

// Node Build Information Structure
//
// Memory note: when changing the string values here (and in
// nodebuildcomms_t) they should be freed using StringFree() and
// allocated with StringDup().  The application has the final
// responsibility to free the strings in here.
//
#define DEFAULT_FACTOR  11

class nodebuildinfo_t
{
public:
	int factor;

	bool gl_nodes;

	// when these two are false, they create an empty lump
	bool do_blockmap;
	bool do_reject;

	bool fast;
	bool warnings;	// NOTE: not currently used

	bool force_v5;
	bool force_xnod;
	bool force_compress;	// NOTE: only supported when HAVE_ZLIB is defined

	// the GUI can set this to tell the node builder to stop
	bool cancelled;

	// from here on, various bits of internal state
	int total_failed_maps;
	int total_warnings;
	int total_minor_issues;

public:
	nodebuildinfo_t() :
		factor(DEFAULT_FACTOR),

		gl_nodes(true),

		do_blockmap(true),
		do_reject  (true),

		fast(false),
		warnings(false),

		force_v5(false),
		force_xnod(false),
		force_compress(false),

		cancelled(false),

		total_failed_maps(0),
		total_warnings(0),
		total_minor_issues(0)
	{ }

	~nodebuildinfo_t()
	{ }
};


typedef enum
{
	// everything went peachy keen
	BUILD_OK = 0,

	// building was cancelled
	BUILD_Cancelled,

	// the WAD file was corrupt / empty / bad filename
	BUILD_BadFile,

	// when saving the map, one or more lumps overflowed
	BUILD_LumpOverflow
}
build_result_e;


build_result_e AJBSP_BuildLevel(nodebuildinfo_t *info, short lev_idx);


}  // namespace ajbsp


#endif /* __AJBSP_BSP_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
