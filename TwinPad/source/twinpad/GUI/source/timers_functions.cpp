#pragma once

#include "timers_functions.h"
#include "DirectInput.h"
#include "Externals.h"
#include "twinpad_gui.h"
#include "labels.h"

// // // // // // // // // // // Timer Related Functions // // // // // // // // // // //

bool IsValidKey(unsigned char key)
{
	switch (key)
	{
	case 0x1:	// ESCAPE
	case 0x3B:	// F1
	case 0x3C:	// F2
	case 0x3D:	// F3
	case 0x3E:	// F4
	case 0x3F:	// F5
	case 0x40:	// F6
	case 0x41:	// F7
	case 0x42:	// F8
	case 0x43:	// F9
	case 0x44:	// F10
		return false;
	default:
		return true;
	}
}

// ReAnimate Analog Sticks timer event. Because after some time, the Animation will go out of sync
void OnTimeReAnimateAnalogSticks()
{
	for (int i = (int)PS2BUTTON::LANALOG_UP; i <= (int)PS2BUTTON::RANALOG_LEFT; ++i)
	{
		GUI_Controls.psComboButtons[i]->Stop();
		GUI_Controls.psComboButtons[i]->Play();
	}
}

// Helper function called from OnTimeGetKeyForKeyboard(), OnTimeGetKeyForCombo() and OnTimeGetHotKey()
keyInformation GetKey()
{
	static bool gotKey = false;

	keyInformation keyInfo;
	keyInfo.keyName = "";
	keyInfo.keyValue = 0;

	if (!gotKey)
	{
		static bool keyIsDown = false;
		static unsigned char key = 0;
		GetKeyboardStatus();
		if (!keyIsDown)
		{
			for (unsigned char i = 0; i < 255; ++i)
				if (DIKEYDOWN(KeyState, i))	// Key pressed
				{
					if (!IsValidKey(i))
						return keyInfo;		// empty key info
					keyIsDown = true;
					key = i;
					break;
				}
		}
		else
		{
			if (!DIKEYDOWN(KeyState, key))	// Key released
			{
				wxString keyName = "";
				for (int i = 0; i < sizeof(DIK_KEYCODES) / sizeof(*DIK_KEYCODES); ++i)	// Size of array 144
					if (key == DIK_KEYCODES[i].keyValue)
						keyName = DIK_KEYCODES[i].name;
				keyName = keyName.substr(4, keyName.length());		// Skip "DIK_"
				keyInfo.keyName = keyName;
				keyInfo.keyValue = key;
				key = 0;
				keyIsDown = false;
				gotKey = false;
				return keyInfo;				// key info contains pressed key
			}
		}
	}
	return keyInfo;		// empty key info
}

// Get one Key for the Keyboard tab when user 
void OnTimeGetKeyForKeyboard()
{
	keyInformation keyInfo = GetKey();
	if (keyInfo.keyName != "")
	{
		if (GUI_Controls.indexOfButton < 24)				// 0 to 23 are PSX/PS2 buttons
		{
			GUI_Controls.lblCtrl[GUI_Controls.indexOfButton]->SetLabel(keyInfo.keyName);
			GUI_Controls.lblCtrl[GUI_Controls.indexOfButton]->SetKeyCode(keyInfo.keyValue);
			GUI_Controls.animCtrl[GUI_Controls.indexOfButton]->Stop();
			GUI_Config.m_pad[(GUI_Controls.pad1RadioBtn->GetValue() ? 0 : 1)][GUI_Controls.indexOfButton] = keyInfo.keyValue;
		}
		else if (GUI_Controls.indexOfButton == 24)
		{
			GUI_Controls.lblWalkRun->SetLabel(keyInfo.keyName);
			GUI_Controls.lblWalkRun->SetKeyCode(keyInfo.keyValue);
			GUI_Config.m_pad[(GUI_Controls.pad1RadioBtn->GetValue() ? 0 : 1)][GUI_Controls.indexOfButton] = keyInfo.keyValue;
		}
		GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#100075"));	// Dark Blue
		GUI_Controls.lblEdit->SetLabel("Current button to edit: NONE");
		// reset timer for the next time
		GUI_Controls.indexOfButton = -1;
		GUI_Controls.mainFrame->tmrGetKey->Stop();
	}
}

