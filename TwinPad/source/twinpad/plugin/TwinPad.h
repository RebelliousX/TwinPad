/*  PADwin
 *  Copyright (C) 2002-2004  PADwin Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TWINPAD_H__
#define __TWINPAD_H__

#include <windows.h>
#include "DirectInput.h"
#define PADdefs
#include "PS2Edefs.h"

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dinput.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma warning(disable : 4996)  //Disable deprecated warnings.

//The following data types will be used as global externs.
Analog lanalog[2], ranalog[2];

static keyEvent curEvent, oldEvent;
unsigned short mousePAD;
unsigned short mouseSensitivity;
RECT WndRect, testRect, rectMouseArea;
POINT MousePt;
u16 status[2];
int pressure;

unsigned long confKeys[2][25];
char MouseButtonMap[8]; //Deal with Mouse Buttons.

void _EmuKeys();
void _PADEvent(int PAD);
void _PADEventExtra(int PAD);
void PADsetMode(int pad, int mode);

extern void ProcMouseInput(int pad);
extern void LoadCombos();
extern void LoadConfig();
extern void InitRects(void);
extern bool ExecCombo();
//extern void ComboAnalog(int analogKey, int pad);

void AllInOne(int pad);

#include "GeneralOptions.h"

extern GeneralOptions ExtendedOptions;

#endif

