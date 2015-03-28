#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "fastCompile.h"
#include "twinpad_gui.h"
#include "timers_functions.h"
#include "events_functions.h"
#include "DirectInput.h"
#include "labels.h"
#include "resources.h"		// All GIFs and resources for TwinPad stored in arrays

#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/grid.h"

#include "wx/mstream.h"		// for wxMemoryInputStream

// Globals defined in TwinPad_Gui.cpp
extern CTwinPad_Gui GUI_Controls;
extern GUI_Configurations GUI_Config;

// Sorry for the inconsistencies, using fstream and wxfile at the same time, just wanted to try both :)

// Creates a default TwinPad.ini file
void CreateNullFile()
{
	int counter;
	
	std::string file = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	std::string strPad;

	std::ofstream nullfile(file.c_str(), std::ios::out);

	if (!nullfile.is_open())
	{
		wxMessageBox("Couldn't create TwinPad.ini configuration file into the specified location!\n\n"
			"\nMake sure at least 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		GUI_Controls.mainFrame->Close(true);
	}

	// Write header and config version number
	nullfile << GUI_Controls.GetTwinPad_Header() << std::endl;
	// Write assigned keys for both pads
	for(int pad = 0; pad <= 1; pad++)
	{
		if (pad == 0) 
			strPad = "[0][";
		else
			strPad = "[1][";
		
		counter = 0;
		while(counter < 24)		// 24 PS buttons
		{
			nullfile << strPad << counter << "] = 0x0" << std::endl;
			// GUI_Config.m_pad[pad][counter] = 0;
			counter++;
		}
		nullfile << strPad << counter << "] = 0x0" << std::endl;  // for walk/run value
	}

	// Write Mouse configuration
	counter = 0;
	while(counter < 10)
	{
		nullfile << wxString("[") << counter << "] = 36" << std::endl;
		GUI_Config.m_mouse[counter] = 36;
		counter++;
	}
	nullfile << "0" << std::endl;		/*  Mouse as PAD 1  */
	GUI_Config.m_mouseAsPad = 0;
	nullfile << "1" << std::endl;		/*  Mouse sensitivity default value.  */
	GUI_Config.m_mouseSensitivity = 1;

	// Write 'Extra Options' configuration
	counter = 0;
	while(counter <= 6)
	{
		nullfile << "0" << std::endl;
		GUI_Config.m_extra[counter] = 0;
		counter++;
	}
	
	nullfile.close();
}

// Creates a default TwinPad_COMBOs.ini file
void CreateNullComboFile()
{
	std::string file = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	std::ofstream txtFile(file.c_str(), std::ios::out);

	if (!txtFile.is_open())
	{
		wxMessageBox("Couldn't create TwinPad_Combos.ini configuration file into the specified location!\n\n"
			"\nMake sure at least 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}

	txtFile << GUI_Controls.GetTwinPad_ComboHeader() << std::endl;
	txtFile << "ComboCount\t= 0\n";

	txtFile.close();
}

bool IsFileOkAndFix(const std::string &file, const std::string &header)
{
	std::ifstream f(file);
		
	std::string str = "";
	wxString strMsg = "";

	int select = 0;
	if (header == GUI_Controls.GetTwinPad_Header())
		select = 1;		// TwinPad.ini
	else if (header == GUI_Controls.GetTwinPad_ComboHeader())
		select = 2;		// TwinPad_COMBOs.ini
		
	// First: Check if file can be opened
	if (f.is_open())
	{
		// Second: Check the Header if version is compatible
		getline(f, str);
		if (select == 1)
		{
			if (str != GUI_Controls.GetTwinPad_Header())
			{
				f.close();
				CreateNullFile();
				int len = str.length();
				std::string strCmp = "TwinPad Configurations";
				std::string substring = str.substr(1, strCmp.length());
				if (len == GUI_Controls.GetTwinPad_Header().length() && substring == strCmp)
					wxMessageBox(wxString::Format("TwinPad configuration file is old, all previous settings are lost.\n\n"
						"Old version: %s\nNew version: %s", str, GUI_Controls.GetTwinPad_Header()),
						"Oops!", wxICON_INFORMATION);
				else
					wxMessageBox("TwinPad configuration file is corrupted, all previous settings are lost.\n\n",
									"Oops!", wxICON_EXCLAMATION);
					
				return true;	// Created null file, then it is OK. If didn't create null file, shouldn't have reached here.
			}
			else
				return true;	// File is OK
		}
		else if (select == 2)
		{
			if (str != GUI_Controls.GetTwinPad_ComboHeader())
			{
				f.close();
				CreateNullComboFile();
				int len = str.length();
				std::string strCmp = "TwinPad COMBO Configurations";
				std::string substring = str.substr(1, strCmp.length());
				if (len == GUI_Controls.GetTwinPad_ComboHeader().length() && substring == strCmp)
					wxMessageBox(wxString::Format("TwinPad COMBOs configuration file is old, all previous settings are lost.\n\n"
						"Old version: %s\nNew version: %s", str, GUI_Controls.GetTwinPad_ComboHeader()),
						"Oops!", wxICON_INFORMATION);
				else
					wxMessageBox("TwinPad COMBOs configuration file is corrupted, all previous settings are lost.\n\n",
									"Oops!", wxICON_EXCLAMATION);

				return true;	// Create null file, then is OK
			}
			else
				return true;	// File is OK
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

void Loading_TwinPad_Main_Config()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
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

	// Get header and skip it
	line = file.GetFirstLine();
	// Read the two pads configurations and Walk/Run key
	for(int pad = 0; pad < 2; pad++)
		for (int key = 0; key < 25; key++)
		{
			line = file.GetNextLine();
			token.SetString(line, " ", wxTOKEN_STRTOK);
			token.GetNextToken();				// Skips first token [n][n]
			token.GetNextToken();				// Skips second token =
			subStr = token.GetNextToken();				// we are interested in this token 0xNN
			subStr.ToLong(&val, 16);					// Convert string to hex
			// GUI_Config.m_pad[pad][key] = int(val);		
		}

	// Read mouse buttons and scrollup/down configuration
	for(int i = 0; i < 10; i++)
	{
		line = file.GetNextLine();
		token.SetString(line, " ", wxTOKEN_STRTOK);
		token.GetNextToken();	// skips first token [n]
		token.GetNextToken();	// skips second token =
		subStr = token.GetNextToken();	// we are interested in this token N (DECIMAL)
		subStr.ToLong(&val, 10);		// Convert string to dec
		GUI_Config.m_mouse[i] = val;
	}

	// Read 'Extra Options' configuration
	for(int i = 0; i < 7; i++)
	{
		line = file.GetNextLine();
		line.ToLong(&val, 10);
		GUI_Config.m_extra[i] = val;
	}
	file.Close();
}

// Save TwinPad configurations (Keyboard, Mouse and the options in Misc tab)
void SaveTwinPadConfigurations()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		wxMessageBox("Couldn't open configuration files from the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		::exit(0);
	}

	// erase contents
	file.Clear();

	wxStringTokenizer token;
	wxString line, subStr;
	long val = 0;
	// Get header and skip it
	file.AddLine(GUI_Controls.GetTwinPad_Header());
	// Read the two pads configurations and Walk/Run key
	for (int pad = 0; pad < 2; pad++)
	{
		file.AddLine(wxString::Format("Pad #%d", pad + 1));
		for (int key = 0; key < 25; key++)
		{
			line = wxString::Format("[%d][%d] = 0x%x", pad, key, GUI_Config.m_pad[pad][key]);
			file.AddLine(line);
		}
	}

	// Read mouse buttons and scrollup/down configuration
	file.AddLine("Mouse Buttons");
	for (int i = 0; i < 10; i++)
	{
		int button = -1;
		for (int j = 0; j < (sizeof(MOUSE_CHOICES) / sizeof(*MOUSE_CHOICES)); ++j)
		{
			if (MOUSE_CHOICES[j].name == GUI_Controls.cmbMouseComboBox[i]->GetStringSelection())
			{
				button = MOUSE_CHOICES[j].keyValue;
				break;
			}
		}
		line = wxString::Format("[%d] = %d", i, button);
		file.AddLine(line);
	}
	// Mouse as pad 1 (0) or 2 (1)
	file.AddLine(wxString::Format("Mouse as pad 1/2  (0/1) = %d", (GUI_Controls.mousePad1radioButton->GetValue() ? 0 : 1)));
	// Mouse sensitivity
	file.AddLine("Mouse Sensitivity \t= " + GUI_Controls.cmbMouseSensitivity->GetStringSelection());
	
	// Write 'Extra Options' configuration
	file.AddLine(wxString::Format("Disable Pad 1 \t\t= %d", (GUI_Controls.chkDisablePad1->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Pad 2 \t\t= %d", (GUI_Controls.chkDisablePad2->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable KeyEvents \t= %d", (GUI_Controls.chkDisableKeyEvents->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Mouse \t\t= %d", (GUI_Controls.chkDisableMouse->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Combos \t\t= %d", (GUI_Controls.chkDisableCombos->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Hot Key \t= %d", (GUI_Controls.chkDisableOnFlyKey->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Enable Hack \t\t= %d", (GUI_Controls.chkEnableHack->GetValue() ? 0 : 1)));
	// Hot key for showing config window
	unsigned char key = 0;
	wxString name = "DIK_" + GUI_Controls.lblHotKey->GetLabel();
	for (int i = 0; i < (sizeof(DIK_KEYCODES) / sizeof(*DIK_KEYCODES)); ++i)
		if (DIK_KEYCODES[i].name == name)
		{
			key = DIK_KEYCODES[i].keyValue;
			break;
		}
	file.AddLine(wxString::Format("Hot Key \t\t= 0x%x", key));

	file.Write();
}

// Save TwinPad combo configurations in Combo tab
void SaveTwinPadComboConfigurations()
{

}

// Loading images into controls
void LoadResources(CPS_Anim *animCtrl, const unsigned int index)
{
	void *iData = 0;	// hold image data
	size_t length = 0;  // hold image size in bytes

	GetImageData(iData, &length, index);

	unsigned char *buffer = new unsigned char[length];
	memcpy(buffer, iData, length);

	wxMemoryInputStream iStream(buffer, length);

	animCtrl->Load(iStream, wxANIMATION_TYPE_ANY);

	delete[] buffer;
}

// This function is called from LoadResources() and SetCustomCellRenderer()
void GetImageData(void* &data, unsigned int *length, const unsigned int index)
{
	// I know this is awkward, but I am not gonna dig into the resources.h 
	// to fix the arrays into 1 and use an index to make this more concise and pithy.
	PS2BUTTON button = (PS2BUTTON)index;
	switch (button)
	{
	case PS2BUTTON::L2:
		data = (void*)L2_GIF;
		*length = sizeof(L2_GIF);
		break;
	case PS2BUTTON::R2:
		data = (void*)R2_GIF;
		*length = sizeof(R2_GIF);
		break;
	case PS2BUTTON::L1:
		data = (void*)L1_GIF;
		*length = sizeof(L1_GIF);
		break;
	case PS2BUTTON::R1:
		data = (void*)R1_GIF;
		*length = sizeof(R1_GIF);
		break;
	case PS2BUTTON::TRIANGLE:
		data = (void*)TRIANGLE_GIF;
		*length = sizeof(TRIANGLE_GIF);
		break;
	case PS2BUTTON::CIRCLE:
		data = (void*)CIRCLE_GIF;
		*length = sizeof(CIRCLE_GIF);
		break;
	case PS2BUTTON::CROSS:
		data = (void*)CROSS_GIF;
		*length = sizeof(CROSS_GIF);
		break;
	case PS2BUTTON::SQUARE:
		data = (void*)SQUARE_GIF;
		*length = sizeof(SQUARE_GIF);
		break;
	case PS2BUTTON::SELECT:
		data = (void*)SELECT_GIF;
		*length = sizeof(SELECT_GIF);
		break;
	case PS2BUTTON::L3:
		data = (void*)L3_GIF;
		*length = sizeof(L3_GIF);
		break;
	case PS2BUTTON::R3:
		data = (void*)R3_GIF;
		*length = sizeof(R3_GIF);
		break;
	case PS2BUTTON::START:
		data = (void*)START_GIF;
		*length = sizeof(START_GIF);
		break;
	case PS2BUTTON::UP:
		data = (void*)UP_GIF;
		*length = sizeof(UP_GIF);
		break;
	case PS2BUTTON::RIGHT:
		data = (void*)RIGHT_GIF;
		*length = sizeof(RIGHT_GIF);
		break;
	case PS2BUTTON::DOWN:
		data = (void*)DOWN_GIF;
		*length = sizeof(DOWN_GIF);
		break;
	case PS2BUTTON::LEFT:
		data = (void*)LEFT_GIF;
		*length = sizeof(LEFT_GIF);
		break;
	case PS2BUTTON::LANALOG_UP:
	case PS2BUTTON::RANALOG_UP:
		data = (void*)ANALOG_UP_GIF;
		*length = sizeof(ANALOG_UP_GIF);
		break;
	case PS2BUTTON::LANALOG_RIGHT:
	case PS2BUTTON::RANALOG_RIGHT:
		data = (void*)ANALOG_RIGHT_GIF;
		*length = sizeof(ANALOG_RIGHT_GIF);
		break;
	case PS2BUTTON::LANALOG_DOWN:
	case PS2BUTTON::RANALOG_DOWN:
		data = (void*)ANALOG_DOWN_GIF;
		*length = sizeof(ANALOG_DOWN_GIF);
		break;
	case PS2BUTTON::LANALOG_LEFT:
	case PS2BUTTON::RANALOG_LEFT:
		data = (void*)ANALOG_LEFT_GIF;
		*length = sizeof(ANALOG_LEFT_GIF);
		break;
	default:
		wxMessageBox(wxString::Format("Unknown button number in GetImageData(): Button #%d", index));
		break;
	}
}
