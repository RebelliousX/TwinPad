//Test wxWidgets, TwinPad

#include "twinpad/twinpad_gui.h"

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/filesys.h"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

DECLARE_APP(MyApp)

class MyFrame : public wxFrame
{
public:
	MyFrame(wxString title, wxSize size) : wxFrame(0, wxID_ANY, title, wxDefaultPosition, size,
		wxMINIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX) {}
};

bool MyApp::OnInit()
{
	MyFrame *myFrame = new MyFrame("TwinPad Configuration Utility", wxDefaultSize);
	//This handler should be created once in OnInit() as documentation says
	wxInitAllImageHandlers();

	CreateControls(myFrame);
	
	myFrame->SetSize(myFrame->GetMinSize());
	
	myFrame->Show();

	return true;
}
