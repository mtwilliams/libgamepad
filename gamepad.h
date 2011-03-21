/**
 * Gamepad Input Library
 * Sean Middleditch <sean@middleditch.us>
 * Copyright (C) 2010,2011  Sean Middleditch
 * LICENSE: MIT/X
 */

#if !defined(GAMEPAD_H)
#define GAMEPAD_H 1

/**
 * Enumeration of the possible devices.
 *
 * Only four devices are supported as this is the limit of Windows.
 */
enum GAMEPAD_DEVICE {
	GAMEPAD_0,	/**< First gamepad */
	GAMEPAD_1,	/**< Second gamepad */
	GAMEPAD_2,	/**< Third gamepad */
	GAMEPAD_3,	/**< Fourth gamepad */

	GAMEPAD_COUNT	/**< Maximum number of supported gamepads */
};

/**
 * Enumeration of the possible buttons.
 */
enum GAMEPAD_BUTTON {
	BUTTON_DPAD_UP			= (1<<0),	/**< UP on the direction pad */
	BUTTON_DPAD_DOWN		= (1<<1),	/**< DOWN on the direction pad */
	BUTTON_DPAD_LEFT		= (1<<2),	/**< LEFT on the direction pad */
	BUTTON_DPAD_RIGHT		= (1<<3),	/**< RIGHT on the direction pad */
	BUTTON_START			= (1<<4),	/**< START button */
	BUTTON_BACK				= (1<<5),	/**< BACK button */
	BUTTON_LEFT_THUMB		= (1<<6),	/**< Left analog stick button */
	BUTTON_RIGHT_THUMB		= (1<<7),	/**< Right analog stick button */
	BUTTON_LEFT_SHOULDER	= (1<<8),	/**< Left bumper button */
	BUTTON_RIGHT_SHOULDER	= (1<<9),	/**< Right bumper button */
	BUTTON_A				= (1<<12),	/**< A button */
	BUTTON_B				= (1<<13),	/**< B button */
	BUTTON_X				= (1<<14),	/**< X button */
	BUTTON_Y				= (1<<15),	/**< Y button */
};

/**
 * Enumeration of the possible pressure/trigger buttons.
 */
enum GAMEPAD_TRIGGER {
	TRIGGER_LEFT,	/**< Left trigger */
	TRIGGER_RIGHT,	/**< Right trigger */

	TRIGGER_COUNT	/**< Number of triggers */
};

/**
 * Enumeration of the analog sticks.
 */
enum GAMEPAD_STICK {
	STICK_LEFT,		/**< Left stick */
	STICK_RIGHT,	/**< Right stick */

	STICK_COUNT		/**< Number of analog sticks */
};

/**
 * Enumeration of main stick directions.
 *
 * This is used for some of the convenience routines in the library.
 */
enum GAMEPAD_STICKDIR {
	STICKDIR_UP,	/**< UP direction */
	STICKDIR_DOWN,	/**< DOWN direction */
	STICKDIR_LEFT,	/**< LEFT direction */
	STICKDIR_RIGHT	/**< RIGHT direction */
};

typedef enum GAMEPAD_DEVICE GAMEPAD_DEVICE;
typedef enum GAMEPAD_BUTTON GAMEPAD_BUTTON;
typedef enum GAMEPAD_TRIGGER GAMEPAD_TRIGGER;
typedef enum GAMEPAD_STICK GAMEPAD_STICK;
typedef enum GAMEPAD_STICKDIR GAMEPAD_STICKDIR;

#define GAMEPAD_TRUE			1	/**< TRUE value for boolean parameters */
#define GAMEPAD_FALSE			0	/**< FALSE value for boolean parameters */

#define GAMEPAD_DEADZONE_LEFT_STICK		7849	/**< Suggested deadzone magnitude for left analog stick */
#define	GAMEPAD_DEADZONE_RIGHT_STICK	8689	/**< Suggested deadzone magnitude for right analog stick */
#define GAMEPAD_DEADZONE_TRIGGER		30		/**< Suggested deadzone for triggers */

/**
 * Initialize the library.
 *
 * This is critical on non-Windows platforms.
 */
extern void		GamepadInit				(void);

/**
 * Shutdown the library.
 *
 * This will release resources allocated by the library internally.
 *
 * This should be called after forking as well.
 */
extern void		GamepadShutdown			(void);

/**
 * Updates the state of the gamepads.
 *
 * This must be called (at least) once per game loop.
 */
extern void		GamepadUpdate			(void);

/**
 * Test if a particular gamepad is connected.
 *
 * \param device The device to check.
 * \returns GAMEPAD_TRUE if the device is connected, GAMEPAD_FALSE if it is not.
 */
