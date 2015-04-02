#pragma once

#include "DirectInput.h"

unsigned long confKeys[2][25];

void _EmuKeys();
void _PADEvent(int PAD);
void _PADEventExtra(int PAD);
void PADsetMode(int pad, int mode);

void ProcMouseInput();
void LoadCombos();
void LoadConfig();
void InitRects(void);

// extern void ComboAnalog(int analogKey, int pad);

void AllInOne(int pad);

#include "GeneralOptions.h"

extern GeneralOptions ExtendedOptions;
