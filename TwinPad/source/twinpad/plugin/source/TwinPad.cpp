#include "stdafx.h"
#include "TwinPad.h"
#include "twinpad_gui.h"
#include "main.h"
#include "Externals.h"
#include "COMBOs.h"
#include "Loading.h"
#include "DirectInput.h"

// The following data types will be used as global externs.
Analog lanalog[2], ranalog[2];

static keyEvent curEvent, oldEvent;
RECT WndRect, rectMouseArea;
POINT MousePt;
u16 status[2];
CPressure Pressure;
unsigned int defaultPressure[2] = { 255, 255 };			// Full pressure

unsigned int pads = 0;

int ScreenWidth = 0, ScreenHeight = 0;		// To handle Analog Coords correctly.

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
unsigned char states[2][10]; // To check if it was pressed before..


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

// int padOpened = 0;
s32  _PADopen(HWND hDsp) 
{
	wxInitializer wxinit;
	if(!hDLL)	// Was DLL previosly loaded?
	{
		const HINSTANCE	hInstance = wxDynamicLibrary::MSWGetModuleHandle("padTwinPad", &gwxMainThread);

		if (!wxinit.IsOk())
			return -1;	// At this point, we can't even display a messagebox, exit.

		if (!hInstance)
		{
			wxMessageBox("Failed to load the DLL!", "Fatal Error", wxICON_ERROR);
			return -1; // failed to get DLL's handle
		}

		// Save the handle of this DLL to be used for DirectInput
		hDLL = hInstance;
		wxSetInstance(hDLL);
	}
	
	memset(&curEvent, 0, sizeof(curEvent));
	oldEvent = curEvent;

	status[0] = 0xffff;
	status[1] = 0xffff;

	PADsetMode(0, 0);
	PADsetMode(1, 0);

	for (int i = 0; i < 2; ++i) 
	{
		ranalog[i].x = ranalog[i].y = lanalog[i].x = lanalog[i].y = 0x80;
		ranalog[i].button =  lanalog[i].button = 0;
	}

	Configurations.Clean();

	{
		// Check to see if configuration files are present, otherwise create null ones
		// Also validate the versions of files (compare headers), shouldn't continue if there is error!
		wxString file1, file2;
		file1 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
		IsFileOkAndFix(file1, GUI_Controls.GetTwinPad_Header());
		file2 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
		IsFileOkAndFix(file2, GUI_Controls.GetTwinPad_ComboHeader());
	}

	if (!LoadConfig() || !LoadCombos())
	{
		wxMessageBox("Failed to load configuration files, please configure TwinPad.", "Fatal Error", wxICON_ERROR);
		return -1; // failed to load TwinPad.ini or TwinPad_COMBOs.ini
	}

	// If DirectInput Fails, return error..
	if (g_DI == NULL)
	{
		if (!IsWindow (hDsp) && !IsBadReadPtr ((u32*)hDsp, 4))
                hGSPUwnd = *(HWND*)hDsp;
        if (!IsWindow (hGSPUwnd))
                hGSPUwnd = NULL;
        else
        {
                while (GetWindowLong (hGSPUwnd, GWL_STYLE) & WS_CHILD)
                        hGSPUwnd = GetParent (hGSPUwnd);
        }
		
		hGFXwnd = hGSPUwnd;

		// PCSXR/PSXeven or other PSX emus do not pass HWND for the GPU
		if (hGFXwnd == NULL)
			hGFXwnd = hGSPUwnd = GetActiveWindow();

		if( !InitDI())
			return 1;
	}

	return 0;
}

// These are the commands' set for PSX/PS2 DualShock1 or 2.. 
// Playstation 2 (Dual Shock) controller protocol notes
// for more information. Please visit those awesome reference: 
// https://gist.github.com/scanlime/5042071 and http://store.curiousinventor.com/guides/PS2/#mappings
u8 stdpar[2][20] = { {0xff, 0x5a, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80,
				      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00},
					 {0xff, 0x5a, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80,
				      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00}};
// Command 0x40 Initialize pressure sensor
u8 cmd40[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5a},
					  {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5a}};
// Command 0x41 Get available polling results
u8 cmd41[2][8]    = { {0xff, 0x5a, 0xff, 0xff, 0x03, 0x00, 0x00, 0x5a},
					  {0xff, 0x5a, 0xff, 0xff, 0x03, 0x00, 0x00, 0x5a}};

// Command 0x42 Controller poll (Read buttons/axes, write actuators)
// 		....missing....

// Command 0x43 Controller Read and Escape
// 		....missing....

// Command 0x44 Set major mode (DualShock/Digital)
// 		....missing....

// Command 0x45 Read extended status 1
// 		....missing....

// Command 0x46 Read constant 1
u8 unk46[2][8]    = { {0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A},
					  {0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A}};
// Command 0x47 Read constant 2
u8 unk47[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00},
					  {0xff, 0x5a, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00}};
