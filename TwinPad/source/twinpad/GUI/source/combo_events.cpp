#include "stdafx.h"
#include "events_functions.h"
#include "twinpad_gui.h"
#include "functions_gui.h"
#include "labels.h"

// Global defined in combo_gui.cpp
extern CCellLocator Cell_Locator;
// Global InputManager object
extern InputManager IM;

// // // // // // // // // // COMBO tab event handling functions // // // // // // // // // //
// -----------------------------------------------------------------------------------------/

// As it says, I thought of making a function of this since it was repeated 3-4 times in the code.
// this is called from other event functions.
void ModifySensitivity()
{
	// Modify sensitivity inside the SpinControl
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	CCellValue *val;
	val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(coords.GetRow(), coords.GetCol(), "");
	GUI_Controls.spnSensitivity->SetValue(val->buttonSensitivity);
}

// This function add one or more rows to the grid (typically we add one row at a time from caller function -event-)
void AddRow(CComboGrid *grid, unsigned int defaultDelay, unsigned int rowPos)
{
	wxString str = wxString::Format("%d", defaultDelay);

	grid->InsertRows(rowPos, 1, true);
	grid->SetCellValue(rowPos, 0, str);

	// Resize column width and change label
	for (int i = 1; i < grid->GetNumberCols(); ++i)
	{
		grid->SetColSize(i, IMG_WIDTH);
		grid->SetColLabelValue(i, wxString::Format("#%d", i));
	}
	grid->SetRowSize(rowPos, IMG_WIDTH);
	// Set Column 0 attr, the range of acceptable numbers from 1 to 99999 (delay values)
	// Setup attributes, seems like I have to do this for each new row!! to ensure I have
	// a spin control, otherwise, it will work but with no spin ctrl and no protection if the
	// delay value is not in range.
	wxGridCellAttr *attrDelayColumn = new wxGridCellAttr;
	attrDelayColumn->SetEditor(new wxGridCellNumberEditor(1, 99999));
	attrDelayColumn->SetBackgroundColour(wxColor(66, 66, 66));
	attrDelayColumn->SetTextColour(wxColor(255, 255, 255));
	attrDelayColumn->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false));
	attrDelayColumn->SetAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	attrDelayColumn->SetReadOnly(false); // only first column is editable
	grid->SetColAttr(0, attrDelayColumn);
}


