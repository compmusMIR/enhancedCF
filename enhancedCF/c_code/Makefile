CC=gcc
CFLAGS= -Wall -pedantic -g -O0 


all: libecfdb als

libecfdb: ecfdb.h ecfdb.c
	$(CC) $(CFLAGS) -c -fPIC ecfdb.c
	$(CC) -shared -o libecfdb.so ecfdb.o 
	

als: libecfdb als.c
	gcc -Wall -o als als.c libecfdb.so

clean: 
	rm -f als
	rm -f *.so *.o


