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
#include <windows.h>
#include <windowsx.h>
#include "TwinPad.h"

HMODULE hDI  = NULL;
HWND hObjWnd = NULL, GShwnd = NULL;
unsigned int pads=0;

int ScreenWidth = 0, ScreenHeight = 0;		//To handle Analog Coords correctly.

u8 *buf;
int padID[2];
int padMode[2];
int curByte[2];
int cmdLen[2];
int ds2mode[2] = {0, 0}; // DS Mode at start
int curPad = NULL;
int curCmd = NULL;
int padOpened = 0;
unsigned char minXY[2] = { 0, 0 };
unsigned char maxXY[2] = { 255, 255 };
unsigned char states[2][10]; //To check if it was pressed before..


unsigned char KeyState[256], BufferKeyState[256];

int lbutDown = 0, rbutDown = 0, mbutDown = 0;
bool inside = false;

void PADsetMode(int pad, int mode) {
	
	padMode[pad] = mode;
	switch(ds2mode[curPad]) {
		case 0: // dualshock
			switch (mode) {
				case 0: // digital
					padID[pad] = 0x41;
					break;

				case 1: // analog
					padID[pad] = 0x73;
					break;
			}
			break;
		case 1: // dualshock2
			switch (mode) {
				case 0: // digital
					padID[pad] = 0x41;
					break;

				case 1: // analog
					padID[pad] = 0x79;
					break;
			}
			break;
	}
}

//int padOpened = 0;
s32  _PADopen(HWND hDsp) {

	/*if (++padOpened == 2) 
		return 0;*/
	
	memset(&curEvent, 0, sizeof(curEvent));
	oldEvent = curEvent;

	status[0] = 0xffff;
	status[1] = 0xffff;

	PADsetMode(0, 0);
	PADsetMode(1, 0);

	pressure = 100;

	for (int i = 0; i <=1; i++) {
		ranalog[i].x = ranalog[i].y = lanalog[i].x = lanalog[i].y = 0x80;
		ranalog[i].button =  lanalog[i].button = 0;
	}

	LoadConfig();
	LoadCombos();

	//If DirectInput Fails, return error..
	if (fDI == NULL)
	{
		
		if (!IsWindow (hDsp) && !IsBadReadPtr ((u32*)hDsp, 4))
                GShwnd = *(HWND*)hDsp;
        if (!IsWindow (GShwnd))
                GShwnd = NULL;
        else
        {
                while (GetWindowLong (GShwnd, GWL_STYLE) & WS_CHILD)
                        GShwnd = GetParent (GShwnd);
        }
		
		hObjWnd = GShwnd;

		if( !InitDI())
			return 1;
	}

	return 0;
}

//Those are the commands' set for PSX/PS2 DualShock1 or 2..
//for more information. visit: http://www.curiousinventor.com/guides/ps2 or Lynxmotion.
u8 stdpar[2][20] = { {0xff, 0x5a, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00},
					 {0xff, 0x5a, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00}};
u8 cmd40[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5a},
					 {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5a}};
u8 cmd41[2][8]    = { {0xff, 0x5a, 0xff, 0xff, 0x03, 0x00, 0x00, 0x5a},
					 {0xff, 0x5a, 0xff, 0xff, 0x03, 0x00, 0x00, 0x5a}};
u8 unk46[2][8]    = { {0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A},
					 {0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A}};
u8 unk47[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00},
					 {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00}};
