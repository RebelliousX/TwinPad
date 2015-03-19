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

#include "Externals.h"

HMODULE hDI = NULL;
HWND hObjWnd = NULL, GShwnd = NULL;

class TwinPad_Frame : public wxDialog
{
public:
	TwinPad_Frame(wxString title, wxSize size) : wxDialog(0, wxID_ANY, title, wxDefaultPosition, size,
		wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
	{
		wxInitAllImageHandlers();

		CreateControls(this);

		SetSize(GetMinSize());
		SetPosition(wxPoint(200, 20));
	}

	void TwinPad_Configuration()
	{
		wxDialog *f = NULL;	
		f = new TwinPad_Frame("TwinPad Configuration Utility", wxDefaultSize);
		
		// Window is destroyed after EndModal() to reclaim memory :)
		f->ShowModal();
	}
};


class TwinPad_DLL : public wxApp
{
public:
	TwinPad_DLL() { }
};

TwinPad_Frame *twinPad_Frame;

IMPLEMENT_APP_NO_MAIN(TwinPad_DLL)

void ConfigureTwinPad()
{
	twinPad_Frame = new TwinPad_Frame("TwinPad Configuration Utility", wxDefaultSize);
	twinPad_Frame->TwinPad_Configuration();
	delete twinPad_Frame;
	twinPad_Frame = 0;
}

#ifdef _WINDOWS
	BOOL APIENTRY DllMain(HANDLE hModule, 
		DWORD  dwReason,
		LPVOID lpReserved) {
		hDI = (HMODULE)hModule;		// Handle for DirectInput
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
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
			wxTheApp->ExitMainLoop();
			break;
		}
		return TRUE;
	}
#endif