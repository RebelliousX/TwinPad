#pragma once

#include "main.h"
// -----Don't change the Include's order for these two 
#include "functions_gui.h"
#include "comboGrid.h"
// -------------------------

// Purpose: to prevent TABs
class CPS_BTN : public wxBitmapButton
{
public:
	CPS_BTN(wxWindow *parent, wxWindowID id, const wxBitmap &bitmap, const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize, long style = 4L, const wxValidator &validator = wxDefaultValidator,
				const wxString &name = wxButtonNameStr) : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name) { }
	// Disable tab traversal
	virtual bool AcceptsFocus() const { return false; }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }
};

class CPS_Anim : public wxAnimationCtrl
{
public:
	CPS_Anim() { }

	CPS_Anim(wxWindow *parent, wxWindowID id)
		: wxAnimationCtrl(parent, id, wxNullAnimation, wxDefaultPosition)
		{ }

	void OnClickAnimInKeyboardTab(wxMouseEvent &event);			// Keyboard tab

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } 

	// Disable tab traversal
	virtual bool AcceptsFocus() const { return false; }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }

private:
	int index;
	wxString name;
};

class CPS_LBL : public wxStaticText
{
public:
	CPS_LBL(wxWindow *parent, wxWindowID id, const wxString &title, const wxSize &size = wxDefaultSize)
		: wxStaticText(parent, id, title, wxDefaultPosition, size) { }

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } // Not the value in text box

	void SetKeyCode(unsigned char key) { keyCode = key; }
	unsigned char GetKeyCode() { return keyCode; }
private:
	int index;
	wxString name;
	unsigned char keyCode;		// Keyboard key value, Used for saving contents to file
};

class CButton : public wxButton
{
public:
	CButton(wxWindow *parent, wxWindowID id, const wxString &title) : wxButton(parent, id, title)
	{
		Connect(wxEVT_KEY_DOWN, wxCommandEventHandler(CButton::DontAcceptKeyEvent));
		Connect(wxEVT_KEY_UP, wxCommandEventHandler(CButton::DontAcceptKeyEvent));
		Connect(wxEVT_ANY, wxEventHandler(CButton::DontAcceptKeyEvent));
	}

	// Disable tab traversal
	virtual bool AcceptsFocus() const { return false; }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }
	
	void DontAcceptKeyEvent(wxCommandEvent &ev)
	{
		if (ev.GetEventType() == wxEVT_KEY_DOWN || ev.GetEventType() == wxEVT_KEY_UP)
		{
			//this->GetParent()->SetFocus();
			ev.StopPropagation();
			return;
		}
		ev.Skip();
	}

	void DontAcceptKeyEvent(wxEvent &ev)
	{
		if (((wxKeyEvent&)ev).GetKeyCode() == WXK_RETURN || ((wxKeyEvent&)ev).GetKeyCode() == WXK_SPACE)
		{
			return;
		}
		ev.Skip();
	}
};

class CRadButton : public wxRadioButton
{
public:
	CRadButton(wxWindow *parent, wxWindowID id, const wxString &title) : wxRadioButton(parent, id, title)
	{
		Connect(wxEVT_KEY_UP, wxKeyEventHandler(CRadButton::DisableKeyboard));
		Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CRadButton::DisableKeyboard));
	}
	// Disable tab traversal
	virtual bool AcceptsFocus() const { return false; }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }
private:
	void DisableKeyboard(wxKeyEvent &ev) { return; }
};

class CNotebook : public wxNotebook
{
public:
	CNotebook(wxWindow *parent, wxWindowID id) : wxNotebook(parent, id) { }
};

// define controls
const int intMOUSE_BUTTONS = 10, intPS_BUTTONS = 24, intANALOG_DIRECTIONS = 8;