// Command 0x4c Read constant 3
u8 unk4c[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					  {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
// Command 0x4d Specify polling command format
u8 unk4d[2][8]    = { {0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
					  {0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
// Command 0x4f Specify polling result format
u8 cmd4f[2][8]    = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a},
					  {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a}};

u8 stdcfg[2][8]   = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					  {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; // 2 & 3 = 0
u8 stdmode[2][8]  = { {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
					  {0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
// return values for cmd45:
u8 stdmodel[2][8] = { {0xff, 0x5a, 
					   0x03, // 03 - dualshock2, 01 - dualshock (Guitar Hero!?)
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
					   0x00} };


u8  _PADpoll(u8 value) 
{
	if (curByte[curPad] == 0) 
	{
		curByte[curPad]++;
		curCmd = value;
		switch (value) 
		{
			case 0x40: // DUALSHOCK2 ENABLER 
				cmdLen[curPad] = 8;
				buf = cmd40[curPad];
				return 0xf3;

			case 0x41: // QUERY_DS2_ANALOG_MODE
				cmdLen[curPad] = 8;
				buf = cmd41[curPad];
				return 0xf3;

			case 0x42: // READ_DATA
				// Read PAD status.
				AllInOne(curPad);

				//if(lanalog[curPad].button) status[curPad] &= ~(1<<9);
				//else status[curPad] |= (1<<9);
				//if(ranalog[curPad].button) status[curPad] &= ~(1<<10);
				//else status[curPad] |= (1<<10);

				//lanalog[curPad].button = 0; // reset them :p (l3)
				//ranalog[curPad].button = 0; // reset them :p (r3)

				stdpar[curPad][2] = status[curPad] >> 8;
				stdpar[curPad][3] = status[curPad] & 0xff;
				stdpar[curPad][4] = ranalog[curPad].x;
				stdpar[curPad][5] = ranalog[curPad].y;
				stdpar[curPad][6] = lanalog[curPad].x;
				stdpar[curPad][7] = lanalog[curPad].y;

				if (padID[curPad] == 0x79) // DualShock 2: Analog
					cmdLen[curPad] = 20;
                else 
					if (padID[curPad] == 0x73) // DualShock 1: Analog
						cmdLen[curPad] = 8;
					else
						if (padID[curPad] == 0x41) // DualShock 1 or 2: Digital
							cmdLen[curPad] = 4;

				if (padID[curPad] == 0x79)
				{
					stdpar[curPad][8] = Pressure.Right;
					stdpar[curPad][9] = Pressure.Left;
					stdpar[curPad][10] = Pressure.Up;
					stdpar[curPad][11] = Pressure.Down;
					stdpar[curPad][12] = Pressure.Triangle;
					stdpar[curPad][13] = Pressure.Circle;
					stdpar[curPad][14] = Pressure.Cross;
					stdpar[curPad][15] = Pressure.Square;
					stdpar[curPad][16] = Pressure.L1;
					stdpar[curPad][17] = Pressure.R1;
					stdpar[curPad][18] = Pressure.L2;
					stdpar[curPad][19] = Pressure.R2;
				}

				curByte[curPad] = 0;
				buf = stdpar[curPad];
				return padID[curPad];

			case 0x43: // CONFIG_MODE
				cmdLen[curPad] = 8;
				buf = stdcfg[curPad];
				if (stdcfg[curPad][3] == 0xff) 
					return 0xf3;
				else 
					return padID[curPad];

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

	switch (curCmd) 
	{
		case 0x43:
			if(curByte[curPad] == 2)
			{
				switch(value)
				{
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
			if (curByte[curPad] == 2) 
				PADsetMode(curPad, value);
			break;

		case 0x46:
			if(curByte[curPad] == 2) 
			{
				switch(value) 
				{
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
			if (curByte[curPad] == 2) 
			{
				switch (value) 
				{
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
	// KeyRelease everything.. 
	status[pad] = 0xffff;
	lanalog[pad].x = lanalog[pad].y = 0x80;
	ranalog[pad].x = ranalog[pad].y = 0x80;
	lanalog[pad].button = ranalog[pad].button = 0;

	// No pressure is the default, will be overridden when keys are pressed
	Pressure = { 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, };

	// Get Current Keyboard status, and save the old status.
	GetKeyboardStatus();
	
	// Process KeyPresses
	if (Configurations.IsEnabled_PAD1() && pad == 0)
		_PADEvent(0);

	if (Configurations.IsEnabled_PAD2() && pad == 1)
		_PADEvent(1);

	// Mouse
	if (Configurations.IsEnabled_MOUSE())
	{
		if (pad == Configurations.m_mouseAsPad)
		{
			// Initializing rectangles for mouse capture.
			InitRects();
			// Get Current Mouse status.
			GetMouseStatus();
			// Process Mouse Input
			ProcMouseInput();
		}
	}

	// Process COMBO Buttons
	if (Configurations.IsEnabled_COMBOS())
		ExecuteCombo(pad);
}

void _PADEvent(int pad) {

	int i;
	_PADEventExtra(pad);
	for (i=0; i<16; i++)
		if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][i]))
		{
			status[pad] &= ~(1 << i);

			switch (i)
			{
			case 0: Pressure.L2 = defaultPressure[pad]; break;
			case 1: Pressure.R2 = defaultPressure[pad]; break;
			case 2: Pressure.L1 = defaultPressure[pad]; break;
			case 3: Pressure.R1 = defaultPressure[pad]; break;
			case 4: Pressure.Triangle = defaultPressure[pad]; break;
			case 5: Pressure.Circle = defaultPressure[pad]; break;
			case 6: Pressure.Cross = defaultPressure[pad]; break;
			case 7: Pressure.Square = defaultPressure[pad]; break;
			case 12: Pressure.Up = defaultPressure[pad]; break;
			case 13: Pressure.Right = defaultPressure[pad]; break;
			case 14: Pressure.Down = defaultPressure[pad]; break;
			case 15: Pressure.Left = defaultPressure[pad]; break;
			default: break;
			}
		}
		// else /*release code became obsolete*/
			//status[pad] |= (1 << i);
}

void _PADEventExtra( int pad )
{
	// Left Analog stick as Keyboard..
	if ( DIKEYDOWN(KeyState, Configurations.m_pad[pad][16]) ) // up (forward)
	{
		states[pad][0] = 1;
		lanalog[pad].y = minXY[pad];
	}
	else if (states[pad][0] == 1)
			{
				states[pad][0] = 0;
				lanalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][18])) // down (backward)
	{
		states[pad][1] = 1;
		lanalog[pad].y = maxXY[pad];
	}
	else if (states[pad][1] == 1)
			{
				states[pad][1] = 0;
				lanalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][17])) // right
	{
		states[pad][2] = 1;
		lanalog[pad].x = maxXY[pad];
	}
	else if (states[pad][2] == 1)
			{
				states[pad][2] = 0;
				lanalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][19])) // left
	{
		states[pad][3] = 1;
		lanalog[pad].x = minXY[pad];
	}
	else if (states[pad][3] == 1)
			{
				states[pad][3] = 0;
				lanalog[pad].x = 0x80;
			}
	// Right Analog stick as Keyboard..
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][20])) // up (forward)
	{
		states[pad][4] = 1;
		ranalog[pad].y = minXY[pad];
	}
	else if (states[pad][4] == 1)
			{
				states[pad][4] = 0;
				ranalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][22])) // down (backward)
	{
		states[pad][5] = 1;
		ranalog[pad].y = maxXY[pad];
	}
	else if (states[pad][5] == 1)
			{
				states[pad][5] = 0;
				ranalog[pad].y = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][21])) // right
	{
		states[pad][6] = 1;
		ranalog[pad].x = maxXY[pad];
	}
	else if (states[pad][6] == 1)
			{
				states[pad][6] = 0;
				ranalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][23])) // left
	{
		states[pad][7] = 1;
		ranalog[pad].x = minXY[pad];
	}
	else if (states[pad][7] == 1)
			{
				states[pad][7] = 0;
				ranalog[pad].x = 0x80;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][9]))	 // L3
	{
		states[pad][8] = 1;
		lanalog[pad].button = 1;
	}
	else if (states[pad][8] == 1)
			{	
				states[pad][8] = 0;
				lanalog[pad].button = 0;
			}
	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][10])) // R3
	{
		states[pad][9] = 1;
		ranalog[pad].button = 1;
	}
	else if (states[pad][9] == 1)
			{
				states[pad][9] = 0;
				ranalog[pad].button = 0;
			}

	// Special Keys to TOGGLE between Sensitivity of Analog controls, (Walk/Run)
	static bool toggle[2] = { false, false };

	if (DIKEYDOWN(KeyState, Configurations.m_pad[pad][24]))							// Walk/Run is pressed now
		toggle[pad] = true;

	if (!DIKEYDOWN(KeyState, Configurations.m_pad[pad][24]) && toggle[pad] == true)	// Walk/Run is released (was pressed before)
	{
		toggle[pad] = false;

		if (minXY[pad] == 0)
		{
			// Walk
			minXY[pad] = 64;   // Average between Center and the Edges. for UP and LEFT
			maxXY[pad] = 192;  // Average between Center and the Edges. for DOWN and RIGHT
			defaultPressure[pad] = 128;
		}
		else
		{
			// Run
			minXY[pad] = 0;   // The Most value for UP and LEFT
			maxXY[pad] = 255;  // The Most value for DOWN and RIGHT	
			defaultPressure[pad] = 255;
		}
	}
}

// PADkeyEvent is called every vsync (return NULL if no event)
keyEvent* CALLBACK PADkeyEvent() {

	if (!Configurations.IsEnabled_KeyEvents())
		return NULL;

	for (int i = 0; i < 256; ++i)
		if (BufferKeyState[i] != KeyState[i])
			{
				curEvent.evt = DIKEYDOWN(KeyState, i) ? KEYPRESS : KEYRELEASE;
				curEvent.key = MapVirtualKey(i, 1);
				memcpy(BufferKeyState, KeyState, sizeof(KeyState));
				return &curEvent;
			}

	return 0;
}
