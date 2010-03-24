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
# O, B, L, H prefixes to use with names of objects, binaries, libraries and headers respectively

# Version
VERMAJOR= 1
VERMINOR= 9
VERPATCH= 0
VERH	= $(VERMAJOR).$(VERMINOR)
VER	= $(VERH).$(VERPATCH)

# Sources directory
_SRC_DIR= ../src/
# Headers directory
H_DIR	= ../fidoconf/

# Header files
HEADERS = $(H)afixcmd.h $(H)arealist.h $(H)areatree.h $(H)stat.h $(H)common.h \
          $(H)grptree.h $(H)fidoconf.h $(H)fidoconf.pas $(H)findtok.h $(H)version.h

# Object files: library
OBJS	= $(O)afixcmd$(_OBJ) $(O)afixcmn$(_OBJ) $(O)arealist$(_OBJ) \
          $(O)areatree$(_OBJ) $(O)stat$(_OBJ) $(O)cfg$(_OBJ) $(O)common$(_OBJ) \
          $(O)fidoconf$(_OBJ) $(O)findtok$(_OBJ) $(O)grptree$(_OBJ) \
          $(O)line$(_OBJ) $(O)version$(_OBJ)

# Object files: programs
TPAROBJ = $(O)tparser$(_OBJ)
LINKOBJ = $(O)linked$(_OBJ)
AEDOBJ	= $(O)fc2aed$(_OBJ)
BINKOBJ = $(O)fc2binkd$(_OBJ)
FGATEOBJ= $(O)fc2fgate$(_OBJ)
GEDOBJ	= $(O)fc2ged$(_OBJ)
MSGEDOBJ= $(O)fc2msged$(_OBJ)
SQOBJ	= $(O)fc2sq$(_OBJ)
TOROBJ	= $(O)fc2tor$(_OBJ) $(O)fc2tor_g$(_OBJ)
FEOBJ	= $(O)fecfg146$(_OBJ) $(O)fecfg2fc$(_OBJ)
PRGOBJS = $(TPAROBJ) $(LINKOBJ) $(AEDOBJ) $(BINKOBJ) $(FGATEOBJ) $(GEDOBJ) $(MSGEDOBJ) $(SQOBJ) $(TOROBJ) $(FEOBJ)

# Binary files(s) to bild from sources
PROGRAMS  = $(TPARSER) $(LINKED) $(LINKED) $(FC2AED) $(FC2BINKD) $(FC2FGATE) $(FC2GED) $(FC2MSGED) $(FC2SQ) $(FC2TOR) $(FECFG2FC)

# Usually compose from LIBPREFIX, LIBSUFFIX and LIB.
TARGETLIB = $(L)$(LIBPREFIX)$(LIBNAME)$(LIBSUFFIX)$(_LIB)
TARGETDLL = $(B)$(DLLPREFIX)$(LIBNAME)$(DLLSUFFIX)$(_DLL)
TARGET_H  = *.h

# List of libraries required to build binary file(s)
LIBS	=
