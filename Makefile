all: sparse

CC=gcc
CFLAGS=-c -W -Wall -O3

sparse: main.o
	${CC} -o sparse main.o -lm

main.o: main.c
	${CC} ${CFLAGS} main.c

clean:
	rm -f *.o sparse *.dat *.gz