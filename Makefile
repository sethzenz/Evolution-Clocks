CC=g++
CFLAGS=-Wall
test: test.o clock.o breeder.o observer.o

clean:
	rm -f test test.o clock.o breeder.o observer.o
