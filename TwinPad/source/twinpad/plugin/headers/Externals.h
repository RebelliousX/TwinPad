#ifndef __EXTERNALS_H_
#define __EXTERNALS_H_

#include "DI_Globals.h"
#include "PS2Etypes.h"

extern HMODULE hDI;
extern HWND hGFXwnd;
extern HWND hGSPUwnd;

extern unsigned char KeyState[256], BufferKeyState[256];

extern int lbutDown;
extern int rbutDown;
extern int mbutDown;
extern bool inside;

extern unsigned long confKeys[2][25];

typedef struct {
	u8 x,y;
	u8 button;
} Analog;

extern unsigned int mousePAD;
extern unsigned int mouseSensitivity;
extern unsigned int mouseScrollUp;
extern unsigned int mouseScrollDown;
extern char MouseButtonMap[8];
extern bool inside;  // if mouse inside the window..
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