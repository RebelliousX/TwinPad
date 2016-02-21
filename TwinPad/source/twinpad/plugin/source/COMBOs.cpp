#include "stdafx.h"
#include "COMBOs.h"
#include "twinpad_gui.h"
#include "Externals.h"

// Execute a Combo for pad 1 or 2 (could be both at the same time)
void ExecuteCombo(const int pad)
{
	static bool ComboStatusPad1 = false;
	static bool ComboStatusPad2 = false;
	bool *ComboStatus = (pad == 0) ? &ComboStatusPad1 : &ComboStatusPad2;

	static int delayPad1 = 0, delayPad2 = 0, activeComboPad1 = -1, activeComboPad2 = -1;
	int *delay = (pad == 0) ? &delayPad1 : &delayPad2;
	int *activeCombo = (pad == 0) ? &activeComboPad1 : &activeComboPad2;

	if (*ComboStatus == false)
		for (size_t i = 0; i < Configurations.Combos.size(); i++)
		{
			CCombo *thisCombo = Configurations.Combos[i];
			if (thisCombo->GetPad() == pad)
			{
				if (IM.IsKeyDown(thisCombo->GetKey()) && thisCombo->GetKey() != 0)
				{
					*ComboStatus = true;
					*activeCombo = i;
				}
			}
		}

	if (*ComboStatus)
	{
		if (Configurations.Combos[*activeCombo]->GetPad() == pad)
		{
			static int curActionPad1 = 0, curActionPad2 = 0;
			int *curAction = (pad == 0) ? &curActionPad1 : &curActionPad2;
			CAction *thisAction = Configurations.Combos[*activeCombo]->GetAction(*curAction);
			int numOfButtonsInAction = thisAction->GetNumberOfButtons();
			if (numOfButtonsInAction > 0)
			{
				for (int button = 0; button < numOfButtonsInAction; ++button)
				{
					int curButton = thisAction->GetButton(button)->buttonValue;
					u8 buttonSensitivity = (u8)thisAction->GetButton(button)->buttonSensitivity;
					if (curButton < 16 && curButton >= 0)
					{
						status[pad] &= ~(1 << curButton);
						switch ((PS2BUTTON)curButton)
						{
						case PS2BUTTON::CIRCLE:
							Pressure.Circle = buttonSensitivity;
							break;
						case PS2BUTTON::SQUARE:
							Pressure.Square = buttonSensitivity;
							break;
						case PS2BUTTON::TRIANGLE:
							Pressure.Triangle = buttonSensitivity;
							break;
						case PS2BUTTON::CROSS:
							Pressure.Cross = buttonSensitivity;
							break;
						case PS2BUTTON::L1:
							Pressure.L1 = buttonSensitivity;
							break;
						case PS2BUTTON::L2:
							Pressure.L2 = buttonSensitivity;
							break;
						case PS2BUTTON::R1:
							Pressure.R1 = buttonSensitivity;
							break;
						case PS2BUTTON::R2:
							Pressure.R2 = buttonSensitivity;
							break;
						case PS2BUTTON::UP:
							Pressure.Up = buttonSensitivity;
							break;
						case PS2BUTTON::RIGHT:
							Pressure.Right = buttonSensitivity;
							break;
						case PS2BUTTON::DOWN:
							Pressure.Down = buttonSensitivity;
							break;
						case PS2BUTTON::LEFT:
							Pressure.Left = buttonSensitivity;
							break;
						}
					}
					else if (curButton >= 16)
					{
						switch (curButton)
						{
						case 16:
							lanalog[pad].y = buttonSensitivity;
							break;
						case 17:
							lanalog[pad].x = buttonSensitivity;
							break;
						case 18:
							lanalog[pad].y = buttonSensitivity;
							break;
						case 19:
							lanalog[pad].x = buttonSensitivity;
							break;
						case 20:
							ranalog[pad].y = buttonSensitivity;
							break;
						case 21:
							ranalog[pad].x = buttonSensitivity;
							break;
						case 22:
							ranalog[pad].y = buttonSensitivity;
							break;
						case 23:
							ranalog[pad].x = buttonSensitivity;
							break;
						default:
							break;
						}
					}
				}
			}

			CCombo *thisCombo = Configurations.Combos[*activeCombo];
			if (++(*delay) >= (int) thisCombo->GetAction(*curAction)->GetDelay())
			{
				(*curAction)++;
				*delay = 0;
			}

			if (*curAction >= thisCombo->GetNumberActions())
				*delay = *curAction = *ComboStatus = 0;
		}
	}
}
