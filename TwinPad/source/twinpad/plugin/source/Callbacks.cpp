#include "stdafx.h"
#include "DirectInput.h"
#include "twinpad_gui.h"
#include "main.h"				// for ConfigureTwinPad_Plugin()
#include "Externals.h"

// Name of this plugin and the version number
char *libraryName      = "TwinPad";

const unsigned char version  = 0x02; // PS2E_PAD_VERSION
const unsigned char revision = 0;    // v0.X.X
const unsigned char build    = 9;    // vX.9.X
const unsigned char subversion = 1;  // vX.X.0

u32 CALLBACK PS2EgetLibType()
{	
	return 0x02; // PS2E_LT_PAD
} 

char* CALLBACK PS2EgetLibName()
{ 
	return libraryName; 
}

u32 CALLBACK PS2EgetLibVersion2(u32 type)
{ 
	return (subversion << 24) | (version << 16) | (revision << 8) | build; 
}

void CALLBACK PADsetSettingsDir(const char* dir)
{
	wxString path = (dir == NULL) ? "inis/" : dir;

	GUI_Controls.SetSettingsPath(path);
}

void CALLBACK PADshutdown() 
{
	Cleanup_TwinPad_DLL();
}

s32 CALLBACK PADopen(HWND hDsp)
{ 
	return _PADopen(hDsp); 
}

void CALLBACK PADclose()
{ 
	_PADclose(); 
}

u32 CALLBACK PADquery()
{ 
	return 3; // both
}

int CALLBACK PADfreeze(int mode, void *pd)
{ 
	return 0;	// Useless function in my plugin since TwinPad can determine the type of PAD when Loading... :p
}  

void CALLBACK PADupdate(int pad)
{
	/* This function is called by PCSX2 every vsync automatically,
	to compute PADs events, but PSX EMUs do NOT call this function
	It's not part of PSEmu Pro specifications, to maintain compatibility between both,
	This function will be called by PSX Emus from PADpoll(), but PADpoll wont call this one
	for PCSX2! testing/playing for hours and hours, with using pthreads no deadlocks whatsoever :p*/
}

u8 CALLBACK PADpoll(u8 value)
{ 
	return _PADpoll(value); 
}

void CALLBACK PADconfigure()
{
	// Check if the settings path is the default directory and if it exists or not, if not create it
	if (GUI_Controls.GetSettingsPath() == "inis/")
		wxFileName::Mkdir("inis", 0777, wxPATH_MKDIR_FULL);

	ConfigureTwinPad();
}

void CALLBACK PADabout()
{
	wxMessageBox("Based on PadWinKeyb by: Linuzappz and Asadr.. 2004\nRewritten as TwinPad by: Rebel_X 2006-2015",
		"TwinPad Plugin ;)", wxICON_INFORMATION);
}

s32 CALLBACK PADtest()
{
	return 0;
}

s32 CALLBACK PADinit(u32 flags)
{
	pads |= flags;
	return 0;
}

u8 CALLBACK PADstartPoll(int pad)
{
	curPad = pad-1;
	curByte[curPad] = 0;
	
	return 0xff;
}

void _PADclose() 
{
	TermDI();
	
	hGSPUwnd = hGFXwnd = NULL;
	curByte[0] = curByte[1] = 0;
	cmdLen[0]  = cmdLen[1]  = 0;
}

// // // // // // // // // // // // // // // //PSX related functions// // // // // // // // // // // // // // /
typedef struct 
{
	unsigned char controllerType;
	unsigned short buttonStatus;
	unsigned char rightJoyX, rightJoyY, leftJoyX, leftJoyY;
	unsigned char moveX, moveY;
	unsigned char reserved[91];
} PadDataS;

// Newer PSX EMUs wont use these 2 functions, instead they'll use PADstartPoll() padPoll()
// they are here for compatibility with PSEmu Pro specifications..

long CALLBACK PADreadPort1(PadDataS* pads)
{
	memset(pads, 0, sizeof(PadDataS));
	if ((padID[0] & 0xf0) == 0x40)
		pads->controllerType = 4; // Digital
	else
		pads->controllerType = 7; // Analog
	pads->buttonStatus = status[0];
	pads->leftJoyX = lanalog[0].x;
	pads->leftJoyY = lanalog[0].y;
	pads->rightJoyX = ranalog[0].x;
	pads->rightJoyY = ranalog[0].y;
	return 0;
}

long CALLBACK PADreadPort2(PadDataS* pads)
{
	memset(pads, 0, sizeof(PadDataS));
	if ((padID[1] & 0xf0) == 0x40)
		pads->controllerType = 4; // Digital
	else
		pads->controllerType = 7; // Analog
	pads->buttonStatus = status[1];
	pads->leftJoyX = lanalog[1].x;
	pads->leftJoyY = lanalog[1].y;
	pads->rightJoyX = ranalog[1].x;
	pads->rightJoyY = ranalog[1].y;
	return 0;
}

u32 CALLBACK PSEgetLibType(void)
{ 
	return 8; // PSE_LT_PAD
}

char* CALLBACK PSEgetLibName(void)
{ 
	return libraryName; 
}

u32 CALLBACK PSEgetLibVersion(void)
{ 
	return (subversion << 24) | (version << 16) | (revision << 8) | build; 
}
