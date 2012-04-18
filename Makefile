CC=g++
CFLAGS=-Wall
test: test.o clock.o

clean:
	rm -f test test.o clock.o
