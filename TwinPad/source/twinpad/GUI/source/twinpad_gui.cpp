#include <exception>

#include "twinpad_gui.h"
#include "labels.h"

using namespace std;	//for exceptions

//I know I should not use globals and externs, but either that or passing pointers and references
//to GUI controls all over the files and functions that want to use them or use a couple globals. I chose the latter.

//Global Structurea that hold all vital GUI controls and their values
CTwinPad_Gui GUI_Controls = { 0 };
GUI_Configurations GUI_Config = { 0 };

const wxString g_HEADER_TWINPAD = "[TwinPad Configurations v1.6]";
const wxString g_HEADER_TWINPAD_COMBO = "[TwinPad COMBO Configurations v1.1]";
const wxString g_Path = "inis/"; //TODO: replace with given dir from emu. 
const wxString g_TWIN_PAD = "TwinPad.ini";
const wxString g_TWIN_PAD_COMBOS = "TwinPad_COMBOs.ini";

void OnNotebookChange(wxCommandEvent &evt)
{
	try
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
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void CreateControls(wxFrame *window)
{
	try
	{
		//Save the pointer of the main frame to make access easier
		GUI_Controls.mainFrame = window;

		//Check to see if configuration files are present, otherwise create null ones
		wxString file1, file2;
		file1 = g_Path + g_TWIN_PAD;
		CheckAndCreateIfNecessary(file1.ToStdString(), g_HEADER_TWINPAD.ToStdString());
		file2 = g_Path + g_TWIN_PAD_COMBOS;
		CheckAndCreateIfNecessary(file2.ToStdString(), g_HEADER_TWINPAD_COMBO.ToStdString());

		Loading_TwinPad_Main_Config();
		//////////////////////////

		GUI_Controls.noteBook = new wxNotebook(window, ID_NOTEBOOK, wxPoint(-1, -1), wxSize(-1, -1));
		GUI_Controls.noteBook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, &::OnNotebookChange);
		
		AddKeyboardTab(GUI_Controls);
		AddMouseTab(GUI_Controls);
		AddCombosTab(GUI_Controls);
		AddMiscTab(GUI_Controls);

		//AddGamePadTab(GUI_Controls);	//TODO: Maybe..
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void AddKeyboardTab(CTwinPad_Gui &GUI_Controls)
{
	//main panel will contain other controls, 
	//this panel will be added to the notebook's first tab.
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook);

	//Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	/////main sizer//////////////////////////////////////
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	/////flex grid sizer/////////////////////////////////
	//This will contain ps2 images, labels, so we can configure them
	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(8, 7, 10, 10);

	//Wrap the flex sizer with nice borders///////
	wxStaticBoxSizer *stcPS2Controls = new wxStaticBoxSizer(wxVERTICAL, panel, "PS2 Buttons");
	stcPS2Controls->Add(flexSizer, 1, wxEXPAND);

	GUI_Controls.noteBook->AddPage(panel, "Keyboard", false);
	GUI_Controls.noteBook->SetPageText(KEYBOARD_TAB, "Keyboard");

	int animIndex = 0, txtIndex = 0, lblIndex = 0;
	for (int r = 0; r < 8; r++)		//rows
	{
		animIndex = txtIndex = r;
		for (int c = 0; c < 7; c++)	//columns
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
				GUI_Controls.animCtrl[animIndex]->Connect((ID_BTN + animIndex), wxEVT_LEFT_UP, wxCommandEventHandler(CPS_Anim::OnClick));
				GUI_Controls.animCtrl[animIndex]->Bind(wxEVT_RIGHT_UP, ::OnTxtCtrlRightClick);
				GUI_Controls.animCtrl[animIndex]->SetToolTip(wxString::Format("%s:\n%s%s", PS_LABEL[txtIndex].name,
					"Left-Click: And then 'press any key' to assign it to the current Button.\n",
					"Right-Click: To erase the configured Key."));
				animIndex += 8;
				break;
			case 1:
			case 3:
			case 6:
				GUI_Controls.txtCtrl[txtIndex] = new CPS_Txt(panel, ID_TXT + txtIndex, "Null");
				flexSizer->Add(GUI_Controls.txtCtrl[txtIndex], 1, wxALIGN_CENTER);
				GUI_Controls.txtCtrl[txtIndex]->SetEditable(false);
				GUI_Controls.txtCtrl[txtIndex]->SetWindowStyle(wxTE_CENTER);
				GUI_Controls.txtCtrl[txtIndex]->SetBackgroundColour(wxColor(66,66,66));		//Dark Grey
				GUI_Controls.txtCtrl[txtIndex]->SetForegroundColour(wxColor("White"));
				GUI_Controls.txtCtrl[txtIndex]->SetIndex(txtIndex); //Same index as the animation control
				GUI_Controls.txtCtrl[txtIndex]->Bind(wxEVT_RIGHT_UP, ::OnTxtCtrlRightClick);
				GUI_Controls.txtCtrl[txtIndex]->SetToolTip(wxString::Format(
					"This shows the current Key assigned to \"%s\" button on the left.\n\n%s%s", PS_LABEL[txtIndex].name,
					"Left-Click The icon on the left and then 'press any key' to assign it to the current Button.\n",
					"Right-Click here to erase the configured Key."));
				txtIndex += 8;
				break;
			case 4:
				//label "L" or "R" for left or right
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
	animIndex = lblIndex = lblIndex = 0;		//in case I need them again

	//Sizer that contains "Choose Pad 1 or 2, and WALK or Run" sizers
	wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);

	//Choose PAD1 or PAD2
	wxStaticBoxSizer *choosePadSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Which PAD?");
	GUI_Controls.pad1RadioBtn = new wxRadioButton(panel, ID_PAD1_RADIOBTN, "PAD 1");
	GUI_Controls.pad2RadioBtn = new wxRadioButton(panel, ID_PAD2_RADIOBTN, "PAD 2");
	
	choosePadSizer->Add(GUI_Controls.pad1RadioBtn, 0, wxALIGN_CENTER);
	choosePadSizer->AddSpacer(20);
	choosePadSizer->Add(GUI_Controls.pad2RadioBtn, 0, wxALIGN_CENTER);
	GUI_Controls.pad1RadioBtn->SetValue(true);

	//WALK or RUN (FULL or HALF) Press, imitating pressure sensitive button on keyboard
	wxStaticBoxSizer *walkRunSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Toggle WALK/RUN (HALF/FULL pressure sensitivity)");
	
	GUI_Controls.txtWalkRun = new CPS_Txt(panel, ID_TXT_WALKRUN, "NONE", wxSize(200,20));
	GUI_Controls.txtWalkRun->SetWindowStyle(wxTE_CENTER);
	GUI_Controls.txtWalkRun->SetEditable(false);
	GUI_Controls.txtWalkRun->SetWindowStyle(wxTE_CENTER);
	GUI_Controls.txtWalkRun->SetBackgroundColour(wxColor(66,66,66));	//Dark Grey
	GUI_Controls.txtWalkRun->SetForegroundColour(wxColor("White"));
	walkRunSizer->Add(GUI_Controls.txtWalkRun, 0, wxLEFT | wxRIGHT, 50);

	middleSizer->Add(choosePadSizer, 0, wxALIGN_CENTER);
	middleSizer->AddSpacer(20);
	middleSizer->Add(walkRunSizer, 0, wxALIGN_CENTER);

	//Edit Button Label: Shows which button currently being configured
	GUI_Controls.lblEdit = new wxStaticText(panel, wxID_ANY, "Edit Button: ", wxDefaultPosition, wxSize(260,20));
	GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#100075"));	//Dark Blue
	GUI_Controls.lblEdit->SetForegroundColour(wxColor("#FFFFFF"));	//White
	GUI_Controls.lblEdit->SetWindowStyle(wxTE_CENTER);

	GUI_Controls.btnAutoNavigate = new wxButton(panel, ID_BTN_AUTO, "&Auto Navigate");
	GUI_Controls.btnNullifiesAll = new wxButton(panel, ID_BTN_NULL, "&Nullifies All");
	GUI_Controls.btnOK = new wxButton(panel, ID_BTN_OK, "&Ok");
	GUI_Controls.btnCancel = new wxButton(panel, ID_BTN_CANCEL, "&Cancel");

	//Bottom Sizer, Contains 4 buttons: OK, Cancel, Auto Navigate, and Nullifies All
	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	bottomSizer->Add(GUI_Controls.btnAutoNavigate, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnNullifiesAll, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnOK, wxALIGN_CENTER);
	bottomSizer->AddSpacer(10);
	bottomSizer->Add(GUI_Controls.btnCancel, wxALIGN_CENTER);
	
	//Notebook labels not calculated with sizer, without this, I'll be missing the buttons
	wxBoxSizer *extraSpaceSizer = new wxBoxSizer(wxVERTICAL);
	extraSpaceSizer->AddSpacer(30);		

	//Add all sizers to parent sizer
	parentSizer->Add(stcPS2Controls, 0, wxALIGN_CENTER | wxTOP | wxLEFT | wxRIGHT, 10);
	parentSizer->Add(middleSizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
	parentSizer->AddSpacer(5);
	parentSizer->Add(GUI_Controls.lblEdit, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
	parentSizer->AddSpacer(15);
	parentSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxBOTTOM | wxLEFT | wxRIGHT, 10);
	parentSizer->Add(extraSpaceSizer, 0);

	//prevent window from getting smaller than the threshold
	panel->GetParent()->GetParent()->SetMinClientSize(parentSizer->GetMinSize());
	panel->SetSizerAndFit(parentSizer);
	//Save current window size, to be used later when switching tabs and resize again
	GUI_Controls.minWinSize[KEYBOARD_TAB] = parentSizer->GetSize();	//Save min size for Keyboard TAB
	GUI_Controls.noteBook->SetSelection(KEYBOARD_TAB);	//Select keyboard tab
}

void AddMouseTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "Mouse", false);
	GUI_Controls.noteBook->SetPageText(MOUSE_TAB, "Mouse");

	//Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	/////main sizer//////////////////////////////////////
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	/////flex grid sizer/////////////////////////////////
	//This will mouse labels and list boxes, so we can configure them
	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(10, 2, 10, 70);

	//Static box around mouse controls
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
				//HACK FIX: wxWidgets doesn't apply background color for combo box
				//unless foreground color is set too, it is a known bug in wxWidgets
				GUI_Controls.cmbMouseComboBox[r]->SetBackgroundColour(wxColor(66,66,66));	//Dark Grey
				GUI_Controls.cmbMouseComboBox[r]->SetForegroundColour(wxColor("White"));
				flexSizer->Add(GUI_Controls.cmbMouseComboBox[r]);
				break;
			}
		}
	}
	mouseSizer->Add(flexSizer, 0, wxALIGN_CENTER | wxALL, 10);

	//Selection of Pad 1 or 2
	wxStaticBoxSizer *selectPadSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Which PAD?");
	GUI_Controls.mousePad1radioButton = new wxRadioButton(panel, ID_PAD1_TAB2_RADIOBTN, "PAD 1");
	GUI_Controls.mousePad2radioButton = new wxRadioButton(panel, ID_PAD2_TAB2_RADIOBTN, "PAD 2");
	selectPadSizer->Add(GUI_Controls.mousePad1radioButton, 0, wxALIGN_CENTER);
	selectPadSizer->AddSpacer(20);
	selectPadSizer->Add(GUI_Controls.mousePad2radioButton, 0, wxALIGN_CENTER);
	GUI_Controls.mousePad1radioButton->SetValue(true);
	
	//Nullifies All for mouse and help buttons
	GUI_Controls.btnMouseNullifiesAll = new wxButton(panel, ID_BTN_NULL_MOUSE, "&Nullifies All");
	GUI_Controls.btnMouseHelp = new wxButton(panel, ID_BTN_HELP_MOUSE, "&Help");
	GUI_Controls.btnMouseHelp->Bind(wxEVT_LEFT_UP, OnClickMouseHelpButton);
	GUI_Controls.btnMouseNullifiesAll->Bind(wxEVT_LEFT_UP, OnClickMouseNullifiesAll);

	wxStaticBoxSizer *sensitivitySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Sensitivity");
	GUI_Controls.cmbMouseSensitivity = new wxComboBox(panel, wxID_ANY, "1", wxDefaultPosition, 
											wxDefaultSize, 6, strMouseSensitivity, wxCB_READONLY); 
	sensitivitySizer->Add(GUI_Controls.cmbMouseSensitivity, wxALL, 10);

	//Sizer contains PAD selection, Null Button, Help Button, and Sensetivity Sizer
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

	////////////////Final/////////////////////////////////////
	parentSizer->Add(mouseSizer, 0, wxALIGN_CENTER  | wxLEFT | wxRIGHT, 25);
	parentSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxALL, 5);
	parentSizer->Add(extraSpaceSizer, 0, wxALIGN_CENTER | wxALL, 5);

	////prevent window from getting smaller than the threshold
	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[MOUSE_TAB] = parentSizer->GetSize();	//Mouse TAB
}

void AddCombosTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "COMBOs", false);
	GUI_Controls.noteBook->SetPageText(COMBOS_TAB, "COMBOs");

	//Default tabs' colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());
	
	SetupComboTab(panel);
	GUI_Controls.minWinSize[COMBOS_TAB] = panel->GetSize();	//COMBOs TAB
}

void AddMiscTab(CTwinPad_Gui &GUI_Controls)
{
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "Misc", false);
	GUI_Controls.noteBook->SetPageText(MISC_TAB, "Misc");

	//Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	/////main sizer//////////////////////////////////////
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);

	//Static box around mouse controls
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

	///////////////Final/////////////////

	parentSizer->Add(lblLabel, 0, wxALIGN_CENTER | wxALL, 10);
	parentSizer->Add(optionSizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 20);
	parentSizer->Add(extraSpaceSizer);

	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[MISC_TAB] = parentSizer->GetSize();		//MISC TAB
}


void AddGamePadTab(CTwinPad_Gui &GUI_Controls)
{
	//////////////Not a priority/////////////////
	/*
	wxPanel *panel = new wxPanel(GUI_Controls.noteBook, wxID_ANY, wxPoint(-1,-1), wxSize(-1,-1));
	GUI_Controls.noteBook->AddPage(panel, "GamePad", false);
	GUI_Controls.noteBook->SetPageText(GAMEPAD_TAB, "GamePad");
	
	//Default tab colors are ugly (pure white), so get color from the frame
	panel->SetBackgroundColour(panel->GetParent()->GetBackgroundColour());

	panel->SetSizerAndFit(parentSizer);
	GUI_Controls.minWinSize[GAMEPAD_TAB] = panel->GetSize();	//GamePad TAB
	*/	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///This function handles the click event for both keyboard tab and combo tab
void CPS_Anim::OnClick(wxCommandEvent &event)
{
	try
	{
		//int winID = (int) event.GetEventUserData();
		int winID = event.GetId();
		
		if (winID >= 1000 && winID < 1024)	//Keyboard tab
		{
			//Implement reading DirectInput keypress
			GUI_Controls.txtCtrl[this->GetIndex()]->SetValue(this->GetName());
			GUI_Controls.lblEdit->SetLabel(wxString::Format("Edit Button: %s", PS_LABEL[this->GetIndex()].name));
			this->Play();
		}
		else if (winID >= 1024 && winID <= 1047)	//Combo tab
			OnClick_psComboButtons(winID);
		else
			throw "Unknown AnimationCtrl ID, #" + winID;
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

///This function handles the click event for Mouse help button
void OnClickMouseHelpButton(wxMouseEvent &ev)
{
	try
	{
		wxMessageBox(strMOUSE_HELP_MSG, "Help", wxICON_INFORMATION);
		ev.Skip();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

///This function handles the click event for Mouse Nullifies All
void OnClickMouseNullifiesAll(wxMouseEvent &ev)
{
	try
	{
		for (int i = 0; i < intMOUSE_BUTTONS; ++i)
			GUI_Controls.cmbMouseComboBox[i]->Select(0);

		GUI_Controls.cmbMouseSensitivity->Select(0);
		GUI_Controls.mousePad1radioButton->SetValue(true);
		//Another bug in wxWidgets! without the skip event, the window freezes, 
		//until it loses focus by another app (hides behind it) then set focused again.
		ev.Skip();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

//This function handles right click even on th txtCrls/animCtrl and delete configuration for a button
//Also, preven the context menu (right-click menu) from showing up
void OnTxtCtrlRightClick(wxMouseEvent &ev)
{
	try
	{
		int id = ev.GetId();
		//Which id? txtCtrl or animCtrl?
		if (ev.GetId() >= ID_TXT && ev.GetId() <= 2023)
			id -= ID_TXT;
		else if (ev.GetId() >= ID_BTN && ev.GetId() <= 1023)
			id -= ID_BTN;
		GUI_Controls.txtCtrl[id]->SetValue("Null");
		GUI_Controls.lblEdit->SetLabel("Edit Button: ");
		GUI_Controls.animCtrl[id]->Stop();	//Stop animation
		return;	//a return will disable the context menu if the even was on txtCtrl
	}
	catch (exception &ex)
	{
		wxMessageBox(ex.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}