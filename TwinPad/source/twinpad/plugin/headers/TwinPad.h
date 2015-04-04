#pragma once

#include "DirectInput.h"

void _EmuKeys();
void _PADEvent(int PAD);
void _PADEventExtra(int PAD);
void PADsetMode(int pad, int mode);

void ProcMouseInput();
void LoadCombos();
void LoadConfig();
void InitRects(void);
void AllInOne(int pad);
