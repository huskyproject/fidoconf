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
  COPT = $(WARNFLAGS) $(DEBCFLAGS) -Ifidoconf -I$(INCDIR)
  LFLAGS = $(DEBLFLAGS)
else
  COPT = $(WARNFLAGS) $(OPTCFLAGS) -Ifidoconf -I$(INCDIR)
  LFLAGS = $(OPTLFLAGS)
endif

ifndef EXENAMEFLAG
  EXENAMEFLAG=-o
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -DCFGDIR=\"$(CFGDIR)\" -DCFGNAME=\"$(CFGNAME)\"

# Use -DVSPRINTF_ONLY on systems without vsnprintf() and snprintf()
# Use this for sunOs 2.5.1 (Thanks to Serguei Revtov)
#CDEFS	+= -DVSPRINTF_ONLY

ifdef DIRSEP
CDEFS+= -DPATH_DELIM=\'$(DIRSEP)\'
endif

LOPT = -L. -L$(LIBDIR) -lhusky

ifneq ($(OSTYPE), UNIX)
#  LIBPREFIX=
else
  LIBPREFIX=lib
endif

# filename settings
ifeq ($(SHORTNAMES), 1)
  FIDOCONFIG     = fidoconf
  FCONF2AREASBBS = fc2abbs.pl
  FCONF2AQUAED   = fc2aed
  FCONF2GOLDED   = fc2ged
  FCONF2MSGED    = fc2msged
  FCONF2FIDOGATE = fc2fgate
  FCONF2SQUISH   = fc2sq
  FCONF2TORNADO  = fc2tor
  FCONF2BINKD    = fc2binkd
  FECFG2FCONF    = fecfg2fc
  LIBFIDOCONFIG  = fidoconf
  CDEFS = $(CDEFS) -DSHORTNAMES
else
  FIDOCONFIG = fidoconfig
  FCONF2AREASBBS = fconf2areasbbs.pl
  FCONF2AQUAED = fconf2aquaed
  FCONF2GOLDED = fconf2golded
  FCONF2MSGED  = fconf2msged
  FCONF2FIDOGATE = fconf2fidogate
  FCONF2SQUISH = fconf2squish
  FCONF2TORNADO  = fconf2tornado
  FCONF2BINKD    = fconf2binkd
  FECFG2FCONF = fecfg2fconf
  LIBFIDOCONFIG = $(LIBPREFIX)fidoconfig
endif

LINKSMAPI = -lhusky -lsmapi
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
	$(LD) $(OPTLFLAGS) -o $(LIBFIDOCONFIG).so.$(VER) \
	    $(LOBJS) $(LOPT)
else
$(LIBFIDOCONFIG).so.$(VER): $(LOBJS)
	$(CC) -shared -Wl,-soname,$(LIBFIDOCONFIG).so.$(VERH) \
         -o $(LIBFIDOCONFIG).so.$(VER) $(LOBJS) $(LOPT)
endif
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so.$(VERH) ;\
	$(LN) $(LNOPT) $(LIBFIDOCONFIG).so.$(VER) $(LIBFIDOCONFIG).so

%$(_OBJ): $(_SRC_DIR)%.c
	$(CC) $(CDEFS) $(COPT) $(_SRC_DIR)$*.c

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
#	$(AR) $(AR_R) $(LIBFIDOCONFIG)$(_LIB) $(LIBDIR)/patmat.o
ifdef RANLIB
	$(RANLIB) $(LIBFIDOCONFIG)$(_LIB)
endif

install: commonlibs progs instdyn
	-$(MKDIR) $(MKDIROPT) $(BINDIR)
	-$(MKDIR) $(MKDIROPT) $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IBOPT) $(FCONF2MSGED)$(_EXE)    $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2GOLDED)$(_EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2AQUAED)$(_EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2FIDOGATE)$(_EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2SQUISH)$(_EXE)   $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2TORNADO)$(_EXE)  $(BINDIR)
	$(INSTALL) $(IBOPT) $(FCONF2BINKD)$(_EXE)    $(BINDIR)
	$(INSTALL) $(IBOPT) linked$(_EXE)	    $(BINDIR)
ifeq ($(CC), gcc)
	$(INSTALL) $(IBOPT) $(FECFG2FCONF)$(_EXE)    $(BINDIR)
endif
	$(INSTALL) $(IBOPT) tparser$(_EXE)          $(BINDIR)
ifeq (${OSTYPE}, UNIX)
	$(INSTALL) $(ISOPT) util/linkedto           $(BINDIR)
endif
	$(INSTALL) $(ISOPT) util/fconf2na.pl        $(BINDIR)
	$(INSTALL) $(ISOPT) util/$(FCONF2AREASBBS)  $(BINDIR)
	$(INSTALL) $(IIOPT) fidoconf/fidoconf.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/areatree.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/findtok.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/common.h       $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/fidoconf.pas   $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/afixcmd.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/arealist.h     $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/version.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(IIOPT) fidoconf/grptree.h      $(INCDIR)$(DIRSEP)fidoconf
	$(INSTALL) $(ISLOPT) $(LIBFIDOCONFIG)$(_LIB) $(LIBDIR)
	(cd doc && $(MAKE) install)
	@echo
	@echo "*** For install man pages run 'gmake install-man' (unixes only)"
	@echo

install-man:
	(cd man && $(MAKE) install)

uninstall:
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2MSGED)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2GOLDED)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2AQUAED)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2FIDOGATE)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2SQUISH)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2TORNADO)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FCONF2BINKD)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)$(FECFG2FCONF)$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)linked$(_EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)tparser$(_EXE)
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
	-$(RM) $(RMOPT) $(INCDIR)$(DIRSEP)fidoconf$(DIRSEP)grptree.h
	-$(RM) $(RMOPT) $(LIBDIR)$(DIRSEP)$(LIBFIDOCONFIG)*
	-(cd doc && $(MAKE) uninstall)
	-(cd man && $(MAKE) uninstall)

