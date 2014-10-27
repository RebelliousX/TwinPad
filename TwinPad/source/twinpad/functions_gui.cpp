#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/grid.h"

#include "wx/mstream.h"		//for wxMemoryInputStream

#include "twinpad_gui.h"
#include "functions_gui.h"
#include "comboGrid.h"
#include "labels.h"

#include "resources.h"		//All GIFs and resources for TwinPad stored in arrays

using namespace std;

//////externs: Defined in TwinPad_Gui.cpp
extern CTwinPad_Gui GUI_Controls;
extern GUI_Configurations GUI_Config;

//Cell Locator: for current/previous cell editing. We need a pointer to the main
//grid after it is initialized, using method SetGrid()
CCellLocator Cell_Locator;

//TODO: (Implement: PADgetSettingDir callback function)
const wxString HEADER_TWINPAD = "[TwinPad Configurations v1.6]";
const wxString HEADER_TWINPAD_COMBO = "[TwinPad COMBO Configurations v1.1]";
const wxString LOCATION = "inis/"; //TODO: replace with given dir from emu. 
const wxString TWIN_PAD = "TwinPad.ini";
const wxString TWIN_PAD_COMBOS = "TwinPad_COMBOs.ini";

const int IMG_WIDTH = 40;

//Sorry for the inconsistencies, using fstream and wxfile at the same time, just wanted to try both :)

