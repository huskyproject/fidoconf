# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else ifdef RPM_BUILD_ROOT
# RPM build requires all files to be in the source directory
include ../huskymak.cfg
else
include ../../huskymak.cfg
endif

all: info html ruhtml txt rutxt dvi rudvi pdf

ifdef INFODIR
info: fidoconfig.info
else ifdef RUINFODIR
info: fidoconfig.info
else
info: ;
endif

ifdef HTMLDIR
html: fidoconfig.html
else
html: ;
endif

ifdef RUHTMLDIR
ruhtml: fidoconfig.ru.html
else
ruhtml: ;
endif

ifdef TXTDIR
txt: fidoconfig.txt
else
txt: ;
endif

ifdef RUTXTDIR
rutxt: fidoconfig.ru.txt
else
rutxt: ;
endif

ifdef DVIDIR
dvi: fidoconfig.dvi
else
dvi: ;
endif

ifdef RUDVIDIR
rudvi: fidoconfig.ru.dvi
else
rudvi: ;
endif

ifdef PDFDIR
pdf: fidoconfig.pdf
else
pdf: ;
endif

ifdef INFODIR
fidoconfig.info: fidoconfig.texi proposal.texi
	-makeinfo --no-split fidoconfig.texi -o fidoconfig.info
else ifdef RUINFODIR
fidoconfig.info: rus/fidoconfig.texi rus/proposal.texi
	-LC_ALL=C makeinfo --no-split -o fidoconfig.info rus/fidoconfig.texi
endif

fidoconfig.html: fidoconfig.texi proposal.texi
	-LC_ALL=C makeinfo --html --no-split fidoconfig.texi -o fidoconfig.html

fidoconfig.ru.html: rus/fidoconfig.texi rus/proposal.texi
	-LC_ALL=C makeinfo --html --no-split -o fidoconfig.ru.html rus/fidoconfig.texi

fidoconfig.txt: fidoconfig.texi proposal.texi
	-LC_ALL=C makeinfo --plaintext --no-split fidoconfig.texi -o fidoconfig.txt

fidoconfig.ru.txt: rus/fidoconfig.texi rus/proposal.texi
	-LC_ALL=C makeinfo --plaintext --no-split rus/fidoconfig.texi -o fidoconfig.ru.txt

fidoconfig.dvi: fidoconfig.texi proposal.texi
	-texi2dvi fidoconfig.texi

fidoconfig.ru.dvi: rus/fidoconfig.texi rus/proposal.texi
	-LC_ALL=C texi2dvi rus/fidoconfig.texi -o fidoconfig.ru.dvi

fidoconfig.pdf: fidoconfig.texi proposal.texi
	-LC_ALL=C makeinfo --pdf --no-split fidoconfig.texi -o fidoconfig.pdf

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
	-$(RM) $(RMOPT) fidoconfig*dvi
	-$(RM) $(RMOPT) fidoconfig.inf
	-$(RM) $(RMOPT) fidoconfig*html
	-$(RM) $(RMOPT) fidoconfig.ps
	-$(RM) $(RMOPT) fidoconfig.doc
	-$(RM) $(RMOPT) fidoconfig*txt
	-$(RM) $(RMOPT) fidoconfig.pdf
	-$(RM) $(RMOPT) fidoconfig.info

install: info html ruhtml txt rutxt dvi rudvi pdf
ifdef INFODIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(INFODIR)
	$(INSTALL) $(IMOPT) fidoconfig.info $(DESTDIR)$(INFODIR)
	-install-info --info-dir=$(INFODIR)  $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
else ifdef RUINFODIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUINFODIR)
	$(INSTALL) $(IMOPT) fidoconfig.info $(DESTDIR)$(RUINFODIR)
	-install-info --info-dir=$(RUINFODIR)  $(DESTDIR)$(RUINFODIR)$(DIRSEP)fidoconfig.info
endif

ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(HTMLDIR)
	$(INSTALL) $(IMOPT) fidoconfig.html $(DESTDIR)$(HTMLDIR)
endif
ifdef RUHTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUHTMLDIR)
	$(INSTALL) $(IMOPT) fidoconfig.ru.html $(DESTDIR)$(RUHTMLDIR)
endif
ifdef TXTDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(TXTDIR)
	$(INSTALL) $(IMOPT) fidoconfig.txt $(DESTDIR)$(TXTDIR)
endif
ifdef RUTXTDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUTXTDIR)
	$(INSTALL) $(IMOPT) fidoconfig.ru.txt $(DESTDIR)$(RUTXTDIR)
endif
ifdef DVIDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(DVIDIR)
	$(INSTALL) $(IMOPT) fidoconfig.dvi $(DESTDIR)$(DVIDIR)
endif
ifdef RUDVIDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(RUDVIDIR)
	$(INSTALL) $(IMOPT) fidoconfig.ru.dvi $(DESTDIR)$(RUDVIDIR)
endif
ifdef PDFDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(PDFDIR)
	$(INSTALL) $(IMOPT) fidoconfig.pdf $(DESTDIR)$(PDFDIR)
endif

uninstall:
ifdef INFODIR
	-install-info --remove --info-dir=$(INFODIR) \
      $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info >& /dev/null || true
	-$(RM) $(RMOPT) $(DESTDIR)$(INFODIR)$(DIRSEP)fidoconfig.info
else ifdef RUINFODIR
	-install-info --remove --info-dir=$(RUINFODIR) \
      $(DESTDIR)$(RUINFODIR)$(DIRSEP)fidoconfig.info >& /dev/null || true
	-$(RM) $(RMOPT) $(DESTDIR)$(RUINFODIR)$(DIRSEP)fidoconfig.info
endif
ifdef HTMLDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(HTMLDIR)$(DIRSEP)fidoconfig.html
	-$(RMDIR) $(DESTDIR)$(HTMLDIR) >& /dev/null || true
endif
ifdef RUHTMLDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(RUHTMLDIR)$(DIRSEP)fidoconfig.ru.html
	-$(RMDIR) $(DESTDIR)$(RUHTMLDIR) >& /dev/null || true
endif
ifdef TXTDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(TXTDIR)$(DIRSEP)fidoconfig.txt
	-$(RMDIR) $(DESTDIR)$(TXTDIR) >& /dev/null || true
endif
ifdef RUTXTDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(RUTXTDIR)$(DIRSEP)fidoconfig.ru.txt
	-$(RMDIR) $(DESTDIR)$(RUTXTDIR) >& /dev/null || true
endif
ifdef DVIDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(DVIDIR)$(DIRSEP)fidoconfig.dvi
	-$(RMDIR) $(DESTDIR)$(DVIDIR) >& /dev/null || true
endif
ifdef RUDVIDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(RUDVIDIR)$(DIRSEP)fidoconfig.ru.dvi
	-$(RMDIR) $(DESTDIR)$(RUDVIDIR) >& /dev/null || true
endif
ifdef PDFDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(PDFDIR)$(DIRSEP)fidoconfig.pdf
	-$(RMDIR) $(DESTDIR)$(PDFDIR) >& /dev/null || true
endif
