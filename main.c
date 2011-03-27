#include <stdio.h>
#include <ncurses.h>

#include "gamepad.h"

static const char* button_names[] = {
	"d-pad up",
	"d-pad down",
	"d-pad left",
	"d-pad right",
	"start",
	"back",
	"left thumb",
	"right thumb",
	"left shoulder",
	"right shoulder",
	"???",
	"???",
	"A",
	"B",
	"X",
	"Y"
};

static int line = 0;

static void logevent(const char* format, ...) {
	va_list va;

	move(9 + line, 0);
	clrtoeol();

	move(8 + line, 0);
	clrtoeol();

	va_start(va, format);
	vwprintw(stdscr, format, va);
	va_end(va);

	if (++line == 14) {
		line = 0;
	}
}

static void update(GAMEPAD_DEVICE dev) {
	float lx, ly, rx, ry;

	move(dev, 0);

	if (!GamepadIsConnected(dev)) {
		printw("%d) n/a\n", dev);
		return;
	}

	GamepadStickNormXY(dev, STICK_LEFT, &lx, &ly);
	GamepadStickNormXY(dev, STICK_RIGHT, &rx, &ry);

	printw("%d) L:(%+.3f,%+.3f :: %+.3f,%+.3f) R:(%+.3f, %+.3f :: %+.3f,%+.3f) LT:%+.3f RT:%+.3f ",
		dev,
		lx, ly,
		GamepadStickAngle(dev, STICK_LEFT),
		GamepadStickLength(dev, STICK_LEFT),
		rx, ry,
		GamepadStickAngle(dev, STICK_RIGHT),
		GamepadStickLength(dev, STICK_RIGHT),
		GamepadTriggerLength(dev, TRIGGER_LEFT),
		GamepadTriggerLength(dev, TRIGGER_RIGHT));
	printw("U:%d D:%d L:%d R:%d ",
		GamepadButtonDown(dev, BUTTON_DPAD_UP),
		GamepadButtonDown(dev, BUTTON_DPAD_DOWN),
		GamepadButtonDown(dev, BUTTON_DPAD_LEFT),
		GamepadButtonDown(dev, BUTTON_DPAD_RIGHT));
	printw("A:%d B:%d X:%d Y:%d Bk:%d St:%d ",
		GamepadButtonDown(dev, BUTTON_A),
		GamepadButtonDown(dev, BUTTON_B),
		GamepadButtonDown(dev, BUTTON_X),
		GamepadButtonDown(dev, BUTTON_Y),
		GamepadButtonDown(dev, BUTTON_BACK),
		GamepadButtonDown(dev, BUTTON_START));
	printw("LB:%d RB:%d LS:%d RS:%d\n",
		GamepadButtonDown(dev, BUTTON_LEFT_SHOULDER),
		GamepadButtonDown(dev, BUTTON_RIGHT_SHOULDER),
		GamepadButtonDown(dev, BUTTON_LEFT_THUMB),
		GamepadButtonDown(dev, BUTTON_RIGHT_THUMB));
}

int main() {
	int ch, i, j, k;

	initscr();
	cbreak();
	noecho();
	timeout(1);

	GamepadInit();

	while ((ch = getch()) != 'q') {
		GamepadUpdate();

		if (ch == 'r') {
			for (i = 0; i != GAMEPAD_COUNT; ++i) {
				GamepadSetRumble((GAMEPAD_DEVICE)i, 0.25f, 0.25f);
			}
		}

		update(GAMEPAD_0);
		update(GAMEPAD_1);
		update(GAMEPAD_2);
		update(GAMEPAD_3);

		for (i = 0; i != GAMEPAD_COUNT; ++i) {
			if (GamepadIsConnected(i)) {
				for (j = 0; j != BUTTON_COUNT; ++j) {
					if (GamepadButtonTriggered(i, j)) {
						logevent("[%d] button triggered: %s", i, button_names[j]);
					} else if (GamepadButtonReleased(i, j)) {
						logevent("[%d] button released:  %s", i, button_names[j]);
					}
				}
				for (j = 0; j != TRIGGER_COUNT; ++j) {
					if (GamepadTriggerTriggered(i, j)) {
						logevent("[%d] trigger pressed:  %d", i, j);
					} else if (GamepadTriggerReleased(i, j)) {
						logevent("[%d] trigger released: %d", i, j);
					}
				}
				for (j = 0; j != STICK_COUNT; ++j) {
					for (k = 0; k != STICKDIR_COUNT; ++k) {
						if (GamepadStickDirTriggered(i, j, k)) {
							logevent("[%d] stick direction:  %d -> %d", i, j, k);
						}
					}
				}
			}
		}

		move(6, 0);
		printw("(q)uit (r)umble");

		refresh();
	}

	endwin();

	return 0;
}
