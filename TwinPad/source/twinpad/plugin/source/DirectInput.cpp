#include "DirectInput.h"

#include "twinpad_gui.h"


#include <dinput.h>

// DirectInput Variables
LPDIRECTINPUT8 g_DI = NULL;	// Root DirectInput Interface
LPDIRECTINPUTDEVICE8 g_DIKeyboard = NULL;	// The keyboard device
LPDIRECTINPUTDEVICE8 g_DIMouse = NULL;	// The mouse device
DIMOUSESTATE2 MouseState;					// this holds the mouse data

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
// // // // // // // // // // // // // // // // // // // // /Added DirectInput Functions// // // // // // // // // // // // // 
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /

// Initialise DirectInput contains: CreateDevice, SetDataFormat, Acuire the Device... :)
bool InitDI()
{
	// Create the abstract DirectInput connection
	if (g_DI == NULL)
	{
		DirectInput8Create(
			hDI,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&g_DI,
			NULL);
	}

	if (g_DI == NULL)
	{
		wxMessageBox("DirectInput Connection Creation Failed!", "TwinPad", wxICON_EXCLAMATION);
		return FALSE;
	}
	
	// // // // // // // // // // Keyboard Initialization// // // // // // // // // // // /

	if (g_DI != NULL)
	{
		// Create the connection to the keyboard device
		g_DI->CreateDevice(GUID_SysKeyboard, &g_DIKeyboard, NULL);
		if (g_DIKeyboard)
		{
			g_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
			
			g_DIKeyboard->SetCooperativeLevel(hGFXwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY); 

			g_DIKeyboard->Acquire();
		}
		else
		{
			wxMessageBox("DirectInput Keyboard initialization Failed!", "TwinPad", wxICON_EXCLAMATION);
			return FALSE;
		}

		// // // // // // // // // Mouse Initializing// // // // // // // // // 
		
		g_DI->CreateDevice(GUID_SysMouse, &g_DIMouse, NULL);
		
		if (g_DIMouse)
		{
			g_DIMouse->SetCooperativeLevel(hGFXwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			g_DIMouse->SetDataFormat(&c_dfDIMouse2);

			g_DIMouse->Acquire();		
		}
		else
		{
			wxMessageBox("DirectInput Mouse Initialization Failed!!", "TwinPad", wxICON_EXCLAMATION);
			return FALSE;
		}
		
		return TRUE;
	}

	return FALSE;
// --- End of DirectInput initialization ---
}

// Terminate DirectInput Objects, First Unacquire Keyboard & Mouse 
// devices, Release them, then Release main DI Obj.
bool TermDI()
{
	if (g_DI != NULL)
	{
		// First Deal with Keyboard.
		g_DIKeyboard->Unacquire();
		g_DIKeyboard->Release();
		g_DIKeyboard = NULL;
		
		// Second Deal with Mouse
		g_DIMouse->Unacquire();
		g_DIMouse->Release();
		g_DIMouse = NULL;

		// Finally Deal with main DI Object.
		g_DI->Release();
		g_DI = NULL;

		return true;
	}

	return false;
}

// Get Keyboard buttons status, and save a snapshot in KeyState Character Array (1 Byte / Element)
void GetKeyboardStatus()
{
	// Test if Error occurred by another application that acquired the device

	while(FAILED(g_DIKeyboard->GetDeviceState(256, (LPVOID)KeyState)))
     {
		// try an re-acquire the device
		if (FAILED(g_DIKeyboard->Acquire()))
			break; // serious error
     }
}

void GetMouseStatus()
{
	if (hGFXwnd != GetForegroundWindow()) return;
	
	// Test if Error occurred by another application that acquired the device
	while(FAILED(g_DIMouse->GetDeviceState(
		sizeof(DIMOUSESTATE2), &MouseState)))
     {
		// try an re-acquire the device
		if (FAILED(g_DIMouse->Acquire()))
        {
        return; // serious error
        }
     }

	// Using Win32Api to get cursor pos, it's more convinient while playing
	// in window mode, as the user may want to run other apps as will.
	GetCursorPos(&MousePt);

	// Check to see if the cursor is inside GS window.
	// if so, process Inputs, else skip them.
	if (MousePt.x >= WndRect.left && MousePt.x <= WndRect.right &&
		MousePt.y >= WndRect.top  && MousePt.y <= WndRect.bottom)
	{
		bool isPressed = false;
		for (int i = 0; i < 8; ++i)
			if (MouseState.rgbButtons[i] & 0x80)
			{
				ClipCursor(&WndRect);
				inside = true;
				isPressed = true;
				break;
			}
		if (isPressed == false)
		{
			ClipCursor(0);
			inside = false;
		}
	}
	else
	{
		ClipCursor(NULL);
		inside = false;
	}
}
// // // // // // // // // // // // // // // // // // // // // // // // End of DirectInput Funtions// // // // // // // // // // // // // // // // // // // // /