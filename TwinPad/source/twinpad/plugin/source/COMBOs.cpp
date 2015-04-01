#include "COMBOs.h"
#include "twinpad_gui.h"
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // // // // // // // // // // // // // /Combo Functions// // // // // // // // // // // // // // // // // // // // // /
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
bool ExecCombo()
{
	static bool ComboStatus = false;

	static int counter = 0, delay = 0, activeCombo = -1;
	if (ComboStatus == false)
		for (int i = 0; i < GUI_Controls.Combos.size(); i++)
		{
			if (DIKEYDOWN(KeyState, GUI_Controls.Combos[i]->GetKey()) && GUI_Controls.Combos[i]->GetKey() != 0)
			{
				ComboStatus = true;
				activeCombo = i;
			}
		}
		
	if (ComboStatus)
	{
		static int curAction = 0;
		int numOfButtonsInAction = GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetNumberOfButtons();
		if (numOfButtonsInAction > 0)
		{
			for (int button = 0; button < numOfButtonsInAction; ++button)
			{
				int curButton = GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonValue;
				if (curButton < 16 && curButton >= 0)
					status[g_comboPAD] &= ~(1 << curButton);
				else
					if (curButton >= 16)
						ComboAnalog((int)curButton, g_comboPAD);
			}
		}
		if (++delay >= GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetDelay())
		{
			curAction++;
			delay = 0;
		}

		if (curAction >= GUI_Controls.Combos[activeCombo]->GetNumberActions())
			delay = curAction = ComboStatus = 0;

		return true;
	}
	else
		return false;

	/*if (ComboStatus)
	{
		static int j = 0;
		for (int k = 0; k < 24; k++)
		{
			if (COMBO[j][k][activeCombo] < 16 && COMBO[j][k][activeCombo] >= 0)
				status[g_comboPAD]&=~(1<<COMBO[j][k][activeCombo]);
			else
				if (COMBO[j][k][activeCombo] >= 16)
					ComboAnalog((int)COMBO[j][k][activeCombo], g_comboPAD);
				else
					if (COMBO[j][k][activeCombo] == -1)
						break;
		}
		if (delay++ >= ActionDelay[j][activeCombo])
		{
			j++;
			delay = 0;
		}
		if (j > ActionCount[activeCombo])
			delay = j = ComboStatus = 0;

		return true;
	}
	else
		return false;*/
}

void ComboAnalog(int analogKey, int pad)
{
	switch(analogKey)
	{
	case 16:
		lanalog[pad].y = minXY[pad];
		break;
	case 17:
		lanalog[pad].x = maxXY[pad];
		break;
	case 18:
		lanalog[pad].y = maxXY[pad];
		break;
	case 19:
		lanalog[pad].x = minXY[pad];
		break;
	case 20:
		ranalog[pad].y = minXY[pad];
		break;
	case 21:
		ranalog[pad].x = maxXY[pad];
		break;
	case 22:
		ranalog[pad].y = maxXY[pad];
		break;
	case 23:
		ranalog[pad].x = minXY[pad];
		break;
	default:
		break;
	}
}
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 