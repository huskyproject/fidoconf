include ../../huskymak.cfg

default: info

html: fidoconfig_toc.html proposal_toc.html

dvi: fidoconfig.dvi proposal.dvi

info: fidoconfig.info proposal.info

fidoconfig_toc.html: fidoconfig.texi
	texi2html -split_node -number -menu fidoconfig.texi

proposal_toc.html: proposal.texi
	texi2html -split_node -number -menu proposal.texi

fidoconfig.dvi: fidoconfig.texi
	texi2dvi fidoconfig.texi

proposal.dvi: proposal.texi
	texi2dvi proposal.texi

fidoconfig.info: fidoconfig.texi
	makeinfo --no-split  fidoconfig.texi

proposal.info: proposal.texi
	makeinfo --no-split proposal.texi

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
	-$(RM) proposal.fns
	-$(RM) proposal.aux
	-$(RM) proposal.toc
	-$(RM) proposal.cp
	-$(RM) proposal.fn
	-$(RM) proposal.vr
	-$(RM) proposal.kr
	-$(RM) proposal.ke
	-$(RM) proposal.tp
	-$(RM) proposal.kes
	-$(RM) proposal.krs
	-$(RM) proposal.ky
	-$(RM) proposal.pg
	-$(RM) proposal.cps
	-$(RM) proposal.log
	-$(RM) proposal.fns
	-$(RM) *~

distclean: clean
	-$(RM) fidoconfig.dvi
	-$(RM) fidoconfig.inf
	-$(RM) fidoconfig*html
	-$(RM) fidoconfig.ps
	-$(RM) fidoconfig.doc
	-$(RM) fidoconfig.info*
	-$(RM) proposal.dvi
	-$(RM) proposal.inf
	-$(RM) proposal*html
	-$(RM) proposal.ps
	-$(RM) proposal.doc
	-$(RM) proposal.info*


install: info
	-$(MKDIR) $(MKDIROPT) $(INFODIR)
	$(INSTALL)  fidoconfig.info $(INFODIR)
	$(INSTALL)  proposal.info $(INFODIR)
	install-info --info-dir=$(INFODIR)  $(INFODIR)$(DIRSEP)fidoconfig.info
	install-info --info-dir=$(INFODIR)  $(INFODIR)$(DIRSEP)proposal.info

