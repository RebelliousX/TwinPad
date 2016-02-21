#include "stdafx.h"
#include "Externals.h"
#include "twinpad_gui.h"

// // // // // // // // // // // // // // // /Mouse Mapping Functions // // // // // // // // // // // // // // // // // /
// ---------------------------------------------------------------------------------------------------------------------/

void MouseInputMapper(int button)
{
	// Check button values, supported values between 0 and 17
	if (button < 0 || button > 17)
	{
		// Error!
		wxString strError = wxString::Format("Invalid Value in MouseInputMapper(), Please Contact the Author.\nValue is: %d", button);

		wxMessageBox(strError, "TwinPad: Error", wxICON_ERROR);
		return;
	}

	// Left-Analog Stick 16, Right-Analog Stick 17
	if (button == 16 || button == 17)
	{
		static int RelativeX, RelativeY;
		static int mouseX, mouseY;

		IM.GetMousePosition(mouseX, mouseY);

		if (Configurations.m_mouseSensitivity == 1)
		{   // Accurate and Old mode.
			// ScreenWidth or Height means screens or windows Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255);
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255);
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			if (mouseX < (ScreenWidth / 2))
				mouseX = mouseX - WndRect.left - rectMouseArea.left;
			else
				mouseX = mouseX - WndRect.left + rectMouseArea.left;

			if (mouseY < (ScreenHeight / 2))
				mouseY = mouseY - WndRect.top - rectMouseArea.top;
			else
				mouseY = mouseY - WndRect.top + rectMouseArea.top;
			
			float Xfactor = (float)rectMouseArea.right / ScreenWidth;
			float Yfactor = (float)rectMouseArea.bottom / ScreenHeight;

			RelativeX = (int)(((float)mouseX / rectMouseArea.right) * Xfactor * 255);
			RelativeY = (int)(((float)mouseY / rectMouseArea.bottom) * Yfactor * 255);

			if ((RelativeX > 128) && (RelativeX > (Xfactor * 255)))
				RelativeX = 255;
			if ((RelativeX < 128) && (RelativeX < (Xfactor * 25.5)))
				RelativeX = 0;

			if ((RelativeY > 128) && (RelativeY > (Yfactor * 255)))
				RelativeY = 255;
			if ((RelativeY < 128) && (RelativeY < (Yfactor * 25.5)))
				RelativeY = 0;
		}

		if (button == 16)
		{
			lanalog[Configurations.m_mouseAsPad].x = RelativeX;
			lanalog[Configurations.m_mouseAsPad].y = RelativeY;
		}

		if (button == 17)
		{
			ranalog[Configurations.m_mouseAsPad].x = RelativeX;
			ranalog[Configurations.m_mouseAsPad].y = RelativeY;
		}
	}
	
	// // /Pressing// // // // // // // // // // /
	// L2, R2, L1, R1, Triangle, Circle, Cross, Square, Select, from 0 to 8 respectively.
	if (button >= 0 && button <= 8)
		status[Configurations.m_mouseAsPad] &= ~(1 << button);
	// L3
	if (button == 9)
		lanalog[Configurations.m_mouseAsPad].button = 1;
	// R3
	if (button == 10)
		ranalog[Configurations.m_mouseAsPad].button = 1;
	// Start, Up, Right, Down, Left, from 11 to 15 respectively.
	if (button >= 11 && button <= 15)
		status[0] &= ~(1 << button);
	// // /Releasing// // // // // // // // // // 
	if (button >= 20 && button <= 28) return;
}

// // /This Function leads MouseInputMapper and tells him what to do.// // // // // // // // // // // // // // // // // 
void ProcMouseInput()
{
	if (!inside) return;  // If Mouse pointer is not inside the GS/GPU window.

	for (int mouseButton = 0; mouseButton < 8; ++mouseButton)
	{
		if ((/*MouseState.rgbButtons[mouseButton] & 0x80*/IM.IsButtonDown(mouseButton)) && Configurations.m_mouse[mouseButton] != 36) // 36 is no button configured
			MouseInputMapper(Configurations.m_mouse[mouseButton]);
	}

	// Mouse wheel, scroll up/down
	if (/*MouseState.lZ*/ IM.GetMouseWheel() != 0)
	{
		if (Configurations.m_mouse[8] == 36 || Configurations.m_mouse[9] == 36)
		{
			short prs = 255;
			prs += ((short)/*MouseState.lZ*/ IM.GetMouseWheel() / 120) * 5;
			if (prs >= 255) prs = 255;
			if (prs <= 0) prs = 0;

			u8 pressure_ = prs;	//VS2015 complains about conversion from short to u8
			Pressure = { pressure_, pressure_, pressure_, pressure_, pressure_, pressure_,
				pressure_, pressure_, pressure_, pressure_, pressure_, pressure_ };
		}
		else
		{
			// Scroll Up
			if (Configurations.m_mouse[8] != 36 && /*MouseState.lZ*/ IM.GetMouseWheel() > 0)
				MouseInputMapper(Configurations.m_mouse[8]);

			// Scroll Down
			if (Configurations.m_mouse[9] != 36 && /*MouseState.lZ*/ IM.GetMouseWheel() < 0)
				MouseInputMapper(Configurations.m_mouse[9]);
		}
	}
}

void InitRects()
{
	static int counter = 60;

	if (counter++  > 59) // update every (second or half)..
	{
		POINT ptClientUL;
		POINT ptClientBR;

		GetClientRect(hGFXwnd, &rectMouseArea);

		ptClientUL.x = rectMouseArea.left;
		ptClientUL.y = rectMouseArea.top;
		ptClientBR.x = rectMouseArea.right + 1;
		ptClientBR.y = rectMouseArea.bottom + 1;

		ClientToScreen(hGFXwnd, &ptClientUL);
		ClientToScreen(hGFXwnd, &ptClientBR);

		SetRect(&WndRect, ptClientUL.x, ptClientUL.y, ptClientBR.x, ptClientBR.y);

		ScreenWidth = rectMouseArea.right;
		ScreenHeight = rectMouseArea.bottom;
		
		IM.SetWindowExtents(ScreenWidth, ScreenHeight);

		if (Configurations.m_mouseSensitivity > 1)
		{
			int Xfactor = (Configurations.m_mouseSensitivity - 1) * ScreenWidth / 20;
			int Yfactor = (Configurations.m_mouseSensitivity - 1) * ScreenHeight / 20;
			
			rectMouseArea.left   += Xfactor;
			rectMouseArea.right  -= Xfactor;
			rectMouseArea.top    += Yfactor;
			rectMouseArea.bottom -= Yfactor;
		}

		counter = 0;
	}
}
