#pragma once

// Set to 0 if you want to use wx.h instead of precompiled header
#define WX_PRECOM 1

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif
