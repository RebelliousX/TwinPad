#include "stdafx.h"
#include "events_functions.h"
#include "twinpad_gui.h"
#include "functions_gui.h"

// Global Input Manager object
extern InputManager IM;

// This function handles the click event for both keyboard tab and combo tab
void CPS_Anim::OnClickAnimInKeyboardTab(wxMouseEvent &event)
{
	if (GUI_Controls.mainFrame->tmrGetKey->IsRunning())
		return;

	int winID = event.GetId();

	if (winID >= 1000 && winID < 1024)	// Keyboard tab
	{
		// Start the timer to get one key
		GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#990000"));	// Crimson -bloody red- :)
		GUI_Controls.lblEdit->SetLabel(wxString::Format("Edit Button: %s", PS_LABEL[this->GetIndex()].name));
		this->Play();
		GUI_Controls.indexOfButton = winID - 1000;
		GUI_Controls.mainFrame->tmrGetKey->Start(50);	// 50 millisecond
	}
	else if (winID >= 1024 && winID <= 1047)	// Combo tab
		OnClick_psComboButtons(winID);
}

// Change between pad 1 and 2 for keyboard tab, save and reload configured keys
void OnRadBtnPadChange(wxCommandEvent &ev)
{
	int switchToPad = 0, curPad = 0;
	if (ev.GetId() == ID_PAD1_RADIOBTN)
	{
		curPad = 1;
		switchToPad = 0;
	}
	else if (ev.GetId() == ID_PAD2_RADIOBTN)
	{
		curPad = 0;
		switchToPad = 1;
	}

	if (GUI_Controls.mainFrame->tmrGetKey->IsRunning())
	{
		if (ev.GetId() == ID_PAD1_RADIOBTN)
			GUI_Controls.pad2RadioBtn->SetValue(true);
		if (ev.GetId() == ID_PAD2_RADIOBTN)
			GUI_Controls.pad1RadioBtn->SetValue(true);
		return;
	}

	// Save current config for the current pad
	for (int i = 0; i < 25; ++i)
	{
		unsigned char key = 0;
		if (i < 24)
		{
			key = GUI_Controls.lblCtrl[i]->GetKeyCode();
			GUI_Controls.lblCtrl[i]->SetLabel("NONE");
			GUI_Controls.lblCtrl[i]->SetKeyCode(0);
		}
		else if (i == 24)
		{
			key = GUI_Controls.lblWalkRun->GetKeyCode();
			GUI_Controls.lblWalkRun->SetLabel("NONE");
			GUI_Controls.lblWalkRun->SetKeyCode(0);
		}
		Configurations.m_pad[curPad][i] = key;
	}

	// Load config for pad we are switching to
	for (int i = 0; i < 25; ++i)
	{
		unsigned char key = Configurations.m_pad[switchToPad][i];
		wxString name;
		if (key == 0)
			name = "NONE";
		else
			name = IM.GetKeyName(key);

		if (i < 24)
		{
			GUI_Controls.lblCtrl[i]->SetLabel(name);
			GUI_Controls.lblCtrl[i]->SetKeyCode(key);
		}
		else if (i == 24)
		{
			GUI_Controls.lblWalkRun->SetLabel(name);
			GUI_Controls.lblWalkRun->SetKeyCode(key);
		}
		Configurations.m_pad[switchToPad][i] = key;
	}
}

// This function handles the click event for Mouse help button
void OnClickMouseHelpButton(wxMouseEvent &ev)
{
	wxMessageBox(strMOUSE_HELP_MSG, "Help", wxICON_INFORMATION);
	ev.Skip();
}

// This function handles the click event for Mouse Nullifies All
void OnClickMouseNullifiesAll(wxMouseEvent &ev)
{
	for (int i = 0; i < intMOUSE_BUTTONS; ++i)
		GUI_Controls.cmbMouseComboBox[i]->Select(0);

	GUI_Controls.cmbMouseSensitivity->Select(0);
	GUI_Controls.mousePad1radioButton->SetValue(true);
	// Another bug in wxWidgets! without the skip event, the window freezes, 
	// until it loses focus by another app (hides behind it) then set focused again.
	ev.Skip();
}

