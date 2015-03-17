#ifndef _DI_GLOBALS_H_
#define _DI_GLOBALS_H_

#include <dinput.h>

// DirectInput Variables
LPDIRECTINPUT8 fDI               = NULL; // Root DirectInput Interface
LPDIRECTINPUTDEVICE8 fDIKeyboard = NULL; // The keyboard device
LPDIRECTINPUTDEVICE8 fDIMouse    = NULL; // The mouse device
DIMOUSESTATE2 MouseState; // this holds the mouse data

#endif