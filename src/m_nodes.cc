//------------------------------------------------------------------------
//  BUILDING NODES
//------------------------------------------------------------------------
//
//  AJ-BSP  Copyright (C) 2012-2018  Andrew Apted
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


// config items
bool opt_fast		= false;
bool opt_verbose	= false;

bool opt_no_gl		= false;
bool opt_force_v5	= false;
bool opt_force_xnod	= false;

int  opt_factor		= DEFAULT_FACTOR;


extern bool inhibit_node_build;


//------------------------------------------------------------------------


static nodebuildinfo_t * nb_info;

static char message_buf[MSG_BUF_LEN];


static const char *build_ErrorString(build_result_e ret)
{
	switch (ret)
	{
		case BUILD_OK: return "OK";

		// building was cancelled
		case BUILD_Cancelled: return "Cancelled by User";

		// the WAD file was corrupt / empty / bad filename
		case BUILD_BadFile: return "Bad File";

		// file errors
		case BUILD_ReadError:  return "Read Error";
		case BUILD_WriteError: return "Write Error";

		default: return "Unknown Error";
	}
}


void GB_PrintMsg(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vsnprintf(message_buf, MSG_BUF_LEN, str, args);
	va_end(args);

	message_buf[MSG_BUF_LEN-1] = 0;

	LogPrintf("BSP: %s", message_buf);
}


static void PrepareInfo(nodebuildinfo_t *info)
{
	info->factor	= CLAMP(1, opt_factor, 31);

	info->gl_nodes	= ! opt_no_gl;
	info->fast		= opt_fast;
	info->warnings	= opt_verbose;

	info->force_v5			= opt_force_v5;
	info->force_xnod		= opt_force_xnod;
	info->force_compress	= false;

	info->total_failed_maps    = 0;
	info->total_warnings       = 0;
	info->total_minor_warnings = 0;

	// clear cancelled flag
	info->cancelled = false;
}


static build_result_e BuildAllNodes(nodebuildinfo_t *info)
{
	LogPrintf("\n");

	// sanity check

	SYS_ASSERT(1 <= info->factor && info->factor <= 32);

	int num_levels = edit_wad->LevelCount();
	SYS_ASSERT(num_levels > 0);

	GB_PrintMsg("\n");

	build_result_e ret;

	// loop over each level in the wad
	for (int n = 0 ; n < num_levels ; n++)
	{
		// TODO : support a limited set of maps

		ret = AJBSP_BuildLevel(info, n);

		if (ret != BUILD_OK)
			break;

		if (false /* WantCancel() */)
		{
			nb_info->cancelled = true;
		}
	}

	if (ret == BUILD_OK)
	{
		GB_PrintMsg("\n");
		GB_PrintMsg("Total filed maps: %d\n", info->total_failed_maps);
		GB_PrintMsg("Total warnings: %d serious, %d minor\n", info->total_warnings,
					info->total_minor_warnings);
	}
	else if (ret == BUILD_Cancelled)
	{
		GB_PrintMsg("\n");
		GB_PrintMsg("Building CANCELLED.\n\n");
	}
	else
	{
		// build nodes failed
		GB_PrintMsg("\n");
		GB_PrintMsg("Building FAILED: %s\n", build_ErrorString(ret));
		GB_PrintMsg("Reason: %s\n\n", nb_info->message);
	}

	return ret;
}


void CMD_BuildAllNodes()
{
	if (! edit_wad)
	{
		DLG_Notify("Cannot build nodes unless you are editing a PWAD.");
		return;
	}


	// this probably cannot happen, but check anyway
	if (edit_wad->LevelCount() == 0)
	{
		DLG_Notify("Cannot build nodes: no levels found!");
		return;
	}


	nb_info = new nodebuildinfo_t;

	PrepareInfo(nb_info);

	build_result_e ret = BuildAllNodes(nb_info);


	if (ret == BUILD_OK)
	{
		Status_Set("Built nodes OK");
	}
	else if (nb_info->cancelled)
	{
		Status_Set("Cancelled");
	}
	else
	{
		Status_Set("Error building nodes");
	}

	delete nb_info; nb_info = NULL;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
