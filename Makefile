CC=gcc
LIBRARIES=-lraylib
CFLAGS=-Wall -g

all: main

main: src/main.c build/draw.o
	$(CC) $(CFLAGS) $(LIBRARIES) -o build/$@ $^

build/draw.o: src/draw.c src/draw.h
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o build/draw.o

clean:
	rm build/main
	rm build/*.o
