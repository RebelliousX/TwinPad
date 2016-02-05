#include "stdafx.h"
#include "twinpad_gui.h"
#include "events_functions.h"
#include "Externals.h"

// I know I should not use globals and externs, but either that or passing pointers and references
// to GUI controls all over the files and functions that want to use them or use a couple globals. I chose the latter.

// Global Structurea that hold all vital GUI controls and their values
CTwinPad_Gui GUI_Controls;
MainConfigurations Configurations;

void OnNotebookChanging(wxCommandEvent &evt)
{
	if (evt.GetEventType() == wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)
	{
		GUI_Controls.Notebook->Freeze();
	}
}

void OnNotebookChange(wxCommandEvent &evt)
{
	if (evt.GetEventType() == wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
	{
		wxSize curTabSize;
		wxString label = GUI_Controls.Notebook->GetPageText(GUI_Controls.Notebook->GetSelection());

		// Do not change notebook page while polling the keyboard for input via timers
		if (GUI_Controls.mainFrame->tmrGetKey->IsRunning() && GUI_Controls.curTab == KEYBOARD_TAB)
		{
			GUI_Controls.Notebook->SetSelection(GUI_Controls.curTab);
			GUI_Controls.Notebook->Thaw();
			return;
		}

		if (GUI_Controls.mainFrame->tmrGetComboKey->IsRunning() && GUI_Controls.curTab == COMBOS_TAB)
		{
			GUI_Controls.Notebook->SetSelection(GUI_Controls.curTab);
			GUI_Controls.Notebook->Thaw();
			return;
		}

		if (GUI_Controls.mainFrame->tmrAutoNavigate->IsRunning() && GUI_Controls.curTab == KEYBOARD_TAB)
		{
			GUI_Controls.Notebook->SetSelection(GUI_Controls.curTab);
			GUI_Controls.Notebook->Thaw();
			return;
		}

		// Save some information about the current page (the one we switched to) to adjust window size
		if ("Keyboard" == label)
		{
			curTabSize = GUI_Controls.minWinSize[KEYBOARD_TAB];
			GUI_Controls.curTab = KEYBOARD_TAB;
		}
		else if ("Mouse" == label)
		{
			curTabSize = GUI_Controls.minWinSize[MOUSE_TAB];
			GUI_Controls.curTab = MOUSE_TAB;
		}
		else if ("COMBOs" == label)
		{
			curTabSize = GUI_Controls.minWinSize[COMBOS_TAB];
			GUI_Controls.curTab = COMBOS_TAB;
		}
		else if ("Misc" == label)
		{
			curTabSize = GUI_Controls.minWinSize[MISC_TAB];
			GUI_Controls.curTab = MISC_TAB;
		}
		else if ("GamePad" == label)
		{
			curTabSize = GUI_Controls.minWinSize[GAMEPAD_TAB];
			GUI_Controls.curTab = GAMEPAD_TAB;
		}

		wxWindow *parent = GUI_Controls.Notebook->GetParent();
		parent->SetMinClientSize(curTabSize);
		parent->SetClientSize(curTabSize);
		parent->ClientToWindowSize(curTabSize);

		GUI_Controls.Notebook->Thaw();
	}
}

void CreateControls(TwinPad_Frame *window)
{
	// Clean some important stuff for subsequent calls to open twinpad (mainly for combo)
	GUI_Controls.Clean();
	Configurations.Clean();

	// Save the pointer of the main frame to make access easier
	GUI_Controls.mainFrame = window;

	// Check to see if configuration files are present, otherwise create null ones
	wxString file1, file2;
	file1 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	IsFileOkAndFix(file1, GUI_Controls.GetTwinPad_Header());
	file2 = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	IsFileOkAndFix(file2, GUI_Controls.GetTwinPad_ComboHeader());

	LoadTwinPadConfigurations();
	LoadTwinPadComboConfigurations();

	GUI_Controls.Notebook = new CNotebook(window, ID_NOTEBOOK);
	GUI_Controls.Notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, ::OnNotebookChange);
	GUI_Controls.Notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, ::OnNotebookChanging);

	AddKeyboardTab();
	AddMouseTab();
	AddCombosTab();
	AddMiscTab();

	// AddGamePadTab();	// TODO: Maybe..


	// Populate the dialog with loaded configurations from file
	// --------------------------------------------------------

	// Keyboard tab:
	for (int i = 0; i < 25; ++i)
	{
		wxString keyName;
		if (Configurations.m_pad[0][i] != 0)
		{
			for (int j = 0; j < (sizeof(DIK_KEYCODES) / sizeof(*DIK_KEYCODES)); ++j)
				if (DIK_KEYCODES[j].keyValue == Configurations.m_pad[0][i])
				{
					keyName = DIK_KEYCODES[j].name;
					keyName = keyName.substr(4, keyName.length());
					break;
				}
		}
		else
			keyName = "NONE";

		if (i < 24)
		{
			GUI_Controls.lblCtrl[i]->SetLabel(keyName);
			GUI_Controls.lblCtrl[i]->SetKeyCode(Configurations.m_pad[0][i]);
		}
		else
		{
			GUI_Controls.lblWalkRun->SetLabel(keyName);
			GUI_Controls.lblWalkRun->SetKeyCode(Configurations.m_pad[0][i]);
		}
	}

	// Mouse tab:
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < (sizeof(MOUSE_CHOICES) / sizeof(*MOUSE_CHOICES)); ++j)
			if (Configurations.m_mouse[i] == MOUSE_CHOICES[j].keyValue)
			{
				GUI_Controls.cmbMouseComboBox[i]->SetStringSelection(MOUSE_CHOICES[j].name);
				break;
			}

	if (Configurations.m_mouseAsPad == 0)
		GUI_Controls.mousePad1radioButton->SetValue(true);
	else
		GUI_Controls.mousePad2radioButton->SetValue(true);

	GUI_Controls.cmbMouseSensitivity->SetStringSelection(wxString::Format("%d", Configurations.m_mouseSensitivity));

	// Misc tab:
	GUI_Controls.chkDisablePad1->SetValue(Configurations.m_extra[Configurations.DISABLE_PAD1] ? 1 : 0);
	GUI_Controls.chkDisablePad2->SetValue(Configurations.m_extra[Configurations.DISABLE_PAD2] ? 1 : 0);
	GUI_Controls.chkDisableKeyEvents->SetValue(Configurations.m_extra[Configurations.DISABLE_KEYEVENTS] ? 1 : 0);
	GUI_Controls.chkDisableMouse->SetValue(Configurations.m_extra[Configurations.DISABLE_MOUSE] ? 1 : 0);
	GUI_Controls.chkDisableCombos->SetValue(Configurations.m_extra[Configurations.DISABLE_COMBOS] ? 1 : 0);

	// Combo tab:
	for (unsigned int combo = 0; combo < Configurations.Combos.size(); ++combo)
		GUI_Controls.cmbComboName->Append(Configurations.Combos[combo]->GetName());

	ShowFirstComboOnGrid();
}

