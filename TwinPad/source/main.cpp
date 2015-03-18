// Test wxWidgets, TwinPad

#include "twinpad_gui.h"
#include "main.h"

 #ifndef __WINDOWS__
	#error "Currently TwinPad is Windows-only"
 #endif

#include "wx/app.h"
#include "wx/dynlib.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/msw/wrapwin.h"

#include <process.h> //  for _beginthreadex()

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL();
private:
	void OnShowWindow(wxThreadEvent& event);
	void OnTerminate(wxThreadEvent& event);
};

class TwinPad_Frame : public wxFrame
{
public:
	TwinPad_Frame(wxString title, wxSize size) : wxFrame(0, wxID_ANY, title, wxDefaultPosition, size,
		wxMINIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX) 
	{
		wxInitAllImageHandlers();

		CreateControls(this);

		SetSize(GetMinSize());
		SetPosition(wxPoint(200, 20));

		Show(true);

		Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TwinPad_Frame::OnClose));
	}

	void OnClose(wxCloseEvent& event)
	{
		Show(false);
	}
};

static const int CMD_SHOW_WINDOW = wxNewId();
static const int CMD_TERMINATE = wxNewId();

void TwinPad_DLL::OnShowWindow(wxThreadEvent& event)
{
	static bool runOnce = false;
	
	if (runOnce)
	{
		GUI_Controls.mainFrame->Show();
		return;
	}

	wxFrame *f = NULL;
	// Each time with new frame, memory increase ~500Kb. Avoid that, we don't really destroy the window
	// until we quit the emulator. wxWidgets does reclaim the memory when the window is distroyed.
	if (!runOnce)
	{
		f = new TwinPad_Frame("TwinPad Configuration Utility", wxDefaultSize);
		runOnce = true;
	}
		
	f->Show();
}

void TwinPad_DLL::OnTerminate(wxThreadEvent& WXUNUSED(event))
{
	ExitMainLoop();
}

TwinPad_DLL::TwinPad_DLL()
{
	// This handler should be created once in OnInit() as documentation says
	
	SetExitOnFrameDelete(false);

	Connect(CMD_SHOW_WINDOW,
		wxEVT_THREAD,
		wxThreadEventHandler(TwinPad_DLL::OnShowWindow));
	Connect(CMD_TERMINATE,
		wxEVT_THREAD,
		wxThreadEventHandler(TwinPad_DLL::OnTerminate));
}

IMPLEMENT_APP_NO_MAIN(TwinPad_DLL)

namespace
{

	//  Critical section that guards everything related to wxWidgets "main" thread
	//  startup or shutdown.
	wxCriticalSection gs_wxStartupCS;
	//  Handle of wx "main" thread if running, NULL otherwise
	HANDLE gs_wxMainThread = NULL;


	//   wx application startup code -- runs from its own thread
	unsigned wxSTDCALL MyAppLauncher(void* event)
	{
		//  Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
		//        at this point and won't release it until we signal it.

		//  We need to pass correct HINSTANCE to wxEntry() and the right value is
		//  HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
		//  function to get it. Notice that under Windows XP and later the name is
		//  not needed/used as we retrieve the DLL handle from an address inside it
		//  but you do need to use the correct name for this code to work with older
		//  systems as well.
		const HINSTANCE
			hInstance = wxDynamicLibrary::MSWGetModuleHandle("padTwinPad",
			&gs_wxMainThread);
		if (!hInstance)
			return 0; //  failed to get DLL's handle

		//  IMPLEMENT_WXWIN_MAIN does this as the first thing
		wxDISABLE_DEBUG_SUPPORT();

		//  We do this before wxEntry() explicitly, even though wxEntry() would
		//  do it too, so that we know when wx is initialized and can signal
		//  run_wx_gui_from_dll() about it *before* starting the event loop.
		wxInitializer wxinit;
		if (!wxinit.IsOk())
			return 0; //  failed to init wx

		//  Signal run_wx_gui_from_dll() that it can continue
		HANDLE hEvent = *(static_cast<HANDLE*>(event));
		if (!SetEvent(hEvent))
			return 0; //  failed setting up the mutex

		//  Run the app:
		wxEntry(hInstance);

		return 1;
	}

} //  anonymous namespace

void ConfigureTwinPad()
{
	wxCriticalSectionLocker lock(gs_wxStartupCS);

	if (!gs_wxMainThread)
	{
		HANDLE hEvent = CreateEvent(
			NULL,  //  default security attributes
			FALSE, //  auto-reset
			FALSE, //  initially non-signaled
			NULL   //  anonymous 
			);
		if (!hEvent)
			return; //  error

		//  NB: If your compiler doesn't have _beginthreadex(), use CreateThread()
		gs_wxMainThread = (HANDLE)_beginthreadex
			(
			NULL,           //  default security
			0,              //  default stack size
			&MyAppLauncher,
			&hEvent,        //  arguments
			0,              //  create running
			NULL
			);

		if (!gs_wxMainThread)
		{
			CloseHandle(hEvent);
			return; //  error
		}

		//  Wait until MyAppLauncher signals us that wx was initialized. This
		//  is because we use wxMessageQueue<> and wxString later and so must
		//  be sure that they are in working state.
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	//  Send a message to wx thread to show a new frame:
	wxThreadEvent *event =
		new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
	wxQueueEvent(wxApp::GetInstance(), event);
}

void ShutdownTwinPad()
{
	// TODO: Remove this line. I will keep this for now
	wxMessageBox("exiting");

 	wxCriticalSectionLocker lock(gs_wxStartupCS);

 	if (!gs_wxMainThread)
 		return;
 
 	//  If wx main thread is running, we need to stop it. To accomplish this,
 	//  send a message telling it to terminate the app.
 	wxThreadEvent *event =
 		new wxThreadEvent(wxEVT_THREAD, CMD_TERMINATE);
 	wxQueueEvent(wxApp::GetInstance(), event);
 
 	//  We must then wait for the thread to actually terminate.
 	WaitForSingleObject(gs_wxMainThread, INFINITE);
 	CloseHandle(gs_wxMainThread);
 	gs_wxMainThread = NULL;
}