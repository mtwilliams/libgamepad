/**
 * Gamepad Input Library
 * Sean Middleditch
 * Copyright (C) 2010  Sean Middleditch
 * LICENSE: MIT/X
 */

#pragma once

#if defined(_WIN32)
#	if defined(GAMEPAD_EXPORT)
#		define PUBLIC __declspec(dllexport)
#	else
#		define PUBLIC __declspec(dllimport)
#	endif
#else
#	define PUBLIC extern
#endif

enum GAMEPAD_DEVICE {
	GAMEPAD_0 = 0,
	GAMEPAD_1 = 1,
	GAMEPAD_2 = 2,
	GAMEPAD_3 = 3,

	GAMEPAD_COUNT
};

enum GAMEPAD_BUTTON {
	BUTTON_DPAD_UP			= (1<<0),
	BUTTON_DPAD_DOWN		= (1<<1),
	BUTTON_DPAD_LEFT		= (1<<2),
	BUTTON_DPAD_RIGHT		= (1<<3),
	BUTTON_START			= (1<<4),
	BUTTON_BACK				= (1<<5),
	BUTTON_LEFT_THUMB		= (1<<6),
	BUTTON_RIGHT_THUMB		= (1<<7),
	BUTTON_LEFT_SHOULDER	= (1<<8),
	BUTTON_RIGHT_SHOULDER	= (1<<9),
	BUTTON_A				= (1<<12),
	BUTTON_B				= (1<<13),
	BUTTON_X				= (1<<14),
	BUTTON_Y				= (1<<15),
};

enum GAMEPAD_TRIGGER {
	TRIGGER_LEFT	= 0,
	TRIGGER_RIGHT	= 1,

	TRIGGER_COUNT
};

enum GAMEPAD_STICK {
	STICK_LEFT		= 0,
	STICK_RIGHT		= 1,

	STICK_COUNT
};

enum GAMEPAD_STICKDIR {
	STICKDIR_UP		= 0,
	STICKDIR_DOWN	= 1,
	STICKDIR_LEFT	= 2,
	STICKDIR_RIGHT	= 3,

	STICKDIR_COUNT
};

typedef enum GAMEPAD_DEVICE GAMEPAD_DEVICE;
typedef enum GAMEPAD_BUTTON GAMEPAD_BUTTON;
typedef enum GAMEPAD_TRIGGER GAMEPAD_TRIGGER;
typedef enum GAMEPAD_STICK GAMEPAD_STICK;
typedef enum GAMEPAD_STICKDIR GAMEPAD_STICKDIR;

enum GAMEPAD_BOOL {
	GAMEPAD_FALSE	= 0,
	GAMEPAD_TRUE	= 1
};

#define GAMEPAD_DEADZONE_LEFT_STICK		7849
#define	GAMEPAD_DEADZONE_RIGHT_STICK	8689	
#define GAMEPAD_DEADZONE_TRIGGER		30

PUBLIC void		GamepadInit				(void);
PUBLIC void		GamepadShutdown			(void);
PUBLIC void		GamepadUpdate			(void);

PUBLIC int		GamepadIsConnected		(GAMEPAD_DEVICE device);

PUBLIC int		GamepadButtonDown		(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
PUBLIC int		GamepadButtonTriggered	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
PUBLIC int		GamepadButtonReleased	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);

PUBLIC int		GamepadTriggerValue		(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);
PUBLIC float	GamepadTriggerLength	(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);

PUBLIC void		GamepadStickXY			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int* outX, int* outY);
PUBLIC void		GamepadStickNormXY		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, float* outX, float* outY);

PUBLIC float	GamepadStickLength		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);
PUBLIC float	GamepadStickAngle		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);

PUBLIC int		GamepadStickDir			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR stickdir);