extern int		GamepadIsConnected		(GAMEPAD_DEVICE device);

/**
 * Test if a particular button is being pressed.
 *
 * \param device The device to check.
 * \param button The button to check.
 * \returns GAMEPAD_TRUE if the button is down, GAMEPAD_FALSE if it is not.
 */
extern int		GamepadButtonDown		(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);

/**
 * Test if a particular button has been depressed since the previous call to GamepadUpdate.
 *
 * \param device The device to check.
 * \param button The button to check.
 * \returns GAMEPAD_TRUE if the button has been pressed, GAMEPAD_FALSE if it is not or if it was depressed the previous frame.
 */
extern int		GamepadButtonTriggered	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);

/**
 * Test if a particular button has been released since the previous call to GamepadUpdate.
 *
 * \param device The device to check.
 * \param button The button to check.
 * \returns GAMEPAD_TRUE if the button has been released, GAMEPAD_FALSE if it is down or if it was not down the previous frame.
 */
extern int		GamepadButtonReleased	(GAMEPAD_DEVICE device, GAMEPAD_BUTTON button);

/**
 * Get the trigger value (depression magnitude) in its raw form.
 *
 * \param device The device to check.
 * \param trigger The trigger to check.
 * \returns Trigger depression magnitude (0 to 32767).
 */
extern int		GamepadTriggerValue		(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);

/**
 * Get the trigger value (depression magnitude) in normalized form.
 *
 * \param device The device to check.
 * \param trigger The trigger to check.
 * \returns Trigger depression magnitude (0 to 1).
 */
extern float	GamepadTriggerLength	(GAMEPAD_DEVICE device, GAMEPAD_TRIGGER trigger);

/**
 * Set the rumble motors on/off.
 *
 * To turn off the rumble effect, set values to 0 for both motors.
 *
 * The left motor is the low-frequency/strong motor, and the right motor is the high-frequency/weak motor.
 *
 * \param device The device to update.
 * \param left Left motor strengh (0 to 1).
 * \param right Right motor strengh (0 to 1).
 */
extern void		GamepadSetRumble		(GAMEPAD_DEVICE device, float left, float right);

/**
 * Query the position of an analog stick as raw values.
 *
 * The values retrieved by this function represent the magnitude of the analog
 * stick in each direction.  Note that it shouldn't be possible to get full
 * magnitude in one direction unless the other direction has a magnitude of
 * zero, as the stick has a circular movement range.
 *
 * \param device The device to check.
 * \param stick The stick to check.
 * \param outX Pointer to integer to store the X magnitude in (-32767 to 32767).
 * \param outX Pointer to integer to store the Y magnitude in (-32767 to 32767).
 */
extern void		GamepadStickXY			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, int* outX, int* outY);

/**
 * Query the position of an analog stick as normalized values.
 *
 * The values retrieved by this function represent the magnitude of the analog
 * stick in each direction.  Note that it shouldn't be possible to get full
 * magnitude in one direction unless the other direction has a magnitude of
 * zero, as the stick has a circular movement range.
 *
 * \param device The device to check.
 * \param stick The stick to check.
 * \param outX Pointer to float to store the X magnitude in (-1 to 1).
 * \param outX Pointer to float to store the Y magnitude in (-1 to 1).
 */
extern void		GamepadStickNormXY		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, float* outX, float* outY);

/**
 * Query the magnitude of an analog stick.
 *
 * This returns the normalized value of the magnitude of the stick.  That is,
 * if the stick is pushed all the way in any direction, it returns 1.0.
 *
 * \param device The device to check.
 * \param stick The stick to check.
 * \returns The magnitude of the stick (0 to 1).
 */
extern float	GamepadStickLength		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);

/**
 * Query the direction of a stick (in radians).
 *
 * This returns the direction of the stick.  This value is in radians, not
 * degrees.  Zero is to the right, and the angle increases in a
 * counter-clockwise direction.
 *
 * \param device The device to check.
 * \param stick The stick to check.
 * \returns The angle of the stick (0 to 2*PI).
 */
extern float	GamepadStickAngle		(GAMEPAD_DEVICE device, GAMEPAD_STICK stick);

/**
 * Test whether a stick is pressed in a particular direction.
 *
 * This is a useful utility function for when the stick should be treated as a simple
 * directional pad, such as for menu UIs.
 *
 * \param device The device to check.
 * \param stick The trigger to check.
 * \param stickdir The direction to check for.
 * \returns GAMEPAD_TRUE if the stick is pressed in the specified direction, GAMEPAD_FALSE otherwise.
 */
extern int		GamepadStickDir			(GAMEPAD_DEVICE device, GAMEPAD_STICK stick, GAMEPAD_STICKDIR stickdir);

#endif
