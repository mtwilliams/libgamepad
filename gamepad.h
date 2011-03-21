/**
 * Gamepad Input Library
 * Sean Middleditch
 * Copyright (C) 2010  Sean Middleditch
 * LICENSE: MIT/X
 */

#if !defined(GAMEPAD_H)
#define GAMEPAD_H 1

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

typedef enum GAMEPAD_DEVICE GAMEPAD_DEVICE;
typedef enum GAMEPAD_BUTTON GAMEPAD_BUTTON;
typedef enum GAMEPAD_TRIGGER GAMEPAD_TRIGGER;
typedef enum GAMEPAD_STICK GAMEPAD_STICK;
typedef enum GAMEPAD_STICKDIR GAMEPAD_STICKDIR;

#define GAMEPAD_TRUE			1
#define GAMEPAD_FALSE			0

#define GAMEPAD_DEADZONE_LEFT_STICK		7849
#define	GAMEPAD_DEADZONE_RIGHT_STICK	8689	
#define GAMEPAD_DEADZONE_TRIGGER		30

extern void		GamepadInit				(void);
extern void		GamepadShutdown			(void);
extern void		GamepadUpdate			(void);

extern int		GamepadIsConnected		(GAMEPAD_DEVICE device);

extern int		GamepadButtonDown		(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
extern int		GamepadButtonTriggered	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);
extern int		GamepadButtonReleased	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);

extern int		GamepadTriggerValue		(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);
extern float	GamepadTriggerLength	(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);

extern void		GamepadSetRumble		(GAMEPAD_DEVICE device, float left, float right);

extern void		GamepadStickXY			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int* outX, int* outY);
extern void		GamepadStickNormXY		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, float* outX, float* outY);

extern float	GamepadStickLength		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);
extern float	GamepadStickAngle		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);

extern int		GamepadStickDir			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR stickdir);

#endif
