/**
 * Gamepad Input Library
 * Sean Middleditch
 * Copyright (C) 2010  Sean Middleditch
 * LICENSE: MIT/X
 */

#include <math.h>
#include <string.h>

#if defined(WIN32)
#	define WIN32_LEAN_AND_MEAN 1
#	include "windows.h"
#	include "xinput.h"
#
#	pragma comment(lib, "xinput.lib")
#else
#	include <linux/joystick.h>
#	include <stdio.h>
#	include <fcntl.h>
#	include <unistd.h>
#endif

#include "gamepad.h"

/* Axis information */
typedef struct GAMEPAD_AXIS GAMEPAD_AXIS;
struct GAMEPAD_AXIS {
	int x, y;
	float nx, ny;
	float length;
	float angle;
};

/* Trigger value information */
typedef struct GAMEPAD_TRIGINFO GAMEPAD_TRIGINFO;
struct GAMEPAD_TRIGINFO {
	int value;
	float length;
};

/* Note whether a gamepad is currently connected */
#define FLAG_CONNECTED (1<<0)

/* Various values of PI */
#define PI_1_4	0.78539816339744f
#define PI_1_2	1.57079632679489f
#define PI_3_4	2.35619449019234f
#define PI		3.14159265358979f

/* Structure for state of a particular gamepad */
struct GamepadState {
	GAMEPAD_AXIS stick[STICK_COUNT];
	GAMEPAD_TRIGINFO trigger[TRIGGER_COUNT];
	int bLast, bCurrent, flags;
#if !defined(WIN32)
	int fd;
#endif
};

/* State of the four gamepads */
static struct GamepadState STATE[4];

#if defined(WIN32)
void GamepadInit() {
	memset(STATE, 0, sizeof(STATE));
}
#else
void GamepadInit() {
	int i;

	memset(STATE, 0, sizeof(STATE));

	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		char dev[128];
		snprintf(dev, sizeof(dev), "/dev/input/js%d", i);
		STATE[i].fd = open(dev, O_RDONLY|O_NONBLOCK);
		if (STATE[i].fd != -1) {
			STATE[i].flags |= FLAG_CONNECTED;
		}
	}
}
#endif

void GamepadShutdown() {
#if !defined(WIN32)
	int i;
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		if (STATE[i].fd != -1) {
			close(STATE[i].fd);
		}
	}
#endif
}

/* Update stick info */
static void GamepadUpdateStick(GAMEPAD_AXIS* axis, float deadzone) {
	// determine magnitude of stick
	axis->length = sqrtf((float)(axis->x*axis->x) + (float)(axis->y*axis->y));

	if (axis->length > deadzone) {
		// clamp length to maximum value
		if (axis->length > 32767.0f) {
			axis->length = 32767.0f;
		}

		// normalized X and Y values
		axis->nx = axis->x / axis->length;
		axis->ny = axis->y / axis->length;

		// adjust length for deadzone and find normalized length
		axis->length -= deadzone;
		axis->length /= (32767.0f - deadzone);

		// find angle of stick in radians
		axis->angle = atan2f((float)axis->y, (float)axis->x);
	} else {
		axis->x = axis->y = 0;
		axis->nx = axis->ny = 0.0f;
		axis->length = axis->angle = 0.0f;
	}
}

/* Update trigger info */
static void GamepadUpdateTrigger(GAMEPAD_TRIGINFO* trig) {
	if (trig->value > GAMEPAD_DEADZONE_TRIGGER) {
		trig->length = ((trig->value - GAMEPAD_DEADZONE_TRIGGER) / (255.0f - GAMEPAD_DEADZONE_TRIGGER));
	} else {
		trig->value = 0;
		trig->length = 0.0f;
	}
}

