//------------------------------------------------------------------------
//  MAIN PROGRAM
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

#include "main.h"

#include <time.h>

#ifndef WIN32
#include <time.h>
#endif


//
//  global variables
//

bool opt_fast		= false;
bool opt_verbose	= false;

bool opt_no_gl		= false;
bool opt_force_v5	= false;
bool opt_force_xnod	= false;

int  opt_split_cost	= DEFAULT_FACTOR;

bool opt_help		= false;
bool opt_version	= false;


std::vector< const char * > wad_list;

const char *Level_name;

map_format_e Level_format;



//
//  show an error message and terminate the program
//
void FatalError(const char *fmt, ...)
{
	va_list arg_ptr;

	static char buffer[MSG_BUF_LEN];

	va_start(arg_ptr, fmt);
	vsnprintf(buffer, MSG_BUF_LEN-1, fmt, arg_ptr);
	va_end(arg_ptr);

	buffer[MSG_BUF_LEN-1] = 0;

	if (true)
	{
		fprintf(stderr, "\nFATAL ERROR: %s", buffer);
	}
	else //???
	{
		LogPrintf("\nFATAL ERROR: %s", buffer);
	}

	exit(2);
}


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
	info->factor	= CLAMP(1, opt_split_cost, 31);

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
		FatalError("Cannot build nodes unless you are editing a PWAD.");
		return;
	}


	// this probably cannot happen, but check anyway
	if (edit_wad->LevelCount() == 0)
	{
		FatalError("Cannot build nodes: no levels found!");
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


void VisitFile(unsigned int idx, const char *filename)
{
	// FIXME !!!
}


// ----- user information -----------------------------

static void ShowHelp()
{
	printf(	"AJBSP is free software, under the terms of the GNU General\n"
			"Public License (GPL), and comes with ABSOLUTELY NO WARRANTY.\n"
	//		"Home page: https://gitlab.com/andwj/ajbsp\n"
			"\n");

	printf( "Usage: ajbsp [options...] FILE...\n"
			"\n"
			"Available options are:\n");

//FIXME	M_PrintCommandLineOptions(stdout);

	fflush(stdout);
}


static void ShowVersion()
{
	printf("ajbsp " AJBSP_VERSION " (" __DATE__ ")\n");

	fflush(stdout);
}


void ParseShortArgument(const char *arg)
{
	// skip the leading '-'
	arg++;

	// FIXME
}


int ParseLongArgument(const char *name, int argc, char *argv[])
{
	// FIXME

	return 0;
}


void ParseCommandLine(int argc, char *argv[])
{
	// skip program name
	argc--;
	argv++;

	bool rest_are_files = false;

	while (argc > 0)
	{
		const char *arg = *argv++;
		argc--;

#ifdef __APPLE__
		// ignore MacOS X rubbish
		if (strncmp(arg, "-psn_", 5) == 0)
			continue;
#endif

		if (strlen(arg) == 0)
			continue;

		// a normal filename?
		if (arg[0] != '-' || rest_are_files)
		{
			wad_list.push_back(arg);
			continue;
		}

		if (strcmp(arg, "-") == 0)
		{
			// FIXME : bad option error
		}

		if (strcmp(arg, "--") == 0)
		{
			rest_are_files = true;
			continue;
		}

		// handle short args which are isolate and require a value
		if (strcmp(arg, "-c") == 0) arg = "--cost";
		if (strcmp(arg, "-m") == 0) arg = "--map";

		if (arg[1] != '-')
		{
			ParseShortArgument(arg);
			continue;
		}

		int count = ParseLongArgument(arg, argc, argv);

		if (count > 0)
		{
			argc -= count;
			argv += count;
		}
	}
}


//
//  the program starts here
//
int main(int argc, char *argv[])
{
	// sanity checks type sizes (useful when porting)
	CheckTypeSizes();


	ParseCommandLine(argc, argv);

	if (opt_version)
	{
		ShowVersion();
		return 0;
	}

	LogPrintf("\n");
	LogPrintf("**** " AJBSP_TITLE " v" AJBSP_VERSION " (C) 2018 Andrew Apted, et al ****\n");
	LogPrintf("\n");

	if (opt_help || wad_list.size() == 0)
	{
		ShowHelp();
		return 0;
	}


	for (unsigned int i = 0 ; i < wad_list.size() ; i++)
	{
		VisitFile(i, wad_list[i]);
	}


	// that's all folks!

	LogPrintf("Quit\n");

	return 0;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
