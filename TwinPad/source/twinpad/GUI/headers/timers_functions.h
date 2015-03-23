#ifndef _TIMER_FUNCTIONS_H_
#define _TIMER_FUNCTIONS_H_

// Check if key is valid: Not F1 to F10 or Escape
bool IsValidKey(unsigned char key);

void OnTimeGetKeyForKeyboard();
void OnTimeReAnimateAnalogSticks();

#endif