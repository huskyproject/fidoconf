# Makefile for fidoconfig man pages
# Written by Stas Degteff 2:5080/102 <g@grumbler.org>
#
# Use with GNU version of make program (gmake)
#
# include Husky Makefile Config
ifeq ($(DEBIAN), 1)
include ../debian/huskymak.cfg
else
include ../../huskymak.cfg
endif

MAN1PAGES     = dumpfcfg.1 fconf2.1 linkedto.1 tparser.1
FCONF2ALIASES = fconf2aquaed.1 fconf2areasbbs.1 fconf2fidogate.1 \
                fconf2golded.1 fconf2msged.1 fconf2squish.1

MAN1DIR   = $(MANDIR)$(DIRSEP)man1

all:

install:	copy link

copy:
	$(MKDIR) $(MKDIROPT) $(MAN1DIR)
	$(INSTALL) $(IMOPT) $(MAN1PAGES) $(MAN1DIR)

link:
	for f in $(FCONF2ALIASES) ; do $(LN) -f $(MAN1DIR)$(DIRSEP)fconf2.1 $(MAN1DIR)$(DIRSEP)$$f ; done

clean:

distclean: clean

uninstall:
	-for f in $(FCONF2ALIASES) $(MAN1PAGES) ; do $(RM) $(RMOPT) $(MAN1DIR)$(DIRSEP)$$f ; done

