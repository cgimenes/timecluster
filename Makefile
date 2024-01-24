CC=clang
LIBS=-lraylib
CFLAGS=-Wall -Wextra -ggdb

all: build/timecluster

build/timecluster: src/timecluster.c build/libdraw.so
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -L./build/ -ldraw 

build/libdraw.so: src/draw.c src/draw.h
	$(CC) $(CFLAGS) -o $@ -fPIC -shared $< $(LIBS)

run: all
	LD_LIBRARY_PATH=./build/ build/timecluster

clean:
	@rm build/timecluster build/*.o build/*.so
