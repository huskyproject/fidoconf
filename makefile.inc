# $Id$
#
# common rules for all platforms
# include into other makefiles

VER = 1.9.0
VERH = 1.9

_SRC_DIR = src/

include makefile.in1

commonlibs: $(LIBFIDOCONFIG)$(_LIB) $(SYMBOLIC)
	$(NULL)

commonprogs: tparser$(_EXE) linked$(_EXE) $(FCONF2BINKD)$(_EXE) $(FCONF2MSGED)$(_EXE) $(FCONF2AQUAED)$(_EXE) $(FCONF2FIDOGATE)$(_EXE) $(FCONF2GOLDED)$(_EXE) $(FCONF2SQUISH)$(_EXE) $(FCONF2TORNADO)$(_EXE) $(FECFG2FCONF)$(_EXE) $(SYMBOLIC)
	$(NULL)

$(LIBFIDOCONFIG)$(_LIB): $(LOBJS)
	$(AR) $(AR_R) $(LIBFIDOCONFIG)$(_LIB) $(LOBJS)

#line$(_OBJ): line.c tokens.h
#	$(CC) $(CFLAGS) $(COPT) $(CDEFS) $(OBJOPT) $<

#{$(_SRC_DIR)}.c$(_OBJ):
%$(_OBJ): ${_SRC_DIR}%.c
	$(CC) $(CFLAGS) $(COPT) $(CDEFS) $<

$(FCONF2MSGED)$(_EXE): fc2msged$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2MSGED)$(_EXE) fc2msged$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2GOLDED)$(_EXE): fc2ged$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2GOLDED)$(_EXE) fc2ged$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2AQUAED)$(_EXE): fc2aed$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2AQUAED)$(_EXE) fc2aed$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2FIDOGATE)$(_EXE): fc2fgate$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2FIDOGATE)$(_EXE) fc2fgate$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2SQUISH)$(_EXE): fc2sq$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2SQUISH)$(_EXE) fc2sq$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2TORNADO)$(_EXE): fc2tor$(_OBJ) fc2tor_g$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2TORNADO)$(_EXE) fc2tor$(_OBJ) fc2tor_g$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FCONF2BINKD)$(_EXE):   fc2binkd$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FCONF2BINKD)$(_EXE) fc2binkd$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

$(FECFG2FCONF)$(_EXE): fecfg2fc$(_OBJ) fecfg146$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)$(FECFG2FCONF)$(_EXE) fecfg2fc$(_OBJ) fecfg146$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

tparser$(_EXE): tparser$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)tparser$(_EXE) tparser$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

linked$(_EXE): linked$(_OBJ) $(TARGET)
	$(CC) $(LFLAGS) $(EXENAMEFLAG)linked$(_EXE) linked$(_OBJ) $(LOPT) $(TARGET) $(LIBS)

commonclean: $(SYMBOLIC)
	-$(RM) $(RMOPT) *$(_OBJ)

commondistclean: clean $(SYMBOLIC)
	-$(RM) $(RMOPT) tparser$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2GOLDED)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2MSGED)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2AQUAED)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2FIDOGATE)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2SQUISH)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2TORNADO)$(_EXE)
	-$(RM) $(RMOPT) $(FCONF2BINKD)$(_EXE)
	-$(RM) $(RMOPT) $(FECFG2FCONF)$(_EXE)
	-$(RM) $(RMOPT) linked$(_EXE)
	-$(RM) $(RMOPT) $(LIBFIDOCONFIG)$(_LIB)

