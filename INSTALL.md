
Compiling AJBSP
===============

The AJBSP code is fairly portable C++, and does not depend
on any third-party libraries.  Both GNU g++ and clang++
have been tested and are known to work.

Building should be fairly straight-forward on any Unix-like
system, such as Linux, the BSDs, and even MacOS X.  Fairly
simple Makefiles are used for building, although they require
GNU make, so on BSD systems you will need the 'gmake' package.


Linux / BSD
-----------

On Debian linux, you will need the following packages:

- g++
- binutils
- make

It is a good idea to open the Makefile in a text editor and
check various settings.  For example, you may want to change
the `PREFIX` variable, or set `CXX` to a specific C++ compiler.

To build the program, type:

    make

To install AJBSP, become root and type:

    make install

NOTE: on BSD systems the manual page may be installed into
the wrong directory (and not be found just when you want to
read it).  To fix this, edit the Makefile, find the `MANDIR`
variable (it is near the top) and change it to this:

    MANDIR=$(PREFIX)/man


Windows (native)
----------------

Sorry, compiling under Windows itself is not currently
supported.


Windows (cross compiler)
------------------------

A "cross compiler" is a compiler which builds binaries for
one platform (e.g. Windows) on another platform (e.g. Linux).
This is how the current Windows executable is built.

On Debian linux, the following packages are needed:

- mingw-w64
- make

To build the program, type:

    make -f Makefile.xming

The executable can be made smaller using the following:

    make -f Makefile.xming stripped

