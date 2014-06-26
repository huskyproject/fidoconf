# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
include /usr/share/husky/huskymak.cfg
else ifdef RPM_BUILD_ROOT
# RPM build requires all files to be in the source directory
include huskymak.cfg
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

LFLAGS += -L$(LIBDIR)

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

ifeq ($(SHORTNAMES), 1)
include make/fn_short.inc
else
include make/fn_long.inc
endif
include make/makefile.inc
include makefile.in2
TARGETLIB = $(LIBPREFIX)$(LIBNAME)$(LIBSUFFIX)$(_LIB)
TARGETDLL = $(DLLPREFIX)$(LIBNAME)$(DLLSUFFIX)$(_DLL)
LIBS=-lhusky

progs: commonprogs

ifeq ($(DYNLIBS), 1)
  TARGET = $(TARGETDLL)
  all: commonlibs $(TARGETDLL).$(VER)
	$(MAKE) progs
	(cd doc && $(MAKE) all)
else
  TARGET = $(TARGETLIB)
  all: commonlibs
	$(MAKE) progs
	(cd doc && $(MAKE) all)
endif


ifeq (~$(MKSHARED)~, ~ld~)
$(TARGETDLL).$(VER): $(LOBJS)
	$(LD) $(LFLAGS) $(EXENAMEFLAG) $(TARGETDLL).$(VER) $(LOBJS) $(LIBS)
else
$(TARGETDLL).$(VER): $(LOBJS)
	$(CC) $(LFLAGS) -shared -Wl,-soname,$(TARGETDLL).$(VERH) \
	-o $(TARGETDLL).$(VER) $(LOBJS) $(LIBS)
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
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(LIBDIR)
	$(INSTALL) $(ILOPT) $(TARGETDLL).$(VER) $(DESTDIR)$(LIBDIR)
	-$(RM) $(RMOPT) $(DESTDIR)$(LIBDIR)$(DIRSEP)$(TARGETDLL).$(VERH)
	-$(RM) $(RMOPT) $(DESTDIR)$(LIBDIR)$(DIRSEP)$(TARGETDLL)
# Removed path from symlinks.
	cd $(DESTDIR)$(LIBDIR) ;\
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL).$(VERH) ;\
	$(LN) $(LNOPT) $(TARGETDLL).$(VER) $(TARGETDLL)
ifneq (~$(LDCONFIG)~, ~~)
	$(LDCONFIG)
endif

else
instdyn: commonlibs
endif


install: commonlibs progs instdyn
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(BINDIR)
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(INCDIR)/fidoconf
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(LIBDIR)
	$(INSTALL) $(IBOPT) $(PROGRAMS) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) linked$(_EXE) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) tparser$(_EXE) $(DESTDIR)$(BINDIR)
ifeq (${OSTYPE}, UNIX)
	$(INSTALL) $(ISOPT) util/linkedto $(DESTDIR)$(BINDIR)
endif
	$(INSTALL) $(ISOPT) util/fconf2na.pl util/sq2fc.pl $(DESTDIR)$(BINDIR)
	$(INSTALL) $(ISOPT) util/fconf2areasbbs.pl $(DESTDIR)$(BINDIR)
	cd fidoconf ; $(INSTALL) $(IIOPT) $(HEADERS) $(DESTDIR)$(INCDIR)/fidoconf
	$(INSTALL) $(ISLOPT) $(TARGETLIB) $(DESTDIR)$(LIBDIR)
	(cd doc && $(MAKE) install)
	@echo
	@echo "*** For install man pages run 'gmake install-man' (unixes only)"
	@echo

install-man:
	(cd man && $(MAKE) install)

uninstall:
	-cd $(DESTDIR)$(BINDIR) ;\
	-$(RM) $(RMOPT) $(PROGRAMS) linked$(_EXE) tparser$(_EXE) linkedto \
	fconf2na.pl fconf2areasbbs.pl sq2fc.pl
	-cd $(DESTDIR)$(INCDIR)/fidoconf ;\
	-$(RM) $(RMOPT) $(HEADERS)
	-$(RM) $(RMOPT) $(DESTDIR)$(LIBDIR)/$(TARGETLIB)
	-$(RM) $(RMOPT) $(DESTDIR)$(LIBDIR)/$(TARGETDLL)*
	-(cd doc && $(MAKE) uninstall)
	-(cd man && $(MAKE) uninstall)
