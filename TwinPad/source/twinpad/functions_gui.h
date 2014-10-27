#pragma once

#include <string>
#include <vector>

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

class CAction
{
public:
	CAction() : m_marked4Deletion(false)
	{ }

	int GetSize(const CAction &act) { return (int) m_buttons.size(); }
	
	void SetToDelete() { m_marked4Deletion = true; }
	bool GetToDelete() const { return m_marked4Deletion; }

	void InitAction(CAction &act, int defaultDelay)
	{
		for(int i = 0; i < GetSize(act); ++i)
			if (i == 0) 
				m_buttons[0] = defaultDelay;
			else 
				m_buttons[i] = -1;	//-1 is sentinel value, 0 == L2 for PS2 pad.
	}

	//void AddButton(const unsigned int button)
	//{
	//	//verify button does not conflict with other buttons in current action
	//	//e.g UP and DOWN at the same time. So, 6 if-clauses get rid of 6 unneeded
	//	//columns in grid from 24 to 18, unlike before.
	//	if( (button == UP && Has(DOWN)) || (button == DOWN && Has(UP)) )
	//	{
	//		wxMessageBox("Can't have both UP and DOWN in the same Action.",
	//					 "Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}
	//	if( (button == RIGHT && Has(LEFT)) || (button == LEFT && Has(RIGHT)) )
	//	{
	//		wxMessageBox("Can't have both LEFT and RIGHT in the same Action.",
	//					"Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}
	//	if( (button == LANALOG_UP && Has(LANALOG_DOWN)) || (button == LANALOG_DOWN && Has(LANALOG_UP)) )
	//	{
	//		wxMessageBox("Can't have both Left Analog's UP and DOWN in the same Action.",
	//					"Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}
	//	if( (button == LANALOG_LEFT && Has(LANALOG_RIGHT)) || (button == LANALOG_RIGHT && Has(LANALOG_LEFT)) )
	//	{
	//		wxMessageBox("Can't have both Left Analog's LEFT and RIGHT in the same Action.",
	//					"Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}
	//	if( (button == RANALOG_UP && Has(RANALOG_DOWN)) || (button == RANALOG_DOWN && Has(RANALOG_UP)) )
	//	{
	//		wxMessageBox("Can't have both Right Analog's UP and DOWN in the same Action.",
	//					"Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}
	//	if( (button == RANALOG_LEFT && Has(RANALOG_RIGHT)) || (button == RANALOG_RIGHT && Has(RANALOG_LEFT)) )
	//	{
	//		wxMessageBox("Can't have both Right Analog's LEFT and RIGHT in the same Action.",
	//					"Not Allowed!", wxICON_INFORMATION);
	//		return;
	//	}

	//	//All is good, then we add the button
	//	m_buttons.push_back(button);

	//	/*
	//	//All is good, then we add the button. Note that we can't use push_back since an action 
	//	//can contain empty buttons "-1" as their value (to match grid), so verify where to add it
	//	for(int i = 1; i < 19; ++i)
	//		if(m_buttons[i] == -1)
	//		{
	//			m_buttons[i] = button;	//add at the first empty location and return
	//			return;
	//		}
	//		*/
	//}

private:
	std::vector<int> m_buttons; //vector for integers or buttons (upto 19 elements)
	bool m_marked4Deletion;

private:
	bool Has(const unsigned int button)
	{
		for(int i = 1; i < 19; ++i)
			if(m_buttons[i] == button)
				return true;
	}
};

class CCombo
{
public:
	//new combo consists of 1 action which has 1 delay value and 0 buttons
	CCombo(int numActions, int defaultDelay) { AddAction(defaultDelay); }

	int GetNumberActions() const { return (int) m_actions.size(); }
	int GetKey() const { return m_key; }
	void SetKey(int key) { m_key = key; }
	void SetName(const wxString &name) { m_comboName = name; }
	const wxString& GetName() const { return m_comboName; }

	void AddAction(int defaultDelay)	//At last action
	{
		int ix = m_actions.size();
		m_actions.resize(ix + 1);
		m_actions[ix].InitAction(m_actions[ix], defaultDelay);
	}
	
	void DeleteAction()					//Last action
	{
		int ix = m_actions.size();
		m_actions.resize(ix - 1);
	}

	void InsertActions(int position, int numActions, int delay)	//needs extensive testing
	{
		if(position == m_actions.size())
		{
			AddAction(delay);
			return;
		}

		CAction temp;
		temp.InitAction(temp, delay);
		std::vector<CAction>::iterator it;
		it = m_actions.begin();

		m_actions.insert(it + position, numActions, temp);
	}

	//receive list of actions to be deleted (selected from grid), not necessarily consecutive actions
	void DeleteActions(std::vector<int> &listActions)	//needs extensive testing
	{
		//If we delete an element, the container size will change, thus remaining elements to be deleted
		//can't be found (their positions in "m_actions" changed), thus listActions will be useless.
		//First we have to mark actions in m_actions for deletion from listActions then delete them.
		for(std::vector<int>::iterator it = listActions.begin(); it < listActions.end(); ++it)
			m_actions[*it].SetToDelete();
		
		int remaining = listActions.size();

		while(remaining)
		{
			for(unsigned int i = 0; i < m_actions.size(); ++i)	//unsigned to remove compiler warning
				if((m_actions[i].GetToDelete()))
				{
					m_actions.erase(m_actions.begin() + i);
					--remaining;
					break;	//condition & size changed, exit the for loop early and start all over till nothing remains
				}
		}
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
void Loading_Gui();

//Forward declaration is necessary, to avoid problems
class CPS_Anim;
void LoadResources(CPS_Anim *animCtrl, int index);
//used by LoadResources() and SetCustomCellRenderer()
//iData: Image data, length: size of image in bytes, index: image index
void GetImageData(void* &iData, unsigned int *length, const unsigned int index);

////Action Events
void OnClickNewAction(wxCommandEvent &ev);
void OnClickDeleteLastAction(wxCommandEvent &ev);
void OnClickInsertAction(wxCommandEvent &ev);
void OnClickDeleteSelectedActions(wxCommandEvent &ev);
/////Combo Events
void OnClickNewCombo(wxCommandEvent &ev);
void OnClickDeleteCombo(wxCommandEvent &ev);
void OnClickRenameCombo(wxCommandEvent &ev);
/////Combo Key
void OnClickComboKey(wxMouseEvent &ev);
/////psButtons
void OnClick_psComboButtons(int winID);
/////Grid Events
void OnClickComboGrid(wxGridEvent &ev);
/////Mouse Events
void OnClickMouseHelpButton(wxMouseEvent &ev);
void OnClickMouseNullifiesAll(wxMouseEvent &ev);
