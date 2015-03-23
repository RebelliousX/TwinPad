
#include "main.h"
#include "twinpad_gui.h"
#include "labels.h"

#include "Externals.h"

using namespace std;	// for exceptions

// I know I should not use globals and externs, but either that or passing pointers and references
// to GUI controls all over the files and functions that want to use them or use a couple globals. I chose the latter.

// Global Structurea that hold all vital GUI controls and their values
CTwinPad_Gui GUI_Controls;
GUI_Configurations GUI_Config;

void OnNotebookChange(wxCommandEvent &evt)
{
	if (evt.GetEventType() == wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
	{
		wxSize curTabSize;
		wxString label = GUI_Controls.noteBook->GetPageText(GUI_Controls.noteBook->GetSelection());

		if ("Keyboard" == label)
			curTabSize = GUI_Controls.minWinSize[KEYBOARD_TAB];
		else if ("Mouse" == label)
			curTabSize = GUI_Controls.minWinSize[MOUSE_TAB];
		else if ("COMBOs" == label)
			curTabSize = GUI_Controls.minWinSize[COMBOS_TAB];
		else if ("Misc" == label)
			curTabSize = GUI_Controls.minWinSize[MISC_TAB];
		else if ("GamePad" == label)
			curTabSize = GUI_Controls.minWinSize[GAMEPAD_TAB];

		GUI_Controls.noteBook->GetParent()->SetMinClientSize(curTabSize);
		GUI_Controls.noteBook->GetParent()->SetClientSize(curTabSize);
		GUI_Controls.noteBook->GetParent()->ClientToWindowSize(curTabSize);
		GUI_Controls.noteBook->GetParent()->UpdateWindowUI();
	}
}

void CreateControls(TwinPad_Frame *window)
{
	// Save the pointer of the main frame to make access easier
	GUI_Controls.mainFrame = window;

	// Check to see if configuration files are present, otherwise create null ones
	wxString file1, file2;
	file1 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	IsFileOkAndFix(file1.ToStdString(), GUI_Controls.GetTwinPad_Header().ToStdString());
	file2 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	IsFileOkAndFix(file2.ToStdString(), GUI_Controls.GetTwinPad_ComboHeader().ToStdString());

	Loading_TwinPad_Main_Config();

	GUI_Controls.noteBook = new wxNotebook(window, ID_NOTEBOOK, wxPoint(-1, -1), wxSize(-1, -1));
	GUI_Controls.noteBook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, &::OnNotebookChange);
		
	AddKeyboardTab(GUI_Controls);
	AddMouseTab(GUI_Controls);
	AddCombosTab(GUI_Controls);
	AddMiscTab(GUI_Controls);

	// AddGamePadTab(GUI_Controls);	// TODO: Maybe..
}

