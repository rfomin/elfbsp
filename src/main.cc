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

bool want_quit = false;

const char *config_file = NULL;
const char *log_file;

const char *install_dir;
const char *home_dir;
const char *cache_dir;


const char *Iwad_name = NULL;
const char *Pwad_name = NULL;

std::vector< const char * > Pwad_list;

const char *Game_name;
const char *Port_name;
const char *Level_name;

map_format_e Level_format;


int  init_progress;

int show_help     = 0;
int show_version  = 0;


static void RemoveSingleNewlines(char *buffer)
{
	for (char *p = buffer ; *p ; p++)
	{
		if (*p == '\n' && p[1] == '\n')
			while (*p == '\n')
				p++;

		if (*p == '\n')
			*p = ' ';
	}
}


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

	if (init_progress < 1 || Quiet || log_file)
	{
		fprintf(stderr, "\nFATAL ERROR: %s", buffer);
	}

	if (init_progress >= 1)
	{
		LogPrintf("\nFATAL ERROR: %s", buffer);
	}

	if (init_progress >= 2)
	{
		RemoveSingleNewlines(buffer);

		DLG_ShowError("%s", buffer);

		init_progress = 1;
	}
#ifdef WIN32
	else
	{
		MessageBox(NULL, buffer, "Eureka : Error",
		           MB_ICONEXCLAMATION | MB_OK |
				   MB_SYSTEMMODAL | MB_SETFOREGROUND);
	}
#endif

	init_progress = 0;

	MasterDir_CloseAll();
	LogClose();

	exit(2);
}


/* ----- user information ----------------------------- */

static void ShowHelp()
{
	printf(	"\n"
			"*** " AJBSP_TITLE " v" AJBSP_VERSION " (C) 2018 Andrew Apted, et al ***\n"
			"\n");

	printf(	"Eureka is free software, under the terms of the GNU General\n"
			"Public License (GPL), and comes with ABSOLUTELY NO WARRANTY.\n"
			"Home page: http://eureka-editor.sf.net/\n"
			"\n");

	printf( "USAGE: eureka [options...] [FILE...]\n"
			"\n"
			"Available options are:\n");

	M_PrintCommandLineOptions(stdout);

	fflush(stdout);
}


static void ShowVersion()
{
	printf("Eureka version " AJBSP_VERSION " (" __DATE__ ")\n");

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
	init_progress = 0;


	// a quick pass through the command line arguments
	// to handle special options, like --help, --install, --config
	M_ParseCommandLine(argc - 1, argv + 1, 1);

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

	init_progress = 1;


	LogPrintf("\n");
	LogPrintf("**** " AJBSP_TITLE " v" AJBSP_VERSION " (C) 2018 Andrew Apted, et al ****\n");
	LogPrintf("\n");

	// sanity checks type sizes (useful when porting)
	CheckTypeSizes();

	ShowTime();


	// load all the config settings
	M_ParseConfigFile();

	// environment variables can override them
	M_ParseEnvironmentVars();

	// and command line arguments will override both
	M_ParseCommandLine(argc - 1, argv + 1, 2);


	// open a specified PWAD now
	// [ the map is loaded later.... ]

	if (Pwad_list.size() > 0)
	{
		// this fatal errors on any missing file
		// [ hence the Open() below is very unlikely to fail ]
		M_ValidateGivenFiles();

		Pwad_name = Pwad_list[0];

		edit_wad = Wad_file::Open(Pwad_name, 'a');
		if (! edit_wad)
			FatalError("Cannot load pwad: %s\n", Pwad_name);

		// Note: the Main_LoadResources() call will ensure this gets
		//       placed at the correct spot (at the end)
		MasterDir_Add(edit_wad);
	}
	// don't auto-load when --iwad or --warp was used on the command line
	else if (auto_load_recent && ! (Iwad_name || Level_name))
	{
		if (M_TryOpenMostRecent())
		{
			MasterDir_Add(edit_wad);
		}
	}



	// config file parsing can depend on the map format, so get it now
	GetLevelFormat(edit_wad ? edit_wad : game_wad, Level_name);


	// load the initial level
	LogPrintf("Loading initial map : %s\n", Level_name);



quit:
	/* that's all folks! */

	LogPrintf("Quit\n");

	init_progress = 0;

	LogClose();

	return 0;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
