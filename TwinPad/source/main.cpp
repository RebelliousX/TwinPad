// TwinPad PAD plugin for PSX (PSEmu Pro specifications) and PS2 (PCSX2) emulators
// Created by: Rebel_X
// Based on the work of PADwin team of the PADwinKeyboard plugin.
// As the original plugin, TwinPad is licensed under GPLv2 or higher.
// Please see LINCENSE.TXT for more information.

#include "fastCompile.h"
//#include "wx/app.h"

#ifndef __WINDOWS__
	#error "Currently TwinPad is Windows-only"
#endif

#include "main.h"
#include "twinpad_gui.h"

#include "DirectInput.h"

#include "wx/dynlib.h"
//#include "wx/msw/wrapwin.h"

#include <process.h> // for _beginthreadex()

HMODULE hDI = NULL;							// DLL handle, passed to DirectInput
HWND hGFXwnd = NULL, hGSPUwnd = NULL;		// Both for getting HWND from GPU/GS window
HWND hEmuWnd = NULL;						// Hackish, get handle to emulator, since wxDialog can't have win32 parent

// ----------------------------------------------------------------------------
// GUI classes
// ----------------------------------------------------------------------------
TwinPad_Frame::TwinPad_Frame(wxString title) : wxDialog(0, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
	wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP)
{
	tmrAnimate		= new CReAnimate(this);
	tmrAutoNavigate = new CAutoNav(this);
	tmrGetKey		= new CGetKey(this);
	tmrGetComboKey	= new CGetComboKey(this);

	CreateControls(this);

	SetSize(GetMinSize());
	SetPosition(wxPoint(200, 20));

	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TwinPad_Frame::OnClose));
}

TwinPad_Frame::~TwinPad_Frame()
{
	delete tmrAnimate;
	tmrAnimate = 0;
	delete tmrAutoNavigate;
	tmrAutoNavigate = 0;
	delete tmrGetKey;
	tmrGetKey = 0;
	delete tmrGetComboKey;
	tmrGetComboKey = 0;
}

void TwinPad_Frame::OnClose(wxCloseEvent &event)
{
	SetForegroundWindow(hEmuWnd);
	ShowWindow(hEmuWnd, SW_RESTORE);
	event.Skip();
}

static const int CMD_SHOW_WINDOW = wxNewId();
static const int CMD_TERMINATE = wxNewId();

void wx_dll_cleanup();

