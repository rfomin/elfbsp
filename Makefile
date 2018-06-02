#
#  --- AJBSP ---
#
#  Makefile for Linux and BSD systems.
#  Requires GNU make.
#

PROGRAM=ajbsp

# prefix choices: /usr  /usr/local  /opt
PREFIX=/usr/local
MANDIR=$(PREFIX)/share/man

OBJ_DIR=obj_linux

# CXX=clang++-6.0
# CXX=g++ -m32   (to compile 32-bit binary on 64-bit system)

WARNINGS=-Wall -Wextra -Wshadow -Wno-unused-parameter
OPTIMISE=-O2 -fno-strict-aliasing -fno-exceptions -fno-rtti
STRIP_FLAGS=--strip-unneeded

# operating system choices: UNIX WIN32
OS=UNIX


#--- Internal stuff from here -----------------------------------

MAN_PAGE=$(PROGRAM).6

CXXFLAGS=$(OPTIMISE) $(WARNINGS) -D$(OS)  \
         -D_THREAD_SAFE -D_REENTRANT

LDFLAGS=
# LDFLAGS=-static

# I needed this when using -m32 and -static:
# LDFLAGS += -L/usr/lib/gcc/i686-linux-gnu/6/

LIBS=-lm

DUMMY=$(OBJ_DIR)/zzdummy


#----- Object files ----------------------------------------------

OBJS = \
	$(OBJ_DIR)/bsp_level.o \
	$(OBJ_DIR)/bsp_node.o  \
	$(OBJ_DIR)/bsp_util.o  \
	$(OBJ_DIR)/lib_util.o  \
	$(OBJ_DIR)/lib_file.o  \
	$(OBJ_DIR)/main.o      \
	$(OBJ_DIR)/w_wad.o

$(OBJ_DIR)/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<


#----- Targets -----------------------------------------------

all: $(DUMMY) $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJ_DIR)/*.* core core.* ERRS

# this is used to create the OBJ_DIR directory
$(DUMMY):
	mkdir -p $(OBJ_DIR)
	@touch $@

$(PROGRAM): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

stripped: all
	strip $(STRIP_FLAGS) $(PROGRAM)

install: stripped
	install -d -m 755 $(PREFIX)/bin
	install -o root -m 755 $(PROGRAM) $(PREFIX)/bin/
	install -d -m 755 $(MANDIR)/man6
	install -o root -m 644 doc/$(MAN_PAGE) $(MANDIR)/man6/

uninstall:
	rm -f -v $(PREFIX)/bin/$(PROGRAM)
	rm -f -v $(MANDIR)/man6/$(MAN_PAGE)

.PHONY: all clean stripped install uninstall

#--- editor settings ------------
# vi:ts=8:sw=8:noexpandtab
