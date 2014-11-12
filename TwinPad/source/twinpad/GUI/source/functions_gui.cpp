#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>

#include "twinpad_gui.h"

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/grid.h"

#include "wx/mstream.h"		//for wxMemoryInputStream

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
//Will be replaced with given dir from emu (if PADgetSettingDir() is supported by emu.)
wxString g_Path = "inis/"; 
const wxString g_HEADER_TWINPAD = "[TwinPad Configurations v1.6]";
const wxString g_HEADER_TWINPAD_COMBO = "[TwinPad COMBO Configurations v1.1]";
const wxString g_TWIN_PAD = "TwinPad.ini";
const wxString g_TWIN_PAD_COMBOS = "TwinPad_COMBOs.ini";

const int IMG_WIDTH = 40;	//40 pixels

//Sorry for the inconsistencies, using fstream and wxfile at the same time, just wanted to try both :)

void CreateNullFile()
{
	try
	{
		int counter;
	
		string file = g_Path + g_TWIN_PAD;
		string strPad;

		ofstream m_nullfile(file.c_str(), ios::out);

		if (!m_nullfile.is_open())
		{
			wxMessageBox("Couldn't create TwinPad.ini configuration file into the specified location!\n\n"
				"\nMake sure at least 'inis' folder exists in the same directory with the emu."
				"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
			GUI_Controls.mainFrame->Close(true);
		}

		//Write header and config version number
		m_nullfile << g_HEADER_TWINPAD << endl;
		//Write assigned keys for both pads
		for(int pad = 0; pad <= 1; pad++)
		{
			if (pad == 0) 
				strPad = "[0][";
			else
				strPad = "[1][";
		
			counter = 0;
			while(counter < 24)		//24 PS buttons
			{
				m_nullfile << strPad << counter << "] = 0x0" << endl;
				GUI_Config.m_pad[pad][counter] = 0;
				counter++;
			}
			m_nullfile << strPad << counter << "] = 0x0" << endl;  //for walk/run value
		}

		//Write Mouse configuration
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

		//Write 'Extra Options' configuration
		counter = 0;
		while(counter <= 6)
		{
			m_nullfile << "0" << endl;
			GUI_Config.m_extra[counter] = 0;
			counter++;
		}
	
		m_nullfile.close();
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

void CreateNullComboFile()
{
	try
	{
		string file = g_Path + g_TWIN_PAD_COMBOS;
		ofstream m_txtFile(file.c_str(), ios::out);

		if (!m_txtFile.is_open())
		{
			wxMessageBox("Couldn't create TwinPad_Combos.ini configuration file into the specified location!\n\n"
				"\nMake sure at least 'inis' folder exists in the same directory with the emu."
				"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
			::exit(0);
		}

		m_txtFile << g_HEADER_TWINPAD_COMBO << endl;
		m_txtFile << "ComboCount\t= 0\n" << "ComboPad\t= 0\n";

		m_txtFile.close();
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

bool CheckAndCreateIfNecessary(const string &file, const string &header)
{
	try
	{
		ifstream f(file);
		
		string str = "";
		wxString strMsg = "";

		int select = 0;
		if (header == g_HEADER_TWINPAD)
		{
			strMsg = "Resetting Configurations in 'TwinPad.ini' File, please reconfigure the plugin.";
			select = 1;
		}
		else if (header == g_HEADER_TWINPAD_COMBO)
		{
			strMsg = "Resetting Configurations in 'TwinPad_COMBOs.ini' File, please reconfigure the plugin.";
			select = 2;
		}
		else
		{
			wxMessageBox("Wrong header passed to 'IsFileOk()' function. Inform the Author please.\nHeader: " + header);
			return false;
		}

		if (!select)
			wxMessageBox(strMsg, "Reset configuration file:", wxICON_INFORMATION);

		//First: Check if file can be opened
		if (f.is_open())
		{
			//Second: Check the Header if version is compatible
			getline(f, str);
			if (select == 1)
			{
				if (str != g_HEADER_TWINPAD)
				{
					f.close();
					CreateNullFile();
					wxMessageBox(wxString::Format("TwinPad configuration file is old, all previous settings are lost.\n\n"
						"Old version: %s\nNew version: %s", str, g_HEADER_TWINPAD),
						"Oops!", wxICON_INFORMATION);
					return true;	//Create null file, then is OK
				}
				else
					return true;	//File is OK
			}
			else if (select == 2)
			{
				if (str != g_HEADER_TWINPAD_COMBO)
				{
					f.close();
					CreateNullComboFile();
					wxMessageBox(wxString::Format("TwinPad COMBOs configuration file is old, all previous settings are lost.\n\n"
						"Old version: %s\nNew version: %s", str, g_HEADER_TWINPAD_COMBO),
						"Oops!", wxICON_INFORMATION);
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
	catch (exception &ex)
	{
		wxMessageBox(ex.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
	return false; //shouldn't reach here
}

void Loading_TwinPad_Main_Config()
{
	try
	{
		wxString fileName = g_Path + g_TWIN_PAD;
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

		//Get header and skip it
		line = file.GetFirstLine();
		//Read the two pads configurations and Walk/Run key
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

		//Read mouse buttons and scrollup/down configuration
		for(int i = 0; i < 10; i++)
		{
			line = file.GetNextLine();
			token.SetString(line, " ", wxTOKEN_STRTOK);
			subStr = token.GetNextToken();	//skips first token [n]
			subStr = token.GetNextToken();	//skips second token =
			subStr = token.GetNextToken();	//we are interested in this token N (DECIMAL)
			subStr.ToLong(&val, 10);		//Convert string to dec
			GUI_Config.m_mouse[i] = val;
		}

		//Read 'Extra Options' configuration
		for(int i = 0; i < 7; i++)
		{
			line = file.GetNextLine();
			line.ToLong(&val, 10);
			GUI_Config.m_extra[i] = val;
		}
		file.Close();
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
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

//This function is called from LoadResources() and SetCustomCellRenderer()
void GetImageData(void* &data, unsigned int *length, const unsigned int index)
{
	//I know this is awkward, but I am not gonna dig into the resources.h 
	//to fix the arrays into 1 and use an index to make this more concise and pithy.
	switch (index)
	{
	case L2:
		data = (void*)L2_GIF;
		*length = sizeof(L2_GIF);
		break;
	case R2:
		data = (void*)R2_GIF;
		*length = sizeof(R2_GIF);
		break;
	case L1:
		data = (void*)L1_GIF;
		*length = sizeof(L1_GIF);
		break;
	case R1:
		data = (void*)R1_GIF;
		*length = sizeof(R1_GIF);
		break;
	case TRIANGLE:
		data = (void*)TRIANGLE_GIF;
		*length = sizeof(TRIANGLE_GIF);
		break;
	case CIRCLE:
		data = (void*)CIRCLE_GIF;
		*length = sizeof(CIRCLE_GIF);
		break;
	case CROSS:
		data = (void*)CROSS_GIF;
		*length = sizeof(CROSS_GIF);
		break;
	case SQUARE:
		data = (void*)SQUARE_GIF;
		*length = sizeof(SQUARE_GIF);
		break;
	case SELECT:
		data = (void*)SELECT_GIF;
		*length = sizeof(SELECT_GIF);
		break;
	case L3:
		data = (void*)L3_GIF;
		*length = sizeof(L3_GIF);
		break;
	case R3:
		data = (void*)R3_GIF;
		*length = sizeof(R3_GIF);
		break;
	case START:
		data = (void*)START_GIF;
		*length = sizeof(START_GIF);
		break;
	case UP:
		data = (void*)UP_GIF;
		*length = sizeof(UP_GIF);
		break;
	case RIGHT:
		data = (void*)RIGHT_GIF;
		*length = sizeof(RIGHT_GIF);
		break;
	case DOWN:
		data = (void*)DOWN_GIF;
		*length = sizeof(DOWN_GIF);
		break;
	case LEFT:
		data = (void*)LEFT_GIF;
		*length = sizeof(LEFT_GIF);
		break;
	case LANALOG_UP:
	case RANALOG_UP:
		data = (void*)ANALOG_UP_GIF;
		*length = sizeof(ANALOG_UP_GIF);
		break;
	case LANALOG_RIGHT:
	case RANALOG_RIGHT:
		data = (void*)ANALOG_RIGHT_GIF;
		*length = sizeof(ANALOG_RIGHT_GIF);
		break;
	case LANALOG_DOWN:
	case RANALOG_DOWN:
		data = (void*)ANALOG_DOWN_GIF;
		*length = sizeof(ANALOG_DOWN_GIF);
		break;
	case LANALOG_LEFT:
	case RANALOG_LEFT:
		data = (void*)ANALOG_LEFT_GIF;
		*length = sizeof(ANALOG_LEFT_GIF);
		break;
	default:
		wxMessageBox(wxString::Format("Unknown button number in GetImageData(): Button #%d", index));
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
	const int MAX_COLUMNS = 19;
	wxString strArrPadChoices[2] = { "Pad 1", "Pad 2" };
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
		wxStaticBoxSizer *topLevelSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Key Combinations (COMBOs)");
			wxStaticBoxSizer *stcComboEditorSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "COMBO Editor");
				GUI_Controls.btnNewCombo = new wxButton(panel, wxID_ANY, "&New Combo");
				GUI_Controls.btnDeleteCombo = new wxButton(panel, wxID_ANY, "&Delete Combo");
				GUI_Controls.btnRenameCombo = new wxButton(panel, wxID_ANY, "&Rename Combo");
				stcComboEditorSizer->Add(GUI_Controls.btnNewCombo);
				stcComboEditorSizer->AddSpacer(5);
				stcComboEditorSizer->Add(GUI_Controls.btnDeleteCombo);
				stcComboEditorSizer->AddSpacer(5);
				stcComboEditorSizer->Add(GUI_Controls.btnRenameCombo);
				//stcComboEditorSizer->AddSpacer(10);
				GUI_Controls.btnNewCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickNewCombo);
				GUI_Controls.btnDeleteCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteCombo);
				GUI_Controls.btnRenameCombo->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickRenameCombo);
			wxStaticBoxSizer *stcComboNameSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "COMBO Name");
				GUI_Controls.cmbComboName = new wxComboBox(panel, wxID_ANY, wxEmptyString, 
																wxDefaultPosition, wxSize(200,25), 0, 0, wxCB_READONLY | wxCB_SORT);
				GUI_Controls.cmbComboName->SetBackgroundColour(wxColor(66,66,66));	//Dark Grey
				GUI_Controls.cmbComboName->SetForegroundColour(wxColor("White"));
				GUI_Controls.cmbComboName->Bind(wxEVT_COMBOBOX, ::OnChangeComboName);
				GUI_Controls.cmbComboName->Bind(wxEVT_KEY_UP, ::OnChangeComboNameKey);
				stcComboNameSizer->Add(GUI_Controls.cmbComboName, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
			wxStaticBoxSizer *stcComboKeySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "KEY");
				GUI_Controls.txtComboKey = new wxTextCtrl(panel, wxID_ANY, "NONE", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
				GUI_Controls.txtComboKey->SetBackgroundColour(wxColor(66,66,66));
				GUI_Controls.txtComboKey->SetForegroundColour(wxColor("White"));
				GUI_Controls.txtComboKey->Bind(wxEVT_LEFT_UP, ::OnClickComboKey);	//Get a key
				GUI_Controls.txtComboKey->Bind(wxEVT_RIGHT_UP, ::OnClickComboKey);	//Delete the key
				stcComboKeySizer->Add(GUI_Controls.txtComboKey, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
			wxStaticBoxSizer *stcDefaultDelaySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Delay");	
				GUI_Controls.spnDefaultDelay = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, 
								wxSize(65,25), 16896L, 1, 99999, 3); /*Min 1, Max 99999, Default 3*/
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
				CComboGrid *comboGrid = new CComboGrid(panel, wxID_ANY, wxPoint(1,1), wxSize(665,390));
				//Complex table holds various data in a cell accessed via grid
				CTableBase *tableBase = new CTableBase();
				tableBase->SetAttrProvider(new CGridCellAttrProvider);
				//We need pointers to the grid and table so we can read their values from Keyboard Tab to save configurations
				GUI_Controls.virtualGrid = comboGrid;
				GUI_Controls.tableBaseGrid = tableBase;
				comboGrid->SetTable(tableBase, true);
				//Setup attributes
				wxGridCellAttr *attrReadOnly = new wxGridCellAttr, *attrDelayColumn = new wxGridCellAttr;
				comboGrid->InsertCols(0, MAX_COLUMNS);		//0: Delay, 1-18 Buttons (columns# fixed, rows# not)
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
				for(int i = 1; i < comboGrid->GetNumberCols(); ++i)					//column 1-18, PS2 buttons images (no text)
				{
					comboGrid->SetColAttr(i, attrReadOnly);
					//Bug in wxWidgets 2.9.4 and up!!?? Ticket #4401, it says fixed, but not here :/
					//Otherwise, a crash when exiting TwinPad, Assert about DecRef.. I hate wxWidgets now.
					//See: http://forums.wxwidgets.org/viewtopic.php?t=36159&p=147976
					attrReadOnly->IncRef();		//Also, added DecRef in the virtual destructor
				}
				comboGrid->SetSelectionBackground(wxColor("#990000"));	//Crimson -bloody red- :)
				comboGridSizer->Add(comboGrid, 0, 0, 5);
			wxBoxSizer *actionAndButtonSizer = new wxBoxSizer(wxVERTICAL);
				wxStaticBoxSizer *editComboSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Action Editor");
					GUI_Controls.btnNewAction = new wxButton(panel, wxID_ANY, "New &Action");
					GUI_Controls.btnDeleteLastAction = new wxButton(panel, wxID_ANY, "Delete &Last Action");
					GUI_Controls.btnInsertActions = new wxButton(panel, wxID_ANY, "Ins&ert Actions");
					GUI_Controls.btnInsertInbetweenAction = new wxButton(panel, wxID_ANY, "&Insert Inbetween Actions");
					GUI_Controls.btnDeleteSelectedActions = new wxButton(panel, wxID_ANY, "Delete &Selected Actions");
					GUI_Controls.btnDeleteButton = new wxButton(panel, wxID_ANY, "Delete Selected &Button");
					wxSize largestButtonSize = GUI_Controls.btnInsertInbetweenAction->GetSize();
					GUI_Controls.btnNewAction->SetMinSize(largestButtonSize);
					GUI_Controls.btnDeleteLastAction->SetMinSize(largestButtonSize);
					GUI_Controls.btnInsertInbetweenAction->SetMinSize(largestButtonSize);
					GUI_Controls.btnNewAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickNewAction);
					GUI_Controls.btnDeleteLastAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteLastAction);
					GUI_Controls.btnInsertInbetweenAction->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickInsertInbetweenAction);
					GUI_Controls.btnInsertActions->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickInsertAction);
					GUI_Controls.btnDeleteSelectedActions->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteSelectedActions);
					GUI_Controls.btnDeleteButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, ::OnClickDeleteButton);
					editComboSizer->Add(GUI_Controls.btnNewAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteLastAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnInsertActions, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnInsertInbetweenAction, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteSelectedActions, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteButton, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
				wxStaticBoxSizer *whichPadSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Which Pad?");
					GUI_Controls.cmbWhichPad = new wxComboBox(panel, wxID_ANY, strArrPadChoices[0], wxDefaultPosition, 
						wxDefaultSize, 2, strArrPadChoices, wxCB_READONLY | wxCB_SORT);
					GUI_Controls.cmbWhichPad->SetBackgroundColour(wxColor(66, 66, 66));	//Dark Grey
					GUI_Controls.cmbWhichPad->SetForegroundColour(wxColor("White"));
					GUI_Controls.cmbWhichPad->SetMinSize(largestButtonSize);
					whichPadSizer->Add(GUI_Controls.cmbWhichPad, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
				wxStaticBoxSizer *sensitivitySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Button Sensitivity");
					/*Min 0, Max 255. Default 255 for buttons. Max is 0 (Up/Left) or 255 (Right/Down) for analogs*/
					GUI_Controls.spnSensitivity = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
						wxDefaultSize, 16896L, 0, 255, 0);	//Initial is 0, will be changed depending on button selected
					GUI_Controls.spnSensitivity->SetBackgroundColour(wxColor(66, 66, 66));
					GUI_Controls.spnSensitivity->SetForegroundColour(wxColor("White"));
					GUI_Controls.spnSensitivity->SetMinSize(largestButtonSize);
					GUI_Controls.spnSensitivity->Bind(wxEVT_SPINCTRL, ::OnChangeSensitivity);
					sensitivitySizer->Add(GUI_Controls.spnSensitivity, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5);
					actionAndButtonSizer->Add(editComboSizer);
					actionAndButtonSizer->Add(whichPadSizer);
					actionAndButtonSizer->Add(sensitivitySizer);
				midLevelSizer->Add(comboGridSizer);
				midLevelSizer->Add(actionAndButtonSizer, 1, wxEXPAND | wxALIGN_CENTER, 5);
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

	//finishing touches
	wxToolTip *ttpSpnSensitivity = new wxToolTip("For Buttons: Maximum value for buttons is 255, and Minimum is 0.\nDefault is Maximum.\n\n"
		"For both Analog Sticks: Maximum is 0 for LEFT/UP, while Maximum is 255 for DOWN/RIGHT. Minimum is 128 (stick is released or not moving). "
		"Default is Maximum whether it is 0 or 255 for Analog Sticks.\n\n"
		"You have to select a button before you can modify its own default sensitivity.");
	ttpSpnSensitivity->SetDelay(500);		//0.5 second
	ttpSpnSensitivity->SetAutoPop(30000);	//30 seconds
	wxToolTip *ttpSpnDefaultDelay = new wxToolTip("Number of frames the current Action will be repeated (executed).");
	ttpSpnDefaultDelay->SetDelay(500);		//0.5 second
	ttpSpnDefaultDelay->SetAutoPop(30000);	//30 seconds
	wxToolTip *ttpTxtComboKey = new wxToolTip("Left-Click: And then 'press any key' to assign it to the current Combo.\n"
											  "Right-Click: To erase the configured Key.");
	ttpTxtComboKey->SetDelay(500);			//0.5 second
	ttpTxtComboKey->SetAutoPop(30000);		//30 seconds
	GUI_Controls.spnSensitivity->SetToolTip(ttpSpnSensitivity);
	GUI_Controls.spnDefaultDelay->SetToolTip(ttpSpnDefaultDelay);
	GUI_Controls.txtComboKey->SetToolTip(ttpTxtComboKey);


	comboGrid->DisableDragColSize();			//Prevent mouse from resizing rows and columns
	comboGrid->DisableDragRowSize();
	
	comboGrid->SetColFormatNumber(0);			//first column accepts integer numbers only
	comboGrid->SetColLabelValue(0, "Delay");

	comboGrid->SetCellHighlightPenWidth(3);		//for Delay
	comboGrid->SetCellHighlightROPenWidth(3);	//Thicker black border around selected "read only" cell

	comboGrid->SetDoubleBuffered(true);		//If supported by platform, it will be enabled

	//Set timer to re-adjust animation sync for Analog Sticks
	GUI_Controls.tmrReAnimate = new CReAnimate();
	GUI_Controls.tmrReAnimate->Start(30000);	//30 seconds
	//Allocate memory but don't start it yet, the user will start the timer when clicking on Combo Key
	GUI_Controls.tmrGetComboKey = new CGetComboKey();

	//Set label and width
	for(int i = 1; i < comboGrid->GetNumberCols(); ++i)
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
		for (int i = 1; i < grid->GetNumberCols(); ++i)
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
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

/////Action Buttons Events
void OnClickNewAction(wxCommandEvent &ev)
{
	try
	{
		if (GUI_Controls.Combos.size() == 0)
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
		//Modify sensitivity inside the SpinControl
		ModifySensitivity();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

void OnClickDeleteLastAction(wxCommandEvent &ev)
{
	try
	{
		if (GUI_Controls.Combos.size() > 0)
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
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

void OnClickInsertAction(wxCommandEvent &ev)
{
	try
	{
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
			wxMessageBox("Both 'Insert Actions' & 'Insert Inbetween Actions' work the same for a single Action. "
				"But they differ when trying to insert more than one Action at once. Please read to know the difference:\n\n"
				"You have to specify where to insert the new Action first, so select an Action "
				"by clicking on the Action number to make sure the whole action is selected. "
				"You have to select at least ONE whole Action before you insert another. "
				"You can select more than one by holding CTRL or SHIFT keys, and new Actions\nwill be inserted there.\n\n"
				"Lastly, selected Actions have to be next to each other unlike 'INSERT INBETWEEN ACTIONS'. "
				"The new inserted Action will have the same 'ABSOLUTE' position (same Action number) as the one "
				"you selected. Thus, the remaining Actions will be pushed further down the list. "
				"Even if you selected more than one Action, the new ones will retain the absolute "
				"position to other nearby Actions.\n\nFor example, it is very similar to Microsoft Excel when inserting new rows. :)",
				"New Action location is unknown!",
				wxICON_EXCLAMATION);
			return;
		}

		if (selectedRows.size() > 1)
			for (int next = 1, previous = 0; next < (signed) selectedRows.size(); ++next, ++previous)
			{
				if (selectedRows[next] - selectedRows[previous] > 1)
				{
					wxMessageBox("Actions selected have to be adjacent to one another. Otherwise, choose\n"
						"'Insert Inbetween Actions' button.", "Not contiguous Actions!", wxICON_EXCLAMATION);
					return;
				}
			}

		for (unsigned int i = 0; i < (unsigned int)selectedRows.size(); ++i)
		{
			//Move grid cursor outside of the table, before we insert anything
			//to avoid hard to fix problems related to grid-cursor's previous location
			Cell_Locator.SetLocation(-1, -1);

			AddRow(GUI_Controls.virtualGrid,
				GUI_Controls.spnDefaultDelay->GetValue(),
				selectedRows[0]);
		}

		GUI_Controls.virtualGrid->ClearSelection();

		if (selectedRows[0] == 0)
			Cell_Locator.SetLocation(0, 1);
		else
			Cell_Locator.SetLocation(selectedRows[0], 1);
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

void OnClickInsertInbetweenAction(wxCommandEvent &ev)
{
	try
	{
		//See OnClickDeleteSelectedActions below for more details about this
		wxArrayInt selectedRows;
		for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
		{
			bool IsRowSelected = false;
			for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				if (GUI_Controls.virtualGrid->IsInSelection(row, col))
					IsRowSelected = true;
				else
				{
					IsRowSelected = false;
					break;	//If even one cell not selected in a row, then the row is not selected, so skip to next row				
				}

			if (IsRowSelected)
				selectedRows.push_back(row);
		}

		if (selectedRows.empty())
		{
			//Long error & help message :)
			wxMessageBox("Both 'Insert Actions' & 'Insert Inbetween Actions' work the same for a single Action. "
				"But they differ when trying to insert more than one Action at once. Please read to know the difference:\n\n"
				"You have to specify where to insert the new Action first, so select an Action "
				"by clicking on the Action number to make sure the whole action is selected. "
				"You have to select at least ONE whole Action before you insert another. "
				"You can select more than one by holding CTRL or SHIFT keys, and new Actions will be inserted there.\n\n"
				"Lastly, the new inserted Action will have the same 'RELATIVE' position as the one "
				"you selected (relative to the Action above and below. Thus, the remaining Actions will be pushed "
				"further down the list. Even if you selected more than one Action, the new ones will retain the relative "
				"position to other nearby Actions.\n\n"
				"This is a unique way of inserting rows (a.k.a Actions), I am not aware of another application can do this "
				"to compare the behavior with this one. :)",
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

		GUI_Controls.virtualGrid->ClearSelection();

		//Move grid cursor to the first inserted action (whether it is one or more)
		//Note that if for example the first selected was row 0, now it is row 1 since it was shifted down 1 row
		Cell_Locator.SetLocation(selectedRows[0] - 1, 1);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
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
			bool IsRowSelected = false;
			for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				if (GUI_Controls.virtualGrid->IsInSelection(row, col))
					IsRowSelected = true;
				else
				{
					IsRowSelected = false;
					break;	//If even one cell not selected in a row, then the row is not selected, so skip to next row				
				}

			if (IsRowSelected)
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

		//Save current cell location
		wxGridCellCoords coords;
		Cell_Locator.GetLocation(coords);
		//Put Cell Locator out of the grid
		Cell_Locator.SetLocation(-1, -1);
		for (int i = selectedRows.GetCount() - 1; i >= 0; --i)
			GUI_Controls.virtualGrid->DeleteRows(selectedRows[i], 1, true);

		//Minimum requirement: to have at least 1 action to have a combo, even if it is empty
		if (GUI_Controls.virtualGrid->GetNumberRows() == 0)
			AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);

		Cell_Locator.SetLocation(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

void OnClickDeleteButton(wxCommandEvent &ev)
{
	try
	{
		wxGridCellCoords coords;
		CCellValue val;
		Cell_Locator.GetLocation(coords);
		val.buttonValue = -1;			//-1 is empty button, since 0 means L2
		val.buttonSensitivity = -1;		//Illegal value
		val.buttonName = "";			//empty button

		GUI_Controls.spnSensitivity->SetValue(0);
		GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, &val);
		GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
		GUI_Controls.virtualGrid->Update();
		GUI_Controls.virtualGrid->Refresh();
		GUI_Controls.virtualGrid->SetFocus();
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
	return;
}

/////Combo Buttons Events
void OnClickNewCombo(wxCommandEvent &ev)
{
	try
	{
		static int counter = 1;
		wxString strResponse = wxGetTextFromUser("Enter a name for the new Combo:",
			"New COMBO name", wxString::Format("I am a Combo! #%d", counter++));
		//Don't accept empty COMBO name
		if (strResponse == wxEmptyString)
			return;
		//Don't accept duplicate COMBO names, we use the unique name to draw on grid
		for (unsigned int i = 0; i < GUI_Controls.Combos.size(); ++i)
			if (strResponse == GUI_Controls.Combos[i]->GetName())
			{
				wxMessageBox("There is another COMBO with the same name! Please choose a different name.\n\n"
					"Note: Name is NOT case sensitive, that is 'Orange', 'oRange' or 'ORANGE' are not the same name.",
					"Duplicate COMBO name found!", wxICON_EXCLAMATION);
				return;
			}
		
		//Save current Combo (before making a new one) from grid to Combos container if we have at least 1 Combo
		//It is very important that we use the cmbComboName to get the name of current Combo. If the same Combo
		//name found in the container, it will be overwritten with this one.
		if(GUI_Controls.Combos.size() > 0)
			SaveGridToCombo(GUI_Controls.cmbComboName->GetStringSelection());
		
		//Clear grid - delete combo
		//If we don't have any COMBOs or the table doesn't exist, skip. Otherwise subscript out of range in Grid TableBase
		if (GUI_Controls.virtualGrid->GetNumberRows() > 0)
		{
			GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());
			GUI_Controls.virtualGrid->Update();
			GUI_Controls.virtualGrid->Refresh();
			GUI_Controls.virtualGrid->ForceRefresh();
			GUI_Controls.virtualGrid->SetFocus();
		}

		//Add name for combo box
		GUI_Controls.cmbComboName->Append(strResponse);

		//Since this is a sorted ComboBox, position or index doesn't mean anything at all.
		GUI_Controls.cmbComboName->Select(GUI_Controls.cmbComboName->FindString(strResponse, true));
		GUI_Controls.strPreviousComboSelection = strResponse;

		//We modified the new Combo, save it and overwrite the duplicate if there is one
		SaveGridToCombo(strResponse);

		//Add first row for the new combo (minimum requirement for a combo is 1 action)
		AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);

		Cell_Locator.SetLocation(0, 1);
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

//Clear current grid then delete combo and combo name from combo box
void OnClickDeleteCombo(wxCommandEvent &ev)
{
	try
	{
		//prevent deletion of none selected item (There are no COMBOs)
		if (GUI_Controls.cmbComboName->GetSelection() < 0 || GUI_Controls.Combos.size() == 0)
			return;

		//Clear grid - delete combo
		GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());
		
		for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
		{
			if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
			{
				GUI_Controls.Combos.erase(it);
				//We changed the iterator. If size is 0, there will be an error/exception if we continue the loop
				//because ++it will be called and it will point to an empty container, so silently get out of the loop
				break;
			}
		}

		//Delete name from combo box
		wxString strTemp = GUI_Controls.cmbComboName->GetValue();
		GUI_Controls.cmbComboName->Delete(GUI_Controls.cmbComboName->GetSelection());
		GUI_Controls.strPreviousComboSelection = "";

		//After deletion, select the first combo by default
		if (GUI_Controls.cmbComboName->GetCount() > 0)
		{
			GUI_Controls.cmbComboName->Select(0);
			GUI_Controls.strPreviousComboSelection = GUI_Controls.cmbComboName->GetStringSelection();
		}
			
		
		//Delete all rows of grid
		GUI_Controls.virtualGrid->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows(), true);
		
		//Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
		//Hide grid to prevent flickering while adding buttons, and it is much faster this way. Show grid when we are done
		GUI_Controls.virtualGrid->Hide();
		for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
		{
			if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
			{
				for (int row = 0; row < (*it)->GetNumberActions(); ++row)
				{
					CAction *curAction = (*it)->GetAction(row);
					int delay = curAction->GetDelay();
					//Add grid row, will populate it with buttons after that
					AddRow(GUI_Controls.virtualGrid, delay, GUI_Controls.virtualGrid->GetNumberRows());
					//Set Action Delay
					GUI_Controls.virtualGrid->SetCellValue(row, 0, wxString::Format("%d", delay));
					//Move to the first button and iterate to add them
					Cell_Locator.SetLocation(row, 1);
					for (int button = 0; button < curAction->GetNumberOfButtons(); ++button)
					{
						CCellValue *val;
						wxGridCellCoords coords;
						val = (CCellValue *)curAction->GetButton(button);
						Cell_Locator.GetLocation(coords);
						GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, val);
						GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
						Cell_Locator.MoveToNextButton();
					}
				}
				break;	//No need to process other COMBOs
			}
		}
		GUI_Controls.virtualGrid->Show(true); //Added all buttons! Show the grid
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
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
		{
			wxString name = GUI_Controls.cmbComboName->GetStringSelection();
			for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
			{
				if ((*it)->GetName() == name)
				{
					(*it)->SetName(strResponse);
					GUI_Controls.cmbComboName->SetString(GUI_Controls.cmbComboName->GetSelection(), strResponse);
					GUI_Controls.strPreviousComboSelection = strResponse;
					break;
				}
			}
			
		}
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

//////Combo Key
void OnClickComboKey(wxMouseEvent &ev)
{
	try
	{
		if (ev.RightUp())
		{
			wxMessageBox("Button is cleared!");
			return; //to suppress context menu
		}
		else if (ev.LeftUp())
			wxMessageBox("Clicked 'Combo Key'");
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

//Only called from OnClick_psComboButton()
bool Has(const unsigned int button, int row)
{
	//Check if 'button' exists in current action 'row' and return true. Otherwise return false.
	int buttonValue;
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	for (int i = 1; i < GUI_Controls.virtualGrid->GetNumberCols(); ++i)
	{
		buttonValue = ((CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, i, "Cell Value"))->buttonValue;
		//If button exists in same row, and not the same one 'cell' selected by user to overwrite, return true
		if (buttonValue == button && i != coords.GetCol())
			return true;
	}
	return false;
}

//Called from the click event function (that handles both keyboard and combo button clicks) to handle combo buttons 
void OnClick_psComboButtons(int winID)
{
	//winID is between 1024 and 1047 inclusive
	try
	{
		if (GUI_Controls.virtualGrid->GetNumberRows() == 0) return;

		int button = winID - 1024;
		wxGridCellCoords coords;
		Cell_Locator.GetLocation(coords);
		int curRow = coords.GetRow();

		//verify button does not conflict with other buttons in current action
		//e.g UP and DOWN at the same time. So, 6 if-clauses get rid of 6 unneeded
		//columns in grid from 24 to 18, unlike before.
		wxString errorMSG = "";
		if ((button == UP && Has(DOWN, curRow)) || (button == DOWN && Has(UP, curRow)))
			errorMSG = "Can't have both UP and DOWN in the same Action.";
		if ((button == RIGHT && Has(LEFT, curRow)) || (button == LEFT && Has(RIGHT, curRow)))
			errorMSG = "Can't have both LEFT and RIGHT in the same Action.";
		if ((button == LANALOG_UP && Has(LANALOG_DOWN, curRow)) || (button == LANALOG_DOWN && Has(LANALOG_UP, curRow)))
			errorMSG = "Can't have both Left Analog's UP and DOWN in the same Action.";
		if ((button == LANALOG_LEFT && Has(LANALOG_RIGHT, curRow)) || (button == LANALOG_RIGHT && Has(LANALOG_LEFT, curRow)))
			errorMSG = "Can't have both Left Analog's LEFT and RIGHT in the same Action.";
		if ((button == RANALOG_UP && Has(RANALOG_DOWN, curRow)) || (button == RANALOG_DOWN && Has(RANALOG_UP, curRow)))
			errorMSG = "Can't have both Right Analog's UP and DOWN in the same Action.";
		if ((button == RANALOG_LEFT && Has(RANALOG_RIGHT, curRow)) || (button == RANALOG_RIGHT && Has(RANALOG_LEFT, curRow)))
			errorMSG = "Can't have both Right Analog's LEFT and RIGHT in the same Action.";
		if (Has(button, curRow))
			errorMSG = "The same button already exists in this Action.";
		
		if (errorMSG != "")
		{
			wxMessageBox(errorMSG, "Not Allowed!", wxICON_INFORMATION);
			GUI_Controls.virtualGrid->SetFocus();
			return;
		}
		
		//Set default sensitivity for new buttons.Regular buttons max is 255.
		//Analogs' Max is 255 for Down/Right. And Max is 0 for UP/LEFT
		int sensitivity;
		if (button >= LANALOG_UP)
		{
			if (button == LANALOG_DOWN || button == RANALOG_DOWN || button == LANALOG_RIGHT || button == RANALOG_RIGHT)
				sensitivity = 255; //max DOWN/RIGHT
			else
				sensitivity = 0; //max UP/LEFT
		}
		else
			sensitivity = 255; //max for regular buttons
		
		CCellValue val;
		val.buttonName = PS_LABEL[button].name;
		val.buttonValue = button;
		val.buttonSensitivity = sensitivity;

		Cell_Locator.GetLocation(coords);
		GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, &val);
		GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
		
		Cell_Locator.MoveToNextButton();
	}
	catch (exception &e)
	{
		wxMessageBox(e.what());
	}
	catch (...)
	{
		wxMessageBox(wxString::Format("Unknown exception occured in %s function and line number: %d"
			" in file: %s", __FUNCTION__, __LINE__, __FILE__));
	}
}

/////Grid mouse click
void OnClickComboGrid(wxGridEvent &ev)
{
	try
	{
		unsigned int row, col;
		row = ev.GetRow();
		col = ev.GetCol();
		//Clear any current highlights too
		GUI_Controls.virtualGrid->ClearSelection();
		GUI_Controls.virtualGrid->Refresh();
		GUI_Controls.virtualGrid->SetGridCursor(row, col);	//to allow dragging and selection too
	
		//Modify sensitivity inside the SpinControl
		CCellValue *val = (CCellValue *) GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");
		GUI_Controls.spnSensitivity->SetValue(val->buttonSensitivity);
		//Move cursor to the selected cell coordinates
		Cell_Locator.SetLocation(row, col);
		ev.Skip();
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

//Show tooltip "Which button" underneath and its sensitivity value, when mouse hovering over grid cells
void OnMouseMoveOverGrid(wxMouseEvent &ev)
{
	try
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

		CCellValue *val;
		val = (CCellValue *) GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(cellPos.y, cellPos.x, "");
		
		wxString buttonInfo = val->buttonName;

		if (cellPos.x == 0)
		{
			wxString strFrame;
			long delay;
			buttonInfo.ToLong(&delay);
			if (delay == 1)
				strFrame = " frame.\n";
			else
				strFrame = " frames.\n";
			buttonInfo = "Delay: Repeat this Action for " + buttonInfo + strFrame + "Click here to modify its value.";
		}
		else if (val->buttonSensitivity >= 0 && val->buttonSensitivity <= 255)
		{
			//For SELECT, START, L3 and R3: they have fixed sensitivity of 255
			if (val->buttonValue == SELECT || val->buttonValue == START ||
				val->buttonValue == L3 || val->buttonValue == R3)
				buttonInfo += wxString::Format("\nNot a pressure sensitive button.", val->buttonSensitivity, val->buttonValue);
			else
				buttonInfo += wxString::Format("\nSensitivity: %d", val->buttonSensitivity, val->buttonValue);
		}

		GUI_Controls.virtualGrid->GetGridWindow()->SetToolTip(buttonInfo);
		ev.Skip();
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

//Modify sensitivity for buttons when we change the value
void OnChangeSensitivity(wxSpinEvent &ev)
{
	try
	{
		CCellValue *val;
		wxGridCellCoords coords;
		int row, col;
		Cell_Locator.GetLocation(coords);
		row = coords.GetRow();
		col = coords.GetCol();

		//get current selected button structure
		val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");

		//avoid changing sensitivity to empty cells
		if (val->buttonName == "")
			return;

		//avoid changing sensitivity fo SELECT, START, L3, R3 as they are not pressure sensitive
		if (val->buttonValue == SELECT || val->buttonValue == START ||
			val->buttonValue == L3 || val->buttonValue == R3)
		{
			Cell_Locator.SetLocation(row, col);
			return;
		}

		val->buttonSensitivity = ev.GetValue();		//modify sensitivity
		//save it back to the table (button structure)
		GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(row, col, "", val);
		Cell_Locator.SetLocation(row, col);
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

//As it says, I thought of making a function of this since it was repeated 3-4 times in the code.
void ModifySensitivity()
{
	//Modify sensitivity inside the SpinControl
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	CCellValue *val;
	val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(coords.GetRow(), coords.GetCol(), "");
	GUI_Controls.spnSensitivity->SetValue(val->buttonSensitivity);
}

//Save the Grid to the Combos container
void SaveGridToCombo(wxString &strUserInput)
{
	//Save Current Combo (if not already saved)
	//when saving, Check to see if the combo exist, if it does, use the same combo otherwise add a new one
	wxString strKeyValue = GUI_Controls.txtComboKey->GetValue();
	long keyValue;
	if (strKeyValue == "NONE")
		keyValue = 0;
	else
		strKeyValue.ToLong(&keyValue);

	//Check to see if we are modifying the same COMBO, if yes, erase it so we can overwrite it or start a new one
	for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
	{
		//If the COMBO already exists, erase it. I know the container should be list instead of vector
		//erase operation is expensive, I might change it later
		if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
		{
			GUI_Controls.Combos.erase(it);
			//We changed the iterator. If size is 0, there will be an error/exception if we continue the loop
			//because ++it will be called and it will point to an empty container, so silently get out of the loop
			break;
		}
	}

	//Save the COMBO from the grid (before clearing the grid) into Combos container
	//if it was erased above, it will be recreated here
	CCombo *curCombo = new CCombo;
	curCombo->SetKey((int)keyValue);
	curCombo->SetName(strUserInput);
	for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
	{
		CAction *action = new CAction;
		CCellValue *val;
		//for column 0, CCellValue's buttonName = Action Delay, while all other members = -1
		val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, 0, "");
		long int delay;
		val->buttonName.ToLong(&delay);
		/*GUI_Controls.virtualGrid->GetCellValue(row, 0).ToLong(&delay);*/
		action->SetDelay(delay);

		for (int col = 1; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
		{
			val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");
			//if button is not empty, add it
			if (val->buttonName != "")
				action->AddButton(val);
		}
		curCombo->AddAction(action);
	}
	GUI_Controls.Combos.push_back(curCombo);
}

//Event handling ComboBox's name change
void OnChangeComboName(wxCommandEvent &ev)
{
	try
	{
		wxString strCurrent, strPrevious;
		strCurrent = GUI_Controls.cmbComboName->GetStringSelection();
		strPrevious = GUI_Controls.strPreviousComboSelection;

		GUI_Controls.strPreviousComboSelection = strCurrent;

		///////////SAVE COMBO////////////////
		//Save Combo (before making a new one) from grid to Combos container if we have at least 1 Combo
		//It is very important that we use the cmbComboName to get the name of current Combo. If the same Combo
		//name found in the container, it will be overwritten with this one.
		if (GUI_Controls.Combos.size() > 0)
		{
			wxString strKeyValue = GUI_Controls.txtComboKey->GetValue();
			long keyValue;
			if (strKeyValue == "NONE")
				keyValue = 0;
			else
				strKeyValue.ToLong(&keyValue);

			for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
			{
				//If the COMBO already exists, erase it. I know the container should be list instead of vector
				//erase operation is expensive, I might change it later
				if ((*it)->GetName() == strPrevious)
				{
					GUI_Controls.Combos.erase(it);
					//We changed the iterator. If size is 0, there will be an error/exception if we continue the loop
					//because ++it will be called and it will point to an empty container, so silently get out of the loop
					break;
				}
			}

			CCombo *curCombo = new CCombo;
			curCombo->SetKey((int)keyValue);
			curCombo->SetName(strPrevious);
			for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
			{
				CAction *action = new CAction;
				CCellValue *val;
				//for column 0, CCellValue's buttonName = Action Delay, while all other members = -1
				val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, 0, "");
				long int delay;
				val->buttonName.ToLong(&delay);
				/*GUI_Controls.virtualGrid->GetCellValue(row, 0).ToLong(&delay);*/
				action->SetDelay(delay);

				for (int col = 1; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				{
					val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");
					//if button is not empty, add it
					if (val->buttonName != "")
						action->AddButton(val);
				}
				curCombo->AddAction(action);
			}
			GUI_Controls.Combos.push_back(curCombo);
		}

		//Clear grid - delete combo
		//If we don't have any COMBOs or the table doesn't exist, skip. Otherwise subscript out of range in Grid TableBase
		if (GUI_Controls.virtualGrid->GetNumberRows() > 0)
			GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());

		//Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
		//Hide grid to prevent flickering while adding buttons, and it is much faster this way. Show grid when we are done
		GUI_Controls.virtualGrid->Hide();
		for (std::vector<CCombo *>::iterator it = GUI_Controls.Combos.begin(); it != GUI_Controls.Combos.end(); ++it)
		{
			//GetValue() gets the new selected COMBO from the list
			if ((*it)->GetName() == strCurrent)
			{
				for (int row = 0; row < (*it)->GetNumberActions(); ++row)
				{
					CAction *curAction = (*it)->GetAction(row);
					int delay = curAction->GetDelay();
					//Add grid row, will populate it with buttons after that
					AddRow(GUI_Controls.virtualGrid, delay, GUI_Controls.virtualGrid->GetNumberRows());
					//Set Action Delay
					GUI_Controls.virtualGrid->SetCellValue(row, 0, wxString::Format("%d", delay));
					//Move to the first button and iterate to add them
					Cell_Locator.SetLocation(row, 1, false);
					for (int button = 0; button < curAction->GetNumberOfButtons(); ++button)
					{
						CCellValue *val;
						wxGridCellCoords coords;
						val = (CCellValue *)curAction->GetButton(button);
						Cell_Locator.GetLocation(coords);
						GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, val);
						GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
						Cell_Locator.MoveToNextButton(false);
					}
				}
				break;	//No need to process other COMBOs
			}
		}
		GUI_Controls.virtualGrid->Show(true); //Added all buttons! Show the grid

		Cell_Locator.SetLocation(0, 1, false);
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

/* This function handles keyboard UP and Down keys while navigating the ComboBox
and refresh the grid according to selection. Note: Grid will refresh once the popup of th combobox disappears.
I could setfocus to grid, but that will disrupt the navigation using Up/Down keys to select Combo names. */
void OnChangeComboNameKey(wxKeyEvent &ev)
{
	try
	{
		if (ev.GetKeyCode() == WXK_UP || ev.GetKeyCode() == WXK_DOWN)
		{
			GUI_Controls.strPreviousComboSelection = GUI_Controls.cmbComboName->GetValue();
			wxCommandEvent e;
			e.SetEventType(wxEVT_COMBOBOX);
			OnChangeComboName(e);
			GUI_Controls.virtualGrid->Update();
			GUI_Controls.virtualGrid->Refresh();
			GUI_Controls.virtualGrid->ForceRefresh();
			ev.Skip();
		}
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

///ReAnimate Analog Sticks timer event. Because after some time, the Animation will go out of sync
void OnTimeReAnimateAnalogSticks()
{
	try
	{
		for (int i = LANALOG_UP; i <= RANALOG_LEFT; ++i)
		{
			GUI_Controls.psComboButtons[i]->Stop();
			GUI_Controls.psComboButtons[i]->Play();
		}
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