u8 unk4c[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					 {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
u8 unk4d[2][8]    = { {0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
					 {0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
u8 cmd4f[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a},
					 {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a}};
u8 stdcfg[2][8]   = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					 {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; // 2 & 3 = 0
u8 stdmode[2][8]  = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					 {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
//return values for cmd45:
u8 stdmodel[2][8] = { {0xff, 0x5a, 
					  0x03, // 03 - dualshock2, 01 - dualshock
					  0x02, // number of modes
					  0x01, // current mode: 01 - analog, 00 - digital
					  0x02,
					  0x01,
					  0x00},
					  {0xff, 0x5a, 
					  0x03, // 03 - dualshock2, 01 - dualshock
					  0x02, // number of modes
					  0x01, // current mode: 01 - analog, 00 - digital
					  0x02,
					  0x01,
					  0x00}};


u8  _PADpoll(u8 value) {
	u8 button_check = 0, button_check2 = 0;
	if (curByte[curPad] == 0) {
		curByte[curPad]++;
		curCmd = value;
		switch (value) {
			case 0x40: // DUALSHOCK2 ENABLER 
				cmdLen[curPad] = 8;
				buf = cmd40[curPad];
				return 0xf3;

			case 0x41: // QUERY_DS2_ANALOG_MODE
				cmdLen[curPad] = 8;
				buf = cmd41[curPad];
				return 0xf3;

			case 0x42: // READ_DATA
				//Read PAD status.
				AllInOne(curPad);

				if(lanalog[curPad].button) status[curPad] &= ~(1<<9);
				else status[curPad] |= (1<<9);
				if(ranalog[curPad].button) status[curPad] &= ~(1<<10);
				else status[curPad] |= (1<<10);

				lanalog[curPad].button = 0; // reset them :p (l3)
				ranalog[curPad].button = 0; // reset them :p (r3)

				stdpar[curPad][2] = status[curPad] >> 8;
				stdpar[curPad][3] = status[curPad] & 0xff;
				stdpar[curPad][4] = ranalog[curPad].x;
				stdpar[curPad][5] = ranalog[curPad].y;
				stdpar[curPad][6] = lanalog[curPad].x;
				stdpar[curPad][7] = lanalog[curPad].y;

				if (padID[curPad] == 0x79) //DualShock 2: Analog
					cmdLen[curPad] = 20;
                else 
					if (padID[curPad] == 0x73) //DualShock 1: Analog
						cmdLen[curPad] = 8;
					else
						if (padID[curPad] == 0x41) //DualShock 1 or 2: Digital
							cmdLen[curPad] = 4;

				button_check2 = stdpar[curPad][2] >> 4;
				
				switch(stdpar[curPad][3])
				{
				
				case 0xbf: // X
					stdpar[curPad][14] = (pressure*255)/100;
					break;
				case 0xdf: // Circle
					stdpar[curPad][13] = (pressure*255)/100;
					break;
				case 0xef: // Triangle
					stdpar[curPad][12] = (pressure*255)/100;
					break;
				case 0x7f: // Square
					stdpar[curPad][15] = (pressure*255)/100;
					break;
				case 0xfb: // L1
					stdpar[curPad][16] = (pressure*255)/100;
					break;
				case 0xf7: // R1
					stdpar[curPad][17] = (pressure*255)/100;
					break;
				case 0xfe: // L2
					stdpar[curPad][18] = (pressure*255)/100;
					break;
				case 0xfd: // R2
					stdpar[curPad][19] = (pressure*255)/100;
					break;
				default:
					stdpar[curPad][14] = 0x00; // Not pressed
					stdpar[curPad][13] = 0x00; // Not pressed
					stdpar[curPad][12] = 0x00; // Not pressed
					stdpar[curPad][15] = 0x00; // Not pressed
					stdpar[curPad][16] = 0x00; // Not pressed
					stdpar[curPad][17] = 0x00; // Not pressed
					stdpar[curPad][18] = 0x00; // Not pressed
					stdpar[curPad][19] = 0x00; // Not pressed
					break;
				}
				switch(button_check2)
				{
				case 0xE: // UP
					stdpar[curPad][10] = (pressure*255)/100; 
					break;
				case 0xB: // DOWN
					stdpar[curPad][11] = (pressure*255)/100; 
					break;
				case 0x7: // LEFT
					stdpar[curPad][9] = (pressure*255)/100; 
					break;
				case 0xD: // RIGHT
					stdpar[curPad][8] = (pressure*255)/100; 
					break;
				default:
					stdpar[curPad][8] = 0x00;  // Not pressed
					stdpar[curPad][9] = 0x00;  // Not pressed
					stdpar[curPad][10] = 0x00; // Not pressed
					stdpar[curPad][11] = 0x00; // Not pressed
					break;
				}

				curByte[curPad] = 0;
				buf = stdpar[curPad];
				return padID[curPad];

			
			case 0x43: // CONFIG_MODE
				cmdLen[curPad] = 8;
				buf = stdcfg[curPad];
				if (stdcfg[curPad][3] == 0xff) return 0xf3;
				else return padID[curPad];

			case 0x44: // SET_MODE_AND_LOCK
				cmdLen[curPad] = 8;
				buf = stdmode[curPad];
				return 0xf3;

			case 0x45: // QUERY_MODEL_AND_MODE
				cmdLen[curPad] = 8;
				buf = stdmodel[curPad];
				buf[4] = padMode[curPad];
				return 0xf3;

			case 0x46: // ??
				cmdLen[curPad] = 8;
				buf = unk46[curPad];
				return 0xf3;

			case 0x47: // ??
				cmdLen[curPad] = 8;
				buf = unk47[curPad];
				return 0xf3;

			case 0x4c: // QUERY_MODE ??
				cmdLen[curPad] = 8;
				buf = unk4c[curPad];
				return 0xf3;

			case 0x4d:
				cmdLen[curPad] = 8;
				buf = unk4d[curPad];
				return 0xf3;

			case 0x4f: // SET_DS2_NATIVE_MODE
				cmdLen[curPad] = 8;
				padID[curPad] = 0x79; // setting Analog mode for ds2
				ds2mode[curPad] = 1; // Set DS2 Mode
				buf = cmd4f[curPad];
				return 0xf3;

			default:
				break;
		}
	}

	switch (curCmd) {
		case 0x43:
			if(curByte[curPad] == 2)
			{
				switch(value){
				case 0:
					buf[2] = 0;
					buf[3] = 0;
					break;
				case 1:
					buf[2] = 0xff;
					buf[3] = 0xff;
					break;
				}
			}
			break;

		case 0x44:
			if (curByte[curPad] == 2) {
				PADsetMode(curPad, value);
			}
			break;

		case 0x46:
			if(curByte[curPad] == 2) {
				switch(value) {
				case 0: // default
					buf[5] = 0x2;
					buf[6] = 0x0;
					buf[7] = 0xA;
					break;
				case 1: // Param std conf change
					buf[5] = 0x1;
					buf[6] = 0x1;
					buf[7] = 0x14;
					break;
				}
			}
			break;

		case 0x4c:
			if (curByte[curPad] == 2) {
				switch (value) {
					case 0: // mode 0 - digital mode
						buf[5] = 0x4;
						break;

					case 1: // mode 1 - analog mode
						buf[5] = 0x7;
						break;
				}
			}
			break;
	}

	if (curByte[curPad] >= cmdLen[curPad]) return 0;
	return buf[curByte[curPad]++];
}

void AllInOne(int pad)
{
	//KeyRelease everything.. 
	status[pad] = 0xffff;
	lanalog[pad].x = lanalog[pad].y = 0x80;

	//Get Current Keyboard status, and save the old status.
	GetKeyboardStatus();
	
	//Process KeyPresses
	if (ExtendedOptions.IsEnabled_PAD1 && pad == 0)
		_PADEvent(0);

	if (ExtendedOptions.IsEnabled_PAD2 && pad == 1)
		_PADEvent(1);

	//Mouse
	if (ExtendedOptions.IsEnabled_MOUSE)
	{
		if (pad == mousePAD)
		{
			//Initializing Rects.
			InitRects();
			//Get Current Mouse status.
			GetMouseStatus();
			//Process Mouse Input
			ProcMouseInput(pad);
		}
	}

	//Process COMBO Buttons
	if (ExtendedOptions.IsEnabled_COMBOS)
	{
		if (ExtendedOptions.IsEnabled_FasterCombo)
		{
			ExecCombo();  //will take g_comboPAD anyways, if I pass it "pad" it will ruin everything
						  //That's why I use this hack "FASTER COMBO", it gets executed everytime if enabled.. 2X
		}
		
		if (!ExtendedOptions.IsEnabled_FasterCombo && g_comboPAD == pad)
		{
			ExecCombo(); //will take g_comboPAD anyways, but the difference is this one gets executed once every
						//two iterations, because pad toggles 0 and 1..
		}
	}

	//HotKey for Loading COMBOs on the fly :), usefull when testing/editing COMBOs while playing.
	if (ExtendedOptions.IsEnabled_ComboHotKey)
	{
		if ( DIKEYDOWN(BufferKeyState, DIK_SPACE) && 
			!DIKEYDOWN(KeyState, DIK_SPACE))
				LoadCombos();
	}

	////Process Special EmuKeys and Map DirectInput to VirtualKey
	if (ExtendedOptions.IsEnabled_KeyEvents)
		_EmuKeys();
	
	//Make a copy of old Keyboard state..
	memcpy(BufferKeyState, KeyState, sizeof(KeyState));
}

BOOL APIENTRY DllMain(HANDLE hModule,                  // DLL INIT
                      DWORD  dwReason, 
                      LPVOID lpReserved) {
	hDI = (HMODULE)hModule;
	return TRUE;                                          // very quick :)
}

void _PADEvent(int pad) {

	int i;
	if (pad == 0) {
		_PADEventExtra(pad);
		for (i=0; i<16; i++)
			if ( DIKEYDOWN(KeyState, confKeys[pad][i]) )
				status[pad]&=~(1<<i);
			//else /*release code became obsolete*/
			//	status[pad]|= (1<<i);
	}
	if (pad == 1) {
		_PADEventExtra(pad);
		for (i=0; i<16; i++)
			if ( DIKEYDOWN(KeyState, confKeys[pad][i]) )
				status[pad]&=~(1<<i);
			//else /*release code became obsolete*/
			//	status[pad]|= (1<<i);

	}
}

//////////////////////////////////////Added Functions////////////////////////////////////////////////
void _PADEventExtra( int pad )
{
	//Left Analog stick as Keyboard..
	if ( DIKEYDOWN(KeyState, confKeys[pad][16]) ) //up (forward)
	{
		states[pad][0] = 1;
		lanalog[pad].y = minXY[pad];
	}
	else if (states[pad][0] == 1)
			{
				states[pad][0] = 0;
				lanalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][18])) //down (backward)
	{
		states[pad][1] = 1;
		lanalog[pad].y = maxXY[pad];
	}
	else if (states[pad][1] == 1)
			{
				states[pad][1] = 0;
				lanalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][17])) //right
	{
		states[pad][2] = 1;
		lanalog[pad].x = maxXY[pad];
	}
	else if (states[pad][2] == 1)
			{
				states[pad][2] = 0;
				lanalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][19])) //left
	{
		states[pad][3] = 1;
		lanalog[pad].x = minXY[pad];
	}
	else if (states[pad][3] == 1)
			{
				states[pad][3] = 0;
				lanalog[pad].x = 0x80;
			}
	//Right Analog stick as Keyboard..
	if (DIKEYDOWN(KeyState, confKeys[pad][20])) //up (forward)
	{
		states[pad][4] = 1;
		ranalog[pad].y = minXY[pad];
	}
	else if (states[pad][4] == 1)
			{
				states[pad][4] = 0;
				ranalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][22])) //down (backward)
	{
		states[pad][5] = 1;
		ranalog[pad].y = maxXY[pad];
	}
	else if (states[pad][5] == 1)
			{
				states[pad][5] = 0;
				ranalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][21])) //right
	{
		states[pad][6] = 1;
		ranalog[pad].x = maxXY[pad];
	}
	else if (states[pad][6] == 1)
			{
				states[pad][6] = 0;
				ranalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][23])) //left
	{
		states[pad][7] = 1;
		ranalog[pad].x = minXY[pad];
	}
	else if (states[pad][7] == 1)
			{
				states[pad][7] = 0;
				ranalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][9]))	 //L3
	{
		states[pad][8] = 1;
		lanalog[pad].button = 1;
	}
	else if (states[pad][8] == 1)
			{	
				states[pad][8] = 0;
				lanalog[pad].button = 0;
			}
	if (DIKEYDOWN(KeyState, confKeys[pad][10])) //R3
	{
		states[pad][9] = 1;
		ranalog[pad].button = 1;
	}
	else if (states[pad][9] == 1)
			{
				states[pad][9] = 0;
				ranalog[pad].button = 0;
			}

	//////////////////////////////////////////////////////////////////////////
	//Special Keys to TOGGLE between Sensitivity of Analog controls, (Walk/Run)
	if ( DIKEYDOWN(BufferKeyState, confKeys[pad][24]) && //was pressed before
		 !DIKEYDOWN(KeyState, confKeys[pad][24]))        //but it's NOT pressed now, there is KEYRELEASE
		{
			if (minXY[pad] == 0)
			{
				//Walk
				minXY[pad] = 64;   //Average between Center and the Edges. for UP and LEFT
				maxXY[pad] = 192;  //Average between Center and the Edges. for DOWN and RIGHT			
			}
			else
				{
					//Run
					minXY[pad] = 0;   //The Most value for UP and LEFT
					maxXY[pad] = 255;  //The Most value for DOWN and RIGHT	
				}
		}
}

