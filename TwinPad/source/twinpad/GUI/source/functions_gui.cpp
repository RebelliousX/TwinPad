#include "stdafx.h"
#include "twinpad_gui.h"
#include "timers_functions.h"
#include "events_functions.h"
#include "DirectInput.h"

// Globals defined in TwinPad_Gui.cpp
extern CTwinPad_Gui GUI_Controls;
extern MainConfigurations Configurations;

// Sorry for the inconsistencies, using fstream and wxfile at the same time, just wanted to try both :)

// Creates a default TwinPad.ini file
void CreateNullFile()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		if (!file.Create(fileName))
		{
			wxMessageBox("Couldn't create TwinPad.ini configuration file into the specified location!\n\n"
				"\nMake sure at least 'inis' folder exists in the same directory with the emu."
				"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
			::exit(0);
		}
		else
			file.Open(fileName);
	}

	MainConfigurations null_config;

	// Erase contents
	file.Clear();

	// Add TwinPad Header
	file.AddLine(GUI_Controls.GetTwinPad_Header());
	
	for (int pad = 0; pad < 2; ++pad)
	{
		file.AddLine(wxString::Format("{-- Pad #%d --}", pad + 1));
		for (int button = 0; button < 25; ++button)
			file.AddLine(wxString::Format("[%d][%d]) = 0x%X", pad, button, null_config.m_pad[pad][button]));
	}

	file.AddLine("{-- Mouse Options --}");

	for (int mouseButton = 0; mouseButton < 10; ++mouseButton)
		file.AddLine(wxString::Format("[%d] = %d", mouseButton, null_config.m_mouse[mouseButton]));

	file.AddLine(wxString::Format("Mouse as pad 1/2  (0/1) = %d", null_config.m_mouseAsPad));
	file.AddLine(wxString::Format("Mouse Sensitivity 	= %d", null_config.m_mouseSensitivity));
	file.AddLine("{-- Extra Options --}");

	file.AddLine(wxString::Format("Disable Pad 1 \t\t= %d", null_config.m_extra[null_config.DISABLE_PAD1]));
	file.AddLine(wxString::Format("Disable Pad 2 \t\t= %d", null_config.m_extra[null_config.DISABLE_PAD2]));
	file.AddLine(wxString::Format("Disable KeyEvents \t= %d", null_config.m_extra[null_config.DISABLE_KEYEVENTS]));
	file.AddLine(wxString::Format("Disable Mouse \t\t= %d", null_config.m_extra[null_config.DISABLE_MOUSE]));
	file.AddLine(wxString::Format("Disable Combos \t\t= %d", null_config.m_extra[null_config.DISABLE_COMBOS]));

	file.Write();
	file.Close();
}

// Creates a default TwinPad_COMBOs.ini file
void CreateNullComboFile()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		if (!file.Create(fileName))
		{
			wxMessageBox("Couldn't create TwinPad_Combos.ini configuration file into the specified location!\n\n"
				"\nMake sure at least 'inis' folder exists in the same directory with the emu."
				"\nAnd that you have permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
			::exit(0);
		}
		else
			file.Open(fileName);
	}

	file.Clear();
	file.AddLine(GUI_Controls.GetTwinPad_ComboHeader());
	file.AddLine("ComboCount\t = 0");

	file.Write();
	file.Close();
}

