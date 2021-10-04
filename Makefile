# fidoconf/Makefile
#
# This file is part of fidoconf, part of the Husky fidonet software project
# Use with GNU make v.3.82 or later
# Requires: husky enviroment
#

# Version
fidoconf_g1:=$(GREP) -Po 'define\s+FC_VER_MAJOR\s+\K\d+'
fidoconf_g2:=$(GREP) -Po 'define\s+FC_VER_MINOR\s+\K\d+'
fidoconf_g3:=$(GREP) -Po 'define\s+FC_VER_PATCH\s+\K\d+'
fidoconf_g4:=$(GREP) -Po 'char\s+cvs_date\[\]\s*=\s*"\K\d+-\d+-\d+'
fidoconf_VERMAJOR := $(shell $(fidoconf_g1) $(fidoconf_ROOTDIR)$(fidoconf_H_DIR)version.h)
fidoconf_VERMINOR := $(shell $(fidoconf_g2) $(fidoconf_ROOTDIR)$(fidoconf_H_DIR)version.h)
fidoconf_VERPATCH := $(shell $(fidoconf_g3) $(fidoconf_ROOTDIR)$(fidoconf_H_DIR)version.h)
fidoconf_VERH     := $(fidoconf_VERMAJOR).$(fidoconf_VERMINOR)
fidoconf_cvsdate  := $(shell $(fidoconf_g4) $(fidoconf_ROOTDIR)cvsdate.h)
fidoconf_reldate  := $(subst -,,$(fidoconf_cvsdate))
fidoconf_VER      := $(fidoconf_VERH).$(fidoconf_reldate)

ifdef MAN1DIR
    fidoconf_MAN1PAGES := tparser.1

    ifeq ($(FIDOCONF_UTIL), 1)
        fidoconf_MAN1PAGES += fconf2.1 linked.1 linkedto.1
        FCONF2ALIASES = fconf2aquaed.1 fconf2areasbbs.pl.1 fconf2binkd.1 \
                fconf2dir.1 fconf2fidogate.1 fconf2golded.1 fconf2msged.1 \
                fconf2na.pl.1 fconf2squish.1 fconf2tornado.1 fecfg2fconf.1
        ALSGZ := $(foreach man,$(FCONF2ALIASES),$(man).gz)
        fidoconf_MAN1ALS := $(foreach man,$(FCONF2ALIASES),$(DESTDIR)$(MAN1DIR)$(DIRSEP)$(man).gz)
    endif

    fidoconf_MAN1BLD := $(foreach man,$(fidoconf_MAN1PAGES),$(fidoconf_BUILDDIR)$(man).gz)
    fidoconf_MAN1DST := $(foreach man,$(fidoconf_MAN1PAGES),$(DESTDIR)$(MAN1DIR)$(DIRSEP)$(man).gz)
endif


# Object files of the library
# Please sort the list to make checking it by human easy
fidoconf_OBJFILES = $(O)afixcmd$(_OBJ) $(O)afixcmn$(_OBJ) $(O)arealist$(_OBJ) \
                    $(O)areatree$(_OBJ) $(O)cfg$(_OBJ) $(O)common$(_OBJ) \
                    $(O)fidoconf$(_OBJ) $(O)findtok$(_OBJ) $(O)grptree$(_OBJ) \
                    $(O)line$(_OBJ) $(O)stat$(_OBJ) $(O)version$(_OBJ)

# Object files of the programs
AEDOBJ  = $(O)fc2aed$(_OBJ)
BINKOBJ = $(O)fc2binkd$(_OBJ)
FGATEOBJ= $(O)fc2fgate$(_OBJ)
GEDOBJ  = $(O)fc2ged$(_OBJ)
MSGEDOBJ= $(O)fc2msged$(_OBJ)
SQOBJ   = $(O)fc2sq$(_OBJ)
TOROBJ  = $(O)fc2tor$(_OBJ) $(O)fc2tor_g$(_OBJ)
FEOBJ   = $(O)fecfg146$(_OBJ) $(O)fecfg2fc$(_OBJ)
LINKOBJ = $(O)linked$(_OBJ)
TPAROBJ = $(O)tparser$(_OBJ)