// This function handles right click event on the txtCrls/animCtrl and delete configuration for a button
// Also, prevent the context menu (right-click menu) from showing up
void OnLblCtrlRightClick(wxMouseEvent &ev)
{
	if (GUI_Controls.mainFrame->tmrGetKey->IsRunning())
		return;

	int id = ev.GetId();
	// Which id? lblCtrl or animCtrl?
	if (ev.GetId() >= ID_TXT && ev.GetId() <= 2023)
		id -= ID_TXT;
	else if (ev.GetId() >= ID_BTN && ev.GetId() <= 1023)
		id -= ID_BTN;
	GUI_Controls.lblCtrl[id]->SetLabel("NONE");
	GUI_Controls.lblCtrl[id]->SetKeyCode(0);
	GUI_Controls.lblEdit->SetLabel("Current button to edit: NONE");
	GUI_Controls.animCtrl[id]->Stop();
	Configurations.m_pad[(GUI_Controls.pad1RadioBtn->GetValue() ? 0 : 1)][id] = 0;
}

// Handle Left-Click to assign a key to Walk/Run, Right-Clcik to remove assigned key
void OnClickWalkRun(wxMouseEvent &ev)
{
	if (GUI_Controls.mainFrame->tmrGetKey->IsRunning())
		return;

	if (ev.LeftUp())
	{
		// Start the timer to get one key
		GUI_Controls.indexOfButton = 24;	// 24 is Walk/Run key
		GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#990000"));	// Crimson -bloody red- :)
		GUI_Controls.lblEdit->SetLabel("Edit Button: Walk or Run");
		GUI_Controls.mainFrame->tmrGetKey->Start(50);	// 50 millisecond
	}
	else if (ev.RightUp())
	{
		GUI_Controls.lblWalkRun->SetLabel("NONE");
		GUI_Controls.lblWalkRun->SetKeyCode(0);
		Configurations.m_pad[(GUI_Controls.pad1RadioBtn->GetValue() ? 0 : 1)][24] = 0;
		return;
	}
}

// This function handles the click event for Keyboard Nullifies All
void OnClickKeyboardNullifiesAll(wxMouseEvent &ev)
{
	for (int i = 0; i < 24; ++i)
	{
		GUI_Controls.lblCtrl[i]->SetLabel("NONE");
		GUI_Controls.lblCtrl[i]->SetKeyCode(0);
	}
	GUI_Controls.lblWalkRun->SetLabel("NONE");
	GUI_Controls.lblWalkRun->SetKeyCode(0);
	// Another bug in wxWidgets! without the skip event, the window freezes, 
	// until it loses focus by another app (hides behind it) then set focused again.
	ev.Skip();
}

// This function handles the click event for Cancel button
void OnClickCancel(wxMouseEvent &ev)
{
	GUI_Controls.mainFrame->Hide();
	GUI_Controls.mainFrame->Close(true);
}

// This function handles the click event for 'Auto Navigate' button
void OnClickAutoNavigate(wxMouseEvent &ev)
{
	GUI_Controls.indexOfButton = 0;
	
	GUI_Controls.mainFrame->tmrAutoNavigate->Start(50);		// 50 millisecond
}

// This function handles the click event for Cancel button
void OnClickOk(wxMouseEvent &ev)
{
	// save current combo first (the one showing in grid), in case we modified it.
	// Because it is not saved until we switch to another combo.
	if (!GUI_Controls.cmbComboName->GetStringSelection().IsEmpty())
		SaveGridToCombo(GUI_Controls.cmbComboName->GetStringSelection());

	SaveTwinPadConfigurations();
	SaveTwinPadComboConfigurations();

	GUI_Controls.mainFrame->Hide();
	GUI_Controls.mainFrame->Close(true);
}