class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL() 
	{
		// Keep the wx "main" thread running even without windows. This greatly
		// simplifies threads handling, because we don't have to correctly
		// implement wx-thread restarting.
		//
		// Note that this only works if you don't explicitly call ExitMainLoop(),
		// except in reaction to wx_dll_cleanup()'s message. wx_dll_cleanup()
		// relies on the availability of wxApp instance and if the event loop
		// terminated, wxEntry() would return and wxApp instance would be
		// destroyed.
		//
		// Also note that this is efficient, because if there are no windows, the
		// thread will sleep waiting for a new event. We could safe some memory
		// by shutting the thread down when it's no longer needed, though.
		SetExitOnFrameDelete(false);

		Connect(CMD_SHOW_WINDOW,
			wxEVT_THREAD,
			wxThreadEventHandler(TwinPad_DLL::OnShowWindow));
		Connect(CMD_TERMINATE,
			wxEVT_THREAD,
			wxThreadEventHandler(TwinPad_DLL::OnTerminate));
	}

	bool OnInit() 
	{
		// Without this, Grid custom cell renderer will fail to load GIFs as a BMPs
		wxImage::AddHandler(new wxGIFHandler);
		return true; 
	}

	int FilterEvent(wxEvent& event)
	{
		if ( (((wxKeyEvent&)event).GetKeyCode() == WXK_LEFT) ||
			 (((wxKeyEvent&)event).GetKeyCode() == WXK_RIGHT) ||
			 (((wxKeyEvent&)event).GetKeyCode() == WXK_TAB) )
				return 1;

		// To prevent navigation of controls using arrow keys and tab, ignore all key down events
		if (event.GetEventType() == wxEVT_KEY_DOWN ||
			event.GetEventType() == wxEVT_KEY_UP)
			if (GUI_Controls.curTab == 0)	// Keyboard tab
				return 1;
		// Accept all other events (it will crash otherwise)
		return -1;
	}

	void OnShowWindow(wxThreadEvent& event)
	{
		// Switch between GS/GPU handle window and TwinPap Config handle window
		// to be used for DirectInput which uses only hGFXwnd. This is just a precaution, because
		// user might want to summon TwinPad Config window using the HOT key and InitDI() would be called again.
		// the only needed things to do are to assign a new hGFXwnd from TwinPad_Frame before calling InitDI()
		// and make sure if DI is initialized, terminated it then reinitialize it. When done with the TwinPad window
		// again terminate DI and reinitialize it for GS/GPU window. NOTE: the emu will assign a new handle when 
		// PADopen is called and GS/GPU window is running.
		HWND hGFXwnd_temp = hGFXwnd;
		TwinPad_Frame *twinPad_Frame = new TwinPad_Frame("TwinPad Configuration Utility");
		hGFXwnd = (HWND)twinPad_Frame->GetHWND();
#ifdef __WINDOWS__
		// Terminate DI, if successfully terminated, then it was running before and user called TwinPad
		// Config using the Hot key
		bool wasDI_Running = TermDI();
		if (!InitDI())
		{
			wxMessageBox("Can't Initialize DirectInput!", "Failure...", wxICON_ERROR);
			delete twinPad_Frame;
			twinPad_Frame = 0;
			GUI_Controls.mainFrame = 0;		// To prevent showing more than one window at a time
			return;
		}
#endif
		
		twinPad_Frame->ShowModal();

#ifdef __WINDOWS__
		// Terminate DirectInput for TwinPad Config window
		TermDI();
		// If DI was running before, reinitialize it again but with hGFXwnd handle, if it was running
		// and interrupted using the Hot key
		if (wasDI_Running)
		{
			hGFXwnd = hGFXwnd_temp;
			if (!InitDI())
			{
				wxMessageBox("Failed to reinitialize DirectInput", "Oops!", wxICON_ERROR);
			}
		}
#endif
		// Restore original hGFXwnd
		hGFXwnd = hGFXwnd_temp;

		delete twinPad_Frame;
		twinPad_Frame = 0;
		GUI_Controls.mainFrame = 0;		// To prevent showing more than one window at a time
		OnTerminate(wxThreadEvent(wxEVT_THREAD, -1));
	}

	void OnTerminate(wxThreadEvent& WXUNUSED(event))
	{
		ExitMainLoop();
	}

	int OnExit()
	{
		wx_dll_cleanup();
		return wxApp::OnExit();
	}
};

// ----------------------------------------------------------------------------
// application startup
// ----------------------------------------------------------------------------

void run_wx_gui_from_dll();

void ConfigureTwinPad()
{
	// Don't show more than one configuration window
	if (GUI_Controls.mainFrame)
		return;

	// This will fool the host app if it was using wxWidgets by creating faux 
	// main thread to run the GUI from the plugin. TwinPad_DLL::OnShowWindow() will be called
	// due to the DLL's main thread and create our frame and controls
	run_wx_gui_from_dll();
	//wx_dll_cleanup();
}


// we can't have WinMain() in a DLL and want to start the app ourselves
IMPLEMENT_APP_NO_MAIN(TwinPad_DLL)

namespace
{

	// Critical section that guards everything related to wxWidgets "main" thread
	// startup or shutdown.
	wxCriticalSection gs_wxStartupCS;
	// Handle of wx "main" thread if running, NULL otherwise
	HANDLE gs_wxMainThread = NULL;