fidoconf_PROG_OBJFILES := $(TPAROBJ)
ifeq ($(FIDOCONF_UTIL), 1)
fidoconf_PROG_OBJFILES += $(LINKOBJ) $(AEDOBJ) $(BINKOBJ) $(FGATEOBJ) \
                          $(GEDOBJ) $(MSGEDOBJ) $(SQOBJ) $(TOROBJ) $(FEOBJ)
endif

fidoconf_ALL_OBJFILES := $(fidoconf_PROG_OBJFILES) $(fidoconf_OBJFILES)
fidoconf_ALL_SRC := $(wildcard $(fidoconf_SRCDIR)*.c)

# Prepend directory
fidoconf_OBJS := $(addprefix $(fidoconf_OBJDIR),$(fidoconf_OBJFILES))
fidoconf_PROG_OBJS := $(addprefix $(fidoconf_OBJDIR),$(fidoconf_PROG_OBJFILES))
fidoconf_TOROBJ := $(addprefix $(fidoconf_OBJDIR),$(TOROBJ))
fidoconf_FEOBJ := $(addprefix $(fidoconf_OBJDIR),$(FEOBJ))
fidoconf_ALL_OBJS := $(addprefix $(fidoconf_OBJDIR), $(notdir $(fidoconf_ALL_SRC:.c=$(_OBJ))))

fidoconf_DEPS := $(addprefix $(fidoconf_DEPDIR),$(notdir $(fidoconf_ALL_SRC:.c=$(_DEP))))

# Executable file(s) to build from sources
TPARSER = $(B)tparser$(_EXE)
LINKED  = $(B)linked$(_EXE)
FC2AED  = $(B)fconf2aquaed$(_EXE)
FC2BINKD= $(B)fconf2binkd$(_EXE)
FC2FGATE= $(B)fconf2fidogate$(_EXE)
FC2GED  = $(B)fconf2golded$(_EXE)
FC2MSGED= $(B)fconf2msged$(_EXE)
FC2SQ   = $(B)fconf2squish$(_EXE)
FC2TOR  = $(B)fconf2tornado$(_EXE)
FECFG2FC= $(B)fecfg2fconf$(_EXE)

fidoconf_PROGS := $(TPARSER)
ifeq ($(FIDOCONF_UTIL), 1)
    fidoconf_PROGS += $(LINKED) $(FC2AED) $(FC2BINKD) $(FC2FGATE) $(FC2GED) \
                      $(FC2MSGED) $(FC2SQ) $(FC2TOR) $(FECFG2FC)
endif

fidoconf_PROGS_BLD := $(addprefix $(fidoconf_BUILDDIR),$(fidoconf_PROGS))
fidoconf_PROGS_DST := $(addprefix $(BINDIR_DST),$(fidoconf_PROGS))

# Static and dynamic target libraries
fidoconf_TARGETLIB := $(L)$(LIBPREFIX)$(fidoconf_LIBNAME)$(LIBSUFFIX)$(_LIB)
fidoconf_TARGETDLL := $(B)$(DLLPREFIX)$(fidoconf_LIBNAME)$(DLLSUFFIX)$(_DLL)

ifeq ($(DYNLIBS), 1)
    fidoconf_TARGET = $(fidoconf_TARGETDLL).$(fidoconf_VER)
else
    fidoconf_TARGET = $(fidoconf_TARGETLIB)
endif

fidoconf_TARGET_OBJ = $(fidoconf_OBJDIR)$(fidoconf_TARGET)
fidoconf_TARGET_BLD = $(fidoconf_BUILDDIR)$(fidoconf_TARGET)
fidoconf_TARGET_DST = $(LIBDIR_DST)$(fidoconf_TARGET)

fidoconf_LIBS := $(huskylib_BUILDDIR)$(huskylib_TARGET) $(smapi_BUILDDIR)$(smapi_TARGET)

fidoconf_CDEFS := $(CDEFS) -DCFGDIR=\"$(CFGDIR)\" \
                  -I$(fidoconf_ROOTDIR)$(fidoconf_H_DIR)\
                  -I$(huskylib_ROOTDIR) -I$(smapi_ROOTDIR)
