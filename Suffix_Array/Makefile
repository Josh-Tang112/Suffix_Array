CC=gcc
CFLAGS=-Wall -Wextra -Iincludes 
LDLIBS=-lm -L/usr/local/lib -l:libdivsufsort.so
VPATH=src

all: buildsa querysa

querysa: querysa.o optparser.o search.o

buildsa: buildsa.o optparser.o

clean:
	rm -f *~ *.o querysa buildsa
