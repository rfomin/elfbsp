
AJBSP 0.70
==========

by Andrew Apted, 2018.


About
-----

AJBSP is a nodes builder for modern DOOM source ports.
It can build standard DOOM nodes, GL-Nodes, and XNOD format nodes.
The code is based on the BSP code in Eureka DOOM Editor, which
was based on the code from glBSP but with significant changes.

AJBSP is a command-line tool.  It can handle multiple wad files,
and modifies each file in-place.  There is an option to backup each
file first.  The output to the terminal is fairly terse, but greater
verbosity can be enabled.  Generally all the maps in a wad will
processed, but this can be limited to a specific set.


Main Site
---------

https://gitlab.com/andwj/ajbsp


Legalese
--------

AJBSP is Copyright &copy; 2018 Andrew Apted, Colin Reed, and
Lee Killough, et al.

AJBSP is Free Software, under the terms of the GNU General Public
License, version 2 or (at your option) any later version.
See the [LICENSE.txt](LICENSE.txt) file for the complete text.

AJBSP comes with NO WARRANTY of any kind, express or implied.
Please read the license for full details.


Installation
------------

TODO


Usage
-----

AJBSP must be run from a terminal (Linux) or the command shell
(cmd.exe in Win32).  The command-line arguments are either files
to process or options.  Where options are placed does not matter,
the set of given options is applied to every given file.

Short options always begin with a hyphen ('-'), followed by one
or more letters, where each letter is a distinct option (i.e. short
options may be mixed).  Long options begin with two hyphens ('--')
followed by a whole word.
When an option needs a value, it should be placed in the next argument,
i.e. separated by the option name by a space.  For short options which
take a number, like '-c', the number can be mixed in immediately
after the letter, such as '-c23'.

The special option '--' causes all following arguments to be
interpreted as filenames.  This allows specifying a file which
begins with a hyphen.

Once invoked, AJBSP will process each wad file.  All the maps in the
file have their nodes rebuilt, unless the --map option is used to
limit which maps are visited.  The normal behavior is to keep the
output to the terminal fairly terse, only showing the name of each
map as it being processed, and a simple summary of each file.
More verbose output can be enabled by the --verbose option.

Running AJBSP with no options, or the --help option, will show
some help text, including a summary of all available options.


Option List
-----------

`-v --verbose`  
    Enables more verbosity.

`-f --fast`  
    Enables more quickness.

