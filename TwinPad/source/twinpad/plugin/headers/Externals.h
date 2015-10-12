#ifndef __EXTERNALS_H_
#define __EXTERNALS_H_

extern HMODULE hDLL;
extern HANDLE gwxMainThread;
extern HWND hGFXwnd;
extern HWND hGSPUwnd;

extern unsigned char KeyState[256], BufferKeyState[256];

typedef struct {
	u8 x,y;
	u8 button;
} Analog;

extern bool inside;  // if mouse inside the window..
extern Analog lanalog[2];
extern Analog ranalog[2];
extern POINT MousePt;
extern RECT WndRect, rectMouseArea;
extern int ScreenWidth;
extern int ScreenHeight;
extern u16 status[2];
extern int padID[2];
extern int cmdLen[2];
extern unsigned int pads;
extern int curPad;
extern int curByte[2];
extern int padOpened;

extern unsigned char minXY[2], maxXY[2];
extern unsigned char states[2][10];

class CPressure
{
public:
	u8 Triangle;
	u8 Circle;
	u8 Cross;
	u8 Square;
	
	u8 Up;
	u8 Right;
	u8 Down;
	u8 Left;

	u8 L1;
	u8 R1;
	u8 L2;
	u8 R2;
};

extern CPressure Pressure;

extern unsigned int defaultPressure[2];

#endif
