# include Husky-Makefile-Config
include ../huskymak.cfg

# program settings

ifeq ($(DEBUG), 1)
  COPT = $(WARNFLAGS) $(DEBCFLAGS) -I. -I$(INCDIR)
  LFLAGS = $(DEBLFLAGS)
else
  COPT = $(WARNFLAGS) $(OPTCFLAGS) -I. -I$(INCDIR)
  LFLAGS = $(OPTLFLAGS)
endif

ifndef EXENAMEFLAG
  EXENAMEFLAG=-o
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -DCFGDIR=\"$(CFGDIR)\"

LOPT = -L. -L$(LIBDIR)

ifneq ($(OSTYPE), UNIX)
  LIBPREFIX=
else
  LIBPREFIX=lib
endif

# filename settings
ifeq ($(SHORTNAMES), 1)
  FIDOCONFIG     = fidoconf
  FCONF2AQUAED   = fc2aed
  FCONF2GOLDED   = fc2ged
  FCONF2MSGED    = fc2msged
  FCONF2FIDOGATE = fc2fgate
  FCONF2SQUISH   = fc2sq
  FECFG2FCONF    = fecfg2fc
  LIBFIDOCONFIG  = fidoconf
  CDEFS = $(CDEFS) -DSHORTNAMES
else
  FIDOCONFIG = fidoconfig
  FCONF2AQUAED = fconf2aquaed
  FCONF2GOLDED = fconf2golded
  FCONF2MSGED  = fconf2msged
  FCONF2FIDOGATE = fconf2fidogate
  FCONF2SQUISH = fconf2squish
  FECFG2FCONF = fecfg2fconf
  LIBFIDOCONFIG = $(LIBPREFIX)fidoconfig
endif

LINKSMAPI = -lsmapi
LINKFIDOCONFIG = -l$(FIDOCONFIG)

default: all

include makefile.inc

fecfg2fc$(OBJ): fecfg2fc.c
	$(CC) $(COPT) $(CDEFS) -fpack-struct fecfg2fc.c

ifeq ($(CC), gcc)
  $(FECFG2FCONF)$(EXE): fecfg2fc$(OBJ)
	$(CC) $(LFLAGS) -fpack-struct fecfg2fc$(OBJ) \
	  -o $(FECFG2FCONF)$(EXE)
endif



ifeq ($(CC), gcc)
  progs: commonprogs $(FECFG2FCONF)$(EXE)
else
  progs: commonprogs
endif

ifeq ($(DYNLIBS), 1)
  all: commonlibs ranlib $(LIBFIDOCONFIG).so.$(VER) progs
	(cd doc && $(MAKE) all)
else
  all: commonlibs ranlib progs
	(cd doc && $(MAKE) all)
endif

clean: commonclean
	-$(RM) so_locations
	(cd doc && $(MAKE) clean)

distclean: commondistclean
	-$(RM) $(LIBFIDOCONFIG).so.$(VER)
	(cd doc && $(MAKE) distclean)

ifeq (~$(MKSHARED)~, ~ld~)
$(LIBFIDOCONFIG).so.$(VER): $(LOBJS)
	$(LD) -s -shared -o $(LIBFIDOCONFIG).so.$(VER) \
	    $(LOBJS) $(LOPT)
else
$(LIBFIDOCONFIG).so.$(VER): $(LOBJS)
	$(CC) -shared -Wl,-soname,$(LIBFIDOCONFIG).so.$(VERH) \
         -o $(LIBFIDOCONFIG).so.$(VER) $(LOBJS) $(LOPT)
endif

%$(OBJ): %.c
	$(CC) $(CDEFS) $(COPT) $*.c

ifeq ($(DYNLIBS), 1)
instdyn: $(LIBFIDOCONFIG).so.$(VER)
	$(INSTALL) $(ILOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBDIR)
	$(LN) $(LNOPT) $(LIBDIR)/$(LIBFIDOCONFIG).so.$(VER) \
          $(LIBDIR)/$(LIBFIDOCONFIG).so.0
	$(LN) $(LNOPT) $(LIBDIR)/$(LIBFIDOCONFIG).so.0 $(LIBDIR)/$(LIBFIDOCONFIG).so
ifneq (~$(LDCONFIG)~, ~~)
	$(LDCONFIG)
endif

else
instdyn: commonlibs

endif

ranlib: commonlibs
#	$(AR) $(AR_R) $(LIBFIDOCONFIG)$(LIB) $(LIBDIR)/patmat.o
ifdef RANLIB
	$(RANLIB) $(LIBFIDOCONFIG)$(LIB)
endif

install: commonlibs ranlib progs instdyn
	-$(MKDIR) $(MKDIROPT) $(BINDIR)
	-$(MKDIR) $(MKDIROPT) $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IBOPT) $(FCONF2MSGED)$(EXE)    $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2GOLDED)$(EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2AQUAED)$(EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2FIDOGATE)$(EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2SQUISH)$(EXE)   $(BINDIR)
ifeq ($(CC), gcc)
	$(INSTALL) $(IBOPT) $(FECFG2FCONF)$(EXE)    $(BINDIR)
endif
	$(INSTALL) $(IBOPT) tparser$(EXE)           $(BINDIR)
	$(INSTALL) $(IBOPT) dumpfcfg$(EXE)          $(BINDIR)
	$(INSTALL) $(ILOPT) linkedto $(BINDIR)
	$(INSTALL) $(IIOPT) fidoconf.h   $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) typesize.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) common.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) dirlayer.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) adcase.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) xstr.h         $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf.pas   $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(ILOPT) $(LIBFIDOCONFIG)$(LIB) $(LIBDIR)
	(cd doc && $(MAKE) install)

uninstall:
	-$(RM) $(BINDIR)$(DIRSEP)$(FCONF2MSGED)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)$(FCONF2GOLDED)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)$(FCONF2AQUAED)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)$(FCONF2FIDOGATE)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)$(FCONF2SQUISH)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)$(FECFG2FCONF)$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)tparser$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)dumpfcfg$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)linkedto
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)fidoconf.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)typesize.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)common.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)dirlayer.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)adcase.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)xstr.h
	-$(RM) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)fidoconf.pas
	-$(RM) $(LIBDIR)$(DIRSEP)$(LIBFIDOCONFIG)$(LIB)
	(cd doc && $(MAKE) uninstall)

