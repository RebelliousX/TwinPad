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

// For handling memory leaks, show the culprit line of code in VS output window
// When debugging, it changes all "new" operator to call "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// For more  informations, visit: https://msdn.microsoft.com/library/x98tx3cf%28v=vs.110%29.aspx

#ifdef _DEBUG
#	include <crtdbg.h>
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif  // _DEBUG
