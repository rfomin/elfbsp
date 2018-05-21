#
#  --- AJBSP ---
#
#  Makefile for Unixy system-wide install
#

PROGRAM=ajbsp

# prefix choices: /usr  /usr/local  /opt
PREFIX=/usr/local

OBJ_DIR=obj_linux

OPTIMISE=-O2 -fno-strict-aliasing

STRIP_FLAGS=--strip-unneeded

# operating system choices: UNIX WIN32
OS=UNIX


#--- Internal stuff from here -----------------------------------

CXXFLAGS=$(OPTIMISE) -Wall -D$(OS)  \
         -D_THREAD_SAFE -D_REENTRANT

LDFLAGS=-L/usr/X11R6/lib

LIBS=-lm


#----- Object files ----------------------------------------------

OBJS = \
	$(OBJ_DIR)/bsp_level.o \
	$(OBJ_DIR)/bsp_node.o  \
	$(OBJ_DIR)/bsp_util.o  \
	$(OBJ_DIR)/lib_util.o  \
	$(OBJ_DIR)/lib_file.o  \
	$(OBJ_DIR)/main.o      \
	$(OBJ_DIR)/m_nodes.o   \
	$(OBJ_DIR)/w_wad.o

$(OBJ_DIR)/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<


#----- Targets -----------------------------------------------

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJ_DIR)/*.* core core.* ERRS

$(PROGRAM): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

stripped: $(PROGRAM)
	strip $(STRIP_FLAGS) $(PROGRAM)

# TODO : install, uninstall

.PHONY: all clean stripped install uninstall

#--- editor settings ------------
# vi:ts=8:sw=8:noexpandtab
