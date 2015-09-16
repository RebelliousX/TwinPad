#pragma once

#include "stdafx.h"

const int IMG_WIDTH = 40;	// 40 pixels


// To handle right click on configured button's Name (delete configuration)
void OnLblCtrlRightClick(wxMouseEvent &ev);
// To handle Left-Click assign a key, Right-Click remove assigned key
void OnClickWalkRun(wxMouseEvent &ev);
// Buttons (Cancel, Ok, 'Nullifies All' for Keyboard and Auto Navigate) Events
void OnClickKeyboardNullifiesAll(wxMouseEvent &ev);
void OnClickAutoNavigate(wxMouseEvent &ev);
void OnClickCancel(wxMouseEvent &ev);
void OnClickOk(wxMouseEvent &ev);
// Change between PAD 1 and 2, save and reload configured buttons
void OnRadBtnPadChange(wxCommandEvent &ev);
// Action Events
void OnClickNewAction(wxMouseEvent &ev);
void OnClickDeleteLastAction(wxMouseEvent &ev);
void OnClickInsertInbetweenAction(wxMouseEvent &ev);
void OnClickInsertAction(wxMouseEvent &ev);
void OnClickDeleteSelectedActions(wxMouseEvent &ev);
void OnClickDeleteButton(wxMouseEvent &ev);
// Combo Events
void OnClickNewCombo(wxMouseEvent &ev);
void OnClickDeleteCombo(wxMouseEvent &ev);
void OnClickRenameCombo(wxMouseEvent &ev);
// ComboBox Events
void OnChangeComboName(wxCommandEvent &ev);
void OnChangeComboNameKey(wxKeyEvent &ev);
// Combo Key
bool Has(const unsigned int button, int row);
void OnClickComboKey(wxMouseEvent &ev);
// psButtons
void OnClick_psComboButtons(int winID);
// Grid Events
void OnClickComboGrid(wxGridEvent &ev);
void OnMouseMoveOverGrid(wxMouseEvent &ev);
// SpinCtrl Events
void OnChangeSensitivity(wxSpinEvent &ev);
// Mouse Events
void OnClickMouseHelpButton(wxMouseEvent &ev);
void OnClickMouseNullifiesAll(wxMouseEvent &ev);
