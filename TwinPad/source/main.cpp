// TwinPad PAD plugin for PSX (PSEmu Pro specifications) and PS2 (PCSX2) emulators
// Created by: Rebel_X
// Based on the work of PADwin team of the PADwinKeyboard plugin.
// As the original plugin, TwinPad is licensed under GPLv2 or higher.
// Please see LINCENSE.TXT for more information.

#include "stdafx.h"

#ifndef __WINDOWS__
	#error "Currently TwinPad is Windows-only"
#endif

#include "main.h"
#include "twinpad_gui.h"
#include "DirectInput.h"

HMODULE hDLL = NULL;						// DLL handle, passed to DirectInput
HWND hGFXwnd = NULL, hGSPUwnd = NULL;		// Both for getting HWND from GPU/GS window
HWND hEmuWnd = NULL;						// Hackish, get handle to emulator, since wxDialog can't have win32 parent

											// ----------------------------------------------------------------------------
											// GUI classes
											// ----------------------------------------------------------------------------
TwinPad_Frame::TwinPad_Frame(wxString title) : wxDialog(0, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
	wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP)
{
	tmrAnimate = new CReAnimate(this);
	tmrAutoNavigate = new CAutoNav(this);
	tmrGetKey = new CGetKey(this);
	tmrGetComboKey = new CGetComboKey(this);

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
	Destroy();
}

static const int CMD_SHOW_WINDOW = wxNewId();

class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL()
	{
		// Note: wx_dll_cleanup() is removed, most emus (like PCSX2) don't call
		// PADshutdown() callback function anymore to cleanup the dll. Thus, I can't
		// do thread terminating/restarting. And for some odd reason, when PCSX2 switched
		// to using wxWidgets 3.0, TwinPad's DllMain function stopped working *correctly*.
		// Despite that both link to wx3.0 statically and shouldn't interfere with each other.
		//
		// -- Taken from dll sample of wxWidgets ----------------------------------------------
		//
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

		Connect(CMD_SHOW_WINDOW, wxEVT_THREAD, wxThreadEventHandler(TwinPad_DLL::OnShowWindow));
	}

	bool OnInit()
	{
		// Without this, Grid custom cell renderer will fail to load GIFs as a BMPs
		wxImage::AddHandler(new wxGIFHandler);
		return true;
	}

	int FilterEvent(wxEvent& event)
	{
		if ((((wxKeyEvent&)event).GetKeyCode() == WXK_LEFT) ||
			(((wxKeyEvent&)event).GetKeyCode() == WXK_RIGHT) ||
			(((wxKeyEvent&)event).GetKeyCode() == WXK_TAB))
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
		TwinPad_Frame *twinPad_Frame = new TwinPad_Frame("TwinPad Configuration Utility");
		hGFXwnd = (HWND)twinPad_Frame->GetHWND();
#ifdef __WINDOWS__
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
#endif
		GUI_Controls.mainFrame = 0;		// To prevent showing more than one window at a time
	}
};

// ----------------------------------------------------------------------------
// application startup
// ----------------------------------------------------------------------------

void Run_wxGUI_From_DLL();

void ConfigureTwinPad()
{
	// Don't show more than one configuration window
	if (GUI_Controls.mainFrame)
		return;

	// This will fool the host app if it was using wxWidgets by creating faux 
	// main thread to run the GUI from the plugin. TwinPad_DLL::OnShowWindow() will be called
	// due to the DLL's main thread and create our frame and controls
	Run_wxGUI_From_DLL();
}

// we can't have WinMain() in a DLL and want to start the app ourselves
IMPLEMENT_APP_NO_MAIN(TwinPad_DLL)

// Handle of wx "main" thread if running, NULL otherwise
HANDLE gwxMainThread = NULL;

namespace
{
	std::mutex gs_mtx;
	std::condition_variable gs_cnd;
	bool gs_canContinue = false;

	//  wx application startup code -- runs from its own thread
	unsigned wxSTDCALL TwinPad_DLL_Launcher()
	{
		// Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
		//       at this point and won't release it until we signal it.

		// We need to pass correct HINSTANCE to wxEntry() and the right value is
		// HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
		// function to get it. Notice that under Windows XP and later the name is
		// not needed/used as we retrieve the DLL handle from an address inside it
		// but you do need to use the correct name for this code to work with older
		// systems as well.

		// The dll could be loaded from PADopen() without showing any GUI. Don't load DLL twice!
		const HINSTANCE	hInstance = wxDynamicLibrary::MSWGetModuleHandle("padTwinPad", &gwxMainThread);

		if (!hInstance)
			return 0; // failed to get DLL's handle

		// Save the handle of this DLL to be used for DirectInput
		hDLL = hInstance;
		wxSetInstance(hDLL);

		// IMPLEMENT_WXWIN_MAIN does this as the first thing
		wxDISABLE_DEBUG_SUPPORT();

		// We do this before wxEntry() explicitly, even though wxEntry() would
		// do it too, so that we know when wx is initialized and can signal
		// run_wx_gui_from_dll() about it *before* starting the event loop.
		wxInitializer wxinit;
		if (!wxinit.IsOk())
			return 0; // failed to init wx

		// Signal Run_wxGUI_From_DLL() that it can continue
		gs_canContinue = true;
		gs_cnd.notify_one();

		// Run the app main's loop:
		wxEntry((HINSTANCE)hDLL);

		return 1;
	}

} // anonymous namespace

void Run_wxGUI_From_DLL()
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
	std::unique_lock<std::mutex> uLocker(gs_mtx);

	static bool bRunOnce = false;	// Do not create more than 1 daemon thread

	if (!bRunOnce)
	{
		std::thread tMainThread(TwinPad_DLL_Launcher);
		gwxMainThread = (HANDLE)tMainThread.native_handle();
		gs_cnd.wait(uLocker, []() { return gs_canContinue; });
		gs_canContinue = false;
		bRunOnce = true;

		// Run it as daemon, it will loop (similar to DllMain) until the calling process exits
		tMainThread.detach();
	}

	// Send a message to wx thread to show a new frame:
	wxThreadEvent *event = new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
	wxQueueEvent(wxApp::GetInstance(), event);
}
