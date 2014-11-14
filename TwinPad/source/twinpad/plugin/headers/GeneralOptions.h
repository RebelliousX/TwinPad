#ifndef _GENERAL_OPTIONS_H_
#define _GENERAL_OPTIONS_H_

class GeneralOptions {
	public:
		bool IsEnabled_PAD1;			//Is PAD 1 Enabled
		bool IsEnabled_PAD2;			//Is PAD 2 Enabled
		bool IsEnabled_MOUSE;			//Is Mouse Enabled
		bool IsEnabled_COMBOS;			//Is COMBOs Enabled
		bool IsEnabled_ComboHotKey;		//Is Combo's HotKey Enabled
		bool IsEnabled_KeyEvents;		//Is KeyEvents Enabled
		bool IsEnabled_FasterCombo;		//Is Combo being executed 2X faster!
	private:
		bool dummy; //Make the whole structure 64bits.. it's not like it's going to improve speed here, but it's good habit :)
};

#endif