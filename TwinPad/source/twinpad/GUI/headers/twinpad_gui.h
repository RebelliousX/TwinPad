#pragma once

#include "fastCompile.h"

#include "wx/notebook.h"
#include "wx/animate.h"
#include <wx/spinctrl.h>

#include <vector>

#include "main.h"
// -----Don't change the Include's order for these two 
#include "functions_gui.h"
#include "comboGrid.h"
// -------------------------

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
// Don't define any IDs within these ranges...
// ID_BTN { 1000 to 1023 }, ID_BTN2 { 1024 to 1047 }, ID_TXT { 2000 to 2023 }, ID_LBL { 3000 to 3007 }
// ID_BTN: For Keyboard TAB, ID_BTN2: For Combos TAB
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
enum ID {	ID_BTN = 1000,
			ID_BTN2 = 1024,
			ID_BTN_OK = 1500, ID_BTN_CANCEL, ID_BTN_AUTO, ID_BTN_NULL,
			ID_BTN_NULL_MOUSE, ID_BTN_HELP_MOUSE, 
			ID_TXT = 2000, ID_TXT_WALKRUN = 2024, 
			ID_LBL = 3000, ID_LBL_WALKRUN = 3008, 
			ID_NOTEBOOK = 4000, 
			ID_PAD1_RADIOBTN = 5000, ID_PAD2_RADIOBTN, ID_PAD1_TAB2_RADIOBTN, ID_PAD2_TAB2_RADIOBTN,
			ID_CHK_PAD1 = 6000, ID_CHK_PAD2, ID_CHK_MOUSE, ID_CHK_COMBOS, ID_CHK_ON_FLY, ID_CHK_KEY_EVENTS,
			ID_CHK_HACK,
			ID_TIMER1 = 7000, ID_TIMER2, ID_TIMER3,
		};

// Purpose: to prevent TABs
class CPS_BTN : public wxBitmapButton
{
public:
	CPS_BTN(wxWindow *parent, wxWindowID id, const wxBitmap &bitmap, const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize, long style = 4L, const wxValidator &validator = wxDefaultValidator,
				const wxString &name = wxButtonNameStr) : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name) { }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }  // Prevent TAB traversing chain for buttons
};

class CPS_Anim : public wxAnimationCtrl
{
public:
	CPS_Anim() { }

	CPS_Anim(wxWindow *parent, wxWindowID id)
		: wxAnimationCtrl(parent, id, wxNullAnimation, wxDefaultPosition)
		{ }

	void OnClickAnimInKeyboardTab(wxCommandEvent &event);		// Keyboard tab
	void OnComboClick(wxCommandEvent &event);					// Combo tab

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } 

private:
	int index;
	wxString name;
};

class CPS_LBL : public wxStaticText
{
public:
	CPS_LBL(wxWindow *parent, wxWindowID id, const wxString &value, const wxSize &size = wxDefaultSize)
		: wxStaticText(parent, id, value, wxDefaultPosition, size) { }

	// void OnClick(wxCommandEvent &event);

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } // Not the value in text box

	void SetKeyCode(unsigned char key) { keyCode = key; }
private:
	int index;
	wxString name;
	unsigned char keyCode;		// Keyboard key value, Used for saving contents to file
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
		indexOfButton = -1;	// invalid value
	}

	~CTwinPad_Gui()
	{
		// When TwinPad exits, clean all dynamic memory to avoid memory leaks.
		// Note that CCombo destructor will call CAction's destructor, which in turn will delete all buttons.
		for (std::vector<CCombo *>::iterator it = Combos.begin(); it != Combos.end(); ++it)
			if (*it)
				delete *it;
		Combos.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		/* for (auto oneCombo : Combo)
			if (oneCombo)
				delete oneCombo;
		Combo.clear(); */
	}

	TwinPad_Frame *mainFrame;
	wxNotebook *noteBook;

	int indexOfButton;	
	
	// TAB 1: Keyboard
	CPS_LBL *lblCtrl[intPS_BUTTONS];					// Defined alias key
	CPS_Anim *animCtrl[intPS_BUTTONS];
	wxStaticText *lblLabel[intANALOG_DIRECTIONS];		// Left or Right (for analog stick)
	CPS_LBL *lblWalkRun;
	wxRadioButton *pad1RadioBtn;
	wxRadioButton *pad2RadioBtn;
	wxStaticText *lblEdit;
	wxButton *btnOK;
	wxButton *btnCancel;
	wxButton *btnAutoNavigate;
	wxButton *btnNullifiesAll;

	// TAB 2: Mouse
	wxStaticText *lblMouseLabel[intMOUSE_BUTTONS];
	wxComboBox *cmbMouseComboBox[intMOUSE_BUTTONS];
	wxRadioButton *mousePad1radioButton;
	wxRadioButton *mousePad2radioButton;
	wxButton *btnMouseNullifiesAll;
	wxButton *btnMouseHelp;
	wxComboBox *cmbMouseSensitivity;

	// TAB 3: COMBOs
	CComboGrid *virtualGrid;
	CTableBase *tableBaseGrid;
	std::vector<CCombo *> Combos;
	CPS_Anim *psComboButtons[24];
	wxButton *btnNewCombo;
	wxButton *btnDeleteCombo;
	wxButton *btnRenameCombo;
	wxButton *btnNewAction;
	wxButton *btnDeleteLastAction;
	wxButton *btnInsertActions;
	wxButton *btnInsertInbetweenAction;
	wxButton *btnDeleteSelectedActions;
	wxButton *btnDeleteButton;
	wxSpinCtrl *spnDefaultDelay;
	wxSpinCtrl *spnSensitivity;
	wxComboBox *cmbComboName;
	wxComboBox *cmbWhichPad;
	wxTextCtrl *txtComboKey;

	// for use when changing ComboBox selection
	wxString strPreviousComboSelection;

	// TAB 4: EXTRA
	wxCheckBox *chkDisablePad1;
	wxCheckBox *chkDisablePad2;
	wxCheckBox *chkDisableMouse;
	wxCheckBox *chkDisableCombos;
	wxCheckBox *chkDisableOnFlyKey;
	wxCheckBox *chkDisableKeyEvents;
	wxCheckBox *chkEnableHack;

	// TAB 3: GamePad (nothing here yet)

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

enum TAB_INDEX { KEYBOARD_TAB, MOUSE_TAB, COMBOS_TAB, MISC_TAB, GAMEPAD_TAB };
enum class PS2BUTTON { L2, R2, L1, R1, TRIANGLE, CIRCLE, CROSS, SQUARE, SELECT, L3, R3,
				START, UP, RIGHT, DOWN, LEFT, LANALOG_UP, LANALOG_RIGHT, LANALOG_DOWN,
				LANALOG_LEFT, RANALOG_UP, RANALOG_RIGHT, RANALOG_DOWN, RANALOG_LEFT };

void CreateControls(TwinPad_Frame * window);
void AddKeyboardTab(CTwinPad_Gui &GUI_Controls);
void AddMouseTab(CTwinPad_Gui &GUI_Controls);
void AddGamePadTab(CTwinPad_Gui &GUI_Controls);
void AddCombosTab(CTwinPad_Gui &GUI_Controls);
void AddMiscTab(CTwinPad_Gui &GUI_Controls);

void SetupComboTab(wxPanel *panel);

// To handle right click on configured button's Name (delete configuration)
void OnLblCtrlRightClick(wxMouseEvent &ev);
// To handle Left-Click assign a key, Right-Click remove assigned key
void OnClickWalkRun(wxMouseEvent &ev);