void GamepadUpdate() {
	int i;
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		STATE[i].bLast = STATE[i].bCurrent;
#if defined(WIN32)
		XINPUT_STATE xs;
		if(XInputGetState(i, &xs) == 0) {
			STATE[i].flags |= FLAG_CONNECTED;

			/* update state */
			STATE[i].bCurrent = xs.Gamepad.wButtons;
			STATE[i].trigger[TRIGGER_LEFT].value = xs.Gamepad.bLeftTrigger;
			STATE[i].trigger[TRIGGER_RIGHT].value = xs.Gamepad.bRightTrigger;
			STATE[i].stick[STICK_LEFT].x = xs.Gamepad.sThumbLX;
			STATE[i].stick[STICK_LEFT].y = xs.Gamepad.sThumbLY;
			STATE[i].stick[STICK_RIGHT].x = xs.Gamepad.sThumbRX;
			STATE[i].stick[STICK_RIGHT].y = xs.Gamepad.sThumbRY;
		} else {
			STATE[i].flags ^= FLAG_CONNECTED;
		}
#else
		if (STATE[i].fd != -1) {
			struct js_event je;
			while (read(STATE[i].fd, &je, sizeof(je)) > 0) {
				int button;
				switch (je.type) {
				case JS_EVENT_BUTTON:
					/* determine which button the event is for */
					switch (je.number) {
					case 0: button = BUTTON_A; break;
					case 1: button = BUTTON_B; break;
					case 2: button = BUTTON_X; break;
					case 3: button = BUTTON_Y; break;
					case 4: button = BUTTON_LEFT_SHOULDER; break;
					case 5: button = BUTTON_RIGHT_SHOULDER; break;
					case 6: button = BUTTON_BACK; break;
					case 7: button = BUTTON_START; break;
					case 8: button = 0; break; /* XBOX button  */
					case 9: button = BUTTON_LEFT_THUMB; break;
					case 10: button = BUTTON_RIGHT_THUMB; break;
					default: button = 0; break;
					}

					/* set or unset the button */
					if (je.value) {
						STATE[i].bCurrent |= button;
					} else {
						STATE[i].bCurrent ^= button;
					}
						
					break;
				case JS_EVENT_AXIS:
					/* normalize and store the axis */
					switch (je.number) {
					case 0:	STATE[i].stick[STICK_LEFT].x = je.value; break;
					case 1:	STATE[i].stick[STICK_LEFT].y = -je.value; break;
					case 2:	STATE[i].trigger[TRIGGER_LEFT].value = (je.value + 32768) >> 8; break;
					case 3:	STATE[i].stick[STICK_RIGHT].x = je.value; break;
					case 4:	STATE[i].stick[STICK_RIGHT].y = -je.value; break;
					case 5:	STATE[i].trigger[TRIGGER_RIGHT].value = (je.value + 32768) >> 8; break;
					case 6:
						if (je.value == -32767) {
							STATE[i].bCurrent |= BUTTON_DPAD_LEFT;
							STATE[i].bCurrent &= ~BUTTON_DPAD_RIGHT;
						} else if (je.value == 32767) {
							STATE[i].bCurrent |= BUTTON_DPAD_RIGHT;
							STATE[i].bCurrent &= ~BUTTON_DPAD_LEFT;
						} else {
							STATE[i].bCurrent &= ~BUTTON_DPAD_LEFT & ~BUTTON_DPAD_RIGHT;
						}
						break;
					case 7:
						if (je.value == -32767) {
							STATE[i].bCurrent |= BUTTON_DPAD_UP;
							STATE[i].bCurrent &= ~BUTTON_DPAD_DOWN;
						} else if (je.value == 32767) {
							STATE[i].bCurrent |= BUTTON_DPAD_DOWN;
							STATE[i].bCurrent &= ~BUTTON_DPAD_UP;
						} else {
							STATE[i].bCurrent &= ~BUTTON_DPAD_UP & ~BUTTON_DPAD_DOWN;
						}
						break;
					default: break;
					}

					break;
				default:
					break;
				}
			}
		}
#endif

		/* calculate refined stick and trigger values */
		if ((STATE[i].flags & FLAG_CONNECTED) != 0) {
			GamepadUpdateStick(&STATE[i].stick[STICK_LEFT], GAMEPAD_DEADZONE_LEFT_STICK);
			GamepadUpdateStick(&STATE[i].stick[STICK_RIGHT], GAMEPAD_DEADZONE_RIGHT_STICK);

			GamepadUpdateTrigger(&STATE[i].trigger[TRIGGER_LEFT]);
			GamepadUpdateTrigger(&STATE[i].trigger[TRIGGER_RIGHT]);
		}
	}
}

int GamepadIsConnected(GAMEPAD_DEVICE device) {
	return STATE[device].flags & FLAG_CONNECTED;
}

int GamepadButtonDown(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) {
	return (STATE[device].bCurrent & button) != 0;
}

int GamepadButtonTriggered(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) {
	return (STATE[device].bLast & button) == 0 &&
			(STATE[device].bCurrent & button) != 0;
}

int GamepadButtonReleased(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button) {
	return (STATE[device].bCurrent & button) == 0 &&
			(STATE[device].bLast & button) != 0;
}

int GamepadTriggerValue(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) {
	return STATE[device].trigger[trigger].value;
}

float GamepadTriggerLength(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger) {
	return STATE[device].trigger[trigger].length;
}

void GamepadStickXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int *outX, int *outY) {
	*outX = STATE[device].stick[stick].x;
	*outY = STATE[device].stick[stick].y;
}

float GamepadStickLength(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) {
	return STATE[device].stick[stick].length;
}

void GamepadStickNormXY(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, float *outX, float *outY) {
	*outX = STATE[device].stick[stick].nx;
	*outY = STATE[device].stick[stick].ny;
}

float GamepadStickAngle(GAMEPAD_DEVICE device, GAMEPAD_STICK stick) {
	return STATE[device].stick[stick].angle;
}

int GamepadStickDir(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR stickdir) {
	/* length must be non-zero */
	if (STATE[device].stick[stick].length == 0.0f) {
		return GAMEPAD_FALSE;
	}

	/* check directions */
	switch (stickdir) {
	case STICKDIR_UP:
		return STATE[device].stick[stick].angle >= PI_1_4 && STATE[device].stick[stick].angle < PI_3_4;
	case STICKDIR_DOWN:
		return STATE[device].stick[stick].angle >= -PI_3_4 && STATE[device].stick[stick].angle < -PI_1_4;
	case STICKDIR_LEFT:
		return STATE[device].stick[stick].angle >= PI_3_4 || STATE[device].stick[stick].angle < -PI_3_4;
	case STICKDIR_RIGHT:
		return STATE[device].stick[stick].angle < PI_1_4 && STATE[device].stick[stick].angle >= -PI_1_4;
	default:
		return GAMEPAD_FALSE;
	}
}
