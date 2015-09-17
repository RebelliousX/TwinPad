#pragma once

void _EmuKeys();
void _PADEvent(int PAD);
void _PADEventExtra(int PAD);
void PADsetMode(int pad, int mode);

void ProcMouseInput();
void InitRects(void);
void AllInOne(int pad);
