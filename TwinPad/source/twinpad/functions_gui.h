#pragma once

#include <string>
#include <vector>

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/grid.h"

//Forward declaraton is important, I can't include 'comboGrid.h' cyclic dependency
class CCellValue;

class CAction
{
public:
	CAction() { }

	//Get the number of Buttons in the current Action
	int GetSize(const CAction &act) { return (int)m_buttons.size(); }
	//Set Delay for current Action
	void SetDelay(int delay) { m_delay = delay; }
	//Returns the Delay value of current Action
	int  GetDelay() { return m_delay; }
	//Add a Button to the current Action
	void AddButton(CCellValue *button) { m_buttons.push_back(button); }
	//Returns a Button at the specified index
	CCellValue * GetButton(int index) { return m_buttons[index]; }
	//Returns the number of Buttons in the current Action
	int GetNumberOfButtons() { return m_buttons.size(); }
	//Delete all buttons in the Current Action
	void DeleteAllButtons() { m_buttons.clear(); m_delay = 0; }

private:
	unsigned int m_delay;						//Delay value of the Action
	std::vector<CCellValue *> m_buttons;		//vector for buttons (up to 18 elements)
};

class CCombo
{
public:
	//new combo consists of 1 action which has 1 delay value and 0 buttons
	CCombo(int numActions, int defaultDelay) 
	{
		CAction action;
		action.SetDelay(defaultDelay);
		for (int i = 0; i < numActions; ++i)
			AddAction(&action);
	}

	int GetNumberActions() const { return (int) m_actions.size(); }
	int GetKey() const { return m_key; }
	void SetKey(int key) { m_key = key; }
	void SetName(const wxString &name) { m_comboName = name; }
	const wxString& GetName() const { return m_comboName; }

	//Add Action at the end
	void AddAction(CAction *action)
	{
		int ix = m_actions.size();
		m_actions.resize(ix + 1);
		m_actions[ix].SetDelay(action->GetDelay());
		for (int i = 0; i < action->GetNumberOfButtons(); ++i)
			m_actions[ix].AddButton(action->GetButton(i));
	}
	
	//Delete last action
	void DeleteAction()
	{
		int ix = m_actions.size();
		m_actions.resize(ix - 1);
	}

private:
	std::vector<CAction> m_actions;		//vector for Actions
	int m_key;							//will hold assigned key for current combo
	wxString m_comboName;				//current combo name
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

bool CheckAndCreateIfNecessary(const std::string &file, const std::string &header);
void CreateNullFile();
void CreateNullComboFile();
void Loading_TwinPad_Main_Config();

//Forward declaration is necessary, to avoid problems
class CPS_Anim;
void LoadResources(CPS_Anim *animCtrl, int index);
//used by LoadResources() and SetCustomCellRenderer()
//iData: Image data, length: size of image in bytes, index: image index
void GetImageData(void* &iData, unsigned int *length, const unsigned int index);

////Action Events
void OnClickNewAction(wxCommandEvent &ev);
void OnClickDeleteLastAction(wxCommandEvent &ev);
void OnClickInsertInbetweenAction(wxCommandEvent &ev);
void OnClickInsertAction(wxCommandEvent &ev);
void OnClickDeleteSelectedActions(wxCommandEvent &ev);
void OnClickDeleteButton(wxCommandEvent &ev);
/////Combo Events
void OnClickNewCombo(wxCommandEvent &ev);
void OnClickDeleteCombo(wxCommandEvent &ev);
void OnClickRenameCombo(wxCommandEvent &ev);
/////Combo Key
bool Has(const unsigned int button, int row);
void OnClickComboKey(wxMouseEvent &ev);
/////psButtons
void OnClick_psComboButtons(int winID);
/////Grid Events
void OnClickComboGrid(wxGridEvent &ev);
void OnMouseMoveOverGrid(wxMouseEvent &ev);
////SpinCtrl Events
void OnChangeSensitivity(wxSpinEvent &ev);
/////Mouse Events
void OnClickMouseHelpButton(wxMouseEvent &ev);
void OnClickMouseNullifiesAll(wxMouseEvent &ev);
////Modify sensitivity inside spin control
void ModifySensitivity();