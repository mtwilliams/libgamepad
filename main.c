#include <stdio.h>
#include <ncurses.h>

#include "gamepad.h"

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
	initscr();
	cbreak();
	noecho();
	timeout(1);

	GamepadInit();

	while (getch() == ERR) {
		GamepadUpdate();

		update(GAMEPAD_0);
		update(GAMEPAD_1);
		update(GAMEPAD_2);
		update(GAMEPAD_3);

		refresh();
	}

	endwin();

	return 0;
}
