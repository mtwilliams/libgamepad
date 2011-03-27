all: test

clean:
	rm -f test libgamepad.so libgamepad.so.1 gamepad.o

gamepad.o: gamepad.c gamepad.h
	$(CC) -c -fPIC -fvisibility=hidden -Wall -Werror -o $@ $< $(CCFLAGS)

libgamepad.so.1: gamepad.o gamepad.h
	$(CC) -shared -Wl,-soname,libgamepad.so.1 -o $@ $< $(CCFLAGS) -lc -lm -ludev

libgamepad.so: libgamepad.so.1
	ln -sf libgamepad.so.1 libgamepad.so

test: main.c libgamepad.so
	$(CC) -o $@ $< -Wl,-rpath,. -L. -lgamepad -lcurses -ludev

install: libgamepad.so

.PHONY: all clean install
