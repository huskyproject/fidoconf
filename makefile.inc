# common rules for all platforms

VER = 0.13
VERH = 0

LOBJS = common$(OBJ) dirlayer$(OBJ) fidoconf$(OBJ) line$(OBJ) adcase$(OBJ)  xstr$(OBJ) cfg$(OBJ) findtok$(OBJ) crc$(OBJ) log$(OBJ) recode$(OBJ) strsep$(OBJ) tree$(OBJ) areatree$(OBJ) afixcmd$(OBJ) afixcmn$(OBJ) arealist$(OBJ) temp$(OBJ)

OBJS = $(LOBJS) fc2msged$(OBJ) fc2ged$(OBJ) fc2aed$(OBJ) fc2fgate$(OBJ) fc2sq$(OBJ) fecfg146$(OBJ) fecfg2fc$(OBJ)

commonlibs: $(LIBFIDOCONFIG)$(LIB) $(SYMBOLIC)
	$(NULL)

commonprogs: tparser$(EXE) $(FCONF2MSGED)$(EXE) $(FCONF2AQUAED)$(EXE) $(FCONF2FIDOGATE)$(EXE) $(FCONF2GOLDED)$(EXE) $(FCONF2SQUISH)$(EXE) $(FECFG2FCONF)$(EXE) $(SYMBOLIC)
	$(NULL)

$(LIBFIDOCONFIG)$(LIB): $(LOBJS)
	$(AR) $(AR_R) $(LIBFIDOCONFIG)$(LIB) $(LOBJS)

line$(OBJ): line.c tokens.h
	$(CC) $(COPT) $(CDEFS) line.c

.c$(OBJ):
	$(CC) $(COPT) $(CDEFS) $<

$(FCONF2MSGED)$(EXE): fc2msged$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2MSGED)$(EXE) fc2msged$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2GOLDED)$(EXE): fc2ged$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2GOLDED)$(EXE) fc2ged$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2AQUAED)$(EXE): fc2aed$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2AQUAED)$(EXE) fc2aed$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2FIDOGATE)$(EXE): fc2fgate$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2FIDOGATE)$(EXE) fc2fgate$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FCONF2SQUISH)$(EXE): fc2sq$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2SQUISH)$(EXE) fc2sq$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

$(FECFG2FCONF)$(EXE): fecfg2fc$(OBJ) fecfg146$(OBJ)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FECFG2FCONF)$(EXE) fecfg2fc$(OBJ) fecfg146$(OBJ)

tparser$(EXE): tparser$(OBJ) $(LIBFIDOCONFIG)$(LIB)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)tparser$(EXE) tparser$(OBJ) $(LOPT) $(LINKFIDOCONFIG) $(LINKSMAPI)

commonclean: $(SYMBOLIC)
	-$(RM) $(RMOPT) *~
	-$(RM) $(RMOPT) *$(OBJ)
	-$(RM) $(RMOPT) *$(TPU)
	-$(RM) $(RMOPT) core

commondistclean: clean $(SYMBOLIC)
	-$(RM) $(RMOPT) tparser$(EXE)
	-$(RM) $(RMOPT) $(FCONF2GOLDED)$(EXE)
	-$(RM) $(RMOPT) $(FCONF2MSGED)$(EXE)
	-$(RM) $(RMOPT) $(FCONF2AQUAED)$(EXE)
	-$(RM) $(RMOPT) $(FCONF2FIDOGATE)$(EXE)
	-$(RM) $(RMOPT) $(FCONF2SQUISH)$(EXE)
	-$(RM) $(RMOPT) $(FECFG2FCONF)$(EXE)
	-$(RM) $(RMOPT) $(LIBFIDOCONFIG)$(LIB)

