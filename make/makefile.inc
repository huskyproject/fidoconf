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

# Library name
LIBNAME	= fidoconf

# Version
VERMAJOR= 1
VERMINOR= 9
VERH	= $(VERMAJOR).$(VERMINOR)
VER	= $(VERH).0

# Object files: library
LOBJS1	= afixcmd$(_OBJ) afixcmn$(_OBJ) arealist$(_OBJ) areatree$(_OBJ)
LOBJS2	= cfg$(_OBJ) common$(_OBJ) fidoconf$(_OBJ) findtok$(_OBJ) grptree$(_OBJ)
LOBJS3	= line$(_OBJ) version$(_OBJ) 
LOBJS	= $(LOBJS1) $(LOBJS2) $(LOBJS3)

# Object files: programs
OBJS1	= fc2aed$(_OBJ) fc2binkd$(_OBJ) fc2fgate$(_OBJ) fc2ged$(_OBJ)
OBJS2	= fc2msged$(_OBJ) fc2sq$(_OBJ) linked$(_OBJ) tparser$(_OBJ)
TOROBJS	= fc2tor$(_OBJ) fc2tor_g$(_OBJ)
FEOBJS	= fecfg146$(_OBJ) fecfg2fc$(_OBJ) 
OBJS	= $(LOBJS) $(OBJS1) $(OBJS2) $(TOROBJS) $(FEOBJS)

# Header files
HEADERS1= afixcmd.h arealist.h areatree.h common.h grptree.h fidoconf.h
HEADERS2= fidoconf.pas findtok.h version.h
HEADERS	= $(HEADERS1) $(HEADERS2)

# Binary files to build from sources
# 1) $(PROGRAMS) - build one binary from one source
PROGS1	= fc2aed$(_EXE) fc2binkd$(_EXE) fc2fgate$(_EXE) fc2ged$(_EXE)
PROGS2	= fc2msged$(_EXE) fc2sq$(_EXE) linked$(_EXE) tparser$(_EXE)
PROGRAMS= $(PROGS1) $(PROGS2)
# 2) $(FC2TOR) - fc2tor build from two sources
FC2TOR	= fc2tor$(_EXE)
# 3) $(FECFG2FC) - fecfg2fc build from two sources
FECFG2FC= fecfg2fc$(_EXE)


# Usually compose from LIBPREFIX, LIBSUFFIX and LIB.
# Static library name
TARGETLIB = $(LIBPREFIX)$(LIBNAME)$(LIBSUFFIX)$(_LIB)
# Shared library name
TARGETDLL = $(DLLPREFIX)$(LIBNAME)$(DLLSUFFIX)$(_DLL)

# Binary package (archive) name
PACKAGEBASE=$(LIBNAME)$(LIBSUFFIX)-$(VER)
PACKAGENAME=$(LIBNAME)$(LIBSUFFIX)$(VER)$(_ARC)

LIBS	= -l$(LIBPREFIX)husky$(_LIB) -l$(LIBPREFIX)smapi$(LIBSUFFIX)$(_LIB)
DLLS	= -l$(DLLPREFIX)husky$(_DLL) -l$(DLLPREFIX)smapi$(DLLSUFFIX)$(_DLL)

commonlibs: $(TARGETLIB) $(SYMBOLIC)
	$(NULL)

commonprogs: $(PROGRAMS) $(FC2TOR) $(FECFG2FC) $(SYMBOLIC)
	$(NULL)


%$(_OBJ): $(_SRC_DIR)%.c
	$(CC) $(CFLAGS) $(DEBUG) $(CDEFS) $(CINCL) $(OBJOPT) $<

$(TARGETLIB): $(LOBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@


$(PROGRAMS): $(TARGETLIB)
	$(CC) $(LFLAGS) $(STRIP) $(EXENAMEOPT)$@ $@$(_OBJ) $(LIBS) $<

$(FC2TOR): $(TOROBJS) $(TARGETLIB)
	$(CC) $(LFLAGS) $(STRIP) $(EXENAMEOPT)$@ $@$(_OBJ) $(LIBS) $<

$(FECFG2FC): $(FEOBJS) $(TARGETLIB)
	$(CC) $(LFLAGS) $(STRIP) $(EXENAMEOPT)$@ $@$(_OBJ) $(LIBS) $<


commonclean: $(SYMBOLIC)
	-$(RM) $(RMOPT) *$(_OBJ)

commondistclean: clean $(SYMBOLIC)
	-$(RM) $(RMOPT) $(PROGRAMS) $(FC2TOR) $(FECFG2FC)
	-$(RM) $(RMOPT) $(TARGETLIB)