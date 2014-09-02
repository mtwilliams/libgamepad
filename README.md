# Gamepad

[![Build Status](https://travis-ci.org/bitbytestudios/gamepad.png?branch=master)](http://travis-ci.org/bitbytestudios/gamepad)

Abstracts gamepad access through an easy to use, cross-platform API.

What platforms does it support?
-------------------------------

| Platform  | Toolchain               | Supported             |
|:---------:|:-----------------------:|:---------------------:|
| Windows   | Microsoft Visual Studio | <span>&#10003;</span> |
| Windows   | MinGW/MSYS              |        Planned        |
| Windows   | Cygwin                  |         Never         |
| Mac OS X  | GCC or Clang            |        Planned        |
| Linux     | GCC or Clang            |        Planned        |
| BSD       | GCC or Clang            |         Maybe         |
| Android   | GCC or Clang            |        Planned        |
| iOS       | GCC or Clang            |         Maybe         |
| Web       | Native Client           |         Maybe         |
| Web       | emscripten              |         Maybe         |

What controllers does it support?
---------------------------------

* Xbox
* Xbox 360
* Xbox One

How do I build it?
------------------

    $ git clone https://github.com/bitbytestudios/gamepad.git
    $ cd gamepad
    $ ./configure
    $ make

For more details please go [here](https://github.com/bitbytestudios/gamepad/wiki/Building).

How do I contribute?
--------------------

See our contribution guide [here](https://github.com/bitbytestudios/meta/wiki/Contributing).

How is it licensed?
-------------------

Under an MIT license with licensing agreements for closed platforms.

```
Copyright (c) 2014 Michael Williams
Copyright (c) 2010-2011 Sean Middleditch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
