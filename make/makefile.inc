# $Id$
#
# Common makefiles part (include-file) for fidoconfig

# Use defines before including this file:
# _OBJ	object files suffix begins with point (".o", ".obj", ... )
# _LIB	library files suffix begins with point (".a", ".lib", ... )
# _DLL	dinamic linking library files suffix begins with point (".dll", ".so", ...)
# LIBPREFIX  library filename prefix ("lib", "", ...)
# LIBSUFFIX  library filename suffix (compiler ID, three characters, somewhere empty)
# DLLPREFIX  dinamic linking library prefix (usually empty)
# DLLSUFFIX  dinamic linking library suffix (compiler ID or empty)
# _SRC_DIR  sources dir

# Version
VERMAJOR= 1
VERMINOR= 9
VERPATCH= 0
VERH	= $(VERMAJOR).$(VERMINOR)
VER	= $(VERH).$(VERPATCH)

ifeq (~$(SHORTNAMES)~, ~1~)
# Library name
  LIBNAME = fconf

# Programms name
  TPARSER = tparser$(_EXE)
  LINKED  = linked$(_EXE)
  FC2AED  = fc2aed$(_EXE)
  FC2BINKD= fc2binkd$(_EXE)
  FC2FGATE= fc2fgate$(_EXE)
  FC2GED  = fc2ged$(_EXE)
  FC2MSGED= fc2msged$(_EXE)
  FC2SQ   = fc2sq$(_EXE)
  FC2TOR  = fc2tor$(_EXE)
  FECFG2FC= fecfg2fc$(_EXE)
else
# Library name
  LIBNAME = fidoconfig

# Programms name
  TPARSER = tparser$(_EXE)
  LINKED  = linked$(_EXE)
  FC2AED  = fconf2aquaed$(_EXE)
  FC2BINKD= fconf2binkd$(_EXE)
  FC2FGATE= fconf2fidogate$(_EXE)
  FC2GED  = fconf2golded$(_EXE)
  FC2MSGED= fconf2msged$(_EXE)
  FC2SQ   = fconf2squish$(_EXE)
  FC2TOR  = fconf2tornado$(_EXE)
  FECFG2FC= fecfg2fconf$(_EXE)
endif

# Object files: library
OBJS	= afixcmd$(_OBJ) afixcmn$(_OBJ) arealist$(_OBJ) areatree$(_OBJ) \
	  stat$(_OBJ) cfg$(_OBJ) common$(_OBJ) fidoconf$(_OBJ) \
	  findtok$(_OBJ) grptree$(_OBJ) line$(_OBJ) version$(_OBJ) 

# Header files
HEADERS = afixcmd.h arealist.h areatree.h stat.h common.h grptree.h fidoconf.h \
	  fidoconf.pas findtok.h version.h

# Sources directory
_SRC_DIR= ../src/

# Headers directory
H_DIR	= ../fidoconf/

# Object files: programs
TPAROBJ = tparser$(_OBJ)
LINKOBJ = linked$(_OBJ)
AEDOBJ	= fc2aed$(_OBJ)
BINKOBJ = fc2binkd$(_OBJ)
FGATEOBJ= fc2fgate$(_OBJ)
GEDOBJ  = fc2ged$(_OBJ)
MSGEDOBJ= fc2msged$(_OBJ)
SQOBJ	= fc2sq$(_OBJ)
TOROBJ	= fc2tor$(_OBJ) fc2tor_g$(_OBJ)
FEOBJ	= fecfg146$(_OBJ) fecfg2fc$(_OBJ) 

# Binary files(s) to bild from sources
PROGRAMS  = $(TPARSER) $(LINKED) $(LINKED) $(FC2AED) $(FC2BINKD) $(FC2FGATE) $(FC2GED) $(FC2MSGED) $(FC2SQ) $(FC2TOR) $(FECFG2FC)

# List of libraries required to build binary file(s)
LIBS	=
