CC = gcc
AR = ar
COPT = -g -s -c -O2 -Wall -I../smapi -DUNIX

ALL: fidoconfig.a \
     libfidoconfig.so.0.3 \
     tparser \
     fconf2msged \
     fconf2golded 

fidoconfig.a: fidoconfig.o line.o common.o 
	$(AR) r fidoconfig.a fidoconfig.o line.o common.o

libfidoconfig.so.0.3: fidoconfig.o line.o common.o
	$(CC) -shared -Wl,-soname,libfidoconfig.so.0 -o libfidoconfig.so.0.3 line.o common.o fidoconfig.o

fidoconfig.o: fidoconfig.c fidoconfig.h
	$(CC) $(COPT) fidoconfig.c

line.o: line.c fidoconfig.h
	$(CC) $(COPT) line.c

common.o: common.c common.h
	$(CC) $(COPT) common.c

tparser.o: tparser.c fidoconfig.h
	$(CC) $(COPT) tparser.c

fconf2msged.o: fconf2msged.c fidoconfig.h
	$(CC) $(COPT) fconf2msged.c

fconf2golded.o: fconf2golded.c fidoconfig.h
	$(CC) $(COPT) fconf2golded.c

fconf2msged: fconf2msged.o fidoconfig.a
	$(CC) fconf2msged.o -o fconf2msged -lfidoconfig

fconf2golded: fconf2golded.o fidoconfig.a
	$(CC) fconf2golded.o -o fconf2golded -lfidoconfig

tparser: tparser.o fidoconfig.a
	$(CC) tparser.o -o tparser -lfidoconfig
