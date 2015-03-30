#pragma once

class GeneralOptions {
	public:
		bool IsEnabled_PAD1;			// Is PAD 1 Enabled
		bool IsEnabled_PAD2;			// Is PAD 2 Enabled
		bool IsEnabled_MOUSE;			// Is Mouse Enabled
		bool IsEnabled_COMBOS;			// Is COMBOs Enabled
		bool IsEnabled_KeyEvents;		// Is KeyEvents Enabled
		bool IsEnabled_FasterCombo;		// Is Combo being executed 2X faster!
		bool IsEnabled_HotKey;			// Is Hot Key enabled
		u8 m_hotKey;					// Hot key for loading configurations on the fly
};
