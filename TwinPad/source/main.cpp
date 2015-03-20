// Test wxWidgets, TwinPad

#include "twinpad_gui.h"
#include "main.h"

 #ifndef __WINDOWS__
	#error "Currently TwinPad is Windows-only"
 #endif

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/app.h"
#include "Externals.h"

#ifdef _WINDOWS

namespace
{
	// Thanks to https://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
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

HMODULE hDI = NULL;						// DLL handle, passed to DirectInput
HWND hObjWnd = NULL, GShwnd = NULL;		// Both for getting HWND from GPU/GS window
HWND hEmuWnd;							// Hackish, get handle to emulator, since wxDialog can't have win32 parent

// processID for the emulator, used to find its main Window, to deal with wxDialog shortcomings.
int processId = NULL;

class TwinPad_Frame : public wxDialog
{
public:
	TwinPad_Frame(wxString title, wxSize size) 
		: wxDialog(0, wxID_ANY, title, wxDefaultPosition, size,
		wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP)
	{
		hEmuWnd = find_main_window(processId);
		
		CreateControls(this);

		SetSize(GetMinSize());
		SetPosition(wxPoint(200, 20));

		Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TwinPad_Frame::OnClose));
	}

	void OnClose(wxCloseEvent &event)
	{
		SetForegroundWindow(hEmuWnd);
		ShowWindow(hEmuWnd, SW_RESTORE);
		event.Skip();
	}

	void TwinPad_Configuration()
	{
		ShowModal();
		Destroy();
	}
};

class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL() { }
	bool OnInit()
	{
		// If different images support is needed, either add 
		
		//wxInitAllImageHandlers();
		
		// ... or one or more image handlers of the following individually,
		// I listed two here but there is more....

		//wxImage::AddHandler(new wxPNGHandler);
		//wxImage::AddHandler(new wxICOHandler); //use only when needed, otherwise big.exe's
	
		return true;
	}
};

TwinPad_Frame *twinPad_Frame;

void ConfigureTwinPad()
{
	twinPad_Frame = new TwinPad_Frame("TwinPad Configuration Utility", wxDefaultSize);
	twinPad_Frame->TwinPad_Configuration();
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