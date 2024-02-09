CC=clang
LIBS=-lraylib
CFLAGS=-Wall -Wextra

build: build/timecluster

build/timecluster: src/timecluster.c build/libdraw.so
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -ldl

build/libdraw.so: src/draw.c src/draw.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -fPIC -shared

run: build
	LD_LIBRARY_PATH=./build/ build/timecluster

watch: build
	make run &
	while true; do \
		inotifywait -qr -e modify -e create -e delete -e move src; \
		make build; \
	done

clean:
	@rm build/timecluster build/*.o build/*.so
