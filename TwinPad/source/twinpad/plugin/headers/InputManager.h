#pragma once

#include "OIS.h"
#include "OISKeyboard.h"
#include "OISMouse.h"
#include "OISJoyStick.h"

#include <vector>

class InputManager
{
public:
	InputManager()
	{
		for (int i = 0; i < 256; ++i)
			KeyState[i] = BufferKeyState[i] = 0;
		Keyboard = 0;
		Mouse = 0;
		Joysticks.clear();
		joystickIDs.clear();
		InputSystem = 0;
		windowHandle = 0;
		numJoysticks = 0;
		ScreenWidth = 0;
		ScreenHeight = 0;
	}

	~InputManager()
	{
	}

	// Create Input Manager system for a window, it receives a handle to that window
	void CreateInputSystem(unsigned long hWnd)
	{
		windowHandle = hWnd;
		OIS::ParamList paramList;
		std::string hWndStr = std::to_string(hWnd);

		paramList.insert(std::make_pair(std::string("WINDOW"), hWndStr));
#if defined OIS_WIN32_PLATFORM
		paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
		paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NOWINKEY")));
#elif defined OIS_LINUX_PLATFORM
		paramList.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		paramList.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		paramList.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		paramList.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif

		InputSystem = OIS::InputManager::createInputSystem(paramList);

		Keyboard = static_cast<OIS::Keyboard *>(InputSystem->createInputObject(OIS::OISKeyboard, false, ""));
		Mouse = static_cast<OIS::Mouse *>(InputSystem->createInputObject(OIS::OISMouse, false, ""));

		numJoysticks = InputSystem->getNumberOfDevices(OIS::OISJoyStick);
		Joysticks.resize(numJoysticks);

		for (unsigned int i = 0; i < numJoysticks; ++i)
			Joysticks[i] = static_cast<OIS::JoyStick *>(InputSystem->createInputObject(OIS::OISJoyStick, false, ""));
	}
	
	// Destroys Input Manager system and cleanup
	void DestroyInputSystem()
	{
		InputSystem->destroyInputObject(Keyboard);
		InputSystem->destroyInputObject(Mouse);
		for (unsigned int i = 0; i < numJoysticks; ++i)
			InputSystem->destroyInputObject(Joysticks[i]);
		OIS::InputManager::destroyInputSystem(InputSystem);

		for (int i = 0; i < 256; ++i)
			KeyState[i] = BufferKeyState[i] = 0;
		Keyboard = 0;
		Mouse = 0;
		Joysticks.clear();
		joystickIDs.clear();
		InputSystem = 0;
		windowHandle = 0;
		numJoysticks = 0;
		ScreenWidth = 0;
		ScreenHeight = 0;
	}

	// Get status of Input Manager system (true if created successfuly, false otherwise)
	inline bool GetInputSystemStatus()
	{
		return InputSystem ? true : false;
	}

	// Capture the state of all connected and created devices
	inline void CaptureState()
	{
		Keyboard->capture();
		Mouse->capture();
		MouseState = &Mouse->getMouseState();
		MouseState->width = ScreenWidth;
		MouseState->height = ScreenHeight;
		for (unsigned int i = 0; i < numJoysticks; ++i)
			Joysticks[i]->capture();
	}

	// Scan Keyboard for pressed keys and save them
	inline void ScanKeyboard()
	{
		for (int i = 0; i < 256; ++i)
			KeyState[i] = Keyboard->isKeyDown((OIS::KeyCode)i) ? 0x80 : 0x00;
	}

	// Save Keyboard state into buffer
	inline void SaveKeyboardState()
	{
		memcpy(BufferKeyState, KeyState, sizeof(KeyState));
	}

	// Test if a specified keyboard key is down (KeyCode is the same as DirectInput scancodes)
	inline bool IsKeyDown(int keycode)
	{
		return !!(KeyState[keycode] & 0x80);
	}

	// Test if a specified (buffered) key was down last frame
	inline bool WasBufferKeyDown(int keycode)
	{
		return !!(BufferKeyState[keycode] & 0x80);
	}

	// Test if a specified mouse button is down
	inline bool IsButtonDown(unsigned int buttonIndex)
	{
		return MouseState->buttonDown((OIS::MouseButtonID)buttonIndex);
	}

	// Get absolute mouse position on screen
	inline void GetMousePosition(int &x, int &y)
	{
		x = MouseState->X.abs;
		y = MouseState->Y.abs;
		/*std::string s = std::to_string(x) + ", " + std::to_string(y);
		OutputDebugString(L""+s+"\n");*/
	}

	// Get relative mouse wheel status
	inline int GetMouseWheel()
	{
		return MouseState->Z.rel;
	}

	// Get applied window width from mouse state
	inline int GetWindowWidth()
	{
		return MouseState->width;
	}

	// Get applied window height from mouse state
	inline int GetWindowHeight()
	{
		return MouseState->height;
	}

	// Set window extents (width and height)
	inline void SetWindowExtents(int width, int height)
	{
		ScreenWidth = width;
		ScreenHeight = height;
	}

private:
	unsigned char KeyState[256];
	unsigned char BufferKeyState[256];

	OIS::Keyboard *Keyboard;
	OIS::Mouse *Mouse;
	OIS::InputManager *InputSystem;
	std::vector<OIS::JoyStick *> Joysticks;
	std::vector<unsigned long> joystickIDs;

	const OIS::MouseState *MouseState;
	unsigned long windowHandle;
	unsigned int numJoysticks;
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;
};
