#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include "timers_functions.h"

// Called from PADconfigure() callback function
void ConfigureTwinPad();

// -----Timer classes
// Timer to re-animate Analog sticks to fix out of sync animation
class CReAnimate : public wxTimer
{
public:
	CReAnimate(wxWindow *win) : wxTimer(), win(win) { }

	virtual void Notify()
	{
		OnTimeReAnimateAnalogSticks();
	}

	virtual bool Start(int milliseconds)
	{
		return wxTimer::Start(milliseconds);
	}

	virtual void Stop()
	{
		return wxTimer::Stop();
	}
private:
	wxWindow *win;
};

// Timer to get a Key for Combo tab
class CGetComboKey : public wxTimer
{
public:
	CGetComboKey(wxWindow *win) : wxTimer(), win(win) { }

	virtual void Notify()
	{
		// POLL Device for Key
	}

	virtual bool Start(int milliseconds)
	{
		return wxTimer::Start(milliseconds);
	}

	virtual void Stop()
	{
		return wxTimer::Stop();
	}
private:
	wxWindow *win;
};

// Timer to get a Key for Keyboard tab
class CGetKey : public wxTimer
{
public:
	CGetKey(wxWindow *win) : wxTimer(), win(win) { }

	virtual void Notify()
	{
		// POLL Device for Key
		OnTimeGetKeyForKeyboard();
	}

	virtual bool Start(int milliseconds)
	{
		return wxTimer::Start(milliseconds);
	}

	virtual void Stop()
	{
		return wxTimer::Stop();
	}
private:
	wxWindow *win;
};

class CAutoNav : public wxTimer
{
public:
	CAutoNav(wxWindow *win) : wxTimer(), win(win) { }

	virtual void Notify()
	{
		// Auto navigation between Keyboard buttons to configure them
		// Note: It is best to use something similar to a state machine when calling the function
	}

	virtual bool Start(int milliseconds)
	{
		return wxTimer::Start(milliseconds);
	}

	virtual void Stop()
	{
		return wxTimer::Stop();
	}
private:
	wxWindow *win;
};

class CGetLoadComboKey : public wxTimer
{
public:
	CGetLoadComboKey(wxWindow *win) : wxTimer(), win(win) { }

	virtual void Notify()
	{
		// Get a key for Loading Combo on the fly in Misc tab
		// Note: It is best to use something similar to a state machine when calling the function
	}

	virtual bool Start(int milliseconds)
	{
		return wxTimer::Start(milliseconds);
	}

	virtual void Stop()
	{
		return wxTimer::Stop();
	}
private:
	wxWindow *win;
};


// Main DialogBox of TwinPad
class TwinPad_Frame : public wxDialog
{
public:
	TwinPad_Frame(wxString title);
	~TwinPad_Frame();
	void OnClose(wxCloseEvent &event);
	void TwinPad_Configuration();

	CReAnimate *tmrAnimate;			// To reAnimate analog-sticks in Combo tab
	CAutoNav *tmrAutoNavigate;		// For Auto Navigation in Keyboard tab
	CGetKey *tmrGetKey;				// To get a key value in keyboard tab
	CGetComboKey *tmrGetComboKey;		// To get a key value in Combo tab
	wxTimer *tmrGetLoadComboKey;	// To get a key value in Misc tab (Combo Load on the fly)
};


#endif