void CreateNullFile()
{
	int counter;
	
	string file = LOCATION + TWIN_PAD;
	string strPad;

	ofstream m_nullfile(file.c_str(), ios::out);

	if (!m_nullfile.is_open())
	{
		wxMessageBox("Couldn't create configuration files into the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}

	m_nullfile << HEADER_TWINPAD << endl;
	for(int pad = 0; pad <= 1; pad++)
	{
		if (pad == 0) 
			strPad = "[0][";
		else
			strPad = "[1][";
		
		counter = 0;
		while(counter < 24)
		{
			m_nullfile << strPad << counter << "] = 0x" << "0" << endl;
			GUI_Config.m_pad[pad][counter] = counter;
			counter++;
		}
		m_nullfile << strPad << counter << "] = 0x" << "0" << endl;  //for walk/run value
	}

	counter = 0;
	while(counter < 10)
	{
		m_nullfile << wxString("[") << counter << "] = 36" << endl;
		GUI_Config.m_mouse[counter] = 36;
		counter++;
	}
	m_nullfile << "0" << endl;		/* Mouse as PAD 1 */ 
	GUI_Config.m_mouseAsPad = 0;
	m_nullfile << "1" << endl;		/* Mouse sensitivity default value. */
	GUI_Config.m_mouseSensitivity = 1;

	counter = 0;
	while(counter <= 6)
	{
		m_nullfile << "0" << endl;
		GUI_Config.m_extra[counter] = 0;
		counter++;
	}
	
	m_nullfile.close();
}

void CreateNullComboFile()
{
	string file = LOCATION + TWIN_PAD_COMBOS;
	ofstream m_txtFile(file.c_str(), ios::out);

	if (!m_txtFile.is_open())
	{
		wxMessageBox("Couldn't create configuration files into the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}

	m_txtFile << HEADER_TWINPAD_COMBO << endl;
	m_txtFile << "ComboCount\t= 0\n" << "ComboPad\t= 0\n";

	m_txtFile.close();
}

bool CheckAndCreateIfNecessary(const string &file, const string &header)
{
	ifstream f(file);
	if (!f.is_open())
	{
		wxMessageBox("Couldn't create configuration files into the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}
	string str, strMsg;

	int select = 0;
	if (header == HEADER_TWINPAD)
	{
		strMsg = "Resetting Configurations in 'TwinPad.ini' File, please reconfigure the plugin.";
		select = 1;
	}
	else if (header == HEADER_TWINPAD_COMBO)
	{
		strMsg = "Resetting Configurations in 'TwinPad_COMBOs.ini' File, please reconfigure the plugin.";
		select = 2;
	}
	else
	{
		wxMessageBox("Wrong header passed to 'IsFileOk()' function. Inform the Author please.\nHeader: " + header);
		return false;
	}

	//First: Check if file can be opened
	if (f.is_open())
	{
		//Second: Check the Header if version is compatible
		getline(f, str);
		if (select == 1)
		{
			if (str != HEADER_TWINPAD)
			{
				f.close();
				CreateNullFile();
				return true;	//Create null file, then is OK
			}
			else
				return true;	//File is OK
		}
		else if (select == 2)
		{
			if (str != HEADER_TWINPAD_COMBO)
			{
				f.close();
				CreateNullComboFile();
				return true;	//Create null file, then is OK
			}
			else
				return true;	//File is OK
		}
	}
	else
	{
		if (select == 1) CreateNullFile();
		if (select == 2) CreateNullComboFile();
	}
	
	f.close();
	return false;
}

void Loading_Gui()
{
	wxString fileName = LOCATION + TWIN_PAD;
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		wxMessageBox("Couldn't open configuration files from the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}

	wxStringTokenizer token;
	wxString line, subStr;
	long val = 0;

	line = file.GetFirstLine();		//Get header and skip it
	for(int pad = 0; pad < 2; pad++)
		for (int key = 0; key < 25; key++)
		{
			line = file.GetNextLine();
			token.SetString(line, " ", wxTOKEN_STRTOK);
			subStr = token.GetNextToken();				//Skips first token [n][n]
			subStr = token.GetNextToken();				//Skips second token =
			subStr = token.GetNextToken();				//we are interested in this token 0xNN
			subStr.ToLong(&val, 16);					//Convert string to hex
			GUI_Config.m_pad[pad][key] = int(val);		
		}

	for(int i = 0; i < 10; i++)		//For mouse buttons and scrollup/down
	{
		line = file.GetNextLine();
		token.SetString(line, " ", wxTOKEN_STRTOK);
		subStr = token.GetNextToken();	//skips first token [n]
		subStr = token.GetNextToken();	//skips second token =
		subStr = token.GetNextToken();	//we are interested in this token N (DECIMAL)
		subStr.ToLong(&val, 10);		//Convert string to dec
		GUI_Config.m_mouse[i] = val;
	}

	for(int i = 0; i < 7; i++)
	{
		line = file.GetNextLine();
		line.ToLong(&val, 10);
		GUI_Config.m_extra[i] = val;
	}
	file.Close();
}

//Loading images into controls
void LoadResources(CPS_Anim *animCtrl, int index)
{
	try
	{
		void *iData = 0;	//hold image data
		size_t length = 0;  //hold image size in bytes

		GetImageData(iData, &length, index);

		unsigned char *buffer = new unsigned char[length];
		memcpy(buffer, iData, length);

		wxMemoryInputStream iStream(buffer, length);

		animCtrl->Load(iStream, wxANIMATION_TYPE_ANY);

		delete[] buffer;
	}
	catch (exception &ex)
	{
		wxMessageBox(ex.what());
	}
}

//This function is called from LoadResources() and SetCustomCellRenderer()
void GetImageData(void* &data, unsigned int *length, const unsigned int index)
{
	//I know this is awkward, but I am not gonna dig into the resources.h 
	//to fix the arrays into 1 and use an index to make this more concise and pithy.
	switch (index)
	{
	case 0:
		data = (void*)L2_GIF;
		*length = sizeof(L2_GIF);
		break;
	case 1:
		data = (void*)R2_GIF;
		*length = sizeof(R2_GIF);
		break;
	case 2:
		data = (void*)L1_GIF;
		*length = sizeof(L1_GIF);
		break;
	case 3:
		data = (void*)R1_GIF;
		*length = sizeof(R1_GIF);
		break;
	case 4:
		data = (void*)TRIANGLE_GIF;
		*length = sizeof(TRIANGLE_GIF);
		break;
	case 5:
		data = (void*)CIRCLE_GIF;
		*length = sizeof(CIRCLE_GIF);
		break;
	case 6:
		data = (void*)CROSS_GIF;
		*length = sizeof(CROSS_GIF);
		break;
	case 7:
		data = (void*)SQUARE_GIF;
		*length = sizeof(SQUARE_GIF);
		break;
	case 8:
		data = (void*)SELECT_GIF;
		*length = sizeof(SELECT_GIF);
		break;
	case 9:
		data = (void*)L3_GIF;
		*length = sizeof(L3_GIF);
		break;
	case 10:
		data = (void*)R3_GIF;
		*length = sizeof(R3_GIF);
		break;
	case 11:
		data = (void*)START_GIF;
		*length = sizeof(START_GIF);
		break;
	case 12:
		data = (void*)UP_GIF;
		*length = sizeof(UP_GIF);
		break;
	case 13:
		data = (void*)RIGHT_GIF;
		*length = sizeof(RIGHT_GIF);
		break;
	case 14:
		data = (void*)DOWN_GIF;
		*length = sizeof(DOWN_GIF);
		break;
	case 15:
		data = (void*)LEFT_GIF;
		*length = sizeof(LEFT_GIF);
		break;
	case 16:
	case 20:
		data = (void*)ANALOG_UP_GIF;
		*length = sizeof(ANALOG_UP_GIF);
		break;
	case 17:
	case 21:
		data = (void*)ANALOG_RIGHT_GIF;
		*length = sizeof(ANALOG_RIGHT_GIF);
		break;
	case 18:
	case 22:
		data = (void*)ANALOG_DOWN_GIF;
		*length = sizeof(ANALOG_DOWN_GIF);
		break;
	case 19:
	case 23:
		data = (void*)ANALOG_LEFT_GIF;
		*length = sizeof(ANALOG_LEFT_GIF);
		break;
	default:
		wxMessageBox("Unknown button number in GetImageData()");
		break;
	}
}

/////////////////////////////// Combo Tab ///////////////////////////////////////

void SetupComboTab(wxPanel *panel)
{
	//Create Animation controls, we don't want to reference to anim controls in Keyboard tab,
	//since they have different behavior and events.
	for(int i = 0; i < 24; ++i)
	{
		//wxAnimation anim = GUI_Controls.animCtrl[i]->GetAnimation();
		GUI_Controls.psComboButtons[i] = new CPS_Anim(panel, ID_BTN2 + i);
		GUI_Controls.psComboButtons[i]->SetAnimation(GUI_Controls.animCtrl[i]->GetAnimation());
		GUI_Controls.psComboButtons[i]->SetIndex(i);
		GUI_Controls.psComboButtons[i]->SetName(PS_LABEL[i].name);
		GUI_Controls.psComboButtons[i]->Connect((ID_BTN2 + i), wxEVT_LEFT_UP, wxCommandEventHandler(CPS_Anim::OnClick));
		if(i >= 16)		//Play Analog Stick animation by default
			GUI_Controls.psComboButtons[i]->Play();
		GUI_Controls.psComboButtons[i]->SetToolTip(PS_LABEL[i].name);
	}
	
	////Creating Layout///////
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
		wxStaticBoxSizer *topLevelSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Key Combinations (COMBOs)");
			wxStaticBoxSizer *stcComboEditorSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "COMBO Editor");
				GUI_Controls.btnNewCombo = new wxButton(panel, wxID_ANY, "&New Combo");
				GUI_Controls.btnDeleteCombo = new wxButton(panel, wxID_ANY, "&Delete Combo");
				GUI_Controls.btnRenameCombo = new wxButton(panel, wxID_ANY, "&Rename Combo");
				stcComboEditorSizer->Add(GUI_Controls.btnNewCombo);
				stcComboEditorSizer->AddSpacer(10);
				stcComboEditorSizer->Add(GUI_Controls.btnDeleteCombo);
				stcComboEditorSizer->AddSpacer(10);
				stcComboEditorSizer->Add(GUI_Controls.btnRenameCombo);
				stcComboEditorSizer->AddSpacer(10);
				GUI_Controls.btnNewCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickNewCombo);
				GUI_Controls.btnDeleteCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteCombo);
				GUI_Controls.btnRenameCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickRenameCombo);
			wxStaticBoxSizer *stcComboNameSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "COMBO Name");
				GUI_Controls.cmbComboName = new wxComboBox(panel, wxID_ANY, wxEmptyString, 
																wxDefaultPosition, wxSize(200,25), 0, 0, wxCB_READONLY | wxCB_SORT);
				GUI_Controls.cmbComboName->SetBackgroundColour(wxColor(66,66,66));	//Dark Grey
				GUI_Controls.cmbComboName->SetForegroundColour(wxColor("White"));
				stcComboNameSizer->Add(GUI_Controls.cmbComboName, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
			wxStaticBoxSizer *stcComboKeySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "KEY");
				GUI_Controls.txtComboKey = new wxTextCtrl(panel, wxID_ANY, "NONE", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
				GUI_Controls.txtComboKey->SetBackgroundColour(wxColor(66,66,66));
				GUI_Controls.txtComboKey->SetForegroundColour(wxColor("White"));
				GUI_Controls.txtComboKey->SetToolTip("Click here and Press any key to assign it to the current Combo.");
				GUI_Controls.txtComboKey->Bind(wxEVT_LEFT_UP, ::OnClickComboKey);
				stcComboKeySizer->Add(GUI_Controls.txtComboKey);
			wxStaticBoxSizer *stcDefaultDelaySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Delay");	
				GUI_Controls.spnDefaultDelay = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, 
								wxDefaultSize, 16896L, 1, 99999, 3); /*Min 1, Max 99999, Default 3*/
				GUI_Controls.spnDefaultDelay->SetBackgroundColour(wxColor(66,66,66));
				GUI_Controls.spnDefaultDelay->SetForegroundColour(wxColor("White"));
				stcDefaultDelaySizer->Add(GUI_Controls.spnDefaultDelay);
			topLevelSizer->Add(stcComboEditorSizer, 0, wxALIGN_CENTER, 5);
			topLevelSizer->AddSpacer(10);
			topLevelSizer->Add(stcComboNameSizer, 1, wxALIGN_CENTER | wxEXPAND | wxLEFT | wxRIGHT, 5);
			topLevelSizer->Add(stcDefaultDelaySizer, 0, wxALIGN_CENTER, 5);
			topLevelSizer->AddSpacer(10);
			topLevelSizer->Add(stcComboKeySizer, 0, wxALIGN_CENTER, 5);
		wxBoxSizer *midLevelSizer = new wxBoxSizer(wxHORIZONTAL);
			wxStaticBoxSizer *comboGridSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "COMBOs Viewer");
				//Virtual grid
				CComboGrid *comboGrid = new CComboGrid(panel, wxID_ANY, wxPoint(1,1), wxSize(640,320));
				//Complex table holds various data in a cell accessed via grid
				CTableBase *tableBase = new CTableBase();
				tableBase->SetAttrProvider(new CGridCellAttrProvider);
				//We need pointers to the grid and table so we can read their values from Keyboard Tab to save configurations
				GUI_Controls.virtualGrid = comboGrid;
				GUI_Controls.tableBaseGrid = tableBase;
				comboGrid->SetTable(tableBase, true);
				//Setup attributes
				wxGridCellAttr *attrReadOnly = new wxGridCellAttr, *attrDelayColumn = new wxGridCellAttr;
				comboGrid->InsertCols(0, 19);		//0: Delay, 1-18 Buttons (columns# fixed, rows# not)
				//Set Column 0 attr, the range of acceptable numbers from 1 to 99999 (delay values)
				attrDelayColumn->SetEditor(new wxGridCellNumberEditor(1, 99999));
				attrDelayColumn->SetBackgroundColour(wxColor(66,66,66));
				attrDelayColumn->SetTextColour(wxColor(255,255,255));
				attrDelayColumn->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false));
				attrDelayColumn->SetAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
				attrDelayColumn->SetReadOnly(false); //only first column is editable
				//Set ReadOnly attr to a column (from 1-18)
				attrReadOnly->SetReadOnly(true);
				//prevents overflow of text to next cell (just in case we use it to store hidden data)
				attrReadOnly->SetOverflow(false); 
				comboGrid->SetColAttr(0, attrDelayColumn);	//column 0: Delay
				for(int i = 1; i < 19; ++i)					//column 1-18, PS2 buttons images (no text)
				{
					comboGrid->SetColAttr(i, attrReadOnly);
					//Bug in wxWidgets 2.9.4 and up!!?? Ticket #4401, it says fixed, but not here :/
					//Otherwise, a crash when exiting TwinPad, Assert about DecRef.. I hate wxWidgets now.
					//See: http://forums.wxwidgets.org/viewtopic.php?t=36159&p=147976
					attrReadOnly->IncRef();		//Also, added DecRef in the virtual destructor
				}
				comboGrid->SetSelectionBackground(wxColor("#990000"));	//Crimson -bloody red- :)
				comboGridSizer->Add(comboGrid, 0, 0, 5);
			wxStaticBoxSizer *editComboSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Action Editor");
				GUI_Controls.btnNewAction = new wxButton(panel, wxID_ANY, "New &Action");
				GUI_Controls.btnDeleteLastAction = new wxButton(panel, wxID_ANY, "Delete &Last Action");
				GUI_Controls.btnInsertAction = new wxButton(panel, wxID_ANY, "&Insert Actions");
				GUI_Controls.btnDeleteSelectedActions = new wxButton(panel, wxID_ANY, "Delete &Selected Actions");
				wxSize largestButtonSize = GUI_Controls.btnDeleteSelectedActions->GetSize();
				GUI_Controls.btnNewAction->SetMinSize(largestButtonSize);
				GUI_Controls.btnDeleteLastAction->SetMinSize(largestButtonSize);
				GUI_Controls.btnInsertAction->SetMinSize(largestButtonSize);
				GUI_Controls.btnNewAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickNewAction);
				GUI_Controls.btnDeleteLastAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteLastAction);
				GUI_Controls.btnInsertAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickInsertAction);
				GUI_Controls.btnDeleteSelectedActions->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteSelectedActions);
				editComboSizer->Add(GUI_Controls.btnNewAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
				editComboSizer->AddSpacer(5);
				editComboSizer->Add(GUI_Controls.btnDeleteLastAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
				editComboSizer->AddSpacer(5);
				editComboSizer->Add(GUI_Controls.btnInsertAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
				editComboSizer->AddSpacer(5);
				editComboSizer->Add(GUI_Controls.btnDeleteSelectedActions, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
		midLevelSizer->Add(comboGridSizer);
		midLevelSizer->Add(editComboSizer, 1, wxEXPAND | wxALIGN_CENTER, 5);
			wxStaticBoxSizer *lowLevelSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "PS2 Buttons");
				wxBoxSizer *lowLevelContainerSizer = new wxBoxSizer(wxHORIZONTAL);
					wxStaticBoxSizer *stcL1L2Sizer = new wxStaticBoxSizer(wxVERTICAL, panel, "L1 && L2");
						wxGridSizer *L1L2Sizer = new wxGridSizer(3, 1, 0, 0);
							L1L2Sizer->Add(GUI_Controls.psComboButtons[L1]);
							L1L2Sizer->AddSpacer(IMG_WIDTH);
							L1L2Sizer->Add(GUI_Controls.psComboButtons[L2]);
							stcL1L2Sizer->Add(L1L2Sizer);
							lowLevelContainerSizer->Add(stcL1L2Sizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcDpadArrowSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "D-Pad Arrows");
						wxGridSizer *dpadArrowSizer = new wxGridSizer(3, 3, 0, 0);
							dpadArrowSizer->AddSpacer(IMG_WIDTH);
							dpadArrowSizer->Add(GUI_Controls.psComboButtons[UP]);
							dpadArrowSizer->AddSpacer(IMG_WIDTH);
							dpadArrowSizer->Add(GUI_Controls.psComboButtons[LEFT]);
							dpadArrowSizer->AddSpacer(IMG_WIDTH);
							dpadArrowSizer->Add(GUI_Controls.psComboButtons[RIGHT]);
							dpadArrowSizer->AddSpacer(IMG_WIDTH);
							dpadArrowSizer->Add(GUI_Controls.psComboButtons[DOWN]);
							dpadArrowSizer->AddSpacer(IMG_WIDTH);
							stcDpadArrowSizer->Add(dpadArrowSizer);
							lowLevelContainerSizer->Add(stcDpadArrowSizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcLeftAnalogSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Left Analog Stick");
						wxGridSizer *leftAnalogSizer = new wxGridSizer(3, 3, 0, 0);
							leftAnalogSizer->AddSpacer(IMG_WIDTH);
							leftAnalogSizer->Add(GUI_Controls.psComboButtons[LANALOG_UP]);
							leftAnalogSizer->AddSpacer(IMG_WIDTH);
							leftAnalogSizer->Add(GUI_Controls.psComboButtons[LANALOG_LEFT]);
							leftAnalogSizer->Add(GUI_Controls.psComboButtons[L3]);
							leftAnalogSizer->Add(GUI_Controls.psComboButtons[LANALOG_RIGHT]);
							leftAnalogSizer->AddSpacer(IMG_WIDTH);
							leftAnalogSizer->Add(GUI_Controls.psComboButtons[LANALOG_DOWN]);
							leftAnalogSizer->AddSpacer(IMG_WIDTH);
							stcLeftAnalogSizer->Add(leftAnalogSizer);
							lowLevelContainerSizer->Add(stcLeftAnalogSizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcStartSelectSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Select & Start");
						wxGridSizer *startSelectSizer = new wxGridSizer(3, 3, 0, 0);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->Add(GUI_Controls.psComboButtons[SELECT]);
							startSelectSizer->AddSpacer(IMG_WIDTH);
							startSelectSizer->Add(GUI_Controls.psComboButtons[START]);
							stcStartSelectSizer->Add(startSelectSizer);
							lowLevelContainerSizer->Add(stcStartSelectSizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcRightAnalogSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Right Analog Stick");
						wxGridSizer *rightAnalogSizer = new wxGridSizer(3, 3, 0, 0);
							rightAnalogSizer->AddSpacer(IMG_WIDTH);
							rightAnalogSizer->Add(GUI_Controls.psComboButtons[RANALOG_UP]);
							rightAnalogSizer->AddSpacer(IMG_WIDTH);
							rightAnalogSizer->Add(GUI_Controls.psComboButtons[RANALOG_LEFT]);
							rightAnalogSizer->Add(GUI_Controls.psComboButtons[R3]);
							rightAnalogSizer->Add(GUI_Controls.psComboButtons[RANALOG_RIGHT]);
							rightAnalogSizer->AddSpacer(IMG_WIDTH);
							rightAnalogSizer->Add(GUI_Controls.psComboButtons[RANALOG_DOWN]);
							rightAnalogSizer->AddSpacer(IMG_WIDTH);
							stcRightAnalogSizer->Add(rightAnalogSizer);
							lowLevelContainerSizer->Add(stcRightAnalogSizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcTriCirCroSqrSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Digital Buttons");
						wxGridSizer *triCirCroSqrSizer = new wxGridSizer(3, 3, 0, 0);
							triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
							triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[TRIANGLE]);
							triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
							triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[SQUARE]);
							triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
							triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[CIRCLE]);
							triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
							triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[CROSS]);
							triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
							stcTriCirCroSqrSizer->Add(triCirCroSqrSizer);
							lowLevelContainerSizer->Add(stcTriCirCroSqrSizer);
							lowLevelContainerSizer->AddSpacer(5);
					wxStaticBoxSizer *stcR1R2Sizer = new wxStaticBoxSizer(wxVERTICAL, panel, "R1 && R2");
						wxGridSizer *R1R2Sizer = new wxGridSizer(3, 1, 0, 0);
							R1R2Sizer->Add(GUI_Controls.psComboButtons[R1]);
							R1R2Sizer->AddSpacer(IMG_WIDTH);
							R1R2Sizer->Add(GUI_Controls.psComboButtons[R2]);
							stcR1R2Sizer->Add(R1R2Sizer);
							lowLevelContainerSizer->Add(stcR1R2Sizer);
							lowLevelContainerSizer->AddSpacer(5);
				lowLevelSizer->Add(lowLevelContainerSizer);
		parentSizer->Add(topLevelSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
		parentSizer->Add(midLevelSizer, 1, wxEXPAND | wxALIGN_CENTER, 5);
		parentSizer->Add(lowLevelSizer, 0, wxALIGN_CENTER, 2);
		parentSizer->AddSpacer(30);

	panel->SetSizerAndFit(parentSizer);
	panel->SetMinClientSize(parentSizer->GetSize());
	panel->GetParent()->ClientToWindowSize(panel->GetClientSize());
	//Done!

	comboGrid->DisableDragColSize();			//Prevent mouse from resizing rows and columns
	comboGrid->DisableDragRowSize();
	
	comboGrid->SetColFormatNumber(0);			//first column accepts integer numbers only
	comboGrid->SetColLabelValue(0, "Delay");

	//Set label and width
	for(int i = 1; i < 19; ++i)
	{
		wxString str = wxString::Format("#%d",i);
		comboGrid->SetColLabelValue(i, str);
		comboGrid->SetColumnWidth(i, IMG_WIDTH);
		comboGrid->SetColLabelValue(i, wxString::Format("#%d", i));
	}

	comboGrid->SetRowLabelSize(IMG_WIDTH + 20);

	//Handle mouse clicks over grid to relocate the cell)
	comboGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, ::OnClickComboGrid);
	GUI_Controls.virtualGrid->GetGridWindow()->Bind(wxEVT_MOTION,OnMouseMoveOverGrid);

	//Associate Cell Locator with this grid
	Cell_Locator.SetGrid(comboGrid);

	comboGrid->Update();
	comboGrid->Refresh();
}

void AddRow(CComboGrid *grid, unsigned int defaultDelay, unsigned int rowPos)
{
	try
	{
		wxString str = wxString::Format("%d", defaultDelay);

		grid->InsertRows(rowPos, 1, true);
		grid->SetCellValue(rowPos, 0, str);

		//Resize column width and change label
		for (int i = 1; i < 19; ++i)
		{
			grid->SetColumnWidth(i, IMG_WIDTH);
			grid->SetColLabelValue(i, wxString::Format("#%d", i));
		}
		grid->SetRowSize(rowPos, IMG_WIDTH);
		//Set Column 0 attr, the range of acceptable numbers from 1 to 99999 (delay values)
		//Setup attributes, seems like I have to do this for each new row!! to ensure I have
		//a spin control, otherwise, it will work but with no spin ctrl and no protection if the
		//delay value is not in range.
		wxGridCellAttr *attrDelayColumn = new wxGridCellAttr;
		attrDelayColumn->SetEditor(new wxGridCellNumberEditor(1, 99999));
		attrDelayColumn->SetBackgroundColour(wxColor(66, 66, 66));
		attrDelayColumn->SetTextColour(wxColor(255, 255, 255));
		attrDelayColumn->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false));
		attrDelayColumn->SetAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
		attrDelayColumn->SetReadOnly(false); //only first column is editable
		grid->SetColAttr(0, attrDelayColumn);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}	
}

