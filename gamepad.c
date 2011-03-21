/**
 * Gamepad Input Library
 * Sean Middleditch
 * Copyright (C) 2010  Sean Middleditch
 * LICENSE: MIT/X
 */

#include <math.h>
#include <string.h>
#include <malloc.h>

#include "gamepad.h"

/* Platform-specific includes */
#if defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN 1
#	undef UNICODE
#	include "windows.h"
#	include "xinput.h"
#	pragma comment(lib, "xinput.lib")
#elif defined(__linux__)
#	include <linux/joystick.h>
#	include <stdio.h>
#	include <fcntl.h>
#	include <unistd.h>
#	include <libudev.h>
#else
#	error "Unknown platform in gamepad.c"
#endif

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

/* Structure for state of a particular gamepad */
typedef struct GAMEPAD_STATE GAMEPAD_STATE;
struct GAMEPAD_STATE {
	GAMEPAD_AXIS stick[STICK_COUNT];
	GAMEPAD_TRIGINFO trigger[TRIGGER_COUNT];
	int bLast, bCurrent, flags;
#if defined(__linux__)
	char* device;
	int fd;
#endif
};

/* State of the four gamepads */
static GAMEPAD_STATE STATE[4];

/* Note whether a gamepad is currently connected */
#define FLAG_CONNECTED (1<<0)

/* Prototypes */
static void GamepadPlatformInit			(void);
static void GamepadPlatformUpdate		(void);
static void GamepadPlatformUpdateDevice	(GAMEPAD_DEVICE gamepad);
static void GamepadPlatformShutdown		(void);

static void GamepadUpdateStick			(GAMEPAD_AXIS* axis, float deadzone);
static void GamepadUpdateTrigger		(GAMEPAD_TRIGINFO* trig);

/* Various values of PI */
#define PI_1_4	0.78539816339744f
#define PI_1_2	1.57079632679489f
#define PI_3_4	2.35619449019234f
#define PI		3.14159265358979f

/* Platform-specific implementation code */
#if defined(_WIN32)

static void GamepadPlatformInit(void) {
	/* no Win32 initialization required */
}

static void GamepadPlatformUpdate(void) {
	/* no Win32 global update required */
}

static void GamepadPlatformUpdateDevice(GAMEPAD_DEVICE gamepad) {
	XINPUT_STATE xs;
	if (XInputGetState(gamepad, &xs) == 0) {
		STATE[gamepad].flags |= FLAG_CONNECTED;

		/* update state */
		STATE[gamepad].bCurrent = xs.Gamepad.wButtons;
		STATE[gamepad].trigger[TRIGGER_LEFT].value = xs.Gamepad.bLeftTrigger;
		STATE[gamepad].trigger[TRIGGER_RIGHT].value = xs.Gamepad.bRightTrigger;
		STATE[gamepad].stick[STICK_LEFT].x = xs.Gamepad.sThumbLX;
		STATE[gamepad].stick[STICK_LEFT].y = xs.Gamepad.sThumbLY;
		STATE[gamepad].stick[STICK_RIGHT].x = xs.Gamepad.sThumbRX;
		STATE[gamepad].stick[STICK_RIGHT].y = xs.Gamepad.sThumbRY;
	} else {
		STATE[gamepad].flags &= ~FLAG_CONNECTED;
	}
}

static void GamepadPlatformShutdown(void) {
	/* no Win32 shutdown required */
}

#elif defined(__linux__)

/* UDev handles */
static struct udev* UDEV = NULL;
static struct udev_monitor* MON = NULL;

/* Helper to add a new device */
static void GamepadAddDevice(const char* devPath) {
	int i;

	/* try to find a free controller */
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		if ((STATE[i].flags & FLAG_CONNECTED) == 0) {
			STATE[i].device = strdup(devPath);
			if (STATE[i].device != NULL) {
				STATE[i].fd = open(STATE[i].device, O_RDONLY|O_NONBLOCK);
				if (STATE[i].fd != -1) {
					STATE[i].flags = FLAG_CONNECTED;
				} else {
					free(STATE[i].device);
					STATE[i].device = NULL;
				}
			}
			break;
		}
	}
}

/* Helper to remove a device */
static void GamepadRemoveDevice(const char* devPath) {
	int i;
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		if (STATE[i].device != NULL && strcmp(STATE[i].device, devPath) == 0) {
			if (STATE[i].fd != -1) {
				close(STATE[i].fd);
				STATE[i].fd = -1;
			}
			free(STATE[i].device);
			STATE[i].device = 0;
			STATE[i].flags = 0;
			break;
		}
	}
}

