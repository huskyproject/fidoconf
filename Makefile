# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else
include ../huskymak.cfg
endif

ifndef ISOPT
# install scripts options
  ISOPT = -c -m 555
endif

# program settings

ifeq ($(DEBUG), 1)
  COPT = $(WARNFLAGS) $(DEBCFLAGS) -I. -I$(INCDIR)
  LFLAGS = $(DEBLFLAGS)
else
  COPT = $(WARNFLAGS) $(OPTCFLAGS) -I. -I$(INCDIR)
  LFLAGS = $(OPTLFLAGS)
endif

ifneq ($(DYNLIBS), 1)
  LFLAGS += -static -lc
endif

ifndef EXENAMEFLAG
  EXENAMEFLAG=-o
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -DCFGDIR=\"$(CFGDIR)\"

# Use -DVSPRINTF_ONLY on systems without vsnprintf() and snprintf()
# Use this for sunOs 2.5.1 (Thanks to Serguei Revtov)
#CDEFS	+= -DVSPRINTF_ONLY

ifdef DIRSEP
CDEFS+= -DPATH_DELIM=\'$(DIRSEP)\'
endif

LOPT = -L. -L$(LIBDIR)

ifneq ($(OSTYPE), UNIX)
#  LIBPREFIX=
else
  LIBPREFIX=lib
endif

# filename settings
ifeq ($(SHORTNAMES), 1)
  FIDOCONFIG     = fidoconf
  FCONF2AREASBBS = fc2abbs
  FCONF2AQUAED   = fc2aed
  FCONF2GOLDED   = fc2ged
  FCONF2MSGED    = fc2msged
  FCONF2FIDOGATE = fc2fgate
  FCONF2SQUISH   = fc2sq
  FCONF2TORNADO  = fc2tor
  FCONF2BINKD    = fc2binkd
  FCONF2PWD      = fc2pwd
  FECFG2FCONF    = fecfg2fc
  LIBFIDOCONFIG  = fidoconf
  CDEFS = $(CDEFS) -DSHORTNAMES
else
  FIDOCONFIG = fidoconfig
  FCONF2AREASBBS = fconf2areasbbs
  FCONF2AQUAED = fconf2aquaed
  FCONF2GOLDED = fconf2golded
  FCONF2MSGED  = fconf2msged
  FCONF2FIDOGATE = fconf2fidogate
  FCONF2SQUISH = fconf2squish
  FCONF2TORNADO  = fconf2tornado
  FCONF2BINKD    = fconf2binkd
  FCONF2PWD      = fconf2pwd
  FECFG2FCONF = fecfg2fconf
  LIBFIDOCONFIG = $(LIBPREFIX)fidoconfig
endif

LINKSMAPI = -lsmapi
LINKFIDOCONFIG = -l$(FIDOCONFIG)

default: all

include makefile.inc

progs: commonprogs

ifeq ($(DYNLIBS), 1)
  all: commonlibs ranlib $(LIBFIDOCONFIG).so.$(VER)
	$(MAKE) progs
	(cd doc && $(MAKE) all)
else
  all: commonlibs ranlib
	$(MAKE) progs
	(cd doc && $(MAKE) all)
endif

clean: commonclean
	-$(RM) $(RMOPT) so_locations
	(cd doc && $(MAKE) clean)

distclean: commondistclean
	-$(RM) $(RMOPT) $(LIBFIDOCONFIG).so*
	(cd doc && $(MAKE) distclean)

ifeq (~$(MKSHARED)~, ~ld~)
$(LIBFIDOCONFIG).so.$(VER): $(LOBJS)
	$(LD) $(LFLAGS) -o $(LIBFIDOCONFIG).so.$(VER) \
	    $(LOBJS) $(LOPT)
else
$(LIBFIDOCONFIG).so.$(VER): $(LOBJS)
	$(CC) -shared -Wl,-soname,$(LIBFIDOCONFIG).so.$(VERH) \
         -o $(LIBFIDOCONFIG).so.$(VER) $(LOBJS) $(LOPT)
endif
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so.$(VERH) ;\
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so

%$(OBJ): %.c
	$(CC) $(CDEFS) $(COPT) $*.c

ifeq ($(DYNLIBS), 1)
instdyn: $(LIBFIDOCONFIG).so.$(VER)
	-$(MKDIR) $(MKDIROPT) $(LIBDIR)
	$(INSTALL) $(ILOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBDIR)
# Removed path from symlinks.
	cd $(LIBDIR) ;\
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so.$(VERH) ;\
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so
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

install: commonlibs progs instdyn
	-$(MKDIR) $(MKDIROPT) $(BINDIR)
	-$(MKDIR) $(MKDIROPT) $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IBOPT) $(FCONF2MSGED)$(EXE)    $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2GOLDED)$(EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2AQUAED)$(EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2FIDOGATE)$(EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2SQUISH)$(EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2TORNADO)$(EXE)  $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2BINKD)$(EXE)    $(BINDIR)
#	$(INSTALL) $(IBOPT) $(FCONF2PWD)$(EXE)      $(BINDIR)
	$(INSTALL) $(IBOPT) linked$(EXE)	    $(BINDIR)
ifeq ($(CC), gcc)
	$(INSTALL) $(IBOPT) $(FECFG2FCONF)$(EXE)    $(BINDIR)
endif
	$(INSTALL) $(IBOPT) tparser$(EXE)           $(BINDIR)
ifeq (${OSTYPE}, UNIX)
	$(INSTALL) $(ISOPT) linkedto                $(BINDIR)
endif
	$(INSTALL) $(ISOPT) fconf2na.pl             $(BINDIR)
	$(INSTALL) $(ISOPT) $(FCONF2AREASBBS)       $(BINDIR)
	$(INSTALL) $(IIOPT) fidoconf.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) areatree.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) findtok.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) typesize.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) common.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) dirlayer.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) adcase.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) xstr.h         $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf.pas   $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) crc.h          $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) log.h          $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) recode.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) tree.h         $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) temp.h         $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) afixcmd.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) arealist.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) version.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(ISLOPT) $(LIBFIDOCONFIG)$(LIB) $(LIBDIR)
	(cd doc && $(MAKE) install)
	@echo
	@echo "*** For install man pages run 'gmake install-man' (unixes only)"
	@echo

install-man:
	(cd man && $(MAKE) install)

uninstall:
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2MSGED)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2GOLDED)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2AQUAED)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2FIDOGATE)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2SQUISH)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2TORNADO)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2BINKD)$(EXE)
#	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2PWD)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FECFG2FCONF)$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)linked$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)tparser$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)linkedto
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)fconf2na.pl
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2AREASBBS)
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)fidoconf.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)typesize.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)common.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)dirlayer.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)adcase.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)xstr.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)fidoconf.pas
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)crc.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)log.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)recode.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)tree.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)temp.h
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)version.h
	-$(RM) $(RMOPT) $(LIBDIR)$(DIRSEP)$(LIBFIDOCONFIG)*
	-(cd doc && $(MAKE) uninstall)
	-(cd man && $(MAKE) uninstall)