// Handles click event on New Action button
void OnClickNewAction(wxMouseEvent &ev)
{
	if (Configurations.Combos.size() == 0)
	{
		wxMessageBox("You need to create a Combo first before you can add Actions.",
			"COMBO's list is empty!", wxICON_INFORMATION);
		return;
	}
	AddRow(GUI_Controls.virtualGrid,
		GUI_Controls.spnDefaultDelay->GetValue(),
		GUI_Controls.virtualGrid->GetNumberRows());

	for (int c = 0; c < GUI_Controls.virtualGrid->GetNumberRows(); ++c)
		GUI_Controls.virtualGrid->SetRowSize(c, IMG_WIDTH);

	// Adding new action, set cursor to the last action and first button
	Cell_Locator.SetLocation(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
	// Modify sensitivity inside the SpinControl
	ModifySensitivity();
}

// Handles click event on Delete Last Action button
void OnClickDeleteLastAction(wxMouseEvent &ev)
{
	if (Configurations.Combos.size() > 0)
	{
		if (GUI_Controls.virtualGrid->GetNumberRows() == 1)
		{
			// Minimum requirement: to have at least 1 action to have a combo, even if it is empty
			GUI_Controls.virtualGrid->DeleteRows(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
			AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);
			Cell_Locator.SetLocation(0, 1);
		}
		else
			GUI_Controls.virtualGrid->DeleteRows(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
	}
	else
		wxMessageBox("There are no Actions, and you did not create a COMBO!!", "No Actions, nor a Combo:",
		wxICON_INFORMATION);

	// if current location was deleted with the deleted action, relocate to a valid location (last row, 2nd column)
	Cell_Locator.TestAndCorrectLocation();
}

// Handles click event on Insert Actions button
void OnClickInsertAction(wxMouseEvent &ev)
{
	wxArrayInt selectedRows;
	for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
	{
		bool blnIsRowSelected = false;
		for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
			if (GUI_Controls.virtualGrid->IsInSelection(row, col))
				blnIsRowSelected = true;
			else
			{
				blnIsRowSelected = false;
				break;	// If even one cell not selected in a row, then the row is not selected, so skip to next row				
			}

		if (blnIsRowSelected)
			selectedRows.push_back(row);
	}

	if (selectedRows.empty())
	{
		// Long error & help message :)
		wxMessageBox("Both 'Insert Actions' & 'Insert Inbetween Actions' work the same for a single Action. "
			"But they differ when trying to insert more than one Action at once. Please read to know the difference:\n\n"
			"You have to specify where to insert the new Action first, so select an Action "
			"by clicking on the Action number to make sure the whole action is selected. "
			"You have to select at least ONE whole Action before you insert another. "
			"You can select more than one by holding CTRL or SHIFT keys, and new Actions will be inserted there.\n\n"
			"Lastly, selected Actions have to be next to each other unlike 'INSERT INBETWEEN ACTIONS'. "
			"The new inserted Action will have the same 'ABSOLUTE' position (same Action number) as the one "
			"you selected. Thus, the remaining Actions will be pushed further down the list. "
			"Even if you selected more than one Action, the new ones will retain the absolute "
			"position to other nearby Actions.\n\nFor example, it is very similar to Microsoft Excel when inserting new rows. :)",
			"New Action location is unknown!",
			wxICON_EXCLAMATION);
		return;
	}

	if (selectedRows.size() > 1)
		for (int next = 1, previous = 0; next < (signed)selectedRows.size(); ++next, ++previous)
		{
			if (selectedRows[next] - selectedRows[previous] > 1)
			{
				wxMessageBox("Actions selected have to be adjacent to one another. Otherwise, choose\n"
					"'Insert Inbetween Actions' button.", "Not contiguous Actions!", wxICON_EXCLAMATION);
				return;
			}
		}

	for (unsigned int i = 0; i < (unsigned int)selectedRows.size(); ++i)
	{
		// Move grid cursor outside of the table, before we insert anything
		// to avoid hard to fix problems related to grid-cursor's previous location
		Cell_Locator.SetLocation(-1, -1);

		AddRow(GUI_Controls.virtualGrid,
			GUI_Controls.spnDefaultDelay->GetValue(),
			selectedRows[0]);
	}

	GUI_Controls.virtualGrid->ClearSelection();

	if (selectedRows[0] == 0)
		Cell_Locator.SetLocation(0, 1);
	else
		Cell_Locator.SetLocation(selectedRows[0], 1);
}

// Handles click event on Insert Inbetween Actions button
void OnClickInsertInbetweenAction(wxMouseEvent &ev)
{
	// See OnClickDeleteSelectedActions below for more details about this
	wxArrayInt selectedRows;
	for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
	{
		bool IsRowSelected = false;
		for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
			if (GUI_Controls.virtualGrid->IsInSelection(row, col))
				IsRowSelected = true;
			else
			{
				IsRowSelected = false;
				break;	// If even one cell not selected in a row, then the row is not selected, so skip to next row				
			}

		if (IsRowSelected)
			selectedRows.push_back(row);
	}

	if (selectedRows.empty())
	{
		// Long error & help message :)
		wxMessageBox("Both 'Insert Actions' & 'Insert Inbetween Actions' work the same for a single Action. "
			"But they differ when trying to insert more than one Action at once. Please read to know the difference:\n\n"
			"You have to specify where to insert the new Action first, so select an Action "
			"by clicking on the Action number to make sure the whole action is selected. "
			"You have to select at least ONE whole Action before you insert another. "
			"You can select more than one by holding CTRL or SHIFT keys, and new Actions will be inserted there.\n\n"
			"Lastly, the new inserted Action will have the same 'RELATIVE' position as the one "
			"you selected (relative to the Action above and below. Thus, the remaining Actions will be pushed "
			"further down the list. Even if you selected more than one Action, the new ones will retain the relative "
			"position to other nearby Actions.\n\n"
			"This is a unique way of inserting rows (a.k.a Actions), I am not aware of another application can do this "
			"to compare the behavior with this one. :)",
			"New Action location is unknown!",
			wxICON_EXCLAMATION);
		return;
	}

	for (unsigned int i = 0; i < (unsigned int)selectedRows.size(); ++i)
	{
		// Move grid cursor outside of the table, before we insert anything
		// to avoid hard to fix problems related to grid-cursor's previous location
		Cell_Locator.SetLocation(-1, -1);

		AddRow(GUI_Controls.virtualGrid,
			GUI_Controls.spnDefaultDelay->GetValue(),
			selectedRows[i]);
		for (unsigned int j = i; j < (unsigned int)selectedRows.size(); ++j)
			selectedRows[j] += 1;
	}

	GUI_Controls.virtualGrid->ClearSelection();

	// Move grid cursor to the first inserted action (whether it is one or more)
	// Note that if for example the first selected was row 0, now it is row 1 since it was shifted down 1 row
	Cell_Locator.SetLocation(selectedRows[0] - 1, 1);
}

void OnClickDeleteSelectedActions(wxMouseEvent &ev)
{
	/*
	Check this ticket for wxWidgets, GetSelectedRows() is not reliable and doesn't work correctly.
	The documentation says it is intended and I have to use GetSlectionTopLeft/BottomRight instead,
	I say this is BS. I hate wxWidgets more now.
	http:// trac.wxwidgets.org/changeset/54665
	// wxArrayInt selectedRows = GUI_Controls.virtualGrid->GetSelectedRows(); <- Doesn't work.
	*/

	// My implementation works better :)
	wxArrayInt selectedRows;
	for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
	{
		bool IsRowSelected = false;
		for (int col = 0; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
			if (GUI_Controls.virtualGrid->IsInSelection(row, col))
				IsRowSelected = true;
			else
			{
				IsRowSelected = false;
				break;	// If even one cell not selected in a row, then the row is not selected, so skip to next row				
			}

		if (IsRowSelected)
			selectedRows.push_back(row);
	}

	if (selectedRows.empty())
	{
		// Long error & help message :)
		wxMessageBox("You have to specify which Actions you want to delete first, so select an Action by clicking on\n"
			"the Action number to make sure the whole action is selected.\n\n"
			"You can select more than one Action by holding CTRL key or SHIFT key while clicking on the\nAction number. "
			"Or even simply by dragging the mouse to highlight the whole row then delete the Action.\n\n"
			"Please note that if some cells are highlighted, that doesn't mean the row was selected!\n\n"
			"You can highlight All Actions simply by clicking on the empty Square to the left of 'Delay'\n"
			"and above Action numbers, then delete them by clicking on 'Delete Selected Actions' button.\n\n"
			"Note: If you deleted all Actions, another Action will be created by default, A COMBO needs\n"
			"at least one empty Action. The created Action will have the delay value specified in the Delay\n"
			"field next to the COMBO's name.\n",
			"Action location is unknown!", wxICON_INFORMATION);
		return;
	}

	// Save current cell location
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	// Put Cell Locator out of the grid
	Cell_Locator.SetLocation(-1, -1);
	for (int i = selectedRows.GetCount() - 1; i >= 0; --i)
		GUI_Controls.virtualGrid->DeleteRows(selectedRows[i], 1, true);

	// Minimum requirement: to have at least 1 action to have a combo, even if it is empty
	if (GUI_Controls.virtualGrid->GetNumberRows() == 0)
		AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);

	Cell_Locator.SetLocation(GUI_Controls.virtualGrid->GetNumberRows() - 1, 1);
}

// Handles click event on delete button "delete PS button" from grid
void OnClickDeleteButton(wxMouseEvent &ev)
{
	wxGridCellCoords coords;
	CCellValue val;
	Cell_Locator.GetLocation(coords);
	val.buttonValue = -1;			// -1 is empty button, since 0 means L2
	val.buttonSensitivity = -1;		// Illegal value
	val.buttonName.Clear();			// empty button

	GUI_Controls.spnSensitivity->SetValue(0);
	GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, &val);
	GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
	GUI_Controls.virtualGrid->Update();
	GUI_Controls.virtualGrid->Refresh();
	GUI_Controls.virtualGrid->SetFocus();
}

// Combo Buttons Events
void OnClickNewCombo(wxMouseEvent &ev)
{
	static int counter = 1;
	wxString strResponse = wxGetTextFromUser("Enter a name for the new Combo:",
		"New COMBO name", wxString::Format("I am a Combo! #%d", counter++));
	// Don't accept empty COMBO name
	if (strResponse == wxEmptyString)
		return;

	// Don't accept duplicate COMBO names, we use the unique name to draw on grid
	for (unsigned int i = 0; i < Configurations.Combos.size(); ++i)
		if (strResponse == Configurations.Combos[i]->GetName())
		{
			wxMessageBox("There is another COMBO with the same name! Please choose a different name.\n\n"
				"Note: Name is case sensitive, that is 'Orange', 'oRange' or 'ORANGE' are not the same name.",
				"Duplicate COMBO name found!", wxICON_EXCLAMATION);
			return;
		}

	// Save current Combo (before making a new one) from grid to Combos container
	// It is very important that we use the cmbComboName to get the name of current Combo. If the same Combo
	// name found in the container, it will be overwritten with this one.
	SaveGridToCombo(GUI_Controls.cmbComboName->GetStringSelection());
	
	// Clear grid
	// If we don't have any COMBOs or the table doesn't exist, skip. Otherwise subscript out of range in Grid TableBase
	if (GUI_Controls.virtualGrid->GetNumberRows() > 0)
	{
		GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());
		GUI_Controls.virtualGrid->Update();
		GUI_Controls.virtualGrid->Refresh();
		GUI_Controls.virtualGrid->ForceRefresh();
		GUI_Controls.virtualGrid->SetFocus();
	}

	GUI_Controls.lblComboKey->SetLabel("NONE");

	// Add name for combo box
	GUI_Controls.cmbComboName->Append(strResponse);

	// Since this is a sorted ComboBox, position or index doesn't mean anything at all.
	GUI_Controls.cmbComboName->Select(GUI_Controls.cmbComboName->FindString(strResponse, true));
	GUI_Controls.strPreviousComboSelection = strResponse;

	// Add first row for the new combo (minimum requirement for a combo is 1 action)
	AddRow(GUI_Controls.virtualGrid, GUI_Controls.spnDefaultDelay->GetValue(), 0);

	// Create new combo
	CCombo *curCombo = new CCombo;
	curCombo->SetKey(0);
	curCombo->SetName(strResponse);
	curCombo->SetPad((GUI_Controls.cmbWhichPad->GetStringSelection() == "Pad 1") ? 0 : 1);
	Configurations.Combos.push_back(curCombo);

	Cell_Locator.SetLocation(0, 1);
}

