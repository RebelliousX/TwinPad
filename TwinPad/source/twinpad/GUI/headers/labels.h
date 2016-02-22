#pragma once

class wxString;

class CPS_Label
{
public:
	int keyValue;
	wxString name;
};

const CPS_Label PS_LABEL[24] =     { 0, "L2",
									 1, "R2",
									 2, "L1",
									 3, "R1",
									 4, "Triangle",
									 5, "Circle",
									 6, "Cross",
									 7, "Square",
									 8, "Select",
									 9, "L3",
									10, "R3",
									11, "Start",
									12, "Up",
									13, "Right",
									14, "Down",
									15, "Left",
									16, "Left Analog Up",
									17, "Left Analog Right",
									18, "Left Analog Down",
									19, "Left Analog Left",
									20, "Right Analog Up",
									21, "Right Analog Right",
									22, "Right Analog Down",
									23, "Right Analog Left" };

const CPS_Label MOUSE_CHOICES[] = { 
	36, "NONE",
	0,  "L2", 
	1,	"R2", 
	2,	"L1", 
	3,	"R1", 
	4,	"Triangle", 
	5,	"Circle", 
	6,	"Cross",
	7,	"Square",
	8,	"Select", 
	9,	"L3", 
	10,	"R3", 
	11,	"Start", 
	12,	"UP", 
	13,	"RIGHT", 
	14,	"DOWN", 
	15,	"LEFT", 
	16,	"Left Analog Stick",
	17,	"Right Analog Stick"
	};

const wxString strMouseChoices[] = { "NONE", "L2", "R2", "L1", "R1", "Triangle", "Circle", "Cross", "Square",
"Select", "L3", "R3", "Start", "UP", "RIGHT", "DOWN", "LEFT", "Left Analog Stick", "Right Analog Stick"
};



const wxString strMouseLabels[] = { "Left Mouse Button", "Right Mouse Button", "Middle Mouse Button",
		"Mouse Button # 4", "Mouse Button # 5", "Mouse Button # 6", "Mouse Button # 7",
		"Mouse Button # 8", "Mouse Scroll Up", "Mouse Scroll Down" };

const wxString strMouseSensitivity[] = { "1", "2", "3", "4", "5", "6" };

// These strings for the Misc tab, quite lengthy.. :/
const wxString strMOUSE_HELP_MSG = "Note: Don't repeat the selection of an option twice!\n" \
							"e.g. Cross for the Left and Right mouse buttons at the same time.\n\n\n" \
							"There is no checking for mouse capabilities, please configure the\n" \
							"buttons you have, and select \"NONE\" for those you don't.\n\n" \
							"Mouse sensitivity #1 is the old and the most accurate one.\n" \
							"#2 to #6 tries to shrink the dead zone, in the center of the GS\\GPU\n" \
							"window, where analog movements are not accepted.\nOr to be more precise, " \
							"are not noticeable by some games..\n\n" \
							"If mouse wheel UP and/or DOWN are not defined (i.e., \"NONE\"), they'll\n" \
							"behave as increasing (UP) or decreasing (DOWN) the pressure sensitivity\n" \
							"buttons. That some games depend on this! (DualShock 2 mode only.)\n\n"\
							"Finally.. Configurations will be saved automatically! When you click\n" \
							"[ OK ] button inside keyboard tab. ";

const wxString strEXTRA_INFO =	"It might be necessary to suppress some features of the plugin.\n" \
								"The user may not be using the mouse, combos, or pad 2...etc!\n" \
								"Or if wanted to handled by different pad plugin (e.g., 2nd one).\n" \
								"So I give the chance to disable some features.\n";

const wxString strEXTRA_KEY   = "Disable the plugin from sending key events to the emu,\n" \
								"when running 2 different pad plugins. To prevent double\n" \
								"key strokes when hitting F1 to save for example.";

enum TAB_INDEX { KEYBOARD_TAB, MOUSE_TAB, COMBOS_TAB, MISC_TAB, GAMEPAD_TAB };
enum class PS2BUTTON {
	L2, R2, L1, R1, TRIANGLE, CIRCLE, CROSS, SQUARE, SELECT, L3, R3,
	START, UP, RIGHT, DOWN, LEFT, LANALOG_UP, LANALOG_RIGHT, LANALOG_DOWN,
	LANALOG_LEFT, RANALOG_UP, RANALOG_RIGHT, RANALOG_DOWN, RANALOG_LEFT
};

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
// Don't define any IDs within these ranges...
// ID_BTN { 1000 to 1023 }, ID_BTN2 { 1024 to 1047 }, ID_TXT { 2000 to 2023 }, ID_LBL { 3000 to 3007 }
// ID_BTN: For Keyboard TAB, ID_BTN2: For Combos TAB
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
enum ID {
	ID_BTN = 1000,
	ID_BTN2 = 1024,
	ID_BTN_OK = 1500, ID_BTN_CANCEL, ID_BTN_AUTO, ID_BTN_NULL,
	ID_BTN_NULL_MOUSE, ID_BTN_HELP_MOUSE,
	ID_TXT = 2000, ID_TXT_WALKRUN = 2024,
	ID_LBL = 3000, ID_LBL_WALKRUN = 3008,
	ID_NOTEBOOK = 4000,
	ID_PAD1_RADIOBTN = 5000, ID_PAD2_RADIOBTN, ID_PAD1_TAB2_RADIOBTN, ID_PAD2_TAB2_RADIOBTN,
	ID_CHK_PAD1 = 6000, ID_CHK_PAD2, ID_CHK_MOUSE, ID_CHK_COMBOS, ID_CHK_KEY_EVENTS,
	ID_TIMER1 = 7000, ID_TIMER2, ID_TIMER3,
};
