# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include ../debian/huskymak.cfg
else
include ../../huskymak.cfg
endif

all: html info dvi

ifdef HTMLDIR
html: fidoconfig_toc.html
else
html:
	@echo
endif

ifdef DVIDIR
dvi: fidoconfig.dvi
else
dvi:
	@echo
endif

ifdef INFODIR
info: fidoconfig.info
else
info:
	@echo
endif

fidoconfig_toc.html: fidoconfig.texi proposal.texi
	export LC_ALL=C; \
	makeinfo --html --no-split fidoconfig.texi
# Old rules: build with texi2html
#	texi2html -split_node -number -menu fidoconfig.texi

fidoconfig.dvi: fidoconfig.texi proposal.texi
	texi2dvi fidoconfig.texi
 
fidoconfig.info: fidoconfig.texi proposal.texi
	makeinfo --no-split  fidoconfig.texi

clean:
	-$(RM) $(RMOPT) fidoconfig.fns
	-$(RM) $(RMOPT) fidoconfig.aux
	-$(RM) $(RMOPT) fidoconfig.toc
	-$(RM) $(RMOPT) fidoconfig.cp
	-$(RM) $(RMOPT) fidoconfig.fn
	-$(RM) $(RMOPT) fidoconfig.vr
	-$(RM) $(RMOPT) fidoconfig.kr
	-$(RM) $(RMOPT) fidoconfig.ke
	-$(RM) $(RMOPT) fidoconfig.tp
	-$(RM) $(RMOPT) fidoconfig.kes
	-$(RM) $(RMOPT) fidoconfig.krs
	-$(RM) $(RMOPT) fidoconfig.ky
	-$(RM) $(RMOPT) fidoconfig.pg
	-$(RM) $(RMOPT) fidoconfig.cps
	-$(RM) $(RMOPT) fidoconfig.log
	-$(RM) $(RMOPT) fidoconfig.fns
	-$(RM) $(RMOPT) *~

distclean: clean
	-$(RM) $(RMOPT) fidoconfig.dvi
	-$(RM) $(RMOPT) fidoconfig.inf
	-$(RM) $(RMOPT) fidoconfig*html
	-$(RM) $(RMOPT) fidoconfig.ps
	-$(RM) $(RMOPT) fidoconfig.doc
	-$(RM) $(RMOPT) fidoconfig.info*

install: info html dvi
	@echo .
ifdef INFODIR
	-$(MKDIR) $(MKDIROPT) $(INFODIR)
	$(INSTALL)  fidoconfig.info $(INFODIR)
	-install-info --info-dir=$(INFODIR)  $(INFODIR)$(DIRSEP)fidoconfig.info
endif
ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(HTMLDIR)
	$(INSTALL)  fidoconfig*html $(HTMLDIR)
endif

uninstall:
	-install-info --remove --info-dir=$(INFODIR) $(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(HTMLDIR)$(DIRSEP)fidoconfig*html