// This is called from OnClickDeleteCombo() and when starting TwinPad to show the first combo if available
void ShowFirstComboOnGrid()
{
	// Select the first combo by default
	if (GUI_Controls.cmbComboName->GetCount() > 0)
	{
		GUI_Controls.cmbComboName->Select(0);
		GUI_Controls.strPreviousComboSelection = GUI_Controls.cmbComboName->GetStringSelection();
	}

	// Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
	// Freeze grid to prevent flickering while adding buttons, and it is much faster this way. Thaw grid when we are done
	// Note: Hide & Show do the same thing, but they show flicker for a split second
	GUI_Controls.virtualGrid->Freeze();
	for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
	{
		if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
		{
			for (int row = 0; row < (*it)->GetNumberActions(); ++row)
			{
				CAction *curAction = (*it)->GetAction(row);
				int delay = curAction->GetDelay();
				// Add grid row, will populate it with buttons after that
				AddRow(GUI_Controls.virtualGrid, delay, GUI_Controls.virtualGrid->GetNumberRows());
				// Set Action Delay
				GUI_Controls.virtualGrid->SetCellValue(row, 0, wxString::Format("%d", delay));
				// Move to the first button and iterate to add them
				Cell_Locator.SetLocation(row, 1);
				for (int button = 0; button < curAction->GetNumberOfButtons(); ++button)
				{
					CCellValue *val;
					wxGridCellCoords coords;
					val = (CCellValue *)curAction->GetButton(button);
					Cell_Locator.GetLocation(coords);
					GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, val);
					GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
					Cell_Locator.MoveToNextButton();
				}
			}

			GUI_Controls.lblComboKey->SetLabel(IM.GetKeyName((*it)->GetKey()));

			if ((*it)->GetPad() == 0)
				GUI_Controls.cmbWhichPad->SetStringSelection("Pad 1");
			else
				GUI_Controls.cmbWhichPad->SetStringSelection("Pad 2");

			break;	// No need to process other COMBOs
		}
	}
	GUI_Controls.virtualGrid->Scroll(0, 0);	// Prevent showing scroll bar moving from bottom to top
	GUI_Controls.virtualGrid->Thaw();
}