	//  wx application startup code -- runs from its own thread
	unsigned wxSTDCALL MyAppLauncher(void* event)
	{
		// Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
		//       at this point and won't release it until we signal it.

		// We need to pass correct HINSTANCE to wxEntry() and the right value is
		// HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
		// function to get it. Notice that under Windows XP and later the name is
		// not needed/used as we retrieve the DLL handle from an address inside it
		// but you do need to use the correct name for this code to work with older
		// systems as well.
		const HINSTANCE
			hInstance = wxDynamicLibrary::MSWGetModuleHandle("padTwinPad",
				&gs_wxMainThread);
		if (!hInstance)
			return 0; // failed to get DLL's handle

		// Save the handle of this DLL to be used for DirectInput
		hDI = hInstance;

		 // IMPLEMENT_WXWIN_MAIN does this as the first thing
		wxDISABLE_DEBUG_SUPPORT();

		// We do this before wxEntry() explicitly, even though wxEntry() would
		// do it too, so that we know when wx is initialized and can signal
		// run_wx_gui_from_dll() about it *before* starting the event loop.
		wxInitializer wxinit;
		if (!wxinit.IsOk())
			return 0; // failed to init wx

					  // Signal run_wx_gui_from_dll() that it can continue
		HANDLE hEvent = *(static_cast<HANDLE*>(event));
		if (!SetEvent(hEvent))
			return 0; // failed setting up the mutex

					  // Run the app:
		wxEntry(hInstance);

		return 1;
	}

} // anonymous namespace


void run_wx_gui_from_dll()
{
	// In order to prevent conflicts with hosting app's event loop, we
	// launch wx app from the DLL in its own thread.
	//
	// We can't even use wxInitializer: it initializes wxModules and one of
	// the modules it handles is wxThread's private module that remembers
	// ID of the main thread. But we need to fool wxWidgets into thinking that
	// the thread we are about to create now is the main thread, not the one
	// from which this function is called.
	//
	// Note that we cannot use wxThread here, because the wx library wasn't
	// initialized yet. wxCriticalSection is safe to use, though.

	wxCriticalSectionLocker lock(gs_wxStartupCS);

	const char *title = "TwinPad Configuration Utility";

	if (!gs_wxMainThread)
	{
		HANDLE hEvent = CreateEvent
			(
				NULL,  // default security attributes
				FALSE, // auto-reset
				FALSE, // initially non-signaled
				NULL   // anonymous
				);
		if (!hEvent)
			return; // error

					// NB: If your compiler doesn't have _beginthreadex(), use CreateThread()
		gs_wxMainThread = (HANDLE)_beginthreadex
			(
				NULL,           // default security
				0,              // default stack size
				&MyAppLauncher,
				&hEvent,        // arguments
				0,              // create running
				NULL
				);

		if (!gs_wxMainThread)
		{
			CloseHandle(hEvent);
			return; // error
		}

		// Wait until MyAppLauncher signals us that wx was initialized. This
		// is because we use wxMessageQueue<> and wxString later and so must
		// be sure that they are in working state.
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	// Send a message to wx thread to show a new frame:
	wxThreadEvent *event =
		new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
	event->SetString(title);
	wxQueueEvent(wxApp::GetInstance(), event);
}

void wx_dll_cleanup()
{
	wxCriticalSectionLocker lock(gs_wxStartupCS);

	if (!gs_wxMainThread)
		return;

	// If wx main thread is running, we need to stop it. To accomplish this,
	// send a message telling it to terminate the app.
	wxThreadEvent *event =
		new wxThreadEvent(wxEVT_THREAD, CMD_TERMINATE);
	wxQueueEvent(wxApp::GetInstance(), event);

	// We must then wait for the thread to actually terminate.
	//WaitForSingleObject(gs_wxMainThread, INFINITE);
	CloseHandle(gs_wxMainThread);
	gs_wxMainThread = NULL;
}