static int SHIFTKEY  = KEYRELEASE;
static int SHIFTFLAG = 0;

void _EmuKeys()
{
	//First handle some DIK_* that can't be mapped to VK_* through MapVirtualKey()
	//The list has more, but these what matters, maybe GSdx?? but Pete's GPUs uses them.. to record or show FPS..etc!
	if (DIKEYDOWN(KeyState, DIK_DELETE)) {
		curEvent.evt = KEYPRESS;
		curEvent.key = VK_DELETE;
		return;
	}
	else
		if (DIKEYDOWN(KeyState, DIK_INSERT)) {
			curEvent.evt = KEYPRESS;
			curEvent.key = VK_INSERT;
			return;
		}
		else
			if (DIKEYDOWN(KeyState, DIK_HOME)) {
				curEvent.evt = KEYPRESS;
				curEvent.key = VK_HOME;
				return;
			}
			else
				if (DIKEYDOWN(KeyState, DIK_PRIOR)) {
					curEvent.evt = KEYPRESS;
					curEvent.key = VK_PRIOR;
					return;
				}
				else
					if (DIKEYDOWN(KeyState, DIK_END)) {
						curEvent.evt = KEYPRESS;
						curEvent.key = VK_END;
						return;
					}
					else
						if (DIKEYDOWN(KeyState, DIK_NEXT)) {
							curEvent.evt = KEYPRESS;
							curEvent.key = VK_NEXT;
							return;
						}
	//Compare between current status (KeyState) and old status (BufferKeyState)
	//And to Determine the Event Occurred using MapVirtualKey function..
	//Also this will handle EmuKeys like ESCAPE, F1,...F12, ..etc!
	int i = 0;
	while( i++ < 256)
	{
		//if it's not pressed now, but was before, there is KeyRelease..
		if (DIKEYDOWN(KeyState, i)) //KeyPress
		{	
			if (VK_SHIFT == MapVirtualKey(i,1))
				if (SHIFTKEY != KEYPRESS)
				{
					SHIFTKEY = KEYPRESS;
					SHIFTFLAG = 1;
					return;
				}
				else
					continue;

			curEvent.evt = KEYPRESS;
			curEvent.key = MapVirtualKey(i,1);
			return;
		}
		else 
			if (DIKEYDOWN(BufferKeyState,i)) //KeyRelease
			{	
				if (VK_SHIFT == MapVirtualKey(i,1))
					if (SHIFTKEY != KEYRELEASE)
					{
						SHIFTKEY = KEYRELEASE;
						SHIFTFLAG = 1;
						return;
					}
					else
						continue;

				curEvent.evt = KEYRELEASE;
				curEvent.key = MapVirtualKey(i,1);
				return;
			}
	} 
}

// PADkeyEvent is called every vsync (return NULL if no event)
keyEvent* CALLBACK PADkeyEvent() {

	if (!ExtendedOptions.IsEnabled_KeyEvents)
		return NULL;

	//Shift Key test..
	if (SHIFTFLAG == 1)
	{
		SHIFTFLAG = 0;
		if (SHIFTKEY == KEYPRESS)
		{
			curEvent.evt = KEYPRESS;
			curEvent.key = VK_SHIFT;
		}
		else
		{
			curEvent.evt = KEYRELEASE;
			curEvent.key = VK_SHIFT;
		}

		oldEvent = curEvent;
		return &curEvent;
	}
	//If no events occured, return null..
	if (oldEvent.evt == curEvent.evt &&
		oldEvent.key == curEvent.key )
		return NULL;

	oldEvent = curEvent;
	return &curEvent;
}
