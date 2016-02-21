#pragma once

////////// Library files	/////////////////////////////////////////
#ifdef _WIN32
//#	pragma comment(lib, "dxguid.lib")
//#	pragma comment(lib, "dinput8.lib")
#	pragma comment(lib, "rpcrt4.lib")
#	pragma comment(lib, "wsock32.lib")
#	pragma comment(lib, "winmm.lib")
#	pragma comment(lib, "comctl32.lib")
#	ifdef _DEBUG
#		pragma comment(lib, "wxmsw31ud_core.lib")
#		pragma comment(lib, "wxbase31ud.lib")
#		pragma comment(lib, "OIS_static_d.lib")
#	else
#		pragma comment(lib, "wxmsw31u_core.lib")
#		pragma comment(lib, "wxbase31u.lib")
#		pragma comment(lib, "OIS_static.lib")
#	endif
#endif

/////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif

#include "PS2Etypes.h"

#include "wx/notebook.h"
#include "wx/animate.h"
#include <wx/spinctrl.h>
#include "wx/grid.h"
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

#include "InputManager.h"

// For handling memory leaks, show the culprit line of code in VS output window
// When debugging, it changes all "new" operator to call "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// For more  informations, visit: https://msdn.microsoft.com/library/x98tx3cf%28v=vs.110%29.aspx
#if defined(_DEBUG) && defined(_WIN32)
#	include <crtdbg.h>
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif  // _DEBUG
