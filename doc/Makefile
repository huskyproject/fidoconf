include ../../huskymak.cfg

all: html info dvi

ifdef HTMLDIR
html: fidoconfig_toc.html
endif

ifdef DVIDIR
dvi: fidoconfig.dvi
endif

ifdef INFODIR
info: fidoconfig.info
endif

fidoconfig_toc.html: fidoconfig.texi
	texi2html -number -menu fidoconfig.texi

fidoconfig.dvi: fidoconfig.texi
	texi2dvi fidoconfig.texi

fidoconfig.info: fidoconfig.texi
	makeinfo --no-split  fidoconfig.texi

clean:
	-$(RM) fidoconfig.fns
	-$(RM) fidoconfig.aux
	-$(RM) fidoconfig.toc
	-$(RM) fidoconfig.cp
	-$(RM) fidoconfig.fn
	-$(RM) fidoconfig.vr
	-$(RM) fidoconfig.kr
	-$(RM) fidoconfig.ke
	-$(RM) fidoconfig.tp
	-$(RM) fidoconfig.kes
	-$(RM) fidoconfig.krs
	-$(RM) fidoconfig.ky
	-$(RM) fidoconfig.pg
	-$(RM) fidoconfig.cps
	-$(RM) fidoconfig.log
	-$(RM) fidoconfig.fns
	-$(RM) *~

distclean: clean
	-$(RM) fidoconfig.dvi
	-$(RM) fidoconfig.inf
	-$(RM) fidoconfig*html
	-$(RM) fidoconfig.ps
	-$(RM) fidoconfig.doc
	-$(RM) fidoconfig.info*

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
	-$(RM) $(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(HTMLDIR)$(DIRSEP)fidoconfig*html

