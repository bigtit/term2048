CC=g++
CFLAGS=-std=c++11 -Wall -ggdb

gridgame: main.o grid.o
	$(CC) main.o grid.o -o gridgame
grid.o: grid.cc
	$(CC) -c $(CFLAGS) grid.cc
main.o: main.cc
	$(CC) -c $(CFLAGS) main.cc
clean:
	rm *.o gridgame
