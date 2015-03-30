#pragma once

#include "fastCompile.h"

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

const CPS_Label DIK_KEYCODES[] = { 
	  0x1   , "DIK_ESCAPE"
    , 0x2   , "DIK_1"
    , 0x3   , "DIK_2"
    , 0x4   , "DIK_3"
    , 0x5   , "DIK_4"
    , 0x6   , "DIK_5"
    , 0x7   , "DIK_6"
    , 0x8   , "DIK_7"
    , 0x9   , "DIK_8"
    , 0xA   , "DIK_9"
    , 0xB   , "DIK_0"
    , 0xC   , "DIK_MINUS"             // - on main keyboard
    , 0xD   , "DIK_EQUALS"
    , 0xE   , "DIK_BACK"              // backspace
    , 0xF   , "DIK_TAB"
    , 0x10  , "DIK_Q"
    , 0x11  , "DIK_W"
    , 0x12  , "DIK_E"
    , 0x13  , "DIK_R"
    , 0x14  , "DIK_T"
    , 0x15  , "DIK_Y"
    , 0x16  , "DIK_U"
    , 0x17  , "DIK_I"
    , 0x18  , "DIK_O"
    , 0x19  , "DIK_P"
    , 0x1A  , "DIK_LBRACKET"
    , 0x1B  , "DIK_RBRACKET"
    , 0x1C  , "DIK_RETURN"				// Enter on main keyboard
    , 0x1D  , "DIK_LCONTROL"
    , 0x1E  , "DIK_A"
    , 0x1F  , "DIK_S"
    , 0x20  , "DIK_D"
    , 0x21  , "DIK_F"
    , 0x22  , "DIK_G"
    , 0x23  , "DIK_H"
    , 0x24  , "DIK_J"
    , 0x25  , "DIK_K"
    , 0x26  , "DIK_L"
    , 0x27  , "DIK_SEMICOLON"
    , 0x28  , "DIK_APOSTROPHE"
    , 0x29  , "DIK_GRAVE"				// accent grave
    , 0x2A  , "DIK_LSHIFT"
    , 0x2B  , "DIK_BACKSLASH"
    , 0x2C  , "DIK_Z"
    , 0x2D  , "DIK_X"
    , 0x2E  , "DIK_C"
    , 0x2F  , "DIK_V"
    , 0x30  , "DIK_B"
    , 0x31  , "DIK_N"
    , 0x32  , "DIK_M"
    , 0x33  , "DIK_COMMA"
    , 0x34  , "DIK_PERIOD"				// . on main keyboard
    , 0x35  , "DIK_SLASH"				// / on main keyboard
    , 0x36  , "DIK_RSHIFT"
    , 0x37  , "DIK_MULTIPLY"			// * on numeric keypad
    , 0x38  , "DIK_LMENU"				// left Alt
    , 0x39  , "DIK_SPACE"
    , 0x3A  , "DIK_CAPITAL"
    , 0x3B  , "DIK_F1"
    , 0x3C  , "DIK_F2"
    , 0x3D  , "DIK_F3"
    , 0x3E  , "DIK_F4"
    , 0x3F  , "DIK_F5"
    , 0x40  , "DIK_F6"
    , 0x41  , "DIK_F7"
    , 0x42  , "DIK_F8"
    , 0x43  , "DIK_F9"
    , 0x44  , "DIK_F10"
    , 0x45  , "DIK_NUMLOCK"
    , 0x46  , "DIK_SCROLL"				// Scroll Lock
    , 0x47  , "DIK_NUMPAD7"
    , 0x48  , "DIK_NUMPAD8"
    , 0x49  , "DIK_NUMPAD9"
    , 0x4A  , "DIK_SUBTRACT"			// - on numeric keypad
    , 0x4B  , "DIK_NUMPAD4"
    , 0x4C  , "DIK_NUMPAD5"
    , 0x4D  , "DIK_NUMPAD6"
    , 0x4E  , "DIK_ADD"					// + on numeric keypad
    , 0x4F  , "DIK_NUMPAD1"
    , 0x50  , "DIK_NUMPAD2"
    , 0x51  , "DIK_NUMPAD3"
    , 0x52  , "DIK_NUMPAD0"
    , 0x53  , "DIK_DECIMAL"				// . on numeric keypad
    , 0x56  , "DIK_OEM_102 < > | on UK/Germany keyboards"
    , 0x57  , "DIK_F11"
    , 0x58  , "DIK_F12"
    , 0x64  , "DIK_F13 on (NEC PC98) "
    , 0x65  , "DIK_F14 on (NEC PC98) "
    , 0x66  , "DIK_F15 on (NEC PC98) "
    , 0x70  , "DIK_KANA on (Japanese keyboard)"
    , 0x73  , "DIK_ABNT_C1 / ? on Portugese (Brazilian) keyboards "
    , 0x79  , "DIK_CONVERT on (Japanese keyboard)"
    , 0x7B  , "DIK_NOCONVERT on (Japanese keyboard)"
    , 0x7D  , "DIK_YEN on (Japanese keyboard)"
    , 0x7E  , "DIK_ABNT_C2 on Numpad . on Portugese (Brazilian) keyboards "
    , 0x8D  , "DIK_NUMPADEQUALS = on numeric keypad (NEC PC98) "
    , 0x90  , "DIK_PREVTRACK on Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) "
    , 0x91  , "DIK_AT (NEC PC98) "
    , 0x92  , "DIK_COLON (NEC PC98) "
    , 0x93  , "DIK_UNDERLINE (NEC PC98) "
    , 0x94  , "DIK_KANJI on (Japanese keyboard)"
    , 0x95  , "DIK_STOP (NEC PC98) "
    , 0x96  , "DIK_AX (Japan AX) "
    , 0x97  , "DIK_UNLABELED (J3100) "
    , 0x99  , "DIK_NEXTTRACK"			// Next Track
    , 0x9C  , "DIK_NUMPADENTER"			// Enter on numeric keypad
    , 0x9D  , "DIK_RCONTROL"
    , 0xA0  , "DIK_MUTE"				// Mute
    , 0xA1  , "DIK_CALCULATOR"			// Calculator
    , 0xA2  , "DIK_PLAYPAUSE"			// Play / Pause
    , 0xA4  , "DIK_MEDIASTOP"			// Media Stop
    , 0xAE  , "DIK_VOLUMEDOWN"			// Volume -
    , 0xB0  , "DIK_VOLUMEUP"			// Volume +
    , 0xB2  , "DIK_WEBHOME"				// Web home
    , 0xB3  , "DIK_NUMPADCOMMA"			// , on numeric keypad (NEC PC98)
    , 0xB5  , "DIK_DIVIDE"				// / on numeric keypad
    , 0xB7  , "DIK_SYSRQ"
    , 0xB8  , "DIK_RMENU"				// right Alt
    , 0xC5  , "DIK_PAUSE"				// Pause
    , 0xC7  , "DIK_HOME"				// Home on arrow keypad
    , 0xC8  , "DIK_UP"					// UpArrow on arrow keypad
    , 0xC9  , "DIK_PRIOR"				// PgUp on arrow keypad
    , 0xCB  , "DIK_LEFT"				// LeftArrow on arrow keypad
    , 0xCD  , "DIK_RIGHT"				// RightArrow on arrow keypad
    , 0xCF  , "DIK_END"					// End on arrow keypad
    , 0xD0  , "DIK_DOWN"				// DownArrow on arrow keypad
    , 0xD1  , "DIK_NEXT"				// PgDn on arrow keypad
    , 0xD2  , "DIK_INSERT"				// Insert on arrow keypad
    , 0xD3  , "DIK_DELETE"				// Delete on arrow keypad
    , 0xDB  , "DIK_LWIN"				// Left Windows key
    , 0xDC  , "DIK_RWIN"				// Right Windows key
    , 0xDD  , "DIK_APPS"				// AppMenu key
    , 0xDE  , "DIK_POWER"				// System Power
    , 0xDF  , "DIK_SLEEP"				// System Sleep
    , 0xE3  , "DIK_WAKE"				// System Wake
    , 0xE5  , "DIK_WEBSEARCH"			// Web Search
    , 0xE6  , "DIK_WEBFAVORITES"		// Web Favorites
    , 0xE7  , "DIK_WEBREFRESH"			// Web Refresh
    , 0xE8  , "DIK_WEBSTOP"				// Web Stop
    , 0xE9  , "DIK_WEBFORWARD"			// Web Forward
    , 0xEA  , "DIK_WEBBACK"				// Web Back
    , 0xEB  , "DIK_MYCOMPUTER"			// My Computer
    , 0xEC  , "DIK_MAIL"				// Mail
    , 0xED  , "DIK_MEDIASELECT"			// Media Select
	};

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

