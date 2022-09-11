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

//
// Node Build Information Structure
//

#define SPLIT_COST_MIN       1
#define SPLIT_COST_DEFAULT  11
#define SPLIT_COST_MAX      32

class buildinfo_t
{
public:
	// use a faster method to pick nodes
	bool fast;

	bool gl_nodes;

	// when these two are false, they create an empty lump
	bool do_blockmap;
	bool do_reject;

	bool force_v5;
	bool force_xnod;
	bool force_compress;	// NOTE: only supported when HAVE_ZLIB is defined

	// the GUI can set this to tell the node builder to stop
	bool cancelled;

	int split_cost;

	// this affects how some messages are shown
	int verbosity;

	// from here on, various bits of internal state
	int total_failed_maps;
	int total_warnings;
	int total_minor_issues;

public:
	buildinfo_t() :
		fast(false),

		gl_nodes(true),

		do_blockmap(true),
		do_reject  (true),

		force_v5(false),
		force_xnod(false),
		force_compress(false),

		cancelled(false),

		split_cost(SPLIT_COST_DEFAULT),
		verbosity(0),

		total_failed_maps(0),
		total_warnings(0),
		total_minor_issues(0)
	{ }

	~buildinfo_t()
	{ }

public:
	virtual void Print(int level, const char *msg, ...) = 0;
	virtual void Debug(const char *msg, ...) = 0;
	virtual void ShowMap(const char *name) = 0;
	virtual void FatalError(const char *fmt, ...) = 0;
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


build_result_e AJBSP_BuildLevel(buildinfo_t *info, short lev_idx);


}  // namespace ajbsp


#endif /* __AJBSP_BSP_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