static void GamepadPlatformInit(void) {
	struct udev_list_entry* devices;
	struct udev_list_entry* item;
	struct udev_enumerate* enu;

	/* open the udev handle */
	UDEV = udev_new();
	if (UDEV == NULL) {
		/* FIXME: flag error? */
		return;
	}
	
	/* open monitoring device (safe to fail) */
	MON = udev_monitor_new_from_netlink(UDEV, "udev");
	/* FIXME: flag error if hot-plugging can't be supported? */
	if (MON != NULL) {
		udev_monitor_enable_receiving(MON);
		/* udev_monitor_filter_add_match_devtype(MON, "input"); */
	}

	/* enumerate joypad devices */
	enu = udev_enumerate_new(UDEV);
	udev_enumerate_add_match_subsystem(enu, "input");
	udev_enumerate_scan_devices(enu);
	devices = udev_enumerate_get_list_entry(enu);

	udev_list_entry_foreach(item, devices) {
		const char* name;
		const char* sysPath;
		const char* devPath;
		struct udev_device* dev;

		name = udev_list_entry_get_name(item);
		dev = udev_device_new_from_syspath(UDEV, name);
		sysPath = udev_device_get_syspath(dev);
		devPath = udev_device_get_devnode(dev);

		if (sysPath != NULL && devPath != NULL && strstr(sysPath, "/js") != 0) {
			GamepadAddDevice(devPath);
		}

		udev_device_unref(dev);
	}

	/* cleanup */
	udev_enumerate_unref(enu);
}

static void GamepadPlatformUpdate(void) {
	if (MON != NULL) {
		fd_set r;
		struct timeval tv;
		int fd = udev_monitor_get_fd(MON);

		/* set up a poll on the udev device */
		FD_ZERO(&r);
		FD_SET(fd, &r);

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		select(fd + 1, &r, 0, 0, &tv);

		/* test if we have a device change */
		if (FD_ISSET(fd, &r)) {
			struct udev_device* dev = udev_monitor_receive_device(MON);
			if (dev) {
				const char* devNode = udev_device_get_devnode(dev);
				const char* sysPath = udev_device_get_syspath(dev);
				const char* action = udev_device_get_action(dev);
				sysPath = udev_device_get_syspath(dev);
				action = udev_device_get_action(dev);

				if (strstr(sysPath, "/js") != 0) {
					if (strcmp(action, "remove") == 0) {
						GamepadRemoveDevice(devNode);
					} else if (strcmp(action, "add") == 0) {
						GamepadAddDevice(devNode);
					}
				}

				udev_device_unref(dev);
			}
		}
	}
}

static void GamepadPlatformUpdateDevice(GAMEPAD_DEVICE gamepad) {
	if (STATE[gamepad].flags & FLAG_CONNECTED) {
		struct js_event je;
		while (read(STATE[gamepad].fd, &je, sizeof(je)) > 0) {
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
					STATE[gamepad].bCurrent |= button;
				} else {
					STATE[gamepad].bCurrent ^= button;
				}
					
				break;
			case JS_EVENT_AXIS:
				/* normalize and store the axis */
				switch (je.number) {
				case 0:	STATE[gamepad].stick[STICK_LEFT].x = je.value; break;
				case 1:	STATE[gamepad].stick[STICK_LEFT].y = -je.value; break;
				case 2:	STATE[gamepad].trigger[TRIGGER_LEFT].value = (je.value + 32768) >> 8; break;
				case 3:	STATE[gamepad].stick[STICK_RIGHT].x = je.value; break;
				case 4:	STATE[gamepad].stick[STICK_RIGHT].y = -je.value; break;
				case 5:	STATE[gamepad].trigger[TRIGGER_RIGHT].value = (je.value + 32768) >> 8; break;
				case 6:
					if (je.value == -32767) {
						STATE[gamepad].bCurrent |= BUTTON_DPAD_LEFT;
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_RIGHT;
					} else if (je.value == 32767) {
						STATE[gamepad].bCurrent |= BUTTON_DPAD_RIGHT;
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_LEFT;
					} else {
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_LEFT & ~BUTTON_DPAD_RIGHT;
					}
					break;
				case 7:
					if (je.value == -32767) {
						STATE[gamepad].bCurrent |= BUTTON_DPAD_UP;
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_DOWN;
					} else if (je.value == 32767) {
						STATE[gamepad].bCurrent |= BUTTON_DPAD_DOWN;
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_UP;
					} else {
						STATE[gamepad].bCurrent &= ~BUTTON_DPAD_UP & ~BUTTON_DPAD_DOWN;
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
}

static void GamepadPlatformShutdown(void) {
	int i;

	/* cleanup udev */
	udev_monitor_unref(MON);
	udev_unref(UDEV);

	/* cleanup devices */
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		if (STATE[i].device != NULL) {
			free(STATE[i].device);
		}

		if (STATE[i].fd != -1) {
			close(STATE[i].fd);
		}
	}
}

#else /* !defined(_WIN32) && !defined(__linux__) */

#	error "Unknown platform in gamepad.c"

#endif /* end of platform implementations */

void GamepadInit() {
	/* zero out state data */
	memset(STATE, 0, sizeof(STATE));

	/* perform platform initialization */
	GamepadPlatformInit();
}

void GamepadShutdown(void) {
	GamepadPlatformShutdown();
}

void GamepadUpdate(void) {
	GamepadPlatformUpdate();

	int i;
	for (i = 0; i != GAMEPAD_COUNT; ++i) {
		/* store previous button state */
		STATE[i].bLast = STATE[i].bCurrent;

		/* per-platform update routines */
		GamepadPlatformUpdateDevice((GAMEPAD_DEVICE)i);

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