ifdef CFGNAME
CDEFS+= -DCFGNAME=\"$(CFGNAME)\"
endif

ifdef DIRSEP
CDEFS+= -DPATH_DELIM=\'$(DIRSEP)\'
endif

.PHONY: fidoconf_all fidoconf_install fidoconf_install-dynlib fidoconf_uninstall \
        fidoconf_clean fidoconf_distclean fidoconf_depend \
        fidoconf_doc fidoconf_doc_install fidoconf_doc_clean \
        fidoconf_doc_distclean fidoconf_doc_uninstall



ifeq ($(OSTYPE), UNIX)
ifdef MAN1DIR
fidoconf_all: $(fidoconf_TARGET_BLD) $(fidoconf_PROGS_BLD) \
              $(fidoconf_MAN1BLD) fidoconf_doc
else
fidoconf_all: $(fidoconf_TARGET_BLD) $(fidoconf_PROGS_BLD) fidoconf_doc
endif
endif


ifneq ($(MAKECMDGOALS), depend)
    include $(fidoconf_ROOTDIR)doc$(DIRSEP)Makefile
    ifneq ($(MAKECMDGOALS), distclean)
        ifneq ($(MAKECMDGOALS), uninstall)
            include $(fidoconf_DEPS)
        endif
    endif
endif


# Make a hard link of the library in $(fidoconf_BUILDDIR)
$(fidoconf_TARGET_BLD): $(fidoconf_TARGET_OBJ)
	$(LN) $(LNHOPT) $< $(fidoconf_BUILDDIR)

# Build the static library
$(fidoconf_OBJDIR)$(fidoconf_TARGETLIB): $(fidoconf_OBJS) | do_not_run_make_as_root
	cd $(fidoconf_OBJDIR); $(AR) $(AR_R) $(fidoconf_TARGETLIB) $(^F)
ifdef RANLIB
	cd $(fidoconf_OBJDIR); $(RANLIB) $(fidoconf_TARGETLIB)
endif

# Build the dynamic library
$(fidoconf_OBJDIR)$(fidoconf_TARGETDLL).$(fidoconf_VER): $(fidoconf_OBJS) | do_not_run_make_as_root
ifeq (~$(MKSHARED)~,~ld~)
	$(LD) $(LFLAGS) \
	-o $@ $(fidoconf_OBJS)
else
	$(CC) $(LFLAGS) -shared -Wl,-soname,$(fidoconf_TARGETDLL).$(fidoconf_VERH) \
	-o $@ $(fidoconf_OBJS)
endif

# Compile .c files
$(fidoconf_ALL_OBJS): $(fidoconf_OBJDIR)%$(_OBJ): $(fidoconf_SRCDIR)%.c | \
    $(fidoconf_OBJDIR) do_not_run_make_as_root
	$(CC) $(CFLAGS) $(fidoconf_CDEFS) -o $(fidoconf_OBJDIR)$*$(_OBJ) $(fidoconf_SRCDIR)$*.c

$(fidoconf_OBJDIR): | $(fidoconf_BUILDDIR) do_not_run_make_as_root
	[ -d $(fidoconf_OBJDIR) ] || $(MKDIR) $(MKDIROPT) $@

