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

int  opt_factor		= DEFAULT_FACTOR;

int show_help     = 0;
int show_version  = 0;


const char *Pwad_name = NULL;

std::vector< const char * > Pwad_list;

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


// ----- user information -----------------------------

static void ShowHelp()
{
	printf(	"\n"
			"*** " AJBSP_TITLE " v" AJBSP_VERSION " (C) 2018 Andrew Apted, et al ***\n"
			"\n");

	printf(	"AJBSP is free software, under the terms of the GNU General\n"
			"Public License (GPL), and comes with ABSOLUTELY NO WARRANTY.\n"
	//		"Home page: https://gitlab.com/andwj/ajbsp\n"
			"\n");

	printf( "USAGE: ajbsp [options...] [FILE...]\n"
			"\n"
			"Available options are:\n");

//FIXME	M_PrintCommandLineOptions(stdout);

	fflush(stdout);
}


static void ShowVersion()
{
	printf("AJBSP version " AJBSP_VERSION " (" __DATE__ ")\n");

	fflush(stdout);
}


static void ShowTime()
{
#ifdef WIN32
	SYSTEMTIME sys_time;

	GetSystemTime(&sys_time);

	LogPrintf("Current time: %02d:%02d on %04d/%02d/%02d\n",
			  sys_time.wHour, sys_time.wMinute,
			  sys_time.wYear, sys_time.wMonth, sys_time.wDay);

#else // LINUX or MACOSX

	time_t epoch_time;
	struct tm *calend_time;

	if (time(&epoch_time) == (time_t)-1)
		return;

	calend_time = localtime(&epoch_time);
	if (! calend_time)
		return;

	LogPrintf("Current time: %02d:%02d on %04d/%02d/%02d\n",
			  calend_time->tm_hour, calend_time->tm_min,
			  calend_time->tm_year + 1900, calend_time->tm_mon + 1,
			  calend_time->tm_mday);
#endif
}


//
//  the program starts here
//
int main(int argc, char *argv[])
{
	// a quick pass through the command line arguments
	// to handle special options, like --help, --install, --config
// FIXME	M_ParseCommandLine(argc - 1, argv + 1);

	if (show_help)
	{
		ShowHelp();
		return 0;
	}
	else if (show_version)
	{
		ShowVersion();
		return 0;
	}


	LogPrintf("\n");
	LogPrintf("**** " AJBSP_TITLE " v" AJBSP_VERSION " (C) 2018 Andrew Apted, et al ****\n");
	LogPrintf("\n");

	// sanity checks type sizes (useful when porting)
	CheckTypeSizes();

	ShowTime();


// FIXME
#if 0
	// environment variables can override them
	M_ParseEnvironmentVars();

	// and command line arguments will override both
	M_ParseCommandLine(argc - 1, argv + 1, 2);
#endif


	// open a specified PWAD now
	// [ the map is loaded later.... ]

	if (Pwad_list.size() > 0)
	{
		// FIXME
	}
	else
	{
		// FIXME
	}


quit:
	/* that's all folks! */

	LogPrintf("Quit\n");

	return 0;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
