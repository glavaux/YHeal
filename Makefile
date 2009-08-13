# these values filled in by    yorick -batch make.i
Y_MAKEDIR=/usr/lib/yorick
Y_EXE=/usr/lib/yorick/bin/yorick
Y_EXE_PKGS=
Y_EXE_HOME=/usr/lib/yorick
Y_EXE_SITE=/usr/lib/yorick

# Update this to the actual configuration you used to build Healpix 
HEALPIX_BUILD=generic_gcc

# ----------------------------------------------------- optimization flags

# options for make command line, e.g.-   make COPT=-g TGT=exe
COPT=$(COPT_DEFAULT)
TGT=$(DEFAULT_TGT)

# ------------------------------------------------ macros for this package

PKG_NAME=yheal
PKG_I=yheal.i

OBJS=yheal.o yalm.o

# change to give the executable a name other than yorick
PKG_EXENAME=yorick

# PKG_DEPLIBS=-Lsomedir -lsomelib   for dependencies of this package
PKG_DEPLIBS=-lhealpix_cxx -lcxxsupport -lcfitsio -lfftpack
# set compiler (or rarely loader) flags specific to this package
HEALPIX_CXX=$(HEALPIX)/src/cxx/$(HEALPIX_BUILD)
PKG_CFLAGS=-I$(HEALPIX_CXX)/include -fopenmp
PKG_LDFLAGS=-L$(HEALPIX_CXX)/lib -fopenmp

# list of additional package names you want in PKG_EXENAME
# (typically Y_EXE_PKGS should be first here)
EXTRA_PKGS=$(Y_EXE_PKGS)

# list of additional files for clean
PKG_CLEAN=

# autoload file for this package, if any
PKG_I_START=
# non-pkg.i include files for this package, if any
PKG_I_EXTRA=

# -------------------------------- standard targets and rules (in Makepkg)

# set macros Makepkg uses in target and dependency names
# DLL_TARGETS, LIB_TARGETS, EXE_TARGETS
# are any additional targets (defined below) prerequisite to
# the plugin library, archive library, and executable, respectively
PKG_I_DEPS=$(PKG_I)
Y_DISTMAKE=distmake

include $(Y_MAKEDIR)/Make.cfg
include $(Y_MAKEDIR)/Makepkg
include $(Y_MAKEDIR)/Make$(TGT)

# override macros Makepkg sets for rules and other macros
# Y_HOME and Y_SITE in Make.cfg may not be correct (e.g.- relocatable)
Y_HOME=$(Y_EXE_HOME)
Y_SITE=$(Y_EXE_SITE)

# ------------begin C++ source hacks
# must use C++ to load yorick with this C++ package
# this assumes make default CXX macro points to C++ compiler
CXXFLAGS=$(CFLAGS)
LD_DLL=$(CXX) $(LDFLAGS) $(PLUG_SHARED)
LD_EXE=$(CXX) $(LDFLAGS) $(PLUG_EXPORT)

# C++ has no standard file extension, supply default make rule(s)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<
# --------------end C++ source hacks

# reduce chance of yorick-1.5 corrupting this Makefile
MAKE_TEMPLATE = protect-against-1.5

# ------------------------------------- targets and rules for this package

# simple example:
#myfunc.o: myapi.h
# more complex example (also consider using PKG_CFLAGS above):
#myfunc.o: myapi.h myfunc.c
#	$(CC) $(CPPFLAGS) $(CFLAGS) -DMY_SWITCH -o $@ -c myfunc.c

# -------------------------------------------------------- end of Makefile
