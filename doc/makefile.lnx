INFODIR = /usr/local/info
IOPT	= -m 644

info fidoconfig.info: fidoconfig.texi proposal.texi config
	makeinfo --no-split fidoconfig.texi

dvi fidoconfig.dvi:  fidoconfig.texi proposal.texi config
	texi2dvi fidoconfig.texi

ps fidoconfig.ps: fidoconfig.dvi
	dvips fidoconfig.dvi -o fidoconfig.ps

html fidoconfig_toc.html: fidoconfig.texi proposal.texi config
	texi2html -number -menu fidoconfig.texi

clean:
	-rm *~
	-rm fidoconfig.fns
	-rm fidoconfig.aux
	-rm fidoconfig.toc
	-rm fidoconfig.cp
	-rm fidoconfig.fn
	-rm fidoconfig.vr
	-rm fidoconfig.kr
	-rm fidoconfig.ke
	-rm fidoconfig.tp
	-rm fidoconfig.kes
	-rm fidoconfig.krs
	-rm fidoconfig.ky
	-rm fidoconfig.pg
	-rm fidoconfig.cps
	-rm fidoconfig.log
	-rm fidoconfig.fns

distclean: clean
	-rm fidoconfig.dvi
	-rm fidoconfig.info
	-rm fidoconfig*html
	-rm fidoconfig.ps

install:
	install $(IOPT) fidoconfig.info ${INFODIR}