// Clear current grid then delete combo and combo name from combo box
void OnClickDeleteCombo(wxMouseEvent &ev)
{
	// prevent deletion of none selected item (There are no COMBOs)
	if (GUI_Controls.cmbComboName->GetSelection() < 0 || Configurations.Combos.size() == 0)
		return;

	// Clear grid - delete combo
	GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());

	for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
	{
		if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
		{
			delete *it;
			Configurations.Combos.erase(it);
			break;
		}
	}

	// Delete name from combo box
	GUI_Controls.cmbComboName->Delete(GUI_Controls.cmbComboName->GetSelection());
	GUI_Controls.strPreviousComboSelection.Clear();

	// Delete all rows of grid
	GUI_Controls.virtualGrid->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows(), true);

	ShowFirstComboOnGrid();
}

// Handles click event on Rename Combo button
void OnClickRenameCombo(wxMouseEvent &ev)
{
	if (GUI_Controls.cmbComboName->GetCount() == 0)
	{
		wxMessageBox("You didn't select a COMBO to rename!", "Rename Failed", wxICON_EXCLAMATION);
		return;
	}

	wxString strResponse = wxGetTextFromUser("Enter a new name for the Combo:",
		"New COMBO name", GUI_Controls.cmbComboName->GetStringSelection());

	if (strResponse != wxEmptyString)
	{
		wxString name = GUI_Controls.cmbComboName->GetStringSelection();
		for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
		{
			if ((*it)->GetName() == name)
			{
				(*it)->SetName(strResponse);
				GUI_Controls.cmbComboName->SetString(GUI_Controls.cmbComboName->GetSelection(), strResponse);
				GUI_Controls.strPreviousComboSelection = strResponse;
				break;
			}
		}

	}
}

