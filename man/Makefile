# Makefile for fidoconfig man pages
# Written by Stas Degteff 2:5080/102 <g@grumbler.org>
#
# Use with GNU version of make program (gmake)
#
# include Husky Makefile Config
ifeq ($(DEBIAN), 1)
include /usr/share/husky/huskymak.cfg
else
include ../../huskymak.cfg
endif

MAN1PAGES     = dumpfcfg.1 fconf2.1 linked.1 linkedto.1 tparser.1
FCONF2ALIASES = fconf2aquaed.1 fconf2areasbbs.pl.1 fconf2binkd.1 \
		fconf2dir.1 fconf2fidogate.1 fconf2golded.1 \
		fconf2msged.1 fconf2na.pl.1 fconf2squish.1 \
		fconf2tornado.1 fecfg2fconf.1

MAN1DIR   = $(MANDIR)$(DIRSEP)man1

all:

install:	copy link

copy:
	$(MKDIR) $(MKDIROPT) $(MAN1DIR)
	$(INSTALL) $(IMOPT) $(MAN1PAGES) $(MAN1DIR)

link:
	cd $(MAN1DIR)$(DIRSEP); \
	for f in $(FCONF2ALIASES) ; do $(LN) -s fconf2.1 $$f ; done

clean:

distclean: clean

uninstall:
	-for f in $(FCONF2ALIASES) $(MAN1PAGES) ; do $(RM) $(RMOPT) $(MAN1DIR)$(DIRSEP)$$f ; done

