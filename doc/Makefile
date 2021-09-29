# A makefile for fidoconfig documentation
# fidoconf/doc/Makefile

.PHONY: fidoconf_info fidoconf_html fidoconf_ruhtml fidoconf_txt \
              fidoconf_rutxt fidoconf_dvi fidoconf_rudvi fidoconf_pdf \
              fidoconf_rmdir_DOC fidoconf_clean_DOC fidoconf_install_info \
              fidoconf_install_html fidoconf_install_ruhtml \
              fidoconf_install_txt fidoconf_install_rutxt \
              fidoconf_install_dvi fidoconf_install_rudvi fidoconf_install_pdf

ifdef DOCDIR
    fidoconf_doc: fidoconf_info fidoconf_html fidoconf_ruhtml fidoconf_txt \
                  fidoconf_rutxt fidoconf_dvi fidoconf_rudvi fidoconf_pdf ;
else
    fidoconf_doc: fidoconf_info ;
endif

ifdef INFODIR
    fidoconf_info: $(fidoconf_BUILDDIR)fidoconfig.info.gz
else
    fidoconf_info: ;
endif

ifeq ($(HTML), 1)
    fidoconf_html: $(fidoconf_BUILDDIR)fidoconfig.html
else
    fidoconf_html: ;
endif

ifeq ($(RUHTML), 1)
    fidoconf_ruhtml: $(fidoconf_BUILDDIR)fidoconfig.ru.html
else
    fidoconf_ruhtml: ;
endif

ifeq ($(TXT), 1)
    fidoconf_txt: $(fidoconf_BUILDDIR)fidoconfig.txt
else
    fidoconf_txt: ;
endif

ifeq ($(RUTXT), 1)
    fidoconf_rutxt: $(fidoconf_BUILDDIR)fidoconfig.ru.txt
else
    fidoconf_rutxt: ;
endif

ifeq ($(DVI), 1)
    fidoconf_dvi: $(fidoconf_BUILDDIR)fidoconfig.dvi
else
    fidoconf_dvi: ;
endif

ifeq ($(RUDVI), 1)
    fidoconf_rudvi: $(fidoconf_BUILDDIR)fidoconfig.ru.dvi
else
    fidoconf_rudvi: ;
endif

ifeq ($(PDF), 1)
    fidoconf_pdf: $(fidoconf_BUILDDIR)fidoconfig.pdf
else
    fidoconf_pdf: ;
endif

fidoconf_DOCDIR_SRC=$(fidoconf_ROOTDIR)doc$(DIRSEP)
fidoconf_TEXSRC=$(wildcard $(fidoconf_DOCDIR_SRC)*.texi ) \
                $(wildcard $(fidoconf_DOCDIR_SRC)keywords.*) \
                $(fidoconf_DOCDIR_SRC)config
fidoconf_TEXI=$(fidoconf_DOCDIR_SRC)fidoconfig.texi
fidoconf_PROP=$(fidoconf_DOCDIR_SRC)proposal.texi
fidoconf_RUS_TEXI=$(fidoconf_DOCDIR_SRC)rus$(DIRSEP)fidoconfig.texi
fidoconf_RUS_PROP=$(fidoconf_DOCDIR_SRC)rus$(DIRSEP)proposal.texi
fidoconf_DOCDIR_BLD=$(fidoconf_BUILDDIR)doc$(DIRSEP)
fidoconf_TEXBLD=$(addprefix $(fidoconf_DOCDIR_BLD),$(notdir $(fidoconf_TEXSRC)))

ifdef INFODIR
    ifneq ($(RUINFO), 1)
        $(fidoconf_BUILDDIR)fidoconfig.info.gz: $(fidoconf_TEXI) $(fidoconf_PROP)
		-$(MAKEINFO) $(MINOSPLIT) "$<" $(MIOUT) $(fidoconf_BUILDDIR)fidoconfig.info
    else 
        $(fidoconf_BUILDDIR)fidoconfig.info.gz: $(fidoconf_RUS_TEXI) $(fidoconf_RUS_PROP)
		-LC_ALL=C $(MAKEINFO) $(MINOSPLIT) "$<" $(MIOUT) $(fidoconf_BUILDDIR)fidoconfig.info
    endif
	gzip -c $(fidoconf_BUILDDIR)fidoconfig.info > $@
	$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.info
