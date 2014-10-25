#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "Externals.h"

s32  _PADopen(HWND hDsp);
void _PADclose();
u8 CALLBACK PADpoll(u8 value);
extern u8 _PADpoll(u8 value);

#endif