test: main.c gamepad.c gamepad.h
	$(CC) -o $@ main.c gamepad.c -lm -Wall -lcurses -ludev
