#include <windows.h>

#include "twinpad_gui.h"

#ifndef WX_PRECOM
#include "wx/wx.h"
#else
#include "wx/wxprec.h"
#endif

#include "MouseMapping.h"
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // // // // // // /Action Mapping Functions // // // // // // // // // // // // // // // // // // // // // // // // // // /
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
void ActionMapper(int func, int pad)
{
	// if func = 36 means NULL, cause the user did NOT configure that Button
	// also func = 56, the offset of 36 value, Also nothing to do here,
	// So quit quietly without error, and do nothing :)

	if (func == 36 || func == 56)
		return;

	// Check func values, supported values between 0 and 35
	if (func < 0 || func > 35)
	{
		// Error!
		char strError[] = "Invalid Value in ActionMapper(), Please Contact the Author.\nValue is:";
		char errCode[5];
		_itoa(func, errCode, 10);
		strcat(strError, errCode);

		wxMessageBox(strError, "TwinPad: Error", wxICON_ERROR);
		return;
	}

	// func 16 press and 17 release, about Left Analog Stick
	if (func == 16)
	{
		lbutDown = 1;

		static int RelativeX, RelativeY;
		static int mouseX, mouseY;

		if (mouseSensitivity == 1)
		{   // Accurate and Old mode.
			mouseX = MousePt.x  -  WndRect.left;
			mouseY = MousePt.y  - WndRect.top;
			ScreenWidth  = WndRect.right  - WndRect.left;
			ScreenHeight = WndRect.bottom - WndRect.top;
			// ScreenWidth or Height means screens or windows Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
			lanalog[pad].x = RelativeX;
			lanalog[pad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			mouseX = MousePt.x  -  rectMouseArea.left;
			mouseY = MousePt.y  - rectMouseArea.top;
			if (MousePt.x > rectMouseArea.left && MousePt.x < rectMouseArea.right)	
			{
				RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
				lanalog[pad].x = RelativeX;
			}
			if (MousePt.y > rectMouseArea.top && MousePt.y < rectMouseArea.bottom)
			{
				RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
				lanalog[pad].y = RelativeY;
			}
			if (MousePt.x >= rectMouseArea.right)
				lanalog[pad].x = 255;
			if (MousePt.x <= rectMouseArea.left)
				lanalog[pad].x = 0;
			if (MousePt.y >= rectMouseArea.bottom)
				lanalog[pad].y = 255;
			if (MousePt.y <= rectMouseArea.top)
				lanalog[pad].y = 0;
		}
	}
	if (func == 17)
		if (lbutDown == 1)
		{
			lbutDown = 0;
			ClipCursor(NULL);
			lanalog[pad].x = 0x80;
			lanalog[pad].y = 0x80;
		}
	
	// func 18 press and 19 release, about Right Analog Stick
	if (func == 18)
	{
		rbutDown = 1;

		static int RelativeX, RelativeY;
		static int mouseX, mouseY;

		
		if (mouseSensitivity == 1)
		{   // Accurate and Old mode.
			mouseX = MousePt.x  -  WndRect.left;
			mouseY = MousePt.y  - WndRect.top;
			ScreenWidth  = WndRect.right  - WndRect.left;
			ScreenHeight = WndRect.bottom - WndRect.top;
			// ScreenWidth or Height means screen's or window's Width or Height..
			RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
			RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
			ranalog[pad].x = RelativeX;
			ranalog[pad].y = RelativeY;
		}
		else
		{   // Trying to shrink the Dead Zone in the Center of the Window..
			mouseX = MousePt.x  -  rectMouseArea.left;
			mouseY = MousePt.y  - rectMouseArea.top;
			if (MousePt.x > rectMouseArea.left && MousePt.x < rectMouseArea.right)	
			{
				RelativeX = (int)((static_cast<float>(mouseX) / ScreenWidth) * 255) & 0xff;
				ranalog[pad].x = RelativeX;
			}
			if (MousePt.y > rectMouseArea.top && MousePt.y < rectMouseArea.bottom)
			{
				RelativeY = (int)((static_cast<float>(mouseY) / ScreenHeight) * 255) & 0xff;
				ranalog[pad].y = RelativeY;
			}
			if (MousePt.x >= rectMouseArea.right)
				ranalog[pad].x = 255;
			if (MousePt.x <= rectMouseArea.left)
				ranalog[pad].x = 0;
			if (MousePt.y >= rectMouseArea.bottom)
				ranalog[pad].y = 255;
			if (MousePt.y <= rectMouseArea.top)
				ranalog[pad].y = 0;
		}
	}
	if (func == 19)
		if (rbutDown == 1)
		{
			rbutDown = 0;
			ClipCursor(NULL);
			ranalog[pad].x = 0x80;
			ranalog[pad].y = 0x80;
		}

	// // /Pressing// // // // // // // // // // /
	// L2, R2, L1, R1, Triangle, Circle, Cross, Square, Select, from 0 to 8 respectively.
	if (func >= 0 && func <= 8)
		status[pad]&=~(1<<func);
	// L3
	if (func == 9)
		lanalog[pad].button = 1;
	// R3
	if (func == 10)
		ranalog[pad].button = 1;
	// Start, Up, Right, Down, Left, from 11 to 15 respectively.
	if (func >= 11 && func <= 15)
		status[0]&=~(1<<func);
	// // /Releasing// // // // // // // // // // 
	if (func >= 20 && func <= 28) return;
	
	/*  <- Releasing code became obsolete.. ->
	// L2, R2, L1, R1, Triangle, Circle, Cross, Square, Select, from 0 to 8 respectively.
	if (func >= 20 && func <= 28)
		status[pad]|= (1<<(func - 20));
	// L3
	if (func == 29)
		lanalog[pad].button = 0;
	// R3
	if (func == 30)
		ranalog[pad].button = 0;
	// Start, Up, Right, Down, Left, from 11 to 15 respectively.
	if (func >= 31 && func <= 35)
		status[pad]|= (1<<(func - 20));
	*/
}

// // /This Function leads ActionMapper and tells him what to do.// // // // // // // // // // // // // // // // // 
void ProcMouseInput(int pad)
{
	if (!inside) return;  // If Mouse pointer is not inside the GS/GPU window.

	// Check func values passed to ActionMapper, and handle Button release Offset values
	// Left Mouse Button
	if (MouseState.rgbButtons[0] & 0x80)
		ActionMapper(MouseButtonMap[0], pad);
	else
		if (MouseButtonMap[0] == 16 || MouseButtonMap[0] == 18)
			ActionMapper(MouseButtonMap[0] + 1, pad);
		else
			ActionMapper(MouseButtonMap[0] + 20, pad);

	// Right Mouse Button
	if (MouseState.rgbButtons[1] & 0x80)
		ActionMapper(MouseButtonMap[1], pad);
	else
		if (MouseButtonMap[1] == 16 || MouseButtonMap[1] == 18)
			ActionMapper(MouseButtonMap[1] + 1, pad);
		else
			ActionMapper(MouseButtonMap[1] + 20, pad);

		
	// Wheel Rotation handling, some games depend on pressure sensitive buttons. just altering pressure value (+/-)X5
	// Action Mapping for Wheel Rotation only if they were configured to do so, otherwise handle prssure sensitive buttons.
	if (MouseState.lZ != 0)
	{
		if (mouseScrollUp == 36 && mouseScrollDown == 36) // 36 = NONE in the configurations.
		{
			pressure += ((short)MouseState.lZ/120)*5;
			if(pressure >= 100) pressure = 100;
			if(pressure <= 0) pressure = 0;
		}
		else
			if (MouseState.lZ > 0)
				ActionMapper(mouseScrollUp, pad);
			else
				ActionMapper(mouseScrollDown, pad);
	}

	// Middle Mouse Button.
	if (MouseState.rgbButtons[2] & 0x80)
		ActionMapper(MouseButtonMap[2], pad);
	else
		if (MouseButtonMap[2] == 16 || MouseButtonMap[2] == 18)
			ActionMapper(MouseButtonMap[2] + 1, pad);
		else
			ActionMapper(MouseButtonMap[2] + 20, pad);

	// Mouse Button # 4.
	if (MouseState.rgbButtons[3] & 0x80)
		ActionMapper(MouseButtonMap[3], pad);
	else
		if (MouseButtonMap[3] == 16 || MouseButtonMap[3] == 18)
			ActionMapper(MouseButtonMap[3] + 1, pad);
		else
			ActionMapper(MouseButtonMap[3] + 20, pad);

	// Mouse Button # 5.
	if (MouseState.rgbButtons[4] & 0x80)
		ActionMapper(MouseButtonMap[4], pad);
	else
		if (MouseButtonMap[4] == 16 || MouseButtonMap[4] == 18)
			ActionMapper(MouseButtonMap[4] + 1, pad);
		else
			ActionMapper(MouseButtonMap[4] + 20, pad);

	// Mouse Button # 6.
	if (MouseState.rgbButtons[5] & 0x80)
		ActionMapper(MouseButtonMap[4], pad);
	else
		if (MouseButtonMap[5] == 16 || MouseButtonMap[5] == 18)
			ActionMapper(MouseButtonMap[5] + 1, pad);
		else
			ActionMapper(MouseButtonMap[5] + 20, pad);

	// Mouse Button # 7.
	if (MouseState.rgbButtons[6] & 0x80)
		ActionMapper(MouseButtonMap[6], pad);
	else
		if (MouseButtonMap[6] == 16 || MouseButtonMap[6] == 18)
			ActionMapper(MouseButtonMap[6] + 1, pad);
		else
			ActionMapper(MouseButtonMap[6] + 20, pad);

	// Mouse Button # 8.
	if (MouseState.rgbButtons[7] & 0x80)
		ActionMapper(MouseButtonMap[7], pad);
	else 
		if (MouseButtonMap[7] == 16 || MouseButtonMap[7] == 18)
			ActionMapper(MouseButtonMap[7] + 1, pad);
		else
			ActionMapper(MouseButtonMap[7] + 20, pad);

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

		if (mouseSensitivity > 1)
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
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // End of Action Mapping Functions// // // // // // // // // // // // // // // // // // // // // // // // // // 
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
