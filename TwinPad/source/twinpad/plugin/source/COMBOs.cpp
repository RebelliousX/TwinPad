#include "COMBOs.h"
#include "labels.h"
#include "twinpad_gui.h"
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // // // // // // // // // // // // // /Combo Functions// // // // // // // // // // // // // // // // // // // // // /
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
void ExecComboPad1(int pad)
{
	static bool ComboStatus = false;

	static int counter = 0, delay = 0, activeCombo = -1;
	if (ComboStatus == false)
		for (unsigned int i = 0; i < Configurations.Combos.size(); i++)
		{
			if (Configurations.Combos[i]->GetPad() == pad)
			{
				if (DIKEYDOWN(KeyState, Configurations.Combos[i]->GetKey()) && Configurations.Combos[i]->GetKey() != 0)
				{
					ComboStatus = true;
					activeCombo = i;
				}
			}
		}
		
	if (ComboStatus)
	{
		if (Configurations.Combos[activeCombo]->GetPad() == pad)
		{
			static int curAction = 0;
			int numOfButtonsInAction = Configurations.Combos[activeCombo]->GetAction(curAction)->GetNumberOfButtons();
			if (numOfButtonsInAction > 0)
			{
				for (int button = 0; button < numOfButtonsInAction; ++button)
				{
					int curButton = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonValue;
					if (curButton < 16 && curButton >= 0)
					{
						status[pad] &= ~(1 << curButton);
						switch ((PS2BUTTON)curButton)
						{
						case PS2BUTTON::CIRCLE:
							Pressure.Circle = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::SQUARE:
							Pressure.Square = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::TRIANGLE:
							Pressure.Triangle = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::CROSS:
							Pressure.Cross = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::L1:
							Pressure.L1 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::L2:
							Pressure.L2 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::R1:
							Pressure.R1 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::R2:
							Pressure.R2 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::UP:
							Pressure.Up = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::RIGHT:
							Pressure.Right = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::DOWN:
							Pressure.Down = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::LEFT:
							Pressure.Left = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						}
					}
					else
						if (curButton >= 16)
							ComboAnalog((int)curButton, pad);
				}
			}
			if (++delay >= Configurations.Combos[activeCombo]->GetAction(curAction)->GetDelay())
			{
				curAction++;
				delay = 0;
			}

			if (curAction >= Configurations.Combos[activeCombo]->GetNumberActions())
				delay = curAction = ComboStatus = 0;
		}
	}
}

void ExecComboPad2(int pad)
{
	static bool ComboStatus = false;

	static int counter = 0, delay = 0, activeCombo = -1;
	if (ComboStatus == false)
		for (unsigned int i = 0; i < Configurations.Combos.size(); i++)
		{
			if (Configurations.Combos[i]->GetPad() == pad)
			{
				if (DIKEYDOWN(KeyState, Configurations.Combos[i]->GetKey()) && Configurations.Combos[i]->GetKey() != 0)
				{
					ComboStatus = true;
					activeCombo = i;
				}
			}
		}

	if (ComboStatus)
	{
		if (Configurations.Combos[activeCombo]->GetPad() == pad)
		{
			static int curAction = 0;
			int numOfButtonsInAction = Configurations.Combos[activeCombo]->GetAction(curAction)->GetNumberOfButtons();
			if (numOfButtonsInAction > 0)
			{
				for (int button = 0; button < numOfButtonsInAction; ++button)
				{
					int curButton = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonValue;
					if (curButton < 16 && curButton >= 0)
					{
						status[pad] &= ~(1 << curButton);
						switch ((PS2BUTTON)curButton)
						{
						case PS2BUTTON::CIRCLE:
							Pressure.Circle = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::SQUARE:
							Pressure.Square = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::TRIANGLE:
							Pressure.Triangle = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::CROSS:
							Pressure.Cross = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::L1:
							Pressure.L1 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::L2:
							Pressure.L2 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::R1:
							Pressure.R1 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::R2:
							Pressure.R2 = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::UP:
							Pressure.Up = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::RIGHT:
							Pressure.Right = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::DOWN:
							Pressure.Down = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						case PS2BUTTON::LEFT:
							Pressure.Left = Configurations.Combos[activeCombo]->GetAction(curAction)->GetButton(button)->buttonSensitivity;
							break;
						}
					}
					else
						if (curButton >= 16)
							ComboAnalog((int)curButton, pad);
				}
			}
			if (++delay >= Configurations.Combos[activeCombo]->GetAction(curAction)->GetDelay())
			{
				curAction++;
				delay = 0;
			}

			if (curAction >= Configurations.Combos[activeCombo]->GetNumberActions())
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