/////Action Buttons Events
void OnClickNewAction(wxCommandEvent &ev)
{
	try
	{
		if (GUI_Controls.Combo.size() == 0)
		{
			wxMessageBox("You need to create a Combo first before you can add Actions.",
				"COMBO's list is empty!", wxICON_INFORMATION);
			return;
		}
		AddRow(GUI_Controls.virtualGrid,
			GUI_Controls.spnDefaultDelay->GetValue(),
			GUI_Controls.virtualGrid->GetNumberRows());

		for (int c = 0; c < GUI_Controls.virtualGrid->GetNumberRows(); ++c)
			GUI_Controls.virtualGrid->SetRowHeight(c, IMG_WIDTH);

		//Adding new action, set cursor to the last action and first button
		Cell_Locator.SetLocation(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void OnClickDeleteLastAction(wxCommandEvent &ev)
{
	try
	{
		if (GUI_Controls.Combo.size() > 0)
		{
			if (GUI_Controls.virtualGrid->GetNumberRows() == 1)
			{
				//Minimum requirement: to have at least 1 action to have a combo, even if it is empty
				GUI_Controls.virtualGrid->DeleteRows(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
				AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);
				Cell_Locator.SetLocation(0, 1);
			}
			else
				GUI_Controls.virtualGrid->DeleteRows(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
		}
		else
			wxMessageBox("There are no Actions, and you did not create a COMBO!!", "No Actions, nor a Combo:",
			wxICON_INFORMATION);

		//if current location was deleted with the deleted action, relocate to a valid location (last row, 2nd column)
		Cell_Locator.TestAndCorrectLocation();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void OnClickInsertAction(wxCommandEvent &ev)
{
	try
	{
		//See OnClickDeleteSelectedActions below for more details about this
		wxArrayInt selectedRows;
		for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
		{
			bool blnIsRowSelected = false;
			for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				if (GUI_Controls.virtualGrid->IsInSelection(row, col))
					blnIsRowSelected = true;
				else
				{
					blnIsRowSelected = false;
					break;	//If even one cell not selected in a row, then the row is not selected, so skip to next row				
				}

			if (blnIsRowSelected)
				selectedRows.push_back(row);
		}

		if (selectedRows.empty())
		{
			//Long error & help message :)
			wxMessageBox("You have to specify where to insert the new Action first, so select an Action\n"
				"by clicking on the Action number to make sure the whole action is selected.\n"
				"You have to select at least ONE whole Action before you insert another.\n"
				"You can select more than one by holding CTRL or SHIFT keys, and new Actions\nwill be inserted there.\n\n"
				"Lastly, the new inserted Action will have the same 'RELATIVE' position as the\none "
				"you selected. Thus, the remaining Actions will be pushed further down the"
				"\nlist. Even if you selected more than one action, the new ones will retain the relative\n"
				"position to other nearby Actions.",
				"New Action location is unknown!",
				wxICON_EXCLAMATION);
			return;
		}

		for (unsigned int i = 0; i < (unsigned int)selectedRows.size(); ++i)
		{
			//Move grid cursor outside of the table, before we insert anything
			//to avoid hard to fix problems related to grid-cursor's previous location
			Cell_Locator.SetLocation(-1, -1);

			AddRow(GUI_Controls.virtualGrid,
				GUI_Controls.spnDefaultDelay->GetValue(),
				selectedRows[i]);
			for (unsigned int j = i; j < (unsigned int)selectedRows.size(); ++j)
				selectedRows[j] += 1;
		}

		for (int i = 0; i < GUI_Controls.virtualGrid->GetNumberRows(); ++i)
			GUI_Controls.virtualGrid->DeselectRow(i);

		//Move grid cursor to the first inserted action (whether it is one or more)
		Cell_Locator.SetLocation(selectedRows[0] - 1, 1);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void OnClickDeleteSelectedActions(wxCommandEvent &ev)
{
	try
	{
		/*
		Check this ticket for wxWidgets, GetSelectedRows() is not reliable and doesn't work correctly.
		The documentation says it is intended and I have to use GetSlectionTopLeft/BottomRight instead,
		I say this is BS. I hate wxWidgets more now.
		http://trac.wxwidgets.org/changeset/54665
		//wxArrayInt selectedRows = GUI_Controls.virtualGrid->GetSelectedRows(); <- Doesn't work.
		*/

		//My implementation works better :)
		wxArrayInt selectedRows;
		for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
		{
			bool blnIsRowSelected = false;
			for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				if (GUI_Controls.virtualGrid->IsInSelection(row, col))
					blnIsRowSelected = true;
				else
				{
					blnIsRowSelected = false;
					break;	//If even one cell not selected in a row, then the row is not selected, so skip to next row				
				}

			if (blnIsRowSelected)
				selectedRows.push_back(row);
		}

		if (selectedRows.empty())
		{
			//Long error & help message :)
			wxMessageBox("You have to specify which Actions you want to delete first, so select an Action by clicking on\n"
				"the Action number to make sure the whole action is selected.\n\n"
				"You can select more than one Action by holding CTRL key or SHIFT key while clicking on the\nAction number. "
				"Or even simply by dragging the mouse to highlight the whole row then delete the Action.\n\n"
				"Please note that if some cells are highlighted, that doesn't mean the row was selected!\n\n"
				"You can highlight All Actions simply by clicking on the empty Square to the left of 'Delay'\n"
				"and above Action numbers, then delete them by clicking on 'Delete Selected Actions' button.\n\n"
				"Note: If you deleted all Actions, another Action will be created by default, A COMBO needs\n"
				"at least one empty Action. The created Action will have the delay value specified in the Delay\n"
				"field next to the COMBO's name.\n",
				"Action location is unknown!", wxICON_INFORMATION);
			return;
		}

		for (int i = selectedRows.GetCount() - 1; i >= 0; --i)
		{
			GUI_Controls.virtualGrid->DeleteRows(selectedRows[i], 1, true);

			//GUI_Controls.virtualGrid->DeleteRows(selectedRows[i], 1, true);
		}

		//Minimum requirement: to have at least 1 action to have a combo, even if it is empty
		if (GUI_Controls.virtualGrid->GetNumberRows() == 0)
			AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);

		//if current location was deleted with the deleted action, relocate to a valid location (last row, 2nd column)
		Cell_Locator.TestAndCorrectLocation();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

/////Combo Buttons Events
void OnClickNewCombo(wxCommandEvent &ev)
{
	try
	{
		wxString strResponse = wxGetTextFromUser("Enter a name for the new Combo:",
			"New COMBO name", "I am a Combo!");

		if (strResponse == wxEmptyString)
			return;

		//Save Current Combo (if not already saved)
		//Clear grid


		//Add name for combo box
		GUI_Controls.cmbComboName->Append(strResponse);

		//Since this is a sorted ComboBox, position or index doesn't mean anything at all.
		GUI_Controls.cmbComboName->Select(GUI_Controls.cmbComboName->FindString(strResponse, true));

		//Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
		CCombo newCombo(1, GUI_Controls.spnDefaultDelay->GetValue());
		newCombo.SetName(strResponse);
		GUI_Controls.Combo.push_back(newCombo);
		//Add first row for the new combo (minimum requirement for a combo is 1 action)
		AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);
		
		Cell_Locator.SetLocation(0, 1);
		//still some stuff to do..
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void OnClickDeleteCombo(wxCommandEvent &ev)
{
	try
	{
		//Clear current grid then delete combo and combo name from combo box

		//Clear grid - delete combo

		//Delete name from combo box
		if (GUI_Controls.cmbComboName->GetSelection() < 0)		//prevent deletion of none selected item
			return;

		wxString strTemp = GUI_Controls.cmbComboName->GetValue();
		GUI_Controls.cmbComboName->Delete(GUI_Controls.cmbComboName->GetSelection());

		//After deletion, select the last combo by default
		if (GUI_Controls.cmbComboName->GetCount() >= 1)
			GUI_Controls.cmbComboName->Select(GUI_Controls.cmbComboName->GetCount() - 1);

		//Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
		for (unsigned int i = 0; i < GUI_Controls.Combo.size(); ++i)
			if (GUI_Controls.Combo[i].GetName() == strTemp)
				GUI_Controls.Combo.erase(GUI_Controls.Combo.begin() + i);
		//still some stuff to do...
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

void OnClickRenameCombo(wxCommandEvent &ev)
{
	try
	{
		if (GUI_Controls.cmbComboName->GetCount() == 0)
		{
			wxMessageBox("You didn't select a COMBO to rename!", "Rename Failed", wxICON_EXCLAMATION);
			return;
		}

		wxString strResponse = wxGetTextFromUser("Enter a new name for the Combo:",
			"New COMBO name", "I am a Combo!");

		if (strResponse != wxEmptyString)
			GUI_Controls.cmbComboName->SetString(GUI_Controls.cmbComboName->GetSelection(), strResponse);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

//////Combo Key
void OnClickComboKey(wxMouseEvent &ev)
{
	try
	{
		wxMessageBox("Clicked 'Combo Key'");
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

//Called from the click event function (that handles both keyboard and combo button clicks) to handle combo buttons 
void OnClick_psComboButtons(int winID)
{
	try
	{
		if (winID >= 1024 && winID <= 1047)	//Combo tab
		{
			//Implement adding psComboButtons into the grid
			//wxMessageBox(PS_LABEL[winID - 1024].name + " was clicked!");

			
			CCellValue val;
			val.resourceFile = PS_LABEL[winID - 1024].name;
			val.buttonValue = winID - 1024;

			wxGridCellCoords coords;
			Cell_Locator.GetLocation(coords);
			GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, &val);
			GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
			GUI_Controls.virtualGrid->Update();
			GUI_Controls.virtualGrid->Refresh();

			if (coords.GetCol() <= 16)
				Cell_Locator.MoveToNextButton();
			else
				Cell_Locator.MoveToNextAction();

			Cell_Locator.TestAndCorrectLocation();
			//Cell_Locator.GetLocation(coords);
		}
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
}

/////Grid mouse click
void OnClickComboGrid(wxGridEvent &ev)
{
	unsigned int row, col;
	row = ev.GetRow();
	col = ev.GetCol();
	//Clear any current highlights too
	GUI_Controls.virtualGrid->ClearSelection();
	GUI_Controls.virtualGrid->Refresh();
	GUI_Controls.virtualGrid->SetGridCursor(row, col);	//to allow dragging and selection too
	
	//Move cursor to the selected cell coordinates
	Cell_Locator.SetLocation(row, col);
	ev.Skip();
}

//Show tooltip "Which button" underneath, when mouse hovering over grid cells
void OnMouseMoveOverGrid(wxMouseEvent &ev)
{
	wxPoint mousePos, cellPos;

	mousePos = GUI_Controls.virtualGrid->CalcUnscrolledPosition(ev.GetPosition());
	cellPos.y = GUI_Controls.virtualGrid->YToRow(mousePos.y);	//row
	cellPos.x = GUI_Controls.virtualGrid->XToCol(mousePos.x);	//column

	if (cellPos.x == wxNOT_FOUND || cellPos.y == wxNOT_FOUND)
	{
		ev.Skip();
		return;
	}

	wxString cellValue = GUI_Controls.virtualGrid->GetCellValue(cellPos.y, cellPos.x);
	if (cellPos.x == 0)
		cellValue = "Delay: Repeat this Action for " + cellValue + " frames.";
	GUI_Controls.virtualGrid->GetGridWindow()->SetToolTip(cellValue);
	ev.Skip();
}