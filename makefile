CC = gcc
AR = ar
COPT = -g -s -c -O2 -Wall -I../smapi -DUNIX
VER = 0.3
LIBDIR = /usr/local/lib
INSTDIR = /usr/local/bin

ALL: fidoconfig.a \
     libfidoconfig.so.$(VER) \
     tparser \
     fconf2msged \
     fconf2golded \
     install

fidoconfig.a: fidoconfig.o line.o common.o 
	$(AR) r fidoconfig.a fidoconfig.o line.o common.o

libfidoconfig.so.$(VER): fidoconfig.o line.o common.o
	$(CC) -shared -Wl,-soname,libfidoconfig.so.0 -o libfidoconfig.so.$(VER) line.o common.o fidoconfig.o

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

clean:
	rm -f *.o

install: 
	cp -f libfidoconfig.so.$(VER) $(LIBDIR)
	ln -sf $(LIBDIR)/libfidoconfig.so.$(VER) $(LIBDIR)/libfidoconfig.so.0
	ln -sf $(LIBDIR)/libfidoconfig.so.0 $(LIBDIR)/libfidoconfig.so
	cp -f fconf2golded $(INSTDIR)
	cp -f fconf2msged $(INSTDIR)