void AddKeyboardTab(CTwinPad_Gui &GUI_Controls)
{
	// main panel will contain other controls, 
	// this panel will be added to the notebook's first tab.
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook);

	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	// main sizer
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	// flex grid sizer
	// This will contain ps2 images, labels, so we can configure them
	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(8, 7, 10, 10);

	// Wrap the flex sizer with nice borders
	wxStaticBoxSizer *stcPS2Controls = new wxStaticBoxSizer(wxVERTICAL, panel, "PS2 Buttons");
	stcPS2Controls->Add(flexSizer, 1, wxEXPAND);

	GUI_Controls.noteBook->AddPage(panel, "Keyboard", false);
	GUI_Controls.noteBook->SetPageText(KEYBOARD_TAB, "Keyboard");

	int animIndex = 0, lbl_Index = 0, lblIndex = 0;
	for (int r = 0; r < 8; r++)		// rows
	{
		animIndex = lbl_Index = r;
		for (int c = 0; c < 7; c++)	// columns
		{
			switch(c)
			{
			case 0:
			case 2:
			case 5:
				GUI_Controls.animCtrl[animIndex] = new CPS_Anim(panel, ID_BTN + animIndex);
				LoadResources(GUI_Controls.animCtrl[animIndex], animIndex);
				GUI_Controls.animCtrl[animIndex]->SetInactiveBitmap(GUI_Controls.animCtrl[animIndex]->GetInactiveBitmap());
				flexSizer->Add(GUI_Controls.animCtrl[animIndex], 1, wxEXPAND);
				GUI_Controls.animCtrl[animIndex]->SetIndex(animIndex);
				GUI_Controls.animCtrl[animIndex]->SetName(PS_LABEL[animIndex].name);
				GUI_Controls.animCtrl[animIndex]->Connect((ID_BTN + animIndex), wxEVT_LEFT_UP, wxCommandEventHandler(CPS_Anim::OnClickAnimInKeyboardTab));
				GUI_Controls.animCtrl[animIndex]->Bind(wxEVT_RIGHT_UP, ::OnLblCtrlRightClick);
				GUI_Controls.animCtrl[animIndex]->SetToolTip(wxString::Format("%s:\n%s%s", PS_LABEL[lbl_Index].name,
					"Left-Click: And then 'press any key' to assign it to the current Button.\n",
					"Right-Click: To erase the configured Key."));
				animIndex += 8;
				break;
			case 1:
			case 3:
			case 6:
				GUI_Controls.lblCtrl[lbl_Index] = new CPS_LBL(panel, ID_TXT + lbl_Index, "Null", wxSize(120,20));
				flexSizer->Add(GUI_Controls.lblCtrl[lbl_Index], 1, wxALIGN_CENTER);
				GUI_Controls.lblCtrl[lbl_Index]->SetWindowStyle(wxTE_CENTER);
				GUI_Controls.lblCtrl[lbl_Index]->SetBackgroundColour(wxColor(66,66,66));		// Dark Grey
				GUI_Controls.lblCtrl[lbl_Index]->SetForegroundColour(wxColor("White"));
				GUI_Controls.lblCtrl[lbl_Index]->SetIndex(lbl_Index); // Same index as the animation control
				GUI_Controls.lblCtrl[lbl_Index]->Bind(wxEVT_RIGHT_UP, ::OnLblCtrlRightClick);
				GUI_Controls.lblCtrl[lbl_Index]->SetToolTip(wxString::Format(
					"This shows the current Key assigned to \"%s\" button on the left.\n\n%s%s", PS_LABEL[lbl_Index].name,
					"Left-Click The icon on the left and then 'press any key' to assign it to the current Button.\n\n",
					"Right-Click here to erase the assigned Key."));
				lbl_Index += 8;
				break;
			case 4:
				// label "L" or "R" for left or right
				if(lblIndex < 4)
					GUI_Controls.lblLabel[lblIndex] = new wxStaticText(panel, ID_LBL + lblIndex, "L");
				else
					GUI_Controls.lblLabel[lblIndex] = new wxStaticText(panel, ID_LBL + lblIndex, "R");

				GUI_Controls.lblLabel[lblIndex]->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD));
				flexSizer->Add(GUI_Controls.lblLabel[lblIndex], 1, wxALIGN_CENTER);

				lblIndex++;
				break;
			}
		}
	}
	animIndex = lblIndex = lblIndex = 0;		// in case I need them again

	// Sizer that contains "Choose Pad 1 or 2, and WALK or Run" sizers
	wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);

	// Choose PAD1 or PAD2
	wxStaticBoxSizer *choosePadSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Which PAD?");
	GUI_Controls.pad1RadioBtn = new wxRadioButton(panel, ID_PAD1_RADIOBTN, "PAD 1");
	GUI_Controls.pad2RadioBtn = new wxRadioButton(panel, ID_PAD2_RADIOBTN, "PAD 2");
	
	choosePadSizer->Add(GUI_Controls.pad1RadioBtn, 0, wxALIGN_CENTER);
	choosePadSizer->AddSpacer(20);
	choosePadSizer->Add(GUI_Controls.pad2RadioBtn, 0, wxALIGN_CENTER);
	GUI_Controls.pad1RadioBtn->SetValue(true);

	// WALK or RUN (FULL or HALF) Press, imitating pressure sensitive button on keyboard
	wxStaticBoxSizer *walkRunSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Toggle WALK/RUN (HALF/FULL pressure sensitivity)");
	
	GUI_Controls.lblWalkRun = new CPS_LBL(panel, ID_TXT_WALKRUN, "NONE", wxSize(200, 20));
	walkRunSizer->Add(GUI_Controls.lblWalkRun, 0, wxLEFT | wxRIGHT, 50);
	GUI_Controls.lblWalkRun->SetWindowStyle(wxTE_CENTER);
	GUI_Controls.lblWalkRun->SetBackgroundColour(wxColor(66,66,66));	// Dark Grey
	GUI_Controls.lblWalkRun->SetForegroundColour(wxColor("White"));
	GUI_Controls.lblWalkRun->Bind(wxEVT_LEFT_UP, ::OnClickWalkRun);		// Assign a key to Walk/Run
	GUI_Controls.lblWalkRun->Bind(wxEVT_RIGHT_UP, ::OnClickWalkRun);	// Remove assigned key
	GUI_Controls.lblWalkRun->SetToolTip("This shows the current Key assigned to Walk or Run toggle button.\n\n"
				"Left-Click here and then 'press any key' to assign it to the current Button.\n"
				"Right-Click here to erase the assigned  Key.");
	middleSizer->Add(choosePadSizer, 0, wxALIGN_CENTER);
	middleSizer->AddSpacer(20);
	middleSizer->Add(walkRunSizer, 0, wxALIGN_CENTER);

	// Edit Button Label: Shows which button currently being configured
	GUI_Controls.lblEdit = new wxStaticText(panel, wxID_ANY, "Current button to edit: NONE", wxDefaultPosition, wxSize(260,20));
	GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#100075"));	// Dark Blue
	GUI_Controls.lblEdit->SetForegroundColour(wxColor("#FFFFFF"));	// White
	GUI_Controls.lblEdit->SetWindowStyle(wxTE_CENTER);

	GUI_Controls.btnAutoNavigate = new wxButton(panel, ID_BTN_AUTO, "Auto Navigate");
	GUI_Controls.btnNullifiesAll = new wxButton(panel, ID_BTN_NULL, "Nullifies All");
	GUI_Controls.btnOK = new wxButton(panel, wxID_OK, "Ok");				// wxID_OK for the dialog return value
	GUI_Controls.btnCancel = new wxButton(panel, wxID_CANCEL, "Cancel");	// wxID_CANCEL for the dialog return value

	GUI_Controls.btnCancel->Bind(wxEVT_BUTTON, OnClickCancel);
	GUI_Controls.btnOK->Bind(wxEVT_BUTTON, OnClickOk);
	GUI_Controls.btnNullifiesAll->Bind(wxEVT_BUTTON, OnClickKeyboardNullifiesAll);
	GUI_Controls.btnAutoNavigate->Bind(wxEVT_BUTTON, OnClickAutoNavigate);

	// Bottom Sizer, Contains 4 buttons: OK, Cancel, Auto Navigate, and Nullifies All
	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	bottomSizer->Add(GUI_Controls.btnAutoNavigate, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnNullifiesAll, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnOK, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnCancel, wxALIGN_CENTER);
	
	// Notebook labels not calculated with sizer, without this, I'll be missing the buttons
	wxBoxSizer *extraSpaceSizer = new wxBoxSizer(wxVERTICAL);
	extraSpaceSizer->AddSpacer(30);		

	// Add all sizers to parent sizer
	parentSizer->Add(stcPS2Controls, 0, wxALIGN_CENTER | wxTOP | wxLEFT | wxRIGHT, 10);
	parentSizer->Add(middleSizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
	parentSizer->AddSpacer(5);
	parentSizer->Add(GUI_Controls.lblEdit, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
	parentSizer->AddSpacer(15);
	parentSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxBOTTOM | wxLEFT | wxRIGHT, 10);
	parentSizer->Add(extraSpaceSizer, 0);

	// prevent window from getting smaller than the threshold
	panel->GetParent()->GetParent()->SetMinClientSize(parentSizer->GetMinSize());
	panel->SetSizerAndFit(parentSizer);
	// Save current window size, to be used later when switching tabs and resize again
	GUI_Controls.minWinSize[KEYBOARD_TAB] = parentSizer->GetSize();	// Save min size for Keyboard TAB
	GUI_Controls.noteBook->SetSelection(KEYBOARD_TAB);	// Select keyboard tab
}

void AddMouseTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "Mouse", false);
	GUI_Controls.noteBook->SetPageText(MOUSE_TAB, "Mouse");

	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	// main sizer
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	// flex grid sizer
	// This will mouse labels and list boxes, so we can configure them
	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(10, 2, 10, 70);

	// Static box around mouse controls
	wxStaticBoxSizer *mouseSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Mouse Button Configuration");

	for (int r = 0; r < 10; r++)
	{
		for (int c = 0; c < 2; c++)
		{
			switch(c)
			{
			case 0:
				GUI_Controls.lblMouseLabel[r] = new wxStaticText(panel, wxID_ANY, strMouseLabels[r]);
				flexSizer->Add(GUI_Controls.lblMouseLabel[r]);
				break;
			case 1:
				GUI_Controls.cmbMouseComboBox[r] = 
						new wxComboBox(panel, wxID_ANY, "NONE", wxDefaultPosition, 	wxSize(200,20), 19, strMouseChoices,
						wxCB_READONLY);
				// HACK FIX: wxWidgets doesn't apply background color for combo box
				// unless foreground color is set too, it is a known bug in wxWidgets
				GUI_Controls.cmbMouseComboBox[r]->SetBackgroundColour(wxColor(66,66,66));	// Dark Grey
				GUI_Controls.cmbMouseComboBox[r]->SetForegroundColour(wxColor("White"));
				flexSizer->Add(GUI_Controls.cmbMouseComboBox[r]);
				break;
			}
		}
	}
	mouseSizer->Add(flexSizer, 0, wxALIGN_CENTER | wxALL, 10);

	// Selection of Pad 1 or 2
	wxStaticBoxSizer *selectPadSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Which PAD?");
	GUI_Controls.mousePad1radioButton = new wxRadioButton(panel, ID_PAD1_TAB2_RADIOBTN, "PAD 1");
	GUI_Controls.mousePad2radioButton = new wxRadioButton(panel, ID_PAD2_TAB2_RADIOBTN, "PAD 2");
	selectPadSizer->Add(GUI_Controls.mousePad1radioButton, 0, wxALIGN_CENTER);
	selectPadSizer->AddSpacer(20);
	selectPadSizer->Add(GUI_Controls.mousePad2radioButton, 0, wxALIGN_CENTER);
	GUI_Controls.mousePad1radioButton->SetValue(true);
	
	// Nullifies All for mouse and help buttons
	GUI_Controls.btnMouseNullifiesAll = new wxButton(panel, ID_BTN_NULL_MOUSE, "Nullifies All");
	GUI_Controls.btnMouseHelp = new wxButton(panel, ID_BTN_HELP_MOUSE, "Help");
	GUI_Controls.btnMouseHelp->Bind(wxEVT_BUTTON, OnClickMouseHelpButton);
	GUI_Controls.btnMouseNullifiesAll->Bind(wxEVT_BUTTON, OnClickMouseNullifiesAll);

	wxStaticBoxSizer *sensitivitySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Sensitivity");
	GUI_Controls.cmbMouseSensitivity = new wxComboBox(panel, wxID_ANY, "1", wxDefaultPosition, 
											wxDefaultSize, 6, strMouseSensitivity, wxCB_READONLY); 
	sensitivitySizer->Add(GUI_Controls.cmbMouseSensitivity, wxALL, 10);

	// Sizer contains PAD selection, Null Button, Help Button, and Sensetivity Sizer
	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	bottomSizer->Add(selectPadSizer, 0, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnMouseNullifiesAll, 0, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnMouseHelp, 0, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(sensitivitySizer, 0, wxALIGN_CENTER);

	wxBoxSizer *extraSpaceSizer = new wxBoxSizer(wxVERTICAL);
	extraSpaceSizer->AddSpacer(60);

	// Finally add sub-sizers to parent sizer
	parentSizer->Add(mouseSizer, 0, wxALIGN_CENTER  | wxLEFT | wxRIGHT, 25);
	parentSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxALL, 5);
	parentSizer->Add(extraSpaceSizer, 0, wxALIGN_CENTER | wxALL, 5);

	// prevent window from getting smaller than the threshold
	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[MOUSE_TAB] = parentSizer->GetSize();	// Mouse TAB
}

void AddCombosTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "COMBOs", false);
	GUI_Controls.noteBook->SetPageText(COMBOS_TAB, "COMBOs");

	// Default tabs' colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());
	
	SetupComboTab(panel);
	GUI_Controls.minWinSize[COMBOS_TAB] = panel->GetSize();	// COMBOs TAB
}

void AddMiscTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "Misc", false);
	GUI_Controls.noteBook->SetPageText(MISC_TAB, "Misc");

	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	// main sizer
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	// Static box around mouse controls
	wxStaticBoxSizer *optionSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Options");


	wxStaticText *lblLabel = new wxStaticText(panel, wxID_ANY, strEXTRA_INFO);

	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(2, 2, 15, 60);

	GUI_Controls.chkDisablePad1	= new wxCheckBox(panel, ID_CHK_PAD1, "Disable PAD 1");
	GUI_Controls.chkDisablePad2	= new wxCheckBox(panel, ID_CHK_PAD2, "Disable PAD 2");
	flexSizer->Add(GUI_Controls.chkDisablePad1);
	flexSizer->Add(GUI_Controls.chkDisablePad2);

	GUI_Controls.chkDisableMouse = new wxCheckBox(panel, ID_CHK_MOUSE, "Disable Mouse");
	GUI_Controls.chkDisableCombos	= new wxCheckBox(panel, ID_CHK_COMBOS, "Disable COMBOs");
	flexSizer->Add(GUI_Controls.chkDisableMouse);
	flexSizer->Add(GUI_Controls.chkDisableCombos);

	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);
	GUI_Controls.chkDisableOnFlyKey = new wxCheckBox(panel, ID_CHK_KEY_EVENTS, strEXTRA_ONFLY);
	GUI_Controls.chkDisableKeyEvents = new wxCheckBox(panel, ID_CHK_KEY_EVENTS,	strEXTRA_KEY);
	GUI_Controls.chkEnableHack = new wxCheckBox(panel, ID_CHK_HACK, strEXTRA_HACK);
	vSizer->Add(GUI_Controls.chkDisableOnFlyKey, 1, wxEXPAND | wxBOTTOM | wxLEFT, 5);
	vSizer->Add(GUI_Controls.chkDisableKeyEvents, 1, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT, 5);
	vSizer->Add(GUI_Controls.chkEnableHack, 1, wxEXPAND | wxTOP | wxLEFT, 5);

	wxBoxSizer *extraSpaceSizer = new wxBoxSizer(wxVERTICAL);
	extraSpaceSizer->AddSpacer(20);

	optionSizer->Add(flexSizer, 0, wxALL, 10);
	optionSizer->Add(vSizer);

	// Finally, add sub-sizers to parent sizer

	parentSizer->Add(lblLabel, 0, wxALIGN_CENTER | wxALL, 10);
	parentSizer->Add(optionSizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 20);
	parentSizer->Add(extraSpaceSizer);

	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[MISC_TAB] = parentSizer->GetSize();		// MISC TAB
}


