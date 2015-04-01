#include "COMBOs.h"
#include "twinpad_gui.h"
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // // // // // // // // // // // // // /Combo Functions// // // // // // // // // // // // // // // // // // // // // /
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
void ExecCombo(int pad)
{
	static bool ComboStatus = false;

	static int counter = 0, delay = 0, activeCombo = -1;
	if (ComboStatus == false)
		for (unsigned int i = 0; i < GUI_Controls.Combos.size(); i++)
		{
			if (GUI_Controls.Combos[i]->GetPad() == pad)
			{
				if (DIKEYDOWN(KeyState, GUI_Controls.Combos[i]->GetKey()) && GUI_Controls.Combos[i]->GetKey() != 0)
				{
					ComboStatus = true;
					activeCombo = i;
				}
			}
		}
		
	if (ComboStatus)
	{
		if (GUI_Controls.Combos[activeCombo]->GetPad() == pad)
		{
			static int curAction = 0;
			int numOfButtonsInAction = GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetNumberOfButtons();
			if (numOfButtonsInAction > 0)
			{
				for (int button = 0; button < numOfButtonsInAction; ++button)
				{
					int curButton = GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonValue;
					if (curButton < 16 && curButton >= 0)
						status[pad] &= ~(1 << curButton);
					else
						if (curButton >= 16)
							ComboAnalog((int)curButton, pad);
				}
			}
			if (++delay >= GUI_Controls.Combos[activeCombo]->GetAction(curAction)->GetDelay())
			{
				curAction++;
				delay = 0;
			}

			if (curAction >= GUI_Controls.Combos[activeCombo]->GetNumberActions())
				delay = curAction = ComboStatus = 0;
		}
	}
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