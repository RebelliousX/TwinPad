#include "stdafx.h"
#include "DirectInput.h"
//#include "twinpad_gui.h"
#include "Externals.h"

InputManager IM;

bool InitializeInputManager()
{
	IM.CreateInputSystem((unsigned long)hGFXwnd);
	
	if (IM.GetInputSystemStatus())
		return true;
	return false;
}

bool TerminateInputManager()
{
	IM.DestroyInputSystem();
	return true;
}

// Capture all input devices status
void CaptureInputDevicesStatus()
{
	IM.CaptureState();
	IM.ScanKeyboard();
}

// For now, it is Windows only. I don't know if linux supports clipping mouse area
void ClipMouseArea()
{
	int x, y;
	IM.GetMousePosition(x, y);
	
#ifdef _WIN32
	// Check to see if the cursor is inside GS window.
	// if so, process Inputs, else skip them.
	// Also, don't clip cursor area when trying to move or resize the window
	if(y != 0 && x != 0 && IM.GetWindowWidth() != x && IM.GetWindowHeight() != y)
	{
		bool isPressed = false;
		
		for (int i = 0; i < 8; ++i)
			if (IM.IsButtonDown(i))
			{
				ClipCursor(&WndRect);
				inside = true;
				isPressed = true;
				break;
			}
		
		if (IM.GetMouseWheel() != 0)
		{
			inside = true;
			isPressed = true;
		}

		if (isPressed == false)
		{
			ClipCursor(0);
			inside = false;
		}
	}
#endif
}
