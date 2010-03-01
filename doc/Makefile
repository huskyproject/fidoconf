# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else ifdef RPM_BUILD_ROOT
# For RPM build is require all files in one directory branch
include ../huskymak.cfg
else
include ../../huskymak.cfg
endif

all: html info dvi

ifdef HTMLDIR
html: fidoconfig.html
	-wget -O fastecho2fidoconfig.html "http://sourceforge.net/apps/mediawiki/husky/index.php?title=%D0%9F%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D0%B0_fecfg2fconf&printable=yes"
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

fidoconfig.html: fidoconfig.texi proposal.texi
	export LC_ALL=C; \
	makeinfo --html --no-split fidoconfig.texi

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
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(INFODIR)
	$(INSTALL) $(IMOPT) fidoconfig.info $(DESTDIR)$(INFODIR)
	-install-info --info-dir=$(INFODIR)  $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
endif
ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(HTMLDIR)
	$(INSTALL) $(IMOPT) fidoconfig*html $(DESTDIR)$(HTMLDIR)
	-$(INSTALL) $(IMOPT) fastecho2fidoconfig.html $(DESTDIR)$(HTMLDIR)
endif
ifdef DVIDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(DVIDIR)
	$(INSTALL) $(IMOPT) fidoconfig.dvi $(DESTDIR)$(DVIDIR)
endif

uninstall:
	-install-info --remove --info-dir=$(INFODIR) $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(DESTDIR)$(HTMLDIR)$(DIRSEP)fidoconfig*html $(DESTDIR)$(HTMLDIR)$(DIRSEP)fastecho2fidoconfig.html
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.dvi
