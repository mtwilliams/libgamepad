Gamepad Libary
==============

This is a simple library whose sole purpose is to provide an easy to
use and portable API for accessing the extremely popular and common
Xbox/Xbox360 gamepads.

Currently supported platforms are Windows and Linux.  OS X is planned.

Requirements
============

The header and source file can be dropped into a project's build.

On Windows, XInput must be linked in.

On Linux, libudev must be linked in.

Limitations
===========

The library only supports the functions which are compatible with
all OSes.  Primarily missing are:

- Headset/audio support (can only be supported on Windows)
- XBOX button support (the big button in the middle of the controller)
- LED control support
- Battery status support

Headsets are not supported in Linux via the Xbox controller, and the
big XBOX button and LED control are not supported via XInput on Windows.

OS X support has not yet been implemented, as I do not have an OS X
machine to develop or test on.

There are no plans to support POSIX-style OSes other than Linux and OS X.

Platform Issues
===============

On Linux, the LED turned on does not reflect correctly which device ID is
used by the library.  I have not yet figured out how to either query the
LED set or how to override the LED, and the xpad driver seems to assign
them quite arbitrarily.

Also on Linux, the library has no way to differentiate between gamepads
and other joystick devices.  The library will almost certainly not work
correctly with any device that isn't an XBox controller.

Todo / Roadmap
==============

- Correct LED enumeration on Linux.
- OS X support.
- Win32-only headset interaction API.
- non-Win32-only LED/XBOX-button support (maybe).
- Feature-detection routines.
