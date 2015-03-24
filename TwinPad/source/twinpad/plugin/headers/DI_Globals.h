#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern LPDIRECTINPUT8 g_DI;
extern LPDIRECTINPUTDEVICE8 g_DIKeyboard;
extern LPDIRECTINPUTDEVICE8 g_DIMouse;
extern DIMOUSESTATE2 MouseState;;

#define DIKEYDOWN(data,n) (data[n] & 0x80) // Macro to Simplify KeyPress..
