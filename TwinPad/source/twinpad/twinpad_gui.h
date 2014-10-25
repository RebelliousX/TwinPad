#ifndef _TWINPAD_GUI_H_
#define _TWINPAD_GUI_H_

//Set to 0 if you want to use wx.h instead of precompiled header
#define WX_PRECOM 1

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/notebook.h"
#include "wx/animate.h"
#include <wx/spinctrl.h>
#include "comboGrid.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Don't define any IDs within these ranges...
//ID_BTN { 1000 to 1023 }, ID_BTN2 { 1024 to 1047 }, ID_TXT { 2000 to 2023 }, ID_LBL { 3000 to 3007 }
//ID_BTN: For Keyboard TAB, ID_BTN2: For Combos TAB
//////////////////////////////////////////////////////////////////////////////////////////////////////
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
		};

//Purpose: to prevent TABs
class CPS_BTN : public wxBitmapButton
{
public:
	CPS_BTN(wxWindow *parent, wxWindowID id, const wxBitmap &bitmap, const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize, long style = 4L, const wxValidator &validator = wxDefaultValidator,
				const wxString &name = wxButtonNameStr) : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name) { }
	virtual bool AcceptsFocusFromKeyboard() const { return false; }  //Prevent TAB traversing chain for buttons
};

class CPS_Anim : public wxAnimationCtrl
{
public:
	CPS_Anim() { }

	CPS_Anim(wxWindow *parent, wxWindowID id)
		: wxAnimationCtrl(parent, id, wxNullAnimation, wxDefaultPosition)
		{ }

	void OnClick(wxCommandEvent &event);		//Keyboard tab
	void OnComboClick(wxCommandEvent &event);	//Combo tab

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } 

private:
	int index;
	wxString name;
};

class CPS_Txt : public wxTextCtrl
{
public:
	CPS_Txt(wxWindow *parent, wxWindowID id, const wxString &value, const wxSize &size = wxDefaultSize)
		: wxTextCtrl(parent, id, value, wxDefaultPosition, size) { }

	//void OnClick(wxCommandEvent &event);

	void SetIndex(int i) { index = i; }
	int GetIndex() { return index; }

	void SetName(const wxString &str) { name = str; }
	wxString GetName() { return name; } //Not the value in text box

private:
	int index;
	wxString name;
};

class CCombo;

//define controls
const int intMOUSE_BUTTONS = 10, intPS_BUTTONS = 24, intANALOG_DIRECTIONS = 8;

class CTwinPad_Gui
{
public:
	wxNotebook *noteBook;
	/////////TAB 1: Keyboard//////////////////////////////////////////////
	CPS_Txt *txtCtrl[intPS_BUTTONS];		//Defined alias key
	CPS_Anim *animCtrl[intPS_BUTTONS];
	wxStaticText *lblLabel[intANALOG_DIRECTIONS];		//Left or Right (for analog stick)
	CPS_Txt *txtWalkRun;
	wxRadioButton *pad1RadioBtn;
	wxRadioButton *pad2RadioBtn;
	wxStaticText *lblEdit;
	wxButton *btnOK;
	wxButton *btnCancel;
	wxButton *btnAutoNavigate;
	wxButton *btnNullifiesAll;
	wxTimer *tmrGetKey;
	wxTimer *tmrAutoNav;

	/////////TAB 2: Mouse//////////////////////////////////////////////
	wxStaticText *lblMouseLabel[intMOUSE_BUTTONS];
	wxComboBox *cmbMouseComboBox[intMOUSE_BUTTONS];
	wxRadioButton *mousePad1radioButton;
	wxRadioButton *mousePad2radioButton;
	wxButton *btnMouseNullifiesAll;
	wxButton *btnMouseHelp;
	wxComboBox *cmbMouseSensitivity;

	/////////TAB 3: COMBOs//////////////////////////////////////////////
	CComboGrid *virtualGrid;
	CTableBase *tableBaseGrid;
	std::vector<CCombo> Combo;
	CPS_Anim *ps2buttons[24];
	wxButton *btnNewCombo;
	wxButton *btnDeleteCombo;
	wxButton *btnRenameCombo;
	wxButton *btnNewAction;
	wxButton *btnDeleteLastAction;
	wxButton *btnInsertAction;
	wxButton *btnDeleteSelectedActions;
	wxSpinCtrl *spnDefaultDelay;
	wxComboBox *cmbComboName;
	wxTextCtrl *txtComboKey;

	/////////TAB 4: EXTRA///////////////////////////////////////////////
	wxCheckBox *chkDisablePad1;
	wxCheckBox *chkDisablePad2;
	wxCheckBox *chkDisableMouse;
	wxCheckBox *chkDisableCombos;
	wxCheckBox *chkDisableOnFlyKey;
	wxCheckBox *chkDisableKeyEvents;
	wxCheckBox *chkEnableHack;

	/////////TAB 3: GamePad//////////////////////////////////////////////

	////////TAB Sizes///////////////////////////////////////////////
	wxSize minWinSize[5];
};

enum TAB_INDEX { KEYBOARD_TAB, MOUSE_TAB, COMBOS_TAB, MISC_TAB, GAMEPAD_TAB };
enum PS2BUTTON { L2, R2, L1, R1, TRIANGLE, CIRCLE, CROSS, SQUARE, SELECT, L3, R3,
				START, UP, RIGHT, DOWN, LEFT, LANALOG_UP, LANALOG_RIGHT, LANALOG_DOWN,
				LANALOG_LEFT, RANALOG_UP, RANALOG_RIGHT, RANALOG_DOWN, RANALOG_LEFT };

void CreateControls(wxWindow * window);
void AddKeyboardTab(CTwinPad_Gui &GUI_Controls);
void AddMouseTab(CTwinPad_Gui &GUI_Controls);
void AddGamePadTab(CTwinPad_Gui &GUI_Controls);
void AddCombosTab(CTwinPad_Gui &GUI_Controls);
void AddMiscTab(CTwinPad_Gui &GUI_Controls);

void SetupComboTab(wxPanel *panel);

#endif