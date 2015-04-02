#include "Loading.h"
#include "twinpad_gui.h"

void LoadCombos()
{
	// Erase the combo vector, in case we load this more than once (using hot key while playing)
	for (unsigned int i = 0; i < GUI_Controls.Combos.size(); ++i)
		delete GUI_Controls.Combos[i];
	GUI_Controls.Combos.clear();

	LoadTwinPadComboConfigurations();
}

// // // // // // // // // // // // // // /Load Configurations// // // // // // // // // // // // // // // // // /

// We only need to load the KEYS, because TwinPad.exe handle the job of Saving them,
void LoadConfig() {

	LoadTwinPadConfigurations();

	for (int pad = 0; pad < 2; ++pad)
		for (int button = 0; button < 25; ++button)
			confKeys[pad][button] = GUI_Config.m_pad[pad][button];

	ExtendedOptions.IsEnabled_PAD1 = !GUI_Config.m_extra[GUI_Config.DISABLE_PAD1];
	ExtendedOptions.IsEnabled_PAD2 = !GUI_Config.m_extra[GUI_Config.DISABLE_PAD2];
	ExtendedOptions.IsEnabled_KeyEvents = !GUI_Config.m_extra[GUI_Config.DISABLE_KEYEVENTS];
	ExtendedOptions.IsEnabled_MOUSE = !GUI_Config.m_extra[GUI_Config.DISABLE_MOUSE];
	ExtendedOptions.IsEnabled_COMBOS = !GUI_Config.m_extra[GUI_Config.DISABLE_COMBOS];
}
