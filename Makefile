# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
include /usr/share/husky/huskymak.cfg
else
include ../huskymak.cfg
endif

ifndef ISOPT
# install scripts options
  ISOPT = -c -m 555
endif

# program settings
CINCL = -Ifidoconf -I$(INCDIR)

ifeq ($(DEBUG), 1)
  CFLAGS += $(WARNFLAGS) $(DEBCFLAGS)
  LFLAGS += $(DEBLFLAGS)
else
  CFLAGS += $(WARNFLAGS) $(OPTCFLAGS)
  LFLAGS += $(OPTLFLAGS)
endif

ifndef EXENAMEFLAG
  EXENAMEFLAG=-o
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -DCFGDIR=\"$(CFGDIR)\"
ifdef CFGNAME
CDEFS+= -DCFGNAME=\"$(CFGNAME)\"
endif

# Use -DVSPRINTF_ONLY on systems without vsnprintf() and snprintf()
# Use this for sunOs 2.5.1 (Thanks to Serguei Revtov)
#CDEFS	+= -DVSPRINTF_ONLY

ifdef DIRSEP
CDEFS+= -DPATH_DELIM=\'$(DIRSEP)\'
endif

ifeq ($(OSTYPE), UNIX)
  LIBPREFIX=lib
endif

# filename settings
ifeq ($(SHORTNAMES), 1)
  CDEFS = $(CDEFS) -DSHORTNAMES
endif

_SRC_DIR = src/

ifeq ($(OSTYPE), UNIX)
  LIBPREFIX=lib
  DLLPREFIX=lib
endif

default: all

include make/makefile.inc
include makefile.in2
TARGETLIB = $(LIBPREFIX)$(LIBNAME)$(LIBSUFFIX)$(_LIB)
TARGETDLL = $(DLLPREFIX)$(LIBNAME)$(DLLSUFFIX)$(_DLL)

progs: commonprogs

ifeq ($(DYNLIBS), 1)
  all: commonlibs $(TARGETDLL).$(VER)
	$(MAKE) progs
	(cd doc && $(MAKE) all)
else
  all: commonlibs
	$(MAKE) progs
	(cd doc && $(MAKE) all)
endif


ifeq (~$(MKSHARED)~, ~ld~)
$(TARGETDLL).$(VER): $(LOBJS)
	$(LD) $(LFLAGS) $(EXENAMEFLAG) $(TARGETDLL).$(VER) $(LOBJS)
else
$(TARGETDLL).$(VER): $(LOBJS)
	$(CC) -shared -Wl,-soname,$(TARGETDLL).$(VERH) \
	-o $(TARGETDLL).$(VER) $(LOBJS)
endif
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL).$(VERH) ;\
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL)


clean: commonclean
	-$(RM) $(RMOPT) $(TARGETDLL).$(VERH)
	-$(RM) $(RMOPT) $(TARGETDLL)
	(cd doc && $(MAKE) clean)

distclean: commondistclean
	-$(RM) $(RMOPT) $(TARGETDLL)*
	(cd doc && $(MAKE) distclean)


ifeq ($(DYNLIBS), 1)
instdyn: $(TARGETLIB) $(TARGETDLL).$(VER)
	-$(MKDIR) $(MKDIROPT) $(LIBDIR)
	$(INSTALL) $(ILOPT) $(TARGETDLL).$(VER) $(LIBDIR)
	-$(RM) $(RMOPT) $(LIBDIR)$(DIRSEP)$(TARGETDLL).$(VERH)
	-$(RM) $(RMOPT) $(LIBDIR)$(DIRSEP)$(TARGETDLL)
# Removed path from symlinks.
	cd $(LIBDIR) ;\
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL).$(VERH) ;\
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL)
ifneq (~$(LDCONFIG)~, ~~)
	$(LDCONFIG)
endif

else
instdyn: commonlibs
endif


install: commonlibs progs instdyn
	-$(MKDIR) $(MKDIROPT) $(BINDIR)
	-$(MKDIR) $(MKDIROPT) $(INCDIR)/fidoconf
	$(INSTALL) $(IBOPT) $(PROGRAMS) $(BINDIR)
	$(INSTALL) $(IBOPT) linked$(_EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) tparser$(_EXE) $(BINDIR)
ifeq (${OSTYPE}, UNIX)
	$(INSTALL) $(ISOPT) util/linkedto $(BINDIR)
endif
	$(INSTALL) $(ISOPT) util/fconf2na.pl $(BINDIR)
	$(INSTALL) $(ISOPT) util/fconf2areasbbs.pl $(BINDIR)
	cd fidoconf ; $(INSTALL) $(IIOPT) $(HEADERS) $(INCDIR)/fidoconf
	$(INSTALL) $(ISLOPT) $(TARGETLIB) $(LIBDIR)
	(cd doc && $(MAKE) install)
	@echo
	@echo "*** For install man pages run 'gmake install-man' (unixes only)"
	@echo

install-man:
	(cd man && $(MAKE) install)

uninstall:
	-cd $(BINDIR) ;\
	-$(RM) $(RMOPT) $(PROGRAMS) linked$(_EXE) tparser$(_EXE) linkedto \
	fconf2na.pl fconf2areasbbs.pl
	-cd $(INCDIR)/fidoconf ;\
	-$(RM) $(RMOPT) $(HEADERS)
	-$(RM) $(RMOPT) $(LIBDIR)/$(TARGETLIB)
	-$(RM) $(RMOPT) $(LIBDIR)/$(TARGETDLL)*
	-(cd doc && $(MAKE) uninstall)
	-(cd man && $(MAKE) uninstall)
