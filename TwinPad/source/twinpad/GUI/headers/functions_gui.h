#pragma once

class CCellValue
{
public:
	wxString buttonName;	// Button name
	int buttonValue;		// Buttons or analog stick direction
	int buttonSensitivity;	// Button sensitivity
};

class CAction
{
public:
	CAction() : m_delay(0), m_buttons(0) { }
	~CAction()
	{
		for (std::vector<CCellValue *>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
			if (*it)
			{
				delete *it;
				*it = 0;
			}
		m_buttons.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		 /*for (auto button : m_buttons)
			 if (button)
			 {
				 delete button;
				 button = 0;
			 }
		m_buttons.clear(); */
	}

	// Get the number of Buttons in the current Action
	int GetSize(const CAction &act) { return (int)m_buttons.size(); }
	// Set Delay for current Action
	void SetDelay(int delay) { m_delay = delay; }
	// Returns the Delay value of current Action
	int  GetDelay() { return m_delay; }
	// Add a Button to the current Action,, it doesn't take ownership of the passed pointer.
	// You have to delete it yourself. It makes a copy of the passed data, so it
	// is safe to pass a reference of object on stack.
	void AddButton(CCellValue *button) 
	{
		// Note that the destructor will delete tempButton, we pushed the pointer of CCellValue into a vector
		// and the destructor will delete by iterating through all of them when the object is destroyed
		CCellValue *tempButton = new CCellValue;
		tempButton->buttonName = button->buttonName;
		tempButton->buttonSensitivity = button->buttonSensitivity;
		tempButton->buttonValue = button->buttonValue;
		m_buttons.push_back(tempButton);
	}
	// Returns a Button at the specified index
	CCellValue * GetButton(int index) { return m_buttons[index]; }
	// Returns the number of Buttons in the current Action
	int GetNumberOfButtons() { return m_buttons.size(); }
	// Delete all buttons in the Current Action
	void DeleteAllButtons() { m_buttons.clear(); m_delay = 0; }

private:
	unsigned int m_delay;						// Delay value of the Action
	std::vector<CCellValue *> m_buttons;		// vector for buttons, type CCellValue
};

class CCombo
{
public:
	CCombo() : m_actions(0), m_comboName(""), m_key(0), m_pad(0) { }

	~CCombo()
	{
		for (std::vector<CAction *>::iterator it = m_actions.begin(); it != m_actions.end(); ++it)
			if (*it)
			{
				delete *it;
				*it = 0;
			}
		m_actions.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		/*for (auto action : m_actions)
		{
			delete action;
			action = 0;
		}
		m_actions.clear(); */
	}

	CAction * GetAction(int number) const { return m_actions[number]; }	// caller have to check for valid number
	int GetNumberActions() const { return (int) m_actions.size(); }
	int GetKey() const { return m_key; }
	void SetKey(int key) { m_key = key; }
	void SetName(const wxString &name) { m_comboName = name; }
	const wxString& GetName() const { return m_comboName; }
	void SetPad(int pad) { m_pad = pad; }
	int GetPad() { return m_pad; }

	// Add Action at the end, it doesn't take ownership of the passed pointer.
	// You have to delete it yourself. It makes a copy of the passed data, so it
	// is safe to pass a reference of object on stack.
	void AddAction(CAction *action)
	{
		CAction *tempAction = new CAction;
		tempAction->SetDelay(action->GetDelay());
		for (int i = 0; i < action->GetNumberOfButtons(); ++i)
			tempAction->AddButton(action->GetButton(i));
		m_actions.push_back(tempAction);
	}
	
	// Delete last action
	void DeleteAction()
	{
		int ix = m_actions.size();
		m_actions.resize(ix - 1);
	}

private:
	std::vector<CAction *> m_actions;	// vector of pointers to Actions
	int m_key;							// holds assigned key for current combo
	int m_pad;							// which pad the current combo is assigned to
	wxString m_comboName;				// current combo name
};

class MainConfigurations
{
public:
	MainConfigurations()
	{
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < 25; ++j)
				m_pad[i][j] = 0;
		for (int i = 0; i < 10; ++i)
			m_mouse[i] = 36;
		for (int i = 0; i < 5; ++i)
			m_extra[i] = 0;
		m_mouseAsPad = 0;
		m_mouseSensitivity = 1;
	}

	~MainConfigurations()
	{
		// When TwinPad exits, clean all dynamic memory to avoid memory leaks.
		// Note that CCombo destructor will call CAction's destructor, which in turn will delete all buttons.
		for (std::vector<CCombo *>::iterator it = Combos.begin(); it != Combos.end(); ++it)
			if (*it)
			{
				delete *it;
				*it = 0;
			}
		Combos.clear();

		// C++11 comes handy in this situation, I might use this later instead.
		/*for (auto aCombo : Combos)
			if (aCombo)
			{
				delete aCombo;
				aCombo = 0;
			}
		Combos.clear(); */
	}

	void Clean()
	{
		for (std::vector<CCombo *>::iterator it = Combos.begin(); it != Combos.end(); ++it)
			if (*it)
			{
				delete *it;
				*it = 0;
			}
		Combos.clear();

		for (int pad = 0; pad < 2; ++pad)
			for (int button = 0; button < 25; ++button)
				m_pad[pad][button] = 0;

		for (int i = 0; i < 10; ++i)
			m_mouse[i] = 36;

		for (int i = 0; i < 5; ++i)
			m_extra[i] = 0;

		m_mouseAsPad = 0;
		m_mouseSensitivity = 1;
	}

	std::vector<CCombo *> Combos;
	int m_pad[2][25];
	int m_mouse[10];
	int m_extra[5];
	int m_mouseAsPad;
	int m_mouseSensitivity;

	enum {
		DISABLE_PAD1 = 0, DISABLE_PAD2, DISABLE_KEYEVENTS, DISABLE_MOUSE,
		DISABLE_COMBOS
	};

	// Is PAD 1 Enabled
	bool IsEnabled_PAD1()
		{ return !m_extra[DISABLE_PAD1]; }
	// Is PAD 2 Enabled
	bool IsEnabled_PAD2()
		{ return !m_extra[DISABLE_PAD2]; }
	// Is Mouse Enabled
	bool IsEnabled_MOUSE()
		{ return !m_extra[DISABLE_MOUSE]; }
	// Is COMBOs Enabled
	bool IsEnabled_COMBOS()
		{ return !m_extra[DISABLE_COMBOS]; }
	// Is KeyEvents Enabled
	bool IsEnabled_KeyEvents()
		{ return !m_extra[DISABLE_KEYEVENTS]; }
};

bool IsFileOkAndFix(const wxString &file, const wxString &header);
void CreateNullFile();
void CreateNullComboFile();
// return true if configs loaded successfully
bool LoadTwinPadConfigurations();
// return true if configs loaded successfully
bool LoadTwinPadComboConfigurations();

// Forward declaration is necessary, to avoid problems
class CPS_Anim;
void LoadResources(CPS_Anim *animCtrl, const unsigned int index);
// used by LoadResources() and SetCustomCellRenderer()
// iData: Image data, length: size of image in bytes, index: image index
void GetImageData(void* &iData, unsigned int *length, const unsigned int index);
// Modify sensitivity inside spin control
void ModifySensitivity();
// Save the Grid to the Combos container
void SaveGridToCombo(wxString &strUserInput);
// Show the first Combo to grid
void ShowFirstComboOnGrid();
// Save configurations on exit (clicked OK)
void SaveTwinPadConfigurations();
void SaveTwinPadComboConfigurations();
