#pragma once

#include <windows.h>
#include "DirectInput.h"

#pragma warning(disable : 4996)  // Disable deprecated warnings.

// The following data types will be used as global externs.
Analog lanalog[2], ranalog[2];

static keyEvent curEvent, oldEvent;
unsigned int mousePAD;
unsigned int mouseSensitivity;
RECT WndRect, testRect, rectMouseArea;
POINT MousePt;
u16 status[2];
int pressure;

unsigned long confKeys[2][25];
char MouseButtonMap[8]; // Deal with Mouse Buttons.

void _EmuKeys();
void _PADEvent(int PAD);
void _PADEventExtra(int PAD);
void PADsetMode(int pad, int mode);

extern void ProcMouseInput(int pad);
extern void LoadCombos();
extern void LoadConfig();
extern void InitRects(void);
extern bool ExecCombo();
// extern void ComboAnalog(int analogKey, int pad);

void AllInOne(int pad);

#include "GeneralOptions.h"

extern GeneralOptions ExtendedOptions;
