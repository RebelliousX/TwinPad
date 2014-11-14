#ifndef _EXTERNALS_H_
#define _EXTERNALS_H_

#define DIKEYDOWN(data,n) (data[n] & 0x80) //Macro to Simplify KeyPress..
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "PS2Etypes.h"

extern HWND hObjWnd;
extern HWND GShwnd;

extern unsigned char KeyState[256], BufferKeyState[256];
extern HMODULE hDI;
extern int lbutDown;
extern int rbutDown;
extern int mbutDown;
extern bool inside;

extern unsigned long confKeys[2][25];

typedef struct {
	u8 x,y;
	u8 button;
} Analog;

extern unsigned short mousePAD;
extern unsigned short mouseSensitivity;
extern unsigned short mouseScrollUp;
extern unsigned short mouseScrollDown;
extern char MouseButtonMap[8];
extern bool inside;  //if mouse inside the window..
extern Analog lanalog[2];
extern Analog ranalog[2];
extern POINT MousePt;
extern RECT WndRect, rectMouseArea, testRect;
extern int ScreenWidth;
extern int ScreenHeight;
extern int pressure;
extern u16 status[2];
extern int padID[2];
extern int cmdLen[2];
extern unsigned int pads;
extern int curPad;
extern int curByte[2];
extern int padOpened;

extern LPDIRECTINPUT8 fDI;
extern LPDIRECTINPUTDEVICE8 fDIKeyboard;
extern LPDIRECTINPUTDEVICE8 fDIMouse;
extern DIMOUSESTATE2 MouseState;

extern unsigned char minXY[2], maxXY[2];
extern unsigned char states[2][10];

extern unsigned short g_comboCounter;
extern unsigned short g_comboPAD;

#include <vector>

using std::vector;

vector<vector<vector<int>>> extern COMBO;
vector<vector<int>> extern ActionDelay;
vector<int> extern ActionCount;
vector<int> extern ComboKey;

#endif