const wxString strEXTRA_ONFLY = "Disable the hot key defined below to load TwinPad configurations\n" \
								"on the fly, the hot key is usefull when running COMBOs editor for\n" \
								"example while playing, - or simply changing your controls. - testing\n" \
								"and editing Combos without exiting the game or the GS/GPU window.\n" \
								"Use the emu to summon TwinPad, edit then save. Finally load the new\n" \
								"contents by pressing the assigned key below.";

const wxString strEXTRA_KEY   = "Disable the plugin from sending key events to the emu,\n" \
								"when running 2 different pad plugins. To prevent\n" \
								"double key strokes when hitting F1 to save for example.";

const wxString strEXTRA_HACK  = "Tick this one if the game has only one player like FFX.\n" \
								"If the game supports two players, leave it empty.\n" \
								"Techincally selecting this one, will double the rate\n" \
								"COMBOs being executed. For example, if you don't select this and\n" \
								"you play FFX, then Lulu's Fury overdrive spells will drop\n" \
								"to half. If you play Tekken 3, in a PSX emu, the Combo timing\n" \
								"will be correct, and vice versa. YMMV :)";

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
	ID_CHK_PAD1 = 6000, ID_CHK_PAD2, ID_CHK_MOUSE, ID_CHK_COMBOS, ID_CHK_ON_FLY, ID_CHK_KEY_EVENTS,
	ID_CHK_HACK,
	ID_TIMER1 = 7000, ID_TIMER2, ID_TIMER3,
};
