#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Loading.h"

#include "twinpad_gui.h"

#ifndef WX_PRECOM
#include "wx/wx.h"
#else
#include "wx/wxprec.h"
#endif

using std::vector;

vector<vector<vector<int>>> COMBO; // 3D Array.. COMBO[Action][Button][COMBO]
vector<vector<int>> ActionDelay;   // 2D Array.. ActionDelay[Action][COMBO]
vector<int> ActionCount;
vector<int> ComboKey;

unsigned short g_comboCounter = 0;
unsigned short g_comboPAD = 0;   

unsigned int mouseScrollUp = 36, mouseScrollDown = 36;

using namespace std;

void LoadCombos()
{
	string strLine;
	char *cstrToken, *cstrTemp;

	int value = 0, maxCount = 0;

	ifstream comboFile ("inis/TwinPad_COMBOs.ini");

	if (comboFile.is_open())
	{
		getline (comboFile, strLine); // The header..
		
		getline (comboFile, strLine); // ComboCounter
		cstrTemp = _strdup(strLine.c_str());
		cstrToken = strtok(cstrTemp, " ="); // First skips "ComboCount   = "
		cstrToken = strtok(NULL, " =");  // now strToken has the value of ComboCounter
		
		value = atoi(cstrToken);
		g_comboCounter = value;
		
		getline (comboFile, strLine); // ComboPAD
		cstrTemp = _strdup(strLine.c_str());
		cstrToken = strtok(cstrTemp, " ="); // First skips "ComboPAD     = "
		cstrToken = strtok(NULL, " =");  // now strToken has the value of ComboPAD
		
		value = atoi(cstrToken);
		g_comboPAD = value - 1;

		// To prevent the infamous *CRASH* from v0.8.0 to v0.8.1
		// We can't resize vector array with 0 elements X_X duh!!
		if (g_comboCounter == 0) return;

		/*
		Resizing COMBO & ActionDelay arrays to the new dimensions
		Note that the only dynamic size is the last dimension, the first & second are fixed
		although I'm not going to use all of the 334 Actions or the 24 ButtonsPerAction, but kept them because
		1) it's the maximum Actions/buttons allowed 2) They vary from COMBO to another, and 3) it's easier this way..
		that's why I use ActionCounter and -1 as default NULL value.. (0 is a legit value which equals the button L2)
		*/
		COMBO.resize(334);
		ActionDelay.resize(334);
		for (int i = 0; i < 334; i++)
		{
			ActionDelay[i].resize(g_comboCounter);
			COMBO[i].resize(24);
			for (int j = 0; j < 24; j++)
				COMBO[i][j].resize(g_comboCounter);
		}

		// Initialize COMBO 3DArray with -1 as default 'NULL' value..
		for (int i = 0; i < 334; i++)
			for (int j = 0; j < 24; j++)
				for (int k = 0; k < g_comboCounter; k++)
					COMBO[i][j][k] = -1;

		ActionCount.resize(g_comboCounter);
		ComboKey.resize(g_comboCounter);
		
		maxCount = g_comboCounter;
		// Now reading each combo's information..
		for (int comboCount = 0; comboCount < maxCount; comboCount++)
		{
			getline(comboFile, strLine); // skip the header 'The name of the Combo'
			// Test if strLine == NULL or reached EOF then return.. if it was empty file.. (user didn't configure anything)

			getline(comboFile, strLine); // ActionCounter
			cstrTemp = _strdup(strLine.c_str());
			cstrToken = strtok(cstrTemp, " ="); // Skip "ActionCount  = "
			cstrToken = strtok(NULL, " ="); // read ActionCount value..
			value = atoi(cstrToken);
			ActionCount[comboCount] = value;

			getline(comboFile, strLine); // Skips TotalButtons, we don't need it here, only for COMBO editor in GUI..

			getline(comboFile, strLine); // ComboKey
			cstrTemp = _strdup(strLine.c_str());
			cstrToken = strtok(cstrTemp, " ="); // Skip "ComboKey     = "
			cstrToken = strtok(NULL, " ="); // Skip 0x
			value = strtol(cstrToken, NULL, 16);
			ComboKey[comboCount] = value;

			getline(comboFile, strLine); // Skips "===============" <=- I put it here to make INI file more readable

			// Now reading COMBOs buttons.
			for (int i = 0; i < ActionCount[comboCount]; i++)
			{
				getline(comboFile, strLine); // ActionDelay
				cstrTemp = _strdup(strLine.c_str());
				strtok(cstrTemp, " ="); // Skip "ActionDelay  = "
				cstrToken = strtok(NULL, " ="); // read ActionDelay value..
				value = atoi(cstrToken);
				ActionDelay[i][comboCount] = value;

				getline(comboFile, strLine); // read a whole Action which is multiple buttons at the same time..
				cstrTemp = _strdup(strLine.c_str());
				
				int curButton = 0; 
				cstrToken = strtok(cstrTemp, " ");
				while(cstrToken != NULL)
				{
					value = atoi(cstrToken);
					COMBO[i][curButton][comboCount] = value;
					cstrToken = strtok(NULL, " ");
					curButton++;
				}
			}
		}

		comboFile.close();
	}
	else
		wxMessageBox("Can't open COMBOs file!!", "TwinPad", wxICON_ERROR);

}