void AddGamePadTab(CTwinPad_Gui &GUI_Controls)
{
	// Not a priority 
	/*
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "GamePad", false);
	GUI_Controls.noteBook->SetPageText(GAMEPAD_TAB, "GamePad");
	
	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[GAMEPAD_TAB] = panel->GetSize();	// GamePad TAB
	*/	
}

// This function handles the click event for both keyboard tab and combo tab
void CPS_Anim::OnClickAnimInKeyboardTab(wxCommandEvent &event)
{
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

// This function handles the click event for Mouse help button
void OnClickMouseHelpButton(wxCommandEvent &ev)
{
	wxMessageBox(strMOUSE_HELP_MSG, "Help", wxICON_INFORMATION);
	ev.Skip();
}

// This function handles the click event for Mouse Nullifies All
void OnClickMouseNullifiesAll(wxCommandEvent &ev)
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
	int id = ev.GetId();
	// Which id? lblCtrl or animCtrl?
	if (ev.GetId() >= ID_TXT && ev.GetId() <= 2023)
		id -= ID_TXT;
	else if (ev.GetId() >= ID_BTN && ev.GetId() <= 1023)
		id -= ID_BTN;
	GUI_Controls.lblCtrl[id]->SetLabel("Null");
	GUI_Controls.lblEdit->SetLabel("Current button to edit: NONE");
	GUI_Controls.animCtrl[id]->Stop();
	return;	// a return will disable the context menu if the event was on lblCtrl
}

// Handle Left-Click to assign a key to Walk/Run, Right-Clcik to remove assigned key
void OnClickWalkRun(wxMouseEvent &ev)
{
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
		return;
	}
}

// This function handles the click event for Keyboard Nullifies All
void OnClickKeyboardNullifiesAll(wxCommandEvent &ev)
{
	// Another bug in wxWidgets! without the skip event, the window freezes, 
	// until it loses focus by another app (hides behind it) then set focused again.
	ev.Skip();
}

// This function handles the click event for Cancel button
void OnClickCancel(wxCommandEvent &ev)
{
	// Stop all Timers
	GUI_Controls.mainFrame->tmrAnimate->Stop();

	GUI_Controls.mainFrame->Hide();
	GUI_Controls.mainFrame->Close(true);
}

// This function handles the click event for 'Auto Navigate' button
void OnClickAutoNavigate(wxCommandEvent &ev)
{
	// TODO: Implement auto navigation feature
}

// This function handles the click event for Cancel button
void OnClickOk(wxCommandEvent &ev)
{
	// TODO: Implement Ok button to save Configurations from all tabs
		
	// Stop all Timers
	GUI_Controls.mainFrame->tmrAnimate->Stop();

	GUI_Controls.mainFrame->Hide();
	GUI_Controls.mainFrame->Close(true);
}
