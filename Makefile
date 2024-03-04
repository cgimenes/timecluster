CC=clang
LIBS=-lraylib -lpython3.11
CFLAGS=-Wall -Wextra -ggdb

build: build/timecluster

build/timecluster: src/timecluster.c build/librender.so
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -ldl

build/librender.so: src/render.c src/render.h
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