// Combo Key
void OnClickComboKey(wxMouseEvent &ev)
{
	if (Configurations.Combos.size() == 0)
		return;

	if (ev.RightUp())
	{
		GUI_Controls.lblComboKey->SetLabel("NONE");
	}
	else if (ev.LeftUp())
	{
		GUI_Controls.mainFrame->tmrGetComboKey->Start(50);	// 50 millisecond
	}
}

// Only called from OnClick_psComboButton()
bool Has(const unsigned int button, int row)
{
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	for (int i = 1; i < GUI_Controls.virtualGrid->GetNumberCols(); ++i)
	{
		// Check if 'button' exists in current action 'row' and return true. Otherwise return false.
		int buttonValue = ((CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, i, "Cell Value"))->buttonValue;
		// If button exists in same row, and not the same one 'cell' selected by user to overwrite, return true
		if (buttonValue == button && i != coords.GetCol())
			return true;
	}
	return false;
}

// Called from the click event function (that handles both keyboard and combo button clicks) to handle combo buttons 
void OnClick_psComboButtons(int winID)
{
	// winID is between 1024 and 1047 inclusive
	if (GUI_Controls.virtualGrid->GetNumberRows() == 0) return;

	int button = winID - 1024;
	wxGridCellCoords coords;
	Cell_Locator.GetLocation(coords);
	int curRow = coords.GetRow();

	// verify button does not conflict with other buttons in current action
	// e.g Left Analog UP and DOWN at the same time.
	wxString errorMSG = "";
	if ((button == (int)PS2BUTTON::LANALOG_UP && Has((int)PS2BUTTON::LANALOG_DOWN, curRow)) ||
		(button == (int)PS2BUTTON::LANALOG_DOWN && Has((int)PS2BUTTON::LANALOG_UP, curRow)))
		errorMSG = "Can't have both Left Analog's UP and DOWN in the same Action.";
	if ((button == (int)PS2BUTTON::LANALOG_LEFT && Has((int)PS2BUTTON::LANALOG_RIGHT, curRow)) ||
		(button == (int)PS2BUTTON::LANALOG_RIGHT && Has((int)PS2BUTTON::LANALOG_LEFT, curRow)))
		errorMSG = "Can't have both Left Analog's LEFT and RIGHT in the same Action.";
	if ((button == (int)PS2BUTTON::RANALOG_UP && Has((int)PS2BUTTON::RANALOG_DOWN, curRow)) ||
		(button == (int)PS2BUTTON::RANALOG_DOWN && Has((int)PS2BUTTON::RANALOG_UP, curRow)))
		errorMSG = "Can't have both Right Analog's UP and DOWN in the same Action.";
	if ((button == (int)PS2BUTTON::RANALOG_LEFT && Has((int)PS2BUTTON::RANALOG_RIGHT, curRow)) ||
		(button == (int)PS2BUTTON::RANALOG_RIGHT && Has((int)PS2BUTTON::RANALOG_LEFT, curRow)))
		errorMSG = "Can't have both Right Analog's LEFT and RIGHT in the same Action.";
	if (Has(button, curRow))
		errorMSG = "The same button already exists in this Action.";

	if (!errorMSG.IsEmpty())
	{
		wxMessageBox(errorMSG, "Not Allowed!", wxICON_INFORMATION);
		GUI_Controls.virtualGrid->SetFocus();
		return;
	}

	// Set default sensitivity for new buttons.Regular buttons max is 255.
	// Analogs' Max is 255 for Down/Right. And Max is 0 for UP/LEFT
	int sensitivity;
	if (button >= (int)PS2BUTTON::LANALOG_UP)
	{
		if (button == (int)PS2BUTTON::LANALOG_DOWN || button == (int)PS2BUTTON::RANALOG_DOWN ||
			button == (int)PS2BUTTON::LANALOG_RIGHT || button == (int)PS2BUTTON::RANALOG_RIGHT)
			sensitivity = 255; // max DOWN/RIGHT
		else
			sensitivity = 0; // max UP/LEFT
	}
	else
		sensitivity = 255; // max for regular buttons

	CCellValue val;
	val.buttonName = PS_LABEL[button].name;
	val.buttonValue = button;
	val.buttonSensitivity = sensitivity;

	Cell_Locator.GetLocation(coords);
	GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, &val);
	GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);

	Cell_Locator.MoveToNextButton();
}

