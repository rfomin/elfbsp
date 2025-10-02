
ELFBSP 1.7
==========

by Guilherme Miranda, 2025.
based on AJBSP, by Andrew Apted, 2022.


About
-----

ELFBSP is a simple nodes builder for modern DOOM source ports.
It can build standard DOOM nodes and Extended ZDoom format nodes,
and since version 1.5 supports the UDMF map format.  The code is
based on the BSP code in Eureka DOOM Editor, which was based on the
code from glBSP but with significant changes.

ELFBSP is a command-line tool.  It can handle multiple wad files,
and modifies each file in-place.  There is an option to backup each
file first.  The output to the terminal is fairly terse, but greater
verbosity can be enabled.  Generally all the maps in a wad will
processed, but this can be limited to a specific set.


Main Site
---------

https://github.com/elf-alchemist/elfbsp


Binary Packages
---------------

https://github.com/elf-alchemist/elfbsp/tags


Legalese
--------

ELFBSP is Copyright &copy; 2025 Guilherme Miranda, Andrew Apted,
Colin Reed, and Lee Killough, et al.

ELFBSP is Free Software, under the terms of the GNU General Public
License, version 2 or (at your option) any later version.
See the [LICENSE.txt](LICENSE.txt) file for the complete text.

ELFBSP comes with NO WARRANTY of any kind, express or implied.
Please read the license for full details.


Compiling
---------

Please see the [INSTALL.md](INSTALL.md) document.


Usage
-----

The simplest possible operation will rebuild all of the nodes in a provided WAD:
```bash
elfbsp example.wad
```

The will rebuild all of the nodes in a seperate copy of the provided WAD:
```bash
elfbsp example1.wad --output example2.wad
```

To build only certain maps' nodes, the following option is available:
```bash
elfbsp example.wad --maps MAP01
elfbsp example.wad --maps MAP01,MAP03,MAP07 # multiple maps can be provided via comma-separation
elfbsp example.wad --maps MAP10-MAP11       # or via a hyphen-separated range
elfbsp example.wad --maps MAP04,MAP22-MAP25 # or you may combine both
```

You may optionally force the use of an advanced node type with the following parameters.
Otherwise, ELFBSP will automatically promote to the advanced Node types depending on map complexity.
```bash
elfbsp example.wad         # vanilla nodes
elfbsp example.wad --xnod  # forces the use of XNOD, leaves SEGS and SSECTORS empty, using only the NODES lump
elfbsp example.wad --ssect # forces the use of XGL3, leaves SEGS and NODES empty, using only the SSECTORS lump
```

For a basic explanation of the main options, type:
```bash
elfbsp --help
```

For a complete options list and documentation for each, type:
```bash
elfbsp --doc
```


Exit Codes
----------

- 0 if OK.
- 1 if nothing was built (no matching maps).
- 2 if one or more maps failed to build properly.
- 3 if a fatal error occurred.
