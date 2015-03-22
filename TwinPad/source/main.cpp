// Test wxWidgets, TwinPad
#include "fastCompile.h"

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/app.h"
#include "Externals.h"

#include "main.h"
#include "twinpad_gui.h"

#ifndef __WINDOWS__
	#error "Currently TwinPad is Windows-only"
#endif

#ifdef _WINDOWS

#include "DirectInput.h"

namespace
{
	// Thanks to https:// stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
	struct handle_data 
	{
		unsigned long process_id;
		HWND best_handle;
	};

	BOOL is_main_window(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	}

	BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		unsigned long process_id = 0;
		GetWindowThreadProcessId(handle, &process_id);
		if (data.process_id != process_id || !is_main_window(handle)) 
			return TRUE;
		
		data.best_handle = handle;
		return FALSE;
	}

	HWND find_main_window(unsigned long process_id)
	{
		handle_data data;
		data.process_id = process_id;
		data.best_handle = 0;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.best_handle;
	}
}
#endif

// ----------------------------------------------------------------
// Very important handles, used everywhere, even for DirectInput   |
// ----------------------------------------------------------------

HMODULE hDI = NULL;							// DLL handle, passed to DirectInput
HWND hGFXwnd = NULL, hGSPUwnd = NULL;		// Both for getting HWND from GPU/GS window
HWND hEmuWnd;								// Hackish, get handle to emulator, since wxDialog can't have win32 parent

// processID for the emulator, used to find its main Window, to deal with wxDialog shortcomings.
int processId = NULL;

TwinPad_Frame::TwinPad_Frame(wxString title) : wxDialog(0, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
	wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP)
{
	hEmuWnd = find_main_window(processId);

	tmrAnimate = new CReAnimate(this);
	tmrAutoNavigate = new CAutoNav(this);
	tmrGetKey = new CGetKey(this);
	tmrGetComboKey = new CGetComboKey(this);
	tmrGetLoadComboKey = new CGetLoadComboKey(this);

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
	delete tmrGetLoadComboKey;
	tmrGetLoadComboKey = 0;
}

void TwinPad_Frame::OnClose(wxCloseEvent &event)
{
	SetForegroundWindow(hEmuWnd);
	ShowWindow(hEmuWnd, SW_RESTORE);
	event.Skip();
}

class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL() { }
	bool OnInit() { return true; }
};


// This is called from PADconfigure which in turn called from the emu
void ConfigureTwinPad()
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
	hGFXwnd = (HWND) twinPad_Frame->GetHWND();
#ifdef _WINDOWS
	// Terminate DI, if successfully terminated, then it was running before and user called TwinPad
	// Config using the Hot key
	bool wasDI_Running = TermDI();
	if (!InitDI())
	{
		wxMessageBox("Can't Initialize DirectInput!", "Failure...", wxICON_ERROR);
		return;
	}
#endif

	twinPad_Frame->ShowModal();

#ifdef _WINDOWS
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
}

IMPLEMENT_APP_NO_MAIN(TwinPad_DLL)

#ifdef _WINDOWS
	BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
	{
		hDI = (HMODULE)hModule;
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			processId = GetCurrentProcessId();
			wxSetInstance((HINSTANCE)hModule);
			wxEntryStart(0, 0);
			if (!wxTheApp || !wxTheApp->CallOnInit())
				return FALSE;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			wxEntryCleanup();
			break;
		}
		return TRUE;
	}
#endif