#pragma once

#include "fastCompile.h"
#include "wx/grid.h"

const int IMG_WIDTH = 40;	// 40 pixels


// To handle right click on configured button's Name (delete configuration)
void OnLblCtrlRightClick(wxMouseEvent &ev);
// To handle Left-Click assign a key, Right-Click remove assigned key
void OnClickWalkRun(wxMouseEvent &ev);
// Buttons (Cancel, Ok, 'Nullifies All' for Keyboard and Auto Navigate) Events
void OnClickKeyboardNullifiesAll(wxCommandEvent &ev);
void OnClickAutoNavigate(wxCommandEvent &ev);
void OnClickCancel(wxCommandEvent &ev);
void OnClickOk(wxCommandEvent &ev);
// Change between PAD 1 and 2, save and reload configured buttons
void OnRadBtnPadChange(wxCommandEvent &ev);
// Action Events
void OnClickNewAction(wxCommandEvent &ev);
void OnClickDeleteLastAction(wxCommandEvent &ev);
void OnClickInsertInbetweenAction(wxCommandEvent &ev);
void OnClickInsertAction(wxCommandEvent &ev);
void OnClickDeleteSelectedActions(wxCommandEvent &ev);
void OnClickDeleteButton(wxCommandEvent &ev);
// Combo Events
void OnClickNewCombo(wxCommandEvent &ev);
void OnClickDeleteCombo(wxCommandEvent &ev);
void OnClickRenameCombo(wxCommandEvent &ev);
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
void OnClickMouseHelpButton(wxCommandEvent &ev);
void OnClickMouseNullifiesAll(wxCommandEvent &ev);