// // // // // // // // // // // // // // /Load Configurations// // // // // // // // // // // // // // // // // /

// We only need to load the KEYS, because TwinPad.exe handle the job of Saving them,
void LoadConfig() {
	static bool LoadOnce = false;

	if (LoadOnce) return;

	LoadOnce = true;

    FILE *f;
	char str[256];
    char cfg[1024];
	int i, j;

	sprintf(cfg,"inis/TwinPad.ini");

    f = fopen(cfg,"r");
    if (f == NULL) return;
	// Skip Header.
	fseek(f, 31, SEEK_SET); // Header is 29 character + '\n' + carriage return.

	// Read data
	for (j=0; j<2; j++) {
		for (i=0; i<25; i++) {
			sprintf(str, "[%d][%d] = 0x%%x\n", j, i);
			fscanf(f, str, &confKeys[j][i]);
		}
	}
	
	for (i = 0; i < 8; i++)
	{
		sprintf(str, "[%d] = %%d\n", i);
		fscanf(f, str, &MouseButtonMap[i]);
	}
	sprintf(str, "[%d] = %%d\n", i);
	fscanf(f, str, &mouseScrollUp);
	i++;
	sprintf(str, "[%d] = %%d\n", i);
	fscanf(f, str, &mouseScrollDown);

	fscanf(f, "%d", &mousePAD);
	fscanf(f, "%d", &mouseSensitivity);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_PAD1);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_PAD2);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_MOUSE);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_COMBOS);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_ComboHotKey);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_KeyEvents);
	fscanf(f, "%d", &ExtendedOptions.IsEnabled_FasterCombo);
    fclose(f);

	// I have to invert every data member of ExtendedOptions, in the INI
	// 0 = Enabled, 1 = Disabled, which is the state of the CheckBoxes..
	// these from 1 to 0..
	ExtendedOptions.IsEnabled_PAD1		  = !ExtendedOptions.IsEnabled_PAD1;
	ExtendedOptions.IsEnabled_PAD2		  = !ExtendedOptions.IsEnabled_PAD2;
	ExtendedOptions.IsEnabled_MOUSE		  = !ExtendedOptions.IsEnabled_MOUSE;
	ExtendedOptions.IsEnabled_KeyEvents   = !ExtendedOptions.IsEnabled_KeyEvents;
	ExtendedOptions.IsEnabled_COMBOS      = !ExtendedOptions.IsEnabled_COMBOS;
	ExtendedOptions.IsEnabled_ComboHotKey = !ExtendedOptions.IsEnabled_ComboHotKey;
	// The last option we leave it intact, it's the only one means ENABLE, all the other are DISABLEd, lol..
	// ExtendedOptions.IsEnabled_FasterCombo = ExtendedOptions.IsEnabled_FasterCombo;
}