endif

$(fidoconf_BUILDDIR)fidoconfig.html: $(fidoconf_TEXI) $(fidoconf_PROP)
	-LC_ALL=C $(MAKEINFO) $(MIHTML) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(fidoconf_BUILDDIR)fidoconfig.ru.html: $(fidoconf_RUS_TEXI) $(fidoconf_RUS_PROP)
	-LC_ALL=C $(MAKEINFO) $(MIHTML) $(MINOSPLIT) $(MIOUT) "$@" "$<"

$(fidoconf_BUILDDIR)fidoconfig.txt: $(fidoconf_TEXI) $(fidoconf_PROP)
	-LC_ALL=C $(MAKEINFO) $(MIPTEXT) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(fidoconf_BUILDDIR)fidoconfig.ru.txt: $(fidoconf_RUS_TEXI) $(fidoconf_RUS_PROP)
	-LC_ALL=C $(MAKEINFO) $(MIPTEXT) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(fidoconf_BUILDDIR)fidoconfig.dvi: $(fidoconf_TEXI) $(fidoconf_PROP)
	-$(TEXI2DVI) "$<"

$(fidoconf_BUILDDIR)fidoconfig.ru.dvi: $(fidoconf_RUS_TEXI) $(fidoconf_RUS_PROP)
	-LC_ALL=C $(TEXI2DVI) "$<" $(MIOUT) "$@"

$(fidoconf_BUILDDIR)fidoconfig.pdf: $(fidoconf_DOCDIR_BLD)fidoconfig.pdf
	$(LN) $(LNHOPT) "$<" $(fidoconf_BUILDDIR)

$(fidoconf_DOCDIR_BLD)fidoconfig.pdf: $(fidoconf_DOCDIR_BLD)fidoconfig.texi \
                                      $(fidoconf_DOCDIR_BLD)proposal.texi
	-cd $(fidoconf_DOCDIR_BLD); \
	LC_ALL=C $(MAKEINFO) $(MIPDF) $(MINOSPLIT) fidoconfig.texi $(MIOUT) fidoconfig.pdf

$(fidoconf_TEXBLD): $(fidoconf_TEXSRC) | $(fidoconf_DOCDIR_BLD)
	$(LN) $(LNHOPT) $? $(fidoconf_DOCDIR_BLD)

$(fidoconf_DOCDIR_BLD):
	[ -d $(fidoconf_BUILDDIR) ] || $(MKDIR) $(MKDIROPT) $(fidoconf_BUILDDIR)
	[ -d "$@" ] || $(MKDIR) $(MKDIROPT) "$@"


# Install
ifdef DOCDIR
# See "all_info_install" in the root Makefile
    fidoconf_doc_install: all_info_install fidoconf_install_html \
                          fidoconf_install_ruhtml fidoconf_install_txt \
                          fidoconf_install_rutxt fidoconf_install_dvi \
                          fidoconf_install_rudvi fidoconf_install_pdf ;
else
    fidoconf_doc_install: all_info_install ;
endif

ifeq ($(HTML), 1)
    fidoconf_install_html: $(DOCDIR_DST)fidoconfig.html ;

    $(DOCDIR_DST)fidoconfig.html: \
        $(fidoconf_BUILDDIR)fidoconfig.html | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" $(DOCDIR_DST); \
		$(TOUCH) "$@"
else
    fidoconf_install_html: ;
endif

ifeq ($(RUHTML), 1)
    fidoconf_install_ruhtml: $(DOCDIR_DST)fidoconfig.ru.html ;

    $(DOCDIR_DST)fidoconfig.ru.html: \
        $(fidoconf_BUILDDIR)fidoconfig.ru.html | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" $(DOCDIR_DST); \
		$(TOUCH) "$@"
