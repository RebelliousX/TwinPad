#pragma once

// Check if key is valid: Not F1 to F10 or Escape
bool IsValidKey(unsigned char key);

void OnTimeGetKeyForKeyboard();
void OnTimeReAnimateAnalogSticks();
void OnTimeAutoNavigateAndAssign();
void OnTimeGetKeyForCombo();

struct keyInformation
{
	wxString keyName;
	unsigned char keyValue;
};
