/**
 * Gamepad Input Library
 * Sean Middleditch
 * Copyright (C) 2010  Sean Middleditch
 * LICENSE: MIT/X
 */

#pragma once

enum GAMEPAD_DEVICE {
	GAMEPAD_0,
	GAMEPAD_1,
	GAMEPAD_2,
	GAMEPAD_3,

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
	TRIGGER_LEFT,
	TRIGGER_RIGHT,

	TRIGGER_COUNT
};

enum GAMEPAD_STICK {
	STICK_LEFT,
	STICK_RIGHT,

	STICK_COUNT
};

enum GAMEPAD_STICKDIR {
	STICKDIR_UP,
	STICKDIR_DOWN,
	STICKDIR_LEFT,
	STICKDIR_RIGHT
};

#define GAMEPAD_STICK_LIGHT		0.35f
#define GAMEPAD_STICK_MEDIUM	0.6f
#define GAMEPAD_STICK_HEAVY		0.85f

extern void		GamepadInit();
extern void		GamepadShutdown();
extern void		GamepadUpdate();

extern bool		GamepadIsConnected		(GAMEPAD_DEVICE device);
extern bool		GamepadButtonDown		(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
extern bool		GamepadButtonTriggered	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
extern bool		GamepadButtonReleased	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
extern float	GamepadTriggerValue		(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);
extern void		GamepadStickXY			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, float* outX, float* outY);
extern float	GamepadStickAngle		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);
extern float	GamepadStickValue		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);
extern bool		GamepadStickDir			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR stickdir);
