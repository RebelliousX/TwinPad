#include "fastCompile.h"
#include "Externals.h"
#include "twinpad_gui.h"

// // // // // // // // // // // // // // // /Mouse Mapping Functions // // // // // // // // // // // // // // // // // /
// ---------------------------------------------------------------------------------------------------------------------/

void MouseInputMapper(int func)
{
	// Check func values, supported values between 0 and 17
	if (func < 0 || func > 17)
	{
		// Error!
		wxString strError = wxString::Format("Invalid Value in MouseInputMapper(), Please Contact the Author.\nValue is: %d", func);

		wxMessageBox(strError, "TwinPad: Error", wxICON_ERROR);
		return;
	}


	// Left-Analog Stick
	if (func == 16)
	{
		static int RelativeX, RelativeY;
		static int mouseX, mouseY;

		if (GUI_Config.m_mouseSensitivity == 1)
		{   // Accurate and Old mode.
			mouseX = MousePt.x - WndRect.left;
			mouseY = MousePt.y - WndRect.top;
			// ScreenWidth or Height means screens or windows Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255);
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255);
			lanalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			lanalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			if (MousePt.x < (ScreenWidth / 2))
				mouseX = MousePt.x - WndRect.left - rectMouseArea.left;
			else
				mouseX = MousePt.x - WndRect.left + rectMouseArea.left;

			if (MousePt.y < (ScreenHeight / 2))
				mouseY = MousePt.y - WndRect.top - rectMouseArea.top;
			else
				mouseY = MousePt.y - WndRect.top + rectMouseArea.top;
			
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

			lanalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			lanalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
	}

	// Right Analog Stick
	if (func == 17)
	{
		static int RelativeX, RelativeY;
		static int mouseX, mouseY;

		if (GUI_Config.m_mouseSensitivity == 1)
		{   // Accurate and Old mode.
			mouseX = MousePt.x - WndRect.left;
			mouseY = MousePt.y - WndRect.top;
			// ScreenWidth or Height means screen's or window's Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255);
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255);
			ranalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			ranalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			if (MousePt.x < (ScreenWidth / 2))
				mouseX = MousePt.x - WndRect.left - rectMouseArea.left;
			else
				mouseX = MousePt.x - WndRect.left + rectMouseArea.left;

			if (MousePt.y < (ScreenHeight / 2))
				mouseY = MousePt.y - WndRect.top - rectMouseArea.top;
			else
				mouseY = MousePt.y - WndRect.top + rectMouseArea.top;

			float Xfactor = (float)rectMouseArea.right / ScreenWidth;
			float Yfactor = (float)rectMouseArea.bottom / ScreenHeight;

			RelativeX = (int)(((float)mouseX / rectMouseArea.right) * Xfactor * 255);
			RelativeY = (int)(((float)mouseY / rectMouseArea.bottom) * Yfactor * 255);

			if ((RelativeX > 128) && (RelativeX > (Xfactor * 255)))
				RelativeX = 255;
			if ((RelativeX < 128) && (RelativeX < (Xfactor * 25.5)))
				RelativeX = 0;

			if ((RelativeY > 128) && (RelativeY >(Yfactor * 255)))
				RelativeY = 255;
			if ((RelativeY < 128) && (RelativeY < (Yfactor * 25.5)))
				RelativeY = 0;

			ranalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			ranalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
	}
	
	// // /Pressing// // // // // // // // // // /
	// L2, R2, L1, R1, Triangle, Circle, Cross, Square, Select, from 0 to 8 respectively.
	if (func >= 0 && func <= 8)
		status[GUI_Config.m_mouseAsPad] &= ~(1 << func);
	// L3
	if (func == 9)
		lanalog[GUI_Config.m_mouseAsPad].button = 1;
	// R3
	if (func == 10)
		ranalog[GUI_Config.m_mouseAsPad].button = 1;
	// Start, Up, Right, Down, Left, from 11 to 15 respectively.
	if (func >= 11 && func <= 15)
		status[0] &= ~(1 << func);
	// // /Releasing// // // // // // // // // // 
	if (func >= 20 && func <= 28) return;
}

// // /This Function leads MouseInputMapper and tells him what to do.// // // // // // // // // // // // // // // // // 
void ProcMouseInput()
{
	if (!inside) return;  // If Mouse pointer is not inside the GS/GPU window.

	for (int mouseButton = 0; mouseButton < 8; ++mouseButton)
	{
		if ((MouseState.rgbButtons[mouseButton] & 0x80) && GUI_Config.m_mouse[mouseButton] != 36) // 36 is no button configured
			MouseInputMapper(GUI_Config.m_mouse[mouseButton]);
	}
}

void InitRects()
{
	static int counter = 0;

	if (counter++  == 60) // update every (second or half)..
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

		if (GUI_Config.m_mouseSensitivity > 1)
		{
			int Xfactor = (GUI_Config.m_mouseSensitivity - 1) * ScreenWidth / 20;
			int Yfactor = (GUI_Config.m_mouseSensitivity - 1) * ScreenHeight / 20;
			
			rectMouseArea.left   += Xfactor;
			rectMouseArea.right  -= Xfactor;
			rectMouseArea.top    += Yfactor;
			rectMouseArea.bottom -= Yfactor;
		}

		counter = 0;
	}
}
