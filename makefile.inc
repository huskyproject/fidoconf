# common rules for all platforms

VER = 0.10
VERH = 0

LOBJS = common$(OBJ) dirlayer$(OBJ) fidoconf$(OBJ) line$(OBJ) adcase$(OBJ) dump$(OBJ) xstr$(OBJ)

OBJS = $(LOBJS) fc2msged$(OBJ) fc2ged$(OBJ) fc2aed$(OBJ) fc2fgate$(OBJ) fc2sq$(OBJ) dumpfcfg$(OBJ) fecfg2fc$(OBJ)

commonlibs: $(LIBFIDOCONFIG)$(LIB)

commonprogs:    tparser$(EXE) \
	        $(FCONF2MSGED)$(EXE) \
		$(FCONF2AQUAED)$(EXE) \
		$(FCONF2FIDOGATE)$(EXE) \
	        $(FCONF2GOLDED)$(EXE) \
		$(FCONF2SQUISH)$(EXE) \
		dumpfcfg$(EXE) \
                $(FECFG2FCONF)$(EXE)

$(LIBFIDOCONFIG)$(LIB): $(LOBJS)
	$(AR) $(AR_R) $(LIBFIDOCONFIG)$(LIB) $(LOBJS)

.c$(OBJ):
	$(CC) $(COPT) $(CDEFS) $<

$(FCONF2MSGED)$(EXE): fc2msged$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2MSGED)$(EXE) fc2msged$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2GOLDED)$(EXE): fc2ged$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2GOLDED)$(EXE) fc2ged$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2AQUAED)$(EXE): fc2aed$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2AQUAED)$(EXE) fc2aed$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2FIDOGATE)$(EXE): fc2fgate$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2FIDOGATE)$(EXE) fc2fgate$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2SQUISH)$(EXE): fc2sq$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2SQUISH)$(EXE) fc2sq$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FECFG2FCONF)$(EXE): fecfg2fc$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FECFG2FCONF)$(EXE) fecfg2fc$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

tparser$(EXE): tparser$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)tparser$(EXE) tparser$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

dumpfcfg$(EXE): dumpfcfg$(OBJ) $(LIBFIDOCONFIG)$(LIB) dump$(OBJ)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)dumpfcfg$(EXE) dumpfcfg$(OBJ) dump$(OBJ) \
	  $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

commonclean:
	-$(RM) *~
	-$(RM) *$(OBJ)
	-$(RM) *$(TPU)
	-$(RM) core

commondistclean: clean
	-$(RM) tparser$(EXE)
	-$(RM) $(FCONF2GOLDED)$(EXE)
	-$(RM) $(FCONF2MSGED)$(EXE)
	-$(RM) $(FCONF2AQUAED)$(EXE)
	-$(RM) $(FCONF2FIDOGATE)$(EXE)
	-$(RM) $(FCONF2SQUISH)$(EXE)
	-$(RM) $(FECFG2FCONF)$(EXE)
	-$(RM) dumpfcfg$(EXE)
	-$(RM) $(LIBFIDOCONFIG)$(LIB)