void AddKeyboardTab()
{
	// main panel will contain other controls, 
	// this panel will be added to the notebook's first tab.
	wxPanel *panel = new wxPanel(GUI_Controls.Notebook);

	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	// main sizer
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	// flex grid sizer
	// This will contain ps2 images, labels, so we can configure them
	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(8, 7, 10, 10);

	// Wrap the flex sizer with nice borders
	wxStaticBoxSizer *stcPS2Controls = new wxStaticBoxSizer(wxVERTICAL, panel, "PSX/PS2 Buttons");
	stcPS2Controls->Add(flexSizer, 1, wxEXPAND);

	GUI_Controls.Notebook->AddPage(panel, "Keyboard", false);
	GUI_Controls.Notebook->SetPageText(KEYBOARD_TAB, "Keyboard");

	int lblIndex = 0;
	for (int r = 0; r < 8; r++)		// rows
	{
		int animIndex, psb_Index;
		animIndex = psb_Index = r;
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
				GUI_Controls.animCtrl[animIndex]->Connect((ID_BTN + animIndex), wxEVT_LEFT_UP, wxMouseEventHandler(CPS_Anim::OnClickAnimInKeyboardTab));
				GUI_Controls.animCtrl[animIndex]->Bind(wxEVT_RIGHT_UP, ::OnLblCtrlRightClick);
				GUI_Controls.animCtrl[animIndex]->SetToolTip(wxString::Format("%s:\n%s%s", PS_LABEL[psb_Index].name,
					"Left-Click: And then 'press any key' to assign it to the current Button.\n",
					"Right-Click: To erase the configured Key."));
				animIndex += 8;
				break;
			case 1:
			case 3:
			case 6:
				GUI_Controls.lblCtrl[psb_Index] = new CPS_LBL(panel, ID_TXT + psb_Index, "NONE", wxSize(120,20));
				GUI_Controls.lblCtrl[psb_Index]->SetKeyCode(0);
				flexSizer->Add(GUI_Controls.lblCtrl[psb_Index], 1, wxALIGN_CENTER);
				GUI_Controls.lblCtrl[psb_Index]->SetWindowStyle(wxTE_CENTER);
				GUI_Controls.lblCtrl[psb_Index]->SetBackgroundColour(wxColor(66,66,66));		// Dark Grey
				GUI_Controls.lblCtrl[psb_Index]->SetForegroundColour(wxColor(255,255,255));
				GUI_Controls.lblCtrl[psb_Index]->SetIndex(psb_Index); // Same index as the animation control
				GUI_Controls.lblCtrl[psb_Index]->Bind(wxEVT_RIGHT_UP, ::OnLblCtrlRightClick);
				GUI_Controls.lblCtrl[psb_Index]->SetToolTip(wxString::Format(
					"This shows the current Key assigned to \"%s\" button on the left.\n\n%s%s", PS_LABEL[psb_Index].name,
					"Left-Click The icon on the left and then 'press any key' to assign it to the current Button.\n\n",
					"Right-Click here to erase the assigned Key."));
				psb_Index += 8;
				break;
			case 4:
				// label "L" or "R" for left or right
				if(lblIndex < 4)
					GUI_Controls.lblLabel[lblIndex] = new wxStaticText(panel, ID_LBL + lblIndex, "L");
				else
					GUI_Controls.lblLabel[lblIndex] = new wxStaticText(panel, ID_LBL + lblIndex, "R");

				GUI_Controls.lblLabel[lblIndex]->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
				flexSizer->Add(GUI_Controls.lblLabel[lblIndex], 1, wxALIGN_CENTER);

				lblIndex++;
				break;
			}
		}
	}

	// Sizer that contains "Choose Pad 1 or 2, and WALK or Run" sizers
	wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);

	// Choose PAD1 or PAD2
	wxStaticBoxSizer *choosePadSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Which PAD?");
	GUI_Controls.pad1RadioBtn = new CRadButton(panel, ID_PAD1_RADIOBTN, "PAD 1");
	GUI_Controls.pad2RadioBtn = new CRadButton(panel, ID_PAD2_RADIOBTN, "PAD 2");
	GUI_Controls.pad1RadioBtn->Bind(wxEVT_RADIOBUTTON, ::OnRadBtnPadChange);
	GUI_Controls.pad2RadioBtn->Bind(wxEVT_RADIOBUTTON, ::OnRadBtnPadChange);
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
	GUI_Controls.lblWalkRun->SetForegroundColour(wxColor(255,255,255));
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

	GUI_Controls.btnAutoNavigate = new CButton(panel, ID_BTN_AUTO, "Auto Navigate");
	GUI_Controls.btnNullifiesAll = new CButton(panel, ID_BTN_NULL, "Nullifies All");
	GUI_Controls.btnOK = new CButton(panel, ID_BTN_OK, "Ok");				// Don't use wxID_OK, will cause trouble with Return key
	GUI_Controls.btnCancel = new CButton(panel, ID_BTN_CANCEL, "Cancel");	// Don't use wxID_CANCEL, disables ESC to close dialog

	GUI_Controls.btnCancel->Bind(wxEVT_LEFT_UP, OnClickCancel);
	GUI_Controls.btnOK->Bind(wxEVT_LEFT_UP, OnClickOk);
	GUI_Controls.btnNullifiesAll->Bind(wxEVT_LEFT_UP, OnClickKeyboardNullifiesAll);
	GUI_Controls.btnAutoNavigate->Bind(wxEVT_LEFT_UP, OnClickAutoNavigate);

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
	GUI_Controls.Notebook->SetSelection(KEYBOARD_TAB);	// Select keyboard tab
}