// Grid mouse click
void OnClickComboGrid(wxGridEvent &ev)
{
	unsigned int row, col;
	row = ev.GetRow();
	col = ev.GetCol();
	// Clear any current highlights too
	GUI_Controls.virtualGrid->ClearSelection();
	GUI_Controls.virtualGrid->Refresh();
	GUI_Controls.virtualGrid->SetGridCursor(row, col);	// to allow dragging and selection too

	// Modify sensitivity inside the SpinControl
	CCellValue *val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");
	GUI_Controls.spnSensitivity->SetValue(val->buttonSensitivity);
	// Move cursor to the selected cell coordinates
	Cell_Locator.SetLocation(row, col);
	ev.Skip();
}

// Show tooltip "Which button" underneath and its sensitivity value, when mouse hovering over grid cells
void OnMouseMoveOverGrid(wxMouseEvent &ev)
{
	wxPoint mousePos, cellPos;

	mousePos = GUI_Controls.virtualGrid->CalcUnscrolledPosition(ev.GetPosition());
	cellPos.y = GUI_Controls.virtualGrid->YToRow(mousePos.y);	// row
	cellPos.x = GUI_Controls.virtualGrid->XToCol(mousePos.x);	// column

	if (cellPos.x == wxNOT_FOUND || cellPos.y == wxNOT_FOUND)
	{
		ev.Skip();
		return;
	}

	CCellValue *val;
	val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(cellPos.y, cellPos.x, "");

	wxString buttonInfo = val->buttonName;

	if (cellPos.x == 0)
	{
		wxString strFrame;
		long delay;
		buttonInfo.ToLong(&delay);
		if (delay == 1)
			strFrame = " frame.\n";
		else
			strFrame = " frames.\n";
		buttonInfo = "Delay: Repeat this Action for " + buttonInfo + strFrame + "Click here to modify its value.";
	}
	else if (val->buttonSensitivity >= 0 && val->buttonSensitivity <= 255)
	{
		// For SELECT, START, L3 and R3: they have fixed sensitivity of 255
		if (val->buttonValue == (int)PS2BUTTON::SELECT || val->buttonValue == (int)PS2BUTTON::START ||
			val->buttonValue == (int)PS2BUTTON::L3 || val->buttonValue == (int)PS2BUTTON::R3)
			buttonInfo += wxString::Format("\nNot a pressure sensitive button.", val->buttonSensitivity, val->buttonValue);
		else
			buttonInfo += wxString::Format("\nSensitivity: %d", val->buttonSensitivity, val->buttonValue);
	}

	GUI_Controls.virtualGrid->GetGridWindow()->SetToolTip(buttonInfo);
	ev.Skip();
}

