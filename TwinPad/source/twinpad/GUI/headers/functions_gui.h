#pragma once

#include <string>
#include <vector>

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/grid.h"

class CCellValue
{
public:
	wxString buttonName;	// Button name (from column 1-18). Delay value column 0
	int buttonValue;		// for buttons, 0-23
	int buttonSensitivity;	// button sensitivity, from column 1-18
};

class CAction
{
public:
	CAction() { }
	~CAction()
	{
		for (std::vector<CCellValue *>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
			if (*it)
				delete *it;
		m_buttons.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		/* for (auto button : m_buttons)
			if (button)
				delete button;
		m_buttons.clear(); */
	}

	// Get the number of Buttons in the current Action
	int GetSize(const CAction &act) { return (int)m_buttons.size(); }
	// Set Delay for current Action
	void SetDelay(int delay) { m_delay = delay; }
	// Returns the Delay value of current Action
	int  GetDelay() { return m_delay; }
	// Add a Button to the current Action
	void AddButton(CCellValue *button) 
	{
		// Note that the destructor will delete tempButton, we pushed the pointer of CCellValue into a vector
		// and the destructor will delete by iterating through all of them when the object is destroyed
		CCellValue *tempButton = new CCellValue;
		tempButton->buttonName = button->buttonName;
		tempButton->buttonSensitivity = button->buttonSensitivity;
		tempButton->buttonValue = button->buttonValue;
		m_buttons.push_back(tempButton);
	}
	// Returns a Button at the specified index
	CCellValue * GetButton(int index) { return m_buttons[index]; }
	// Returns the number of Buttons in the current Action
	int GetNumberOfButtons() { return m_buttons.size(); }
	// Delete all buttons in the Current Action
	void DeleteAllButtons() { m_buttons.clear(); m_delay = 0; }

private:
	unsigned int m_delay;						// Delay value of the Action
	std::vector<CCellValue *> m_buttons;		// vector for buttons (up to 18 elements)
};

class CCombo
{
public:
	CCombo() { }
	// new combo consists of 1 action which has 1 delay value and 0 buttons
	CCombo(int numActions, int defaultDelay)
	{
		CAction *action = new CAction;
		action->SetDelay(defaultDelay);
		for (int i = 0; i < numActions; ++i)
			AddAction(action);
	}

	~CCombo()
	{
		for (std::vector<CAction *>::iterator it = m_actions.begin(); it != m_actions.end(); ++it)
			if (*it)
				delete *it;
		m_actions.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		/* for (auto action : m_actions)
			delete action;
			m_actions.clear(); */
	}

	CAction * GetAction(int number) const { return m_actions[number]; }	// caller have to check for valid number
	int GetNumberActions() const { return (int) m_actions.size(); }
	int GetKey() const { return m_key; }
	void SetKey(int key) { m_key = key; }
	void SetName(const wxString &name) { m_comboName = name; }
	const wxString& GetName() const { return m_comboName; }

	// Add Action at the end
	void AddAction(CAction *action)
	{
		CAction *tempAction = new CAction();
		tempAction->SetDelay(action->GetDelay());
		for (int i = 0; i < action->GetNumberOfButtons(); ++i)
			tempAction->AddButton(action->GetButton(i));
		m_actions.push_back(tempAction);
	}
	
	// Delete last action
	void DeleteAction()
	{
		int ix = m_actions.size();
		m_actions.resize(ix - 1);
	}

private:
	std::vector<CAction *> m_actions;	// vector of pointers to Actions
	int m_key;							// holds assigned key for current combo
	wxString m_comboName;				// current combo name
};

class GUI_Configurations
{
public:
	int m_pad[2][25];
	int m_mouse[10];
	int m_extra[7];
	int m_mouseAsPad;
	int m_mouseSensitivity;
};

bool IsFileOkAndFix(const std::string &file, const std::string &header);
void CreateNullFile();
void CreateNullComboFile();
void Loading_TwinPad_Main_Config();

// Forward declaration is necessary, to avoid problems
class CPS_Anim;
void LoadResources(CPS_Anim *animCtrl, const unsigned int index);
// used by LoadResources() and SetCustomCellRenderer()
// iData: Image data, length: size of image in bytes, index: image index
void GetImageData(void* &iData, unsigned int *length, const unsigned int index);

// Action Events
void OnClickNewAction(wxCommandEvent &ev);
void OnClickDeleteLastAction(wxCommandEvent &ev);
void OnClickInsertInbetweenAction(wxCommandEvent &ev);
void OnClickInsertAction(wxCommandEvent &ev);
void OnClickDeleteSelectedActions(wxCommandEvent &ev);
void OnClickDeleteButton(wxCommandEvent &ev);
// Combo Events
void OnClickNewCombo(wxCommandEvent &ev);
void OnClickDeleteCombo(wxCommandEvent &ev);
void OnClickRenameCombo(wxCommandEvent &ev);
// ComboBox Events
void OnChangeComboName(wxCommandEvent &ev);
void OnChangeComboNameKey(wxKeyEvent &ev);
// Combo Key
bool Has(const unsigned int button, int row);
void OnClickComboKey(wxMouseEvent &ev);
// psButtons
void OnClick_psComboButtons(int winID);
// Grid Events
void OnClickComboGrid(wxGridEvent &ev);
void OnMouseMoveOverGrid(wxMouseEvent &ev);
// SpinCtrl Events
void OnChangeSensitivity(wxSpinEvent &ev);
// Mouse Events
void OnClickMouseHelpButton(wxCommandEvent &ev);
void OnClickMouseNullifiesAll(wxCommandEvent &ev);
// Modify sensitivity inside spin control
void ModifySensitivity();
// Save the Grid to the Combos container
void SaveGridToCombo(wxString &strUserInput);
// Buttons (Cancel, Ok, 'Nullifies All' for Keyboard and Auto Navigate) Events
void OnClickKeyboardNullifiesAll(wxCommandEvent &ev);
void OnClickAutoNavigate(wxCommandEvent &ev);
void OnClickCancel(wxCommandEvent &ev);
void OnClickOk(wxCommandEvent &ev);