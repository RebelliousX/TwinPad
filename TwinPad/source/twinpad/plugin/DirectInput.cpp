#include <windows.h>
#include "DirectInput.h"
#include "DI_Globals.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////Added DirectInput Functions//////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//Initialise DirectInput contains: CreateDevice, SetDataFormat, Acuire the Device... :)
bool InitDI()
{
	// Create the abstract DirectInput connection
	if (fDI == NULL)
	{
		DirectInput8Create(
			hDI,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&fDI,
			NULL);
	}

	if (fDI == NULL)
	{
		MessageBox(NULL, "DirectInput Connection Creation Failed!", "TwinPad", NULL);
		return FALSE;
	}
	
	////////////////////Keyboard Initialization///////////////////////

	if (fDI != NULL)
	{
		// Create the connection to the keyboard device
		fDI->CreateDevice(GUID_SysKeyboard, &fDIKeyboard, NULL);
		if (fDIKeyboard)
		{
			fDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
			
			fDIKeyboard->SetCooperativeLevel(hObjWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY); 

			fDIKeyboard->Acquire();
		}
		else
		{
			MessageBox(NULL,"DirectInput Keyboard initialization Failed!", "TwinPad", NULL);
			return FALSE;
		}

		//////////////////Mouse Initializing//////////////////
		
		fDI->CreateDevice(GUID_SysMouse, &fDIMouse, NULL);
		
		if (fDIMouse)
		{
			fDIMouse->SetCooperativeLevel(hObjWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			fDIMouse->SetDataFormat(&c_dfDIMouse2);

			fDIMouse->Acquire();		
		}
		else
		{
			MessageBox(0,"DirectInput Mouse Initialization Failed!!", "TwinPad",0);
			return FALSE;
		}
		
		return TRUE;
	}

	return FALSE;
// --- End of DirectInput initialization ---
}

//Terminate DirectInput Objects, First Unacquire Keyboard & Mouse 
//devices, Release them, then Release main DI Obj.
void TermDI()
{
	if (fDI != NULL)
	{
		//First Deal with Keyboard.
		fDIKeyboard->Unacquire();
		fDIKeyboard->Release();
		fDIKeyboard = NULL;
		
		//Second Deal with Mouse
		fDIMouse->Unacquire();
		fDIMouse->Release();
		fDIMouse = NULL;

		//Finally Deal with main DI Object.
		fDI->Release();
		fDI = NULL;
	}
}

//Get Keyboard buttons status, and save a snapshot in KeyState Character Array (1 Byte / Element)
void GetKeyboardStatus()
{
	//Test if Error occurred by another application that acquired the device

	while(FAILED(fDIKeyboard->GetDeviceState(256, (LPVOID)KeyState)))
     {
		// try an re-acquire the device
		if (FAILED(fDIKeyboard->Acquire()))
			break; // serious error
     }
}

void GetMouseStatus()
{
	if (hObjWnd != GetForegroundWindow()) return;
	//////////////////////////////////////////////////////////////////////////////////////////
	//Using Win32Api to get cursor pos, it's more convinient while playing
	//in window mode, as the user may want to run other apps as will.
	GetCursorPos(&MousePt);

	//Check to see if the cursor is inside GS window.
	//if so, process Inputs, else skip them.
	if (MousePt.x >= WndRect.left && MousePt.x <= WndRect.right &&
		MousePt.y >= WndRect.top  && MousePt.y <= WndRect.bottom)
		{
			if (lbutDown == 1 || rbutDown == 1 || mbutDown == 1)
				ClipCursor(&WndRect);
			inside = true;
		}
		else
		{
			ClipCursor(NULL);
			inside = false;
			return;
		}

	//Test if Error occurred by another application that acquired the device
	while(FAILED(fDIMouse->GetDeviceState(
		sizeof(DIMOUSESTATE2), &MouseState)))
     {
		// try an re-acquire the device
		if (FAILED(fDIMouse->Acquire()))
        {
        return; // serious error
        }
     }
}
////////////////////////////////////////////////End of DirectInput Funtions/////////////////////////////////////////