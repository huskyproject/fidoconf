include ../../huskymak.cfg

default: info

html: fidoconfig_toc.html

dvi: fidoconfig.dvi

info: fidoconfig.info

fidoconfig_toc.html: fidoconfig.texi
	texi2html -split_node -number -menu fidoconfig.texi

fidoconfig.dvi: fidoconfig.texi
	texi2dvi fidoconfig.texi

fidoconfig.info: fidoconfig.texi
	makeinfo --no-split  fidoconfig.texi

clean:
	-$(RM) fidoconfig.fns
	-$(RM) *~
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


install: info
	-$(MKDIR) $(MKDIROPT) $(INFODIR)
	$(INSTALL)  fidoconfig.info $(INFODIR)
	install-info --info-dir=$(INFODIR)  $(INFODIR)$(DIRSEP)fidoconfig.info
