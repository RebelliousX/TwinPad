#pragma once

// Set to 0 if you want to use wx.h instead of precompiled header
#define WX_PRECOM 1

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "PS2Etypes.h"

#include "wx/notebook.h"
#include "wx/animate.h"
#include <wx/spinctrl.h>
#include "wx/grid.h"
#include "wx/mstream.h"		// for wxMemoryInputStream
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/mstream.h"		// for wxMemoryInputStream
#include "wx/filename.h"	// to check if directory exists or not
#include "wx/dynlib.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "resources.h"		// All GIFs and resources for TwinPad stored in arrays
#include "labels.h"
#include "CALLBACKS.h"

#include "DI_Globals.h"
#include <dinput.h>
