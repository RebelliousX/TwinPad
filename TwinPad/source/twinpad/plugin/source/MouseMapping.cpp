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
			ScreenWidth = WndRect.right - WndRect.left;
			ScreenHeight = WndRect.bottom - WndRect.top;
			// ScreenWidth or Height means screens or windows Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
			lanalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			lanalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			mouseX = MousePt.x - rectMouseArea.left;
			mouseY = MousePt.y - rectMouseArea.top;
			if (MousePt.x > rectMouseArea.left && MousePt.x < rectMouseArea.right)
			{
				RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
				lanalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			}
			if (MousePt.y > rectMouseArea.top && MousePt.y < rectMouseArea.bottom)
			{
				RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
				lanalog[GUI_Config.m_mouseAsPad].y = RelativeY;
			}
			if (MousePt.x >= rectMouseArea.right)
				lanalog[GUI_Config.m_mouseAsPad].x = 255;
			if (MousePt.x <= rectMouseArea.left)
				lanalog[GUI_Config.m_mouseAsPad].x = 0;
			if (MousePt.y >= rectMouseArea.bottom)
				lanalog[GUI_Config.m_mouseAsPad].y = 255;
			if (MousePt.y <= rectMouseArea.top)
				lanalog[GUI_Config.m_mouseAsPad].y = 0;
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
			ScreenWidth = WndRect.right - WndRect.left;
			ScreenHeight = WndRect.bottom - WndRect.top;
			// ScreenWidth or Height means screen's or window's Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
			ranalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			ranalog[GUI_Config.m_mouseAsPad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			mouseX = MousePt.x - rectMouseArea.left;
			mouseY = MousePt.y - rectMouseArea.top;
			if (MousePt.x > rectMouseArea.left && MousePt.x < rectMouseArea.right)
			{
				RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
				ranalog[GUI_Config.m_mouseAsPad].x = RelativeX;
			}
			if (MousePt.y > rectMouseArea.top && MousePt.y < rectMouseArea.bottom)
			{
				RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
				ranalog[GUI_Config.m_mouseAsPad].y = RelativeY;
			}
			if (MousePt.x >= rectMouseArea.right)
				ranalog[GUI_Config.m_mouseAsPad].x = 255;
			if (MousePt.x <= rectMouseArea.left)
				ranalog[GUI_Config.m_mouseAsPad].x = 0;
			if (MousePt.y >= rectMouseArea.bottom)
				ranalog[GUI_Config.m_mouseAsPad].y = 255;
			if (MousePt.y <= rectMouseArea.top)
				ranalog[GUI_Config.m_mouseAsPad].y = 0;
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
		if (MouseState.rgbButtons[mouseButton] & 0x80)
			MouseInputMapper(GUI_Config.m_mouse[mouseButton]);
	}
}

void InitRects()
{
	static int counter = 0;

	if (counter++  == 60) // update every (second or half)..
	{
		GetWindowRect(hGFXwnd, &testRect);

		rectMouseArea = testRect;

		WndRect.left   = testRect.left   + 5;
		WndRect.right  = testRect.right  - 5;
		WndRect.bottom = testRect.bottom - 5;
		WndRect.top    = testRect.top    + 30;

		ScreenWidth  = rectMouseArea.right - rectMouseArea.left;
		ScreenHeight = rectMouseArea.bottom - rectMouseArea.top;

		if (GUI_Config.m_mouseSensitivity > 1)
		{
			int Xfactor = (mouseSensitivity - 1) * 64;
			int Yfactor = (mouseSensitivity - 1) * 48;
			
			rectMouseArea.left   += Xfactor;
			rectMouseArea.right  -= Xfactor;
			rectMouseArea.top    += Yfactor;
			rectMouseArea.bottom -= Yfactor;

			ScreenWidth  = rectMouseArea.right  - rectMouseArea.left;
			ScreenHeight = rectMouseArea.bottom - rectMouseArea.top;
		}

		counter = 0;
	}
}