bool IsFileOkAndFix(const wxString &fileName, const wxString &header)
{
	int select = 0;

	if (header == GUI_Controls.GetTwinPad_Header())
		select = 1;
	else if (header == GUI_Controls.GetTwinPad_ComboHeader())
		select = 2;
	else
	{
		wxMessageBox("Unknown header passed to IsFileOkAndFix()", "Fatal error! Exiting.", wxICON_ERROR);
		return false;
	}

	wxTextFile file(fileName);
	if (!file.Exists())
	{
		file.Create();
		file.Write();
		file.Close();
		if (select == 1)
		{
			CreateNullFile();
			return true;
		}
		else if (select == 2)
		{
			CreateNullComboFile();
			return true;
		}
		else
		{
			wxMessageBox(wxString::Format("Couldn't create %s configuration file into the specified location!\n\n"
				"\nMake sure at least 'inis' folder exists in the same directory with the emu."
				"\nAnd that you have permission to read/write in that directory.", fileName),
				"Open files failed!", wxICON_ERROR);
			::exit(0);
		}
	}

	// Check the Header if version is compatible
	wxString str	= "";
	if (file.Open(fileName))
	{
		str = file.GetFirstLine();
		if (select == 1)
		{
			if (str != GUI_Controls.GetTwinPad_Header())
			{
				file.Close();
				CreateNullFile();
				int len = str.length();
				wxString strCmp = "TwinPad Configurations";
				wxString substring = str.substr(1, strCmp.length());
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
		else
		{
			if (str != GUI_Controls.GetTwinPad_ComboHeader())
			{
				file.Close();
				CreateNullComboFile();
				int len = str.length();
				wxString strCmp = "TwinPad COMBO Configurations";
				wxString substring = str.substr(1, strCmp.length());
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
	file.Close();
	return true;
}

bool LoadTwinPadConfigurations()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_FileName();
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		wxMessageBox("Couldn't open configuration files from the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		return false;
	}

	wxStringTokenizer token;
	wxString line, subStr;
	long val = 0;

	// Skip twinpad header
	file.GetFirstLine();
	
	// Read the two pads configurations and Walk/Run key
	for (int pad = 0; pad < 2; pad++)
	{
		// Skip pad number comment
		file.GetNextLine();
		for (int key = 0; key < 25; key++)
		{
			line = file.GetNextLine();
			token.SetString(line, " ", wxTOKEN_STRTOK);
			token.GetNextToken();				// Skips first token [n][n]
			token.GetNextToken();				// Skips second token =
			subStr = token.GetNextToken();		// we are interested in this token 0xNN
			subStr.ToLong(&val, 16);			// Convert string to hex
			Configurations.m_pad[pad][key] = int(val);		
		}
	}

	// Skip mouse comment
	file.GetNextLine();

	// Read mouse buttons and scrollup/down configuration
	for(int i = 0; i < 10; i++)
	{
		line = file.GetNextLine();
		token.SetString(line, " ", wxTOKEN_STRTOK);
		token.GetNextToken();			// skips first token [n]
		token.GetNextToken();			// skips second token =
		subStr = token.GetNextToken();	// we are interested in this token N (DECIMAL)
		subStr.ToLong(&val, 10);		// Convert string to dec
		Configurations.m_mouse[i] = val;
	}

	// Mouse as pad 1 or 2 (0/1)
	line = file.GetNextLine();
	token.SetString(line, "=", wxTOKEN_STRTOK);
	token.GetNextToken();				// skips "Mouse as pad 1/2  (1/2) "
	subStr = token.GetNextToken();		// mouse as pad value
	subStr.ToLong(&val, 10);
	Configurations.m_mouseAsPad = val;

	// Mouse sensitivity
	line = file.GetNextLine();
	token.SetString(line, "=", wxTOKEN_STRTOK);
	token.GetNextToken();				// skips "Mouse Sensitivity 	"
	subStr = token.GetNextToken();		// mouse sensitivity value
	subStr.ToLong(&val, 10);
	Configurations.m_mouseSensitivity = val;

	// Skip extra options comment
	file.GetNextLine();
	// Read 'Extra Options' configuration
	for(int i = 0; i < 5; i++)
	{
		line = file.GetNextLine();
		token.SetString(line, "=", wxTOKEN_STRTOK);
		token.GetNextToken();			// skips comment
		subStr = token.GetNextToken();	// interested in the value here
		subStr.ToLong(&val, 10);
		Configurations.m_extra[i] = val;
	}

	file.Close();
	return true;
}

// Load TwinPad Combos configurations
bool LoadTwinPadComboConfigurations()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	wxTextFile file(fileName);
	if (!file.Open(fileName))
	{
		wxMessageBox("Couldn't open configuration files from the specified location!"
			"\nMake sure 'inis' folder exists in the same directory with the emu."
			"\nAnd that you have at least permission to read/write in that directory.", "Open files failed!", wxICON_ERROR);
		return false;
	}

	// Erase contents of COMBOs vector
	for (unsigned int i = 0; i < Configurations.Combos.size(); ++i)
		delete Configurations.Combos[i];
	Configurations.Combos.clear();

	// Skip TwinPad COMBOs header
	file.GetFirstLine();

	wxStringTokenizer token;
	wxString line, subStr;
	long val = 0;

	// Combo count
	line = file.GetNextLine();
	token.SetString(line, "=", wxTOKEN_STRTOK);
	token.GetNextToken();				// skip ComboCount
	subStr = token.GetNextToken();		// interested in the value here
	subStr.ToLong(&val, 10);

	int comboCount = (int)val;

	for (int combo = 0; combo < comboCount; ++combo)
	{
		CCombo *tempCombo = new CCombo;
		
		line = file.GetNextLine();
		line = line.substr(4, (line.length() - 8));	// trim beginning "{-- " and ending " --}
		tempCombo->SetName(line);

		line = file.GetNextLine();
		token.SetString(line, "=", wxTOKEN_STRTOK);	
		token.GetNextToken();						// skip ActionCount =
		subStr = token.GetNextToken();				// interested in the value here
		subStr.ToLong(&val, 10);
		
		int actionCount = (int)val;

		line = file.GetNextLine();
		token.SetString(line, "=", wxTOKEN_STRTOK);
		token.GetNextToken();						// skip ComboAsPad =
		subStr = token.GetNextToken();				// interested in the value here
		subStr.ToLong(&val, 10);
		tempCombo->SetPad((int)val);

		line = file.GetNextLine();
		token.SetString(line, "=", wxTOKEN_STRTOK);
		token.GetNextToken();						// skip ComboKey =
		subStr = token.GetNextToken();				// interested in the value here
		subStr.ToLong(&val, 16);
		tempCombo->SetKey((int)val);

		file.GetNextLine();							// skip line separator '-===================-'

		for (int action = 0; action < actionCount; ++action)
		{
			CAction *tempAction = new CAction;
			
			line = file.GetNextLine();
			token.SetString(line, "=", wxTOKEN_STRTOK);
			token.GetNextToken();						// skip ActionDelay =
			subStr = token.GetNextToken();				// interested in the value here
			subStr.ToLong(&val, 10);
			tempAction->SetDelay((int)val);

			line = file.GetNextLine();
			token.SetString(line, "=", wxTOKEN_STRTOK);
			token.GetNextToken();						// skip NumOfButtons =
			subStr = token.GetNextToken();				// interested in the value here
			subStr.ToLong(&val, 10);

			int buttonCount = (int)val;

			line = file.GetNextLine();					// contains buttons and their sensitivity

			for (int button = 0; button < buttonCount; ++button)
			{
				CCellValue *tempButton = new CCellValue;
				wxString subSubStr;
				
				int trimNumChar = 0;

				token.SetString(line, ")", wxTOKEN_STRTOK);
				subStr = token.GetNextToken();			// Get "(nn, mmm"
				subSubStr = subStr;						// save to deal with second value
				trimNumChar = subStr.length() + 2;		// save "(nn, mmm) " length to trim line later, 2 for ") "
				token.SetString(subSubStr, "(", wxTOKEN_STRTOK);
				subSubStr = token.GetNextToken();		// skip '('
				token.SetString(subSubStr, ",", wxTOKEN_STRTOK);
				subSubStr = token.GetNextToken();		// Get 'nn'
				subSubStr.ToLong(&val, 10);
				tempButton->buttonValue = (int)val;

				token.SetString(subStr, " ", wxTOKEN_STRTOK);
				token.GetNextToken();							// skip '(nn, '
				subSubStr = token.GetNextToken();				// Get 'mmm'
				subSubStr.ToLong(&val, 10);
				tempButton->buttonSensitivity = (int)val;

				tempButton->buttonName = PS_LABEL[tempButton->buttonValue].name;

				line = line.substr(trimNumChar, line.length());
				
				tempAction->AddButton(tempButton);
				delete tempButton;
				tempButton = 0;
			}
			tempCombo->AddAction(tempAction);
			delete tempAction;
			tempAction = 0;
		}
		Configurations.Combos.push_back(tempCombo);
	}
	file.Close();
	return true;
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
	wxString line;
	
	// Get header and skip it
	file.AddLine(GUI_Controls.GetTwinPad_Header());
	// Read the two pads configurations and Walk/Run key
	for (int pad = 0; pad < 2; pad++)
	{
		file.AddLine(wxString::Format("{-- Pad #%d --}", pad + 1));
		for (int key = 0; key < 25; key++)
		{
			line = wxString::Format("[%d][%d] = 0x%X", pad, key, Configurations.m_pad[pad][key]);
			file.AddLine(line);
		}
	}

	// Write mouse buttons and scrollup/down configuration
	file.AddLine("{-- Mouse Options --}");
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
	file.AddLine("{-- Extra Options --}");
	file.AddLine(wxString::Format("Disable Pad 1 \t\t= %d", (GUI_Controls.chkDisablePad1->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Pad 2 \t\t= %d", (GUI_Controls.chkDisablePad2->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable KeyEvents \t= %d", (GUI_Controls.chkDisableKeyEvents->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Mouse \t\t= %d", (GUI_Controls.chkDisableMouse->GetValue() ? 1 : 0)));
	file.AddLine(wxString::Format("Disable Combos \t\t= %d", (GUI_Controls.chkDisableCombos->GetValue() ? 1 : 0)));
	
	file.Write();
	file.Close();
}

// Save TwinPad combo configurations in Combo tab
void SaveTwinPadComboConfigurations()
{
	wxString fileName = GUI_Controls.GetSettingsPath() + GUI_Controls.GetTwinPad_ComboFileName();
	wxTextFile file(fileName);

	// Erase contents
	file.Clear();

	// Add Combo file header
	file.AddLine(GUI_Controls.GetTwinPad_ComboHeader());

	// Add number of Combos
	file.AddLine(wxString::Format("ComboCount \t= %d", Configurations.Combos.size()));

	for (unsigned int combo = 0; combo < Configurations.Combos.size(); ++combo)
	{
		CCombo *thisCombo = Configurations.Combos[combo];
		file.AddLine(wxString::Format("{-- %s --}", thisCombo->GetName()));
		file.AddLine(wxString::Format("ActionCount \t= %d", thisCombo->GetNumberActions()));
		file.AddLine(wxString::Format("ComboAsPad \t= %d", thisCombo->GetPad()));
		file.AddLine(wxString::Format("ComboKey \t= 0x%X", thisCombo->GetKey()));
		file.AddLine("-===================-");

		for (int action = 0; action < Configurations.Combos[combo]->GetNumberActions(); ++action)
		{
			CAction *thisAction = thisCombo->GetAction(action);
			file.AddLine(wxString::Format("ActionDelay \t\t= %d", thisAction->GetDelay()));
			file.AddLine(wxString::Format("NumOfButtons \t\t= %d", thisAction->GetNumberOfButtons()));
			
			wxString line_action = "";
			
			for (int button = 0; button < thisAction->GetNumberOfButtons(); ++button)
			{
				wxString buttonNumber, buttonSensitivity;

				buttonNumber = wxString::Format("(%d, ", thisAction->GetButton(button)->buttonValue);
				buttonSensitivity = wxString::Format("%d) ", thisAction->GetButton(button)->buttonSensitivity);

				line_action += (buttonNumber + buttonSensitivity);
			}

			file.AddLine(line_action);
		}
	}

	file.Write();
	file.Close();
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