void AddMouseTab()
{
	wxPanel *panel = new wxPanel(GUI_Controls.Notebook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.Notebook->AddPage(panel, "Mouse", false);
	GUI_Controls.Notebook->SetPageText(MOUSE_TAB, "Mouse");

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
	GUI_Controls.mousePad1radioButton = new CRadButton(panel, ID_PAD1_TAB2_RADIOBTN, "PAD 1");
	GUI_Controls.mousePad2radioButton = new CRadButton(panel, ID_PAD2_TAB2_RADIOBTN, "PAD 2");
	selectPadSizer->Add(GUI_Controls.mousePad1radioButton, 0, wxALIGN_CENTER);
	selectPadSizer->AddSpacer(20);
	selectPadSizer->Add(GUI_Controls.mousePad2radioButton, 0, wxALIGN_CENTER);
	GUI_Controls.mousePad1radioButton->SetValue(true);
	
	// Nullifies All for mouse and help buttons
	GUI_Controls.btnMouseNullifiesAll = new CButton(panel, ID_BTN_NULL_MOUSE, "Nullifies All");
	GUI_Controls.btnMouseHelp = new CButton(panel, ID_BTN_HELP_MOUSE, "Help");
	GUI_Controls.btnMouseHelp->Bind(wxEVT_LEFT_UP, OnClickMouseHelpButton);
	GUI_Controls.btnMouseNullifiesAll->Bind(wxEVT_LEFT_UP, OnClickMouseNullifiesAll);

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

void AddCombosTab()
{
	wxPanel *panel = new wxPanel(GUI_Controls.Notebook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.Notebook->AddPage(panel, "COMBOs", false);
	GUI_Controls.Notebook->SetPageText(COMBOS_TAB, "COMBOs");

	// Default tabs' colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());
	
	SetupComboTab(panel);
	GUI_Controls.minWinSize[COMBOS_TAB] = panel->GetSize();	// COMBOs TAB
}

void AddMiscTab()
{
	wxPanel *panel = new wxPanel(GUI_Controls.Notebook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.Notebook->AddPage(panel, "Misc", false);
	GUI_Controls.Notebook->SetPageText(MISC_TAB, "Misc");

	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	// main sizer
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *lblLabel = new wxStaticText(panel, wxID_ANY, strEXTRA_INFO);

	// Static box around mouse controls
	wxStaticBoxSizer *optionSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Options");

	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(2, 2, 15, 100);

	GUI_Controls.chkDisablePad1	= new wxCheckBox(panel, ID_CHK_PAD1, "Disable PAD 1");
	GUI_Controls.chkDisablePad2	= new wxCheckBox(panel, ID_CHK_PAD2, "Disable PAD 2");
	GUI_Controls.chkDisableMouse = new wxCheckBox(panel, ID_CHK_MOUSE, "Disable Mouse");
	GUI_Controls.chkDisableCombos = new wxCheckBox(panel, ID_CHK_COMBOS, "Disable COMBOs");
	flexSizer->Add(GUI_Controls.chkDisablePad1);
	flexSizer->Add(GUI_Controls.chkDisablePad2);
	flexSizer->Add(GUI_Controls.chkDisableMouse);
	flexSizer->Add(GUI_Controls.chkDisableCombos);

	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	GUI_Controls.chkDisableKeyEvents = new wxCheckBox(panel, ID_CHK_KEY_EVENTS,	strEXTRA_KEY);
	hSizer->Add(GUI_Controls.chkDisableKeyEvents, 1, wxEXPAND | wxALL, 10);

	optionSizer->Add(flexSizer, 1, wxALL, 10);
	optionSizer->Add(hSizer, 2, wxEXPAND, 10);

	// Finally, add sub-sizers to parent sizer
	parentSizer->Add(lblLabel, 0, wxEXPAND | wxALL, 10);
	parentSizer->Add(optionSizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
	parentSizer->AddSpacer(30);
	
	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[MISC_TAB] = parentSizer->GetSize();		// MISC TAB
}


void AddGamePadTab()
{
	// Not a priority 
	/*
	wxPanel *panel = new wxPanel(GUI_Controls.Notebook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.Notebook->AddPage(panel, "GamePad", false);
	GUI_Controls.Notebook->SetPageText(GAMEPAD_TAB, "GamePad");
	
	// Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[GAMEPAD_TAB] = panel->GetSize();	// GamePad TAB
	*/	
}