class CTwinPad_Gui
{
public:
	CTwinPad_Gui()
	{
		HEADER_TWINPAD = "[TwinPad Configurations v1.6]";
		HEADER_TWINPAD_COMBO = "[TwinPad COMBO Configurations v1.1]";
		TWIN_PAD = "TwinPad.ini";
		TWIN_PAD_COMBOS = "TwinPad_COMBOs.ini";
		PATH_DIR = "inis/";		// Will be replaced with a proper directory if supported by the emu using PADsettingsDir()
		indexOfButton = -1;		// invalid value
		curTab = 0;
	}

	void Clean()
	{
		indexOfButton = -1;
		curTab = 0;
		strPreviousComboSelection.Clear();
	}

	TwinPad_Frame *mainFrame;
	CNotebook *Notebook;

	int indexOfButton;	
	int curTab;

	// TAB 1: Keyboard
	CPS_LBL *lblCtrl[intPS_BUTTONS];					// Defined alias key
	CPS_Anim *animCtrl[intPS_BUTTONS];
	wxStaticText *lblLabel[intANALOG_DIRECTIONS];		// Left or Right (for analog stick)
	CPS_LBL *lblWalkRun;
	CRadButton *pad1RadioBtn;
	CRadButton *pad2RadioBtn;
	wxStaticText *lblEdit;
	CButton *btnOK;
	CButton *btnCancel;
	CButton *btnAutoNavigate;
	CButton *btnNullifiesAll;

	// TAB 2: Mouse
	wxStaticText *lblMouseLabel[intMOUSE_BUTTONS];
	wxComboBox *cmbMouseComboBox[intMOUSE_BUTTONS];
	CRadButton *mousePad1radioButton;
	CRadButton *mousePad2radioButton;
	CButton *btnMouseNullifiesAll;
	CButton *btnMouseHelp;
	wxComboBox *cmbMouseSensitivity;

	// TAB 3: COMBOs
	CComboGrid *virtualGrid;
	CTableBase *tableBaseGrid;
	
	CPS_Anim *psComboButtons[24];
	CButton *btnNewCombo;
	CButton *btnDeleteCombo;
	CButton *btnRenameCombo;
	CButton *btnNewAction;
	CButton *btnDeleteLastAction;
	CButton *btnInsertActions;
	CButton *btnInsertInbetweenAction;
	CButton *btnDeleteSelectedActions;
	CButton *btnDeleteButton;
	wxSpinCtrl *spnDefaultDelay;
	wxSpinCtrl *spnSensitivity;
	wxComboBox *cmbComboName;
	wxComboBox *cmbWhichPad;
	wxStaticText *lblComboKey;
	// for use when changing ComboBox selection
	wxString strPreviousComboSelection;

	// TAB 4: EXTRA
	wxCheckBox *chkDisablePad1;
	wxCheckBox *chkDisablePad2;
	wxCheckBox *chkDisableMouse;
	wxCheckBox *chkDisableCombos;
	wxCheckBox *chkDisableOnFlyKey;
	wxCheckBox *chkDisableKeyEvents;

	// TAB #?: GamePad (nothing here yet)

	// TAB Sizes (stores minimum window size for each tab)
	wxSize minWinSize[5];

	// Files header names, Files names and Settings directory
	void SetSettingsPath(wxString dir) { PATH_DIR = dir; }
	wxString GetTwinPad_Header() { return HEADER_TWINPAD; }
	wxString GetTwinPad_ComboHeader() { return HEADER_TWINPAD_COMBO; }
	wxString GetTwinPad_FileName()	{ return TWIN_PAD; }
	wxString GetTwinPad_ComboFileName() { return TWIN_PAD_COMBOS; }
	wxString GetSettingsPath() { return PATH_DIR; }
private:
	// Some strings
	wxString HEADER_TWINPAD;
	wxString HEADER_TWINPAD_COMBO;
	wxString TWIN_PAD;
	wxString TWIN_PAD_COMBOS;
	wxString PATH_DIR;
};

extern CTwinPad_Gui GUI_Controls;
extern MainConfigurations Configurations;

void CreateControls(TwinPad_Frame * window);
void AddKeyboardTab();
void AddMouseTab();
void AddGamePadTab();
void AddCombosTab();
void AddMiscTab();

void SetupComboTab(wxPanel *panel);
