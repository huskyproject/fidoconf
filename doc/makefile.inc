alldocs: info ruinfo html ruhtml txt rutxt dvi rudvi pdf

ifdef INFODIR
info: doc/fidoconfig.info
else
info:
	@cd ./

ifdef RUINFODIR
ruinfo: doc/fidoconfig.ru.info
else
ruinfo:
	@cd ./
endif
endif

ifdef HTMLDIR
html: doc/fidoconfig.html
else
html:
	@cd ./
endif

ifdef RUHTMLDIR
ruhtml: doc/fidoconfig.ru.html
else
ruhtml:
	@cd ./
endif

ifdef TXTDIR
txt: doc/fidoconfig.txt
else
txt:
	@cd ./
endif

ifdef RUTXTDIR
rutxt: doc/fidoconfig.ru.txt
else
rutxt:
	@cd ./
endif

ifdef DVIDIR
dvi: doc/fidoconfig.dvi
else
dvi:
	@cd ./
endif

ifdef RUDVIDIR
rudvi: doc/fidoconfig.ru.dvi
else
rudvi:
	@cd ./
endif

ifdef PDFDIR
pdf: doc/fidoconfig.pdf
else
pdf:
	@cd ./
endif

doc/fidoconfig.info: doc/fidoconfig.texi doc/proposal.texi
	-makeinfo --no-split doc/fidoconfig.texi -o doc/fidoconfig.info

doc/fidoconfig.ru.info: doc/rus/fidoconfig.texi doc/rus/proposal.texi
	-LC_ALL=C makeinfo --no-split -o doc/fidoconfig.ru.info doc/rus/fidoconfig.texi

doc/fidoconfig.html: doc/fidoconfig.texi doc/proposal.texi
	-LC_ALL=C makeinfo --html --no-split doc/fidoconfig.texi -o doc/fidoconfig.html

doc/fidoconfig.ru.html: doc/rus/fidoconfig.texi doc/rus/proposal.texi
	-LC_ALL=C makeinfo --html --no-split -o doc/fidoconfig.ru.html doc/rus/fidoconfig.texi

doc/fidoconfig.txt: doc/fidoconfig.texi doc/proposal.texi
	-LC_ALL=C makeinfo --plaintext --no-split doc/fidoconfig.texi -o doc/fidoconfig.txt

doc/fidoconfig.ru.txt: doc/rus/fidoconfig.texi doc/rus/proposal.texi
	-LC_ALL=C makeinfo --plaintext --no-split doc/rus/fidoconfig.texi -o doc/fidoconfig.ru.txt

doc/fidoconfig.dvi: doc/fidoconfig.texi doc/proposal.texi
	-texi2dvi doc/fidoconfig.texi

doc/fidoconfig.ru.dvi: doc/rus/fidoconfig.texi doc/rus/proposal.texi
	-LC_ALL=C texi2dvi doc/rus/fidoconfig.texi -o doc/fidoconfig.ru.dvi

doc/fidoconfig.pdf: doc/fidoconfig.texi doc/proposal.texi
	-cd doc; LC_ALL=C makeinfo --pdf --no-split fidoconfig.texi -o fidoconfig.pdf; cd ..

docs-clean:
	-$(RM) $(RMOPT) doc/fidoconfig.fns
	-$(RM) $(RMOPT) doc/fidoconfig.aux
	-$(RM) $(RMOPT) doc/fidoconfig.toc
	-$(RM) $(RMOPT) doc/fidoconfig.cp
	-$(RM) $(RMOPT) doc/fidoconfig.fn
	-$(RM) $(RMOPT) doc/fidoconfig.vr
	-$(RM) $(RMOPT) doc/fidoconfig.kr
	-$(RM) $(RMOPT) doc/fidoconfig.ke
	-$(RM) $(RMOPT) doc/fidoconfig.tp
	-$(RM) $(RMOPT) doc/fidoconfig.kes
	-$(RM) $(RMOPT) doc/fidoconfig.krs
	-$(RM) $(RMOPT) doc/fidoconfig.ky
	-$(RM) $(RMOPT) doc/fidoconfig.pg
	-$(RM) $(RMOPT) doc/fidoconfig.cps
	-$(RM) $(RMOPT) doc/fidoconfig.log
	-$(RM) $(RMOPT) doc/*~

docs-distclean: docs-clean
	-$(RM) $(RMOPT) doc/fidoconfig*dvi
	-$(RM) $(RMOPT) doc/fidoconfig.inf
	-$(RM) $(RMOPT) doc/fidoconfig*html
	-$(RM) $(RMOPT) doc/fidoconfig.ps
	-$(RM) $(RMOPT) doc/fidoconfig.doc
	-$(RM) $(RMOPT) doc/fidoconfig*txt
	-$(RM) $(RMOPT) doc/fidoconfig.pdf
	-$(RM) $(RMOPT) doc/fidoconfig*info

docs-install: info ruinfo html ruhtml txt rutxt dvi rudvi pdf
ifdef INFODIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(INFODIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.info $(DESTDIR)$(INFODIR)
	-install-info --info-dir=$(INFODIR)  $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
else
ifdef RUINFODIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUINFODIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.ru.info $(DESTDIR)$(RUINFODIR)
	-install-info --info-dir=$(RUINFODIR)  $(DESTDIR)$(RUINFODIR)$(DIRSEP)fidoconfig.info
endif
endif

ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(HTMLDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.html $(DESTDIR)$(HTMLDIR)
endif
ifdef RUHTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUHTMLDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.ru.html $(DESTDIR)$(RUHTMLDIR)
endif
ifdef TXTDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(TXTDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.txt $(DESTDIR)$(TXTDIR)
endif
ifdef RUTXTDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUTXTDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.ru.txt $(DESTDIR)$(RUTXTDIR)
endif
ifdef DVIDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(DVIDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.dvi $(DESTDIR)$(DVIDIR)
endif
ifdef RUDVIDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUDVIDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.ru.dvi $(DESTDIR)$(RUDVIDIR)
endif
ifdef PDFDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(PDFDIR)
	$(INSTALL) $(IMOPT) doc/fidoconfig.pdf $(DESTDIR)$(PDFDIR)
endif

docs-uninstall:
	-install-info --remove --info-dir=$(INFODIR) $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.html
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.ru.html
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.txt
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.ru.txt
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.dvi
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.ru.dvi
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.pdf
