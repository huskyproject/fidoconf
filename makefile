CC = gcc
AR = ar
COPT = -g -s -c -O2 -Wall -I../smapi -DUNIX

ALL: fidoconfig.a \
     tparser \
    libfidoconfig.so.0.1

fidoconfig.a: fidoconfig.o line.o common.o 
	$(AR) r fidoconfig.a fidoconfig.o line.o common.o

libfidoconfig.so.0.1: fidoconfig.o line.o common.o
	$(CC) -shared -Wl,-soname,libfidoconfig.so.0 -o libfidoconfig.so.0.1 line.o common.o fidoconfig.o

fidoconfig.o: fidoconfig.c fidoconfig.h
	$(CC) $(COPT) fidoconfig.c

line.o: line.c fidoconfig.h
	$(CC) $(COPT) line.c

common.o: common.c common.h
	$(CC) $(COPT) common.c

tparser.o: tparser.c fidoconfig.h
	$(CC) $(COPT) tparser.c

tparser: tparser.o fidoconfig.a
	$(CC) tparser.o fidoconfig.a -o tparser