// Modify sensitivity for buttons when we change the value
void OnChangeSensitivity(wxSpinEvent &ev)
{
	CCellValue *val;
	wxGridCellCoords coords;
	int row, col;
	Cell_Locator.GetLocation(coords);
	row = coords.GetRow();
	col = coords.GetCol();

	// get current selected button structure
	val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");

	// avoid changing sensitivity to empty cells
	if (val->buttonName.IsEmpty())
		return;

	// avoid changing sensitivity fo SELECT, START, L3, R3 as they are not pressure sensitive
	if (val->buttonValue == (int)PS2BUTTON::SELECT || val->buttonValue == (int)PS2BUTTON::START ||
		val->buttonValue == (int)PS2BUTTON::L3 || val->buttonValue == (int)PS2BUTTON::R3)
	{
		Cell_Locator.SetLocation(row, col);
		return;
	}

	val->buttonSensitivity = ev.GetValue();		// modify sensitivity
	// save it back to the table (button structure)
	GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(row, col, "", val);
	Cell_Locator.SetLocation(row, col);
}


// Event handling ComboBox's name change
void OnChangeComboName(wxCommandEvent &ev)
{
	wxString strCurrent, strPrevious;
	strCurrent = GUI_Controls.cmbComboName->GetStringSelection();
	strPrevious = GUI_Controls.strPreviousComboSelection;

	GUI_Controls.strPreviousComboSelection = strCurrent;

	// -------- SAVE COMBO ----------- // 
	// Save Combo (before making a new one) from grid to Combos container if we have at least 1 Combo
	// It is very important that we use the cmbComboName to get the name of current Combo. If the same Combo
	// name found in the container, it will be overwritten with this one.
	if (Configurations.Combos.size() > 0)
	{
		wxString keyName = GUI_Controls.lblComboKey->GetLabel();
		long keyValue;
		if (keyName == "NONE")
			keyValue = 0;
		else
			keyValue = IM.GetKeyCode(keyName.ToStdString());

		// Get which pad is assigned for the combo
		int pad = (GUI_Controls.cmbWhichPad->GetStringSelection() == "Pad 1") ? 0 : 1;


		for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
		{
			// If the COMBO already exists, erase it. I know the container should be list instead of vector
			// erase operation is expensive, I might change it later
			if ((*it)->GetName() == strPrevious)
			{
				delete *it;
				Configurations.Combos.erase(it);
				break;
			}
		}

		if (!strPrevious.IsEmpty())
		{
			CCombo *curCombo = new CCombo;
			curCombo->SetKey((int)keyValue);
			curCombo->SetName(strPrevious);
			curCombo->SetPad(pad);
			for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
			{
				CAction *action = new CAction;
				CCellValue *val;
				// for column 0, CCellValue's buttonName = Action Delay, while all other members = -1
				val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, 0, "");
				long int delay;
				val->buttonName.ToLong(&delay);
				action->SetDelay(delay);

				for (int col = 1; col < GUI_Controls.virtualGrid->GetNumberCols(); ++col)
				{
					val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, col, "");
					// if button is not empty, add it
					if (!val->buttonName.IsEmpty())
						action->AddButton(val);
				}
				curCombo->AddAction(action);
				delete action;
				action = 0;
			}
			Configurations.Combos.push_back(curCombo);
		}
	}
	
	// Clear grid - delete combo
	// If we don't have any COMBOs or the table doesn't exist, skip. Otherwise subscript out of range in Grid TableBase
	if (GUI_Controls.virtualGrid->GetNumberRows() > 0)
	{
		GUI_Controls.virtualGrid->GetTable()->DeleteRows(0, GUI_Controls.virtualGrid->GetNumberRows());
		GUI_Controls.lblComboKey->SetLabel("NONE");
	}

	// Refresh/redraw grid and set current combo to match the one in comboGrid/tableBase.
	// Freeze grid to prevent flickering while adding buttons, and it is much faster this way. Thaw grid when we are done
	GUI_Controls.virtualGrid->Freeze();
	for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
	{
		// GetValue() gets the new selected COMBO from the list
		if ((*it)->GetName() == strCurrent)
		{
			unsigned char keyValue = (*it)->GetKey();
			GUI_Controls.lblComboKey->SetLabel(IM.GetKeyName(keyValue));

			if ((*it)->GetPad() == 0)
				GUI_Controls.cmbWhichPad->SetStringSelection("Pad 1");
			else
				GUI_Controls.cmbWhichPad->SetStringSelection("Pad 2");

			for (int row = 0; row < (*it)->GetNumberActions(); ++row)
			{
				CAction *curAction = (*it)->GetAction(row);
				int delay = curAction->GetDelay();
				// Add grid row, will populate it with buttons after that
				AddRow(GUI_Controls.virtualGrid, delay, GUI_Controls.virtualGrid->GetNumberRows());
				// Set Action Delay
				GUI_Controls.virtualGrid->SetCellValue(row, 0, wxString::Format("%d", delay));
				// Move to the first button and iterate to add them
				Cell_Locator.SetLocation(row, 1, false);
				for (int button = 0; button < curAction->GetNumberOfButtons(); ++button)
				{
					CCellValue *val;
					wxGridCellCoords coords;
					val = (CCellValue *)curAction->GetButton(button);
					Cell_Locator.GetLocation(coords);
					GUI_Controls.virtualGrid->GetTable()->SetValueAsCustom(coords.GetRow(), coords.GetCol(), wxGRID_VALUE_STRING, val);
					GUI_Controls.virtualGrid->SetCellRenderer(coords.GetRow(), coords.GetCol(), new CComboCellRenderer);
					Cell_Locator.MoveToNextButton(false);
				}
			}
			break;	// No need to process other COMBOs
		}
	}
	GUI_Controls.virtualGrid->Thaw(); // Added all buttons! Thaw the grid

	Cell_Locator.SetLocation(0, 1, false);
}

/* This function handles keyboard UP and Down keys while navigating the ComboBox
and refresh the grid according to selection. Note: Grid will refresh once the popup of th combobox disappears.
I could setfocus to grid, but that will disrupt the navigation using Up/Down keys to select Combo names. */
void OnChangeComboNameKey(wxKeyEvent &ev)
{
	if (ev.GetKeyCode() == WXK_UP || ev.GetKeyCode() == WXK_DOWN)
	{
		GUI_Controls.strPreviousComboSelection = GUI_Controls.cmbComboName->GetValue();
		wxCommandEvent e;
		e.SetEventType(wxEVT_COMBOBOX);
		OnChangeComboName(e);
		GUI_Controls.virtualGrid->Update();
		GUI_Controls.virtualGrid->Refresh();
		GUI_Controls.virtualGrid->ForceRefresh();
		ev.Skip();
	}
}
