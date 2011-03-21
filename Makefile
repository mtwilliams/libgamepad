all: test

gamepad.o: gamepad.c gamepad.h
	$(CC) -o $@ -c $< $(CCFLAGS)

libgamepad.so: gamepad.c gamepad.h
	$(CC) -shared -o $@ -Wl,-soname,libgamepad.so.1 -fPIC -c $< $(CCFLAGS) -ludev

test: main.c libgamepad.so
	$(CC) -o $@ $< -Wl,-rpath,. -L. -lgamepad -lm -lcurses -ludev