else
    fidoconf_install_ruhtml: ;
endif

ifeq ($(TXT), 1)
    fidoconf_install_txt: $(DOCDIR_DST)fidoconfig.txt ;

    $(DOCDIR_DST)fidoconfig.txt: \
        $(fidoconf_BUILDDIR)fidoconfig.txt | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" $(DOCDIR_DST); \
		$(TOUCH) "$@"
else
    fidoconf_install_txt: ;
endif

ifeq ($(RUTXT), 1)
    fidoconf_install_rutxt: $(DOCDIR_DST)fidoconfig.ru.txt ;

    $(DOCDIR_DST)fidoconfig.ru.txt: \
        $(fidoconf_BUILDDIR)fidoconfig.ru.txt | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" $(DOCDIR_DST); \
		$(TOUCH) "$@"
else
    fidoconf_install_rutxt: ;
endif

ifeq ($(DVI), 1)
    fidoconf_install_dvi: $(DOCDIR_DST)fidoconfig.dvi ;

    $(DOCDIR_DST)fidoconfig.dvi: \
        $(fidoconf_BUILDDIR)fidoconfig.dvi | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" $(DOCDIR_DST); \
		$(TOUCH) "$@"
else
    fidoconf_install_dvi: ;
endif

ifeq ($(RUDVI), 1)
    fidoconf_install_rudvi: $(DOCDIR_DST)fidoconfig.ru.dvi ;

    $(DOCDIR_DST)fidoconfig.ru.dvi: \
        $(fidoconf_BUILDDIR)fidoconfig.ru.dvi | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$@" "$|"; \
		$(TOUCH) "$@"
else
    fidoconf_install_rudvi: ;
endif

ifeq ($(PDF), 1)
    fidoconf_install_pdf: $(DOCDIR_DST)fidoconfig.pdf ;

    $(DOCDIR_DST)fidoconfig.pdf: \
        $(fidoconf_BUILDDIR)fidoconfig.pdf | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" "$|"; \
		$(TOUCH) "$@"
else
    fidoconf_install_pdf: ;
endif

# Clean
fidoconf_doc_clean: fidoconf_rmdir_DOC ;

fidoconf_rmdir_DOC: fidoconf_clean_DOC
	-[ -d "$(fidoconf_DOCDIR_BLD)" ] && $(RMDIR) $(fidoconf_DOCDIR_BLD) || true

fidoconf_clean_DOC:
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.fns
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.aux
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.toc
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.cp
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.fn
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.vr
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.kr
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.ke
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.tp
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.kes
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.krs
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.ky
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.pg
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.cps
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)*.log
	-$(RM) $(RMOPT) $(fidoconf_DOCDIR_BLD)fidoconfig.pdf
	-$(RM) $(RMOPT) $(fidoconf_TEXBLD)

# Distclean
fidoconf_doc_distclean: fidoconf_doc_clean
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig*dvi
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.inf
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig*html
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.ps
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.doc
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig*txt
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.pdf
	-$(RM) $(RMOPT) $(fidoconf_BUILDDIR)fidoconfig.info*


# Uninstall
ifdef DOCDIR
    fidoconf_doc_uninstall: fidoconf_info_uninstall fidoconf_docs_uninstall ;
else
    fidoconf_doc_uninstall: fidoconf_info_uninstall ;
endif

ifdef INFODIR
# See "all_info_uninstall" in the root Makefile
    fidoconf_info_uninstall: all_info_uninstall
		-$(RM) $(RMOPT) $(INFODIR_DST)fidoconfig.info.gz
else
    fidoconf_info_uninstall: ;
endif

fidoconf_docs_uninstall:
ifeq ($(HTML), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.html
endif
ifeq ($(RUHTML), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.ru.html
endif
ifeq ($(TXT), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.txt
endif
ifeq ($(RUTXT), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.ru.txt
endif
ifeq ($(DVI), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.dvi
endif
ifeq ($(RUDVI), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.ru.dvi
endif
ifeq ($(PDF), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)fidoconfig.pdf
endif
