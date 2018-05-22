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

bool opt_quiet		= false;
bool opt_verbose	= false;
bool opt_backup		= false;
bool opt_fast		= false;

bool opt_no_gl		= false;
bool opt_force_v5	= false;
bool opt_force_xnod	= false;
int  opt_split_cost	= DEFAULT_FACTOR;

bool opt_help		= false;
bool opt_version	= false;


std::vector< const char * > wad_list;

const char *Level_name;

map_format_e Level_format;


typedef struct map_range_s
{
	const char *low;
	const char *high;

} map_range_t;

std::vector< map_range_t > map_list;


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
	info->factor	= CLAMP(1, opt_split_cost, 32);

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
		// FIXME : support map_list

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


void BackupFile(const char *filename)
{
	// FIXME
}


void VisitFile(unsigned int idx, const char *filename)
{
	edit_wad = Wad_file::Open(filename, 'a');
	if (! edit_wad)
		FatalError("Cannot open pwad: %s\n", filename);

	if (edit_wad->LevelCount() == 0)
	{
		// FIXME : display something
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

	// this closes the file
	delete edit_wad; edit_wad = NULL;
}


// ----- user information -----------------------------

static void ShowHelp()
{
	/*
	printf(	"AJBSP is free software, under the terms of the GNU GPL\n"
			"(General Public License), and has ABSOLUTELY NO WARRANTY.\n"
			"\n");
	*/

	printf("\n");

	printf( "Usage: ajbsp [options...] FILE...\n"
			"\n"
			"Available options are:\n"
			"    -q --quiet         Quiet output, only show errors\n"
			"    -v --verbose       Verbose output, show all warnings\n"
			"\n"
			"    -b --backup        Backup input files (.bak extension)\n"
			"    -f --fast          Faster partition selection\n"
			"    -m --map    XXX    Control which map(s) are built\n"
			"\n"
			"    -n --nogl          Disable creation of GL-Nodes\n"
			"    -x --xnod          Use XNOD format for normal nodes\n"
			"    -c --cost   ###    Cost assigned to seg splits (1-32)\n"
			"\n"
			"Short options may be mixed, for example: -fbc23\n"
			"Long options must always begin with a double hyphen\n"
			"\n"
			"Map names should be full, like E1M3 or MAP24, but a list\n"
			"and/or ranges can be specified, such as: MAP01,MAP04-MAP07\n"
			);

	//	"Home page: https://gitlab.com/andwj/ajbsp\n"

	fflush(stdout);
}


static void ShowVersion()
{
	printf("ajbsp " AJBSP_VERSION "  (" __DATE__ ")\n");

	fflush(stdout);
}


bool ValidateMapName(char *name)
{
	if (strlen(name) < 2 || strlen(name) > 8)
		return false;

	if (! isalpha(name[0]))
		return false;

	for (const char *p = name ; *p ; p++)
	{
		if (! (isalnum(*p) || *p == '_'))
			return false;
	}

	// Ok, convert to upper case
	for (char *s = name ; *s ; s++)
	{
		*s = toupper(*s);
	}

	return true;
}


void ParseMapRange(char *tok)
{
	char *low  = tok;
	char *high = tok;

	// look for '-' separator
	char *p = strchr(tok, '-');

	if (p)
	{
		*p++ = 0;

		high = p;
	}

	if (! ValidateMapName(low))
		FatalError("illegal map name: '%s'\n", low);

	if (! ValidateMapName(high))
		FatalError("illegal map name: '%s'\n", high);

	if (strlen(low) < strlen(high))
		FatalError("bad map range (%s shorter than %s)\n", low, high);

	if (strlen(low) > strlen(high))
		FatalError("bad map range (%s longer than %s)\n", low, high);

	if (low[0] != high[0])
		FatalError("bad map range (%s and %s start with different letters)\n", low, high);

	if (strcmp(low, high) > 0)
		FatalError("bad map range (%s comes after %s)\n", low, high);

	// Ok

	map_range_t range;

	range.low  = low;
	range.high = high;

	map_list.push_back(range);
}


void ParseMapList(const char *from_arg)
{
	// create a mutable copy of the string
	// [ we will keep long-term pointers into this buffer ]
	char *buf = StringDup(from_arg);

	char *arg = buf;

	while (*arg)
	{
		if (*arg == ',')
			FatalError("bad map list (empty element)\n");

		// find next comma
		char *tok = arg;
		arg++;

		while (*arg && *arg != ',')
			arg++;

		if (*arg == ',')
		{
			*arg++ = 0;
		}

		ParseMapRange(tok);
	}
}


void ParseShortArgument(const char *arg)
{
	// skip the leading '-'
	arg++;

	while (*arg)
	{
		char c = *arg++;
		int val = 0;

		switch (c)
		{
			case 'b': opt_backup = true; continue;
			case 'f': opt_fast = true; continue;
			case 'h': opt_help = true; continue;
			case 'n': opt_no_gl = true; continue;
			case 'q': opt_quiet = true; continue;
			case 'v': opt_verbose = true; continue;
			case 'x': opt_force_xnod = true; continue;

			case 'm':
				FatalError("cannot use option '-m' like that\n");
				return;

			case 'c':
				if (*arg == 0 || ! isdigit(*arg))
					FatalError("missing value for '-c' option\n");

				// we only accept one or two digits here
				val = *arg - '0';
				arg++;

				if (*arg && isdigit(*arg))
				{
					val = (val * 10) + (*arg - '0');
					arg++;
				}

				if (val < 1 || val > 32)
					FatalError("illegal value for '-c' option\n");

				opt_split_cost = val;
				continue;

			default:
				if (isprint(c) && !isspace(c))
					FatalError("unknown short option: '-%c'\n", c);
				else
					FatalError("illegal short option (ascii code %d)\n", (int)(unsigned char)c);
				return;
		}
	}
}


int ParseLongArgument(const char *name, int argc, char *argv[])
{
	int used = 0;

	if (strcmp(name, "--help") == 0)
	{
		opt_help = true;
	}
	else if (strcmp(name, "--version") == 0)
	{
		opt_version = true;
	}
	else if (strcmp(name, "--quiet") == 0)
	{
		opt_quiet = true;
	}
	else if (strcmp(name, "--verbose") == 0)
	{
		opt_verbose = true;
	}
	else if (strcmp(name, "--backup") == 0 || strcmp(name, "--backups") == 0)
	{
		opt_backup = true;
	}
	else if (strcmp(name, "--fast") == 0)
	{
		opt_fast = true;
	}
	else if (strcmp(name, "--map") == 0 || strcmp(name, "--maps") == 0)
	{
		if (argc < 1 || argv[0][0] == '-')
			FatalError("missing value(s) for '--map' option\n");

		ParseMapList(argv[0]);

		used = 1;
	}
	else if (strcmp(name, "--nogl") == 0 || strcmp(name, "--no-gl") == 0)
	{
		opt_no_gl = true;
	}
	else if (strcmp(name, "--xnod") == 0)
	{
		opt_force_xnod = true;
	}
	else if (strcmp(name, "--cost") == 0)
	{
		if (argc < 1 || ! isdigit(argv[0][0]))
			FatalError("missing value for '--cost' option\n");

		int val = atoi(argv[0]);

		if (val < 1 || val > 32)
			FatalError("illegal value for '--cost' option\n");

		opt_split_cost = val;
		used = 1;
	}
	else
	{
		FatalError("unknown long option: '%s'\n", name);
	}

	return used;
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
			FatalError("illegal option '-'\n");

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
	// sanity check on type sizes (useful when porting)
	CheckTypeSizes();

	ParseCommandLine(argc, argv);

	if (opt_version)
	{
		ShowVersion();
		return 0;
	}

	printf("+-----------------------------------------------+\n");
	printf("|   AJBSP " AJBSP_VERSION "   (C) 2018 Andrew Apted, et al   |\n");
	printf("+-----------------------------------------------+\n");

	if (opt_help || argc <= 1)
	{
		ShowHelp();
		return 0;
	}

	if (wad_list.size() == 0)
	{
		FatalError("no files to process\n");
		return 0;
	}

	for (unsigned int i = 0 ; i < wad_list.size() ; i++)
	{
		const char *filename = wad_list[i];

		//FIXME
		// CheckInputFilename(filename);

		if (! FileExists(filename))
			FatalError("no such file: %s\n", filename);

		if (opt_backup)
			BackupFile(filename);
	}

	for (unsigned int i = 0 ; i < wad_list.size() ; i++)
	{
		VisitFile(i, wad_list[i]);
	}

	// that's all folks!
	return 0;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