# Build fidoconf utilities
ifeq ($(FIDOCONF_UTIL), 1)
    $(fidoconf_BUILDDIR)$(FC2MSGED): $(fidoconf_OBJDIR)$(MSGEDOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2GED): $(fidoconf_OBJDIR)$(GEDOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2AED): $(fidoconf_OBJDIR)$(AEDOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2FGATE): $(fidoconf_OBJDIR)$(FGATEOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2SQ): $(fidoconf_OBJDIR)$(SQOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2TOR): $(fidoconf_TOROBJ) $(fidoconf_TARGET_BLD) \
        $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FC2BINKD): $(fidoconf_OBJDIR)$(BINKOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(FECFG2FC): $(fidoconf_FEOBJ) $(fidoconf_TARGET_BLD) \
        $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

    $(fidoconf_BUILDDIR)$(LINKED): $(fidoconf_OBJDIR)$(LINKOBJ) \
        $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^
endif

# Build tparser
$(fidoconf_BUILDDIR)$(TPARSER): $(fidoconf_OBJDIR)$(TPAROBJ) \
    $(fidoconf_TARGET_BLD) $(fidoconf_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

# Build man pages
ifdef MAN1DIR
    $(fidoconf_MAN1BLD): $(fidoconf_BUILDDIR)%.gz: $(fidoconf_MANDIR)% | \
        do_not_run_make_as_root
		gzip -c $(fidoconf_MANDIR)$* > $(fidoconf_BUILDDIR)$*.gz
else
    $(fidoconf_MAN1BLD): ;
endif


# Install
ifneq ($(MAKECMDGOALS), install)
    fidoconf_install: ;
else
    fidoconf_install: fidoconf_install_dynlib fidoconf_install_man \
                      fidoconf_install_util fidoconf_install_programs \
                      fidoconf_doc_install ;
endif

ifneq ($(DYNLIBS), 1)
    # Do not install the static library
    fidoconf_install_dynlib: ;
else
    # Install the dynamic library
    ifneq ($(strip $(LDCONFIG)),)
        fidoconf_install_dynlib: \
        $(LIBDIR_DST)$(fidoconf_TARGETDLL).$(fidoconf_VER)
		-@$(LDCONFIG) >& /dev/null || true
    else
        fidoconf_install_dynlib: \
        $(LIBDIR_DST)$(fidoconf_TARGETDLL).$(fidoconf_VER) ;
    endif

    $(LIBDIR_DST)$(fidoconf_TARGETDLL).$(fidoconf_VER): \
        $(fidoconf_BUILDDIR)$(fidoconf_TARGETDLL).$(fidoconf_VER) | \
        $(DESTDIR)$(LIBDIR)
		$(INSTALL) $(ILOPT) $< $(DESTDIR)$(LIBDIR); \
		cd $(DESTDIR)$(LIBDIR); \
		$(TOUCH) $(fidoconf_TARGETDLL).$(fidoconf_VER); \
		$(LN) $(LNOPT) $(fidoconf_TARGETDLL).$(fidoconf_VER) \
		$(fidoconf_TARGETDLL).$(fidoconf_VERH) ;\
		$(LN) $(LNOPT) $(fidoconf_TARGETDLL).$(fidoconf_VER) \
		$(fidoconf_TARGETDLL)
endif

# Install man pages
ifdef MAN1DIR
    ifneq ($(FIDOCONF_UTIL), 1)
        fidoconf_install_man: $(fidoconf_MAN1DST) ;
    else
        fidoconf_install_man: $(fidoconf_MAN1DST) $(fidoconf_MAN1ALS) ;
    endif

    $(fidoconf_MAN1DST): $(DESTDIR)$(MAN1DIR)$(DIRSEP)%: \
        $(fidoconf_BUILDDIR)% | $(DESTDIR)$(MAN1DIR)
	$(INSTALL) $(IMOPT) $(fidoconf_BUILDDIR)$* $(DESTDIR)$(MAN1DIR); \
	$(TOUCH) $(DESTDIR)$(MAN1DIR)$(DIRSEP)$*

    ifeq ($(FIDOCONF_UTIL), 1)
    $(fidoconf_MAN1ALS): $(DESTDIR)$(MAN1DIR)$(DIRSEP)fconf2.1.gz | \
        $(DESTDIR)$(MAN1DIR)
	-cd $(DESTDIR)$(MAN1DIR); \
	for f in $(ALSGZ); do $(LN) $(LNOPT) fconf2.1.gz $$f; done
    endif
else
    fidoconf_install_man: ;
endif

fidoconf_install_programs: $(fidoconf_PROGS_DST) ;

$(fidoconf_PROGS_DST): $(BINDIR_DST)%: $(fidoconf_BUILDDIR)% | \
    $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) $(fidoconf_BUILDDIR)$* $(DESTDIR)$(BINDIR); \
	$(TOUCH) $(BINDIR_DST)$*

# fidoconf utilities
ifeq ($(FIDOCONF_UTIL), 1)
    ifeq (${OSTYPE}, UNIX)
        fidoconf_UTIL:=linkedto fconf2na.pl sq2fc.pl fconf2areasbbs.pl
    else
        fidoconf_UTIL:=fconf2na.pl sq2fc.pl fconf2areasbbs.pl
    endif

    fidoconf_UTIL_DST=$(addprefix $(BINDIR_DST),$(fidoconf_UTIL))

    fidoconf_install_util: $(fidoconf_UTIL_DST) ;

    # Install fidoconf utilities
    $(fidoconf_UTIL_DST): $(BINDIR_DST)%: \
        $(fidoconf_ROOTDIR)util$(DIRSEP)% | $(DESTDIR)$(BINDIR)
		$(INSTALL) $(ISOPT) $(fidoconf_ROOTDIR)util$(DIRSEP)$* $(DESTDIR)$(BINDIR); \
		$(TOUCH) $(BINDIR_DST)$*
else
    fidoconf_install_util: ;
endif

# Clean
fidoconf_clean: fidoconf_rmdir_OBJ ;

fidoconf_rmdir_OBJ: fidoconf_clean_OBJ fidoconf_doc_clean
	-[ -d "$(fidoconf_OBJDIR)" ] && $(RMDIR) $(fidoconf_OBJDIR) || true

fidoconf_clean_OBJ:
	-$(RM) $(RMOPT) $(fidoconf_OBJDIR)*

# Distclean
fidoconf_distclean: fidoconf_rmdir_BLD ;

fidoconf_rmdir_BLD: fidoconf_doc_distclean fidoconf_main_distclean fidoconf_rmdir_DEP;
	-[ -d "$(fidoconf_BUILDDIR)" ] && $(RMDIR) $(fidoconf_BUILDDIR) || true

fidoconf_rmdir_DEP: fidoconf_rm_DEPS
	-[ -d "$(fidoconf_DEPDIR)" ] && $(RMDIR) $(fidoconf_DEPDIR) || true

fidoconf_rm_DEPS:
	-$(RM) $(RMOPT) $(fidoconf_DEPDIR)*

fidoconf_main_distclean: fidoconf_clean ;
	-$(RM) $(RMOPT) $(fidoconf_TARGET_BLD)
	-$(RM) $(RMOPT) $(fidoconf_PROGS_BLD)
ifeq ($(OSTYPE), UNIX)
    ifdef MAN1DIR
		-$(RM) $(RMOPT) $(fidoconf_MAN1BLD)
    endif
endif


# Uninstall
fidoconf_uninstall: fidoconf_main_uninstall fidoconf_doc_uninstall

fidoconf_main_uninstall:
ifeq ($(DYNLIBS), 1)
	-$(RM) $(RMOPT) $(DESTDIR)$(LIBDIR)$(DIRSEP)$(fidoconf_TARGETDLL)*
endif
	-$(RM) $(RMOPT) $(fidoconf_PROGS_DST)
ifeq ($(FIDOCONF_UTIL), 1)
	-$(RM) $(RMOPT) $(fidoconf_UTIL_DST)
endif
ifdef MAN1DIR
    ifeq ($(FIDOCONF_UTIL), 1)
		-$(RM) $(RMOPT) $(fidoconf_MAN1ALS)
    endif
	-$(RM) $(RMOPT) $(fidoconf_MAN1DST)
endif


# Depend
ifeq ($(MAKECMDGOALS),depend)
fidoconf_depend: $(fidoconf_DEPS) ;

# Build a dependency makefile for every source file
$(fidoconf_DEPS): $(fidoconf_DEPDIR)%$(_DEP): $(fidoconf_SRCDIR)%.c | \
    $(fidoconf_DEPDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(fidoconf_CDEFS) $< > $@.$$$$; \
	sed 's,\($*\)$(_OBJ)[ :]*,$(fidoconf_OBJDIR)\1$(_OBJ) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(fidoconf_DEPDIR): | $(fidoconf_BUILDDIR) do_not_run_depend_as_root
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@
endif

$(fidoconf_BUILDDIR):
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@
