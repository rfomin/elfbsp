
Compiling AJBSP
===============

The AJBSP code is fairly portable C++, and does not depend
on any third-party libraries.  Both GNU g++ and clang++
have been tested and are known to work.

Building should be fairly straight-forward on any Unix-like
system, such as Linux, the BSDs, and even MacOS X.  Fairly
simple Makefiles are used for building, although they require
GNU make.  On BSD systems you will need the 'gmake' package.


Linux / BSD
-----------

On Debian linux, you will need the following packages:

- g++
- binutils
- make

To build the program, type:

    make

To install AJBSP, become root and type:

    make install

NOTE: on BSD systems the manual page may be installed into
the wrong directory.  Edit the Makefile, find where the
MANDIR variable is set and change it to this:

    MANDIR=$(PREFIX)/man


Windows (native)
----------------

Sorry, compiling this under Windows itself is not supported.


Windows (cross compiler)
------------------------

A "cross compiler" is a compiler which builds binaries for one
platform (e.g. Windows) when on another platform (e.g. Linux).
This is how the current Windows executable was built.

On Debian linux, the following packages are needed:

- mingw-w64
- make

To build the program, type:

    make -f Makefile all stripped

