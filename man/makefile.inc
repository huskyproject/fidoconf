MAN1PAGES     = dumpfcfg.1.gz fconf2.1.gz linked.1.gz linkedto.1.gz tparser.1.gz
FCONF2ALIASES = fconf2aquaed.1.gz fconf2areasbbs.pl.1.gz fconf2binkd.1.gz \
                fconf2dir.1.gz fconf2fidogate.1.gz fconf2golded.1.gz \
                fconf2msged.1.gz fconf2na.pl.1.gz fconf2squish.1.gz \
                fconf2tornado.1.gz fecfg2fconf.1.gz

MAN1DIR   = $(DESTDIR)$(MANDIR)$(DIRSEP)man1

allman: man$(DIRSEP)dumpfcfg.1.gz man$(DIRSEP)fconf2.1.gz man$(DIRSEP)linked.1.gz \
        man$(DIRSEP)linkedto.1.gz man$(DIRSEP)tparser.1.gz

man$(DIRSEP)dumpfcfg.1.gz: man$(DIRSEP)dumpfcfg.1
	gzip -c man$(DIRSEP)dumpfcfg.1 > man$(DIRSEP)dumpfcfg.1.gz

man$(DIRSEP)fconf2.1.gz: man$(DIRSEP)fconf2.1
	gzip -c man$(DIRSEP)fconf2.1 > man$(DIRSEP)fconf2.1.gz

man$(DIRSEP)linked.1.gz: man$(DIRSEP)linked.1
	gzip -c man$(DIRSEP)linked.1 > man$(DIRSEP)linked.1.gz

man$(DIRSEP)linkedto.1.gz: man$(DIRSEP)linkedto.1
	gzip -c man$(DIRSEP)linkedto.1 > man$(DIRSEP)linkedto.1.gz

man$(DIRSEP)tparser.1.gz: man$(DIRSEP)tparser.1
	gzip -c man$(DIRSEP)tparser.1 > man$(DIRSEP)tparser.1.gz

install-man: copy-man link-man

copy-man:
	$(MKDIR) $(MKDIROPT) $(MAN1DIR)
	cd man; $(INSTALL) $(IMOPT) $(MAN1PAGES) $(MAN1DIR)

link-man:
	cd $(MAN1DIR)$(DIRSEP); \
	for f in $(FCONF2ALIASES) ; do $(LN) -s -f fconf2.1.gz $$f ; done

clean-man:

distclean-man: clean-man
	$(RM) $(RMOPT) man$(DIRSEP)*.gz

uninstall-man:
	-for f in $(FCONF2ALIASES) $(MAN1PAGES) ; do $(RM) $(RMOPT) $(MAN1DIR)$(DIRSEP)$$f ; done