void OnTimeAutoNavigateAndAssign()
{
	static int curIndex = 0;	// Start navigation with the first button: L2

	GUI_Controls.indexOfButton = curIndex;

	if (curIndex == 0 && !GUI_Controls.animCtrl[0]->IsPlaying())
		GUI_Controls.animCtrl[0]->Play();

	if (curIndex > 24)	// 0-23 ps buttons, 24 walk/run
	{
		curIndex = 0;
		GUI_Controls.indexOfButton = -1;
		GUI_Controls.mainFrame->tmrAutoNavigate->Stop();

		GUI_Controls.pad1RadioBtn->Enable(true);
		GUI_Controls.pad2RadioBtn->Enable(true);
		GUI_Controls.btnAutoNavigate->Enable(true);
		GUI_Controls.btnNullifiesAll->Enable(true);
		GUI_Controls.btnOK->Enable(true);
		GUI_Controls.btnCancel->Enable(true);
	}

	if (curIndex == GUI_Controls.indexOfButton)
	{
		GUI_Controls.lblEdit->SetBackgroundColour(wxColor("#990000"));	// Crimson -bloody red- :)
		if (curIndex < 24)
			GUI_Controls.lblEdit->SetLabel(wxString::Format("Edit Button: %s", PS_LABEL[curIndex].name));
		else if (curIndex == 24)
			GUI_Controls.lblEdit->SetLabel("Edit Button: Walk or Run");

		OnTimeGetKeyForKeyboard();

		if (GUI_Controls.indexOfButton == -1)
		{
			if (curIndex < 24)
				GUI_Controls.animCtrl[curIndex]->Stop();

			++curIndex;

			if (curIndex < 24)
				GUI_Controls.animCtrl[curIndex]->Play();

			GUI_Controls.indexOfButton = curIndex;
		}
	}
}

void OnTimeGetKeyForCombo()
{
	static int counter = 20;
	
	if (++counter > 20)		// Toggle background color every 1 second (50 milliseconds x 20)
	{
		counter = 0;
		if (GUI_Controls.lblComboKey->GetBackgroundColour() == wxColor("#990000"))	// Crimson bloody red :)
		{
			GUI_Controls.lblComboKey->SetBackgroundColour(wxColor("#100075"));		// Dark Blue
			GUI_Controls.lblComboKey->SetForegroundColour(wxColor("White"));
		}
		else
		{
			GUI_Controls.lblComboKey->SetBackgroundColour(wxColor("#990000"));
			GUI_Controls.lblComboKey->SetForegroundColour(wxColor("White"));
		}
		GUI_Controls.lblComboKey->Refresh();
	}

	keyInformation keyInfo;
	keyInfo = GetKey();
	if (keyInfo.keyName != "")
	{
		GUI_Controls.mainFrame->tmrGetComboKey->Stop();
		GUI_Controls.lblComboKey->SetLabel(keyInfo.keyName);
		GUI_Controls.lblComboKey->SetBackgroundColour(wxColor(66,66,66));			// Dark Grey
		GUI_Controls.lblComboKey->SetForegroundColour(wxColor("White"));
		GUI_Controls.lblComboKey->Refresh();
		counter = 20;
	}
}

void OnTimeGetHotKey()
{
	static int counter = 20;

	if (++counter > 20)		// Toggle background color every 1 second (50 milliseconds x 20)
	{
		counter = 0;
		if (GUI_Controls.lblHotKey->GetBackgroundColour() == wxColor("#990000"))	// Crimson bloody red :)
		{
			GUI_Controls.lblHotKey->SetBackgroundColour(wxColor("#100075"));		// Dark Blue
			GUI_Controls.lblHotKey->SetForegroundColour(wxColor("White"));
		}
		else
		{
			GUI_Controls.lblHotKey->SetBackgroundColour(wxColor("#990000"));
			GUI_Controls.lblHotKey->SetForegroundColour(wxColor("White"));
		}
		GUI_Controls.lblHotKey->Refresh();
	}

	keyInformation keyInfo;
	keyInfo = GetKey();
	if (keyInfo.keyName != "")
	{
		GUI_Controls.mainFrame->tmrGetHotKey->Stop();
		GUI_Controls.lblHotKey->SetLabel(keyInfo.keyName);
		GUI_Controls.lblHotKey->SetBackgroundColour(wxColor(66, 66, 66));			// Dark Grey
		GUI_Controls.lblHotKey->SetForegroundColour(wxColor("White"));
		GUI_Controls.lblHotKey->Refresh();
		counter = 21;
	}
}