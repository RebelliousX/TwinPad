#include "stdafx.h"
#include "twinpad_gui.h"
#include "functions_gui.h"
#include "events_functions.h"

// // // // // // // // // // // // // // // // Combo  Tab // // // // // // // // // // // // // // // // 

// Cell Locator: for current/previous cell editing. We need a pointer to the main
// grid after it is initialized, using method SetGrid()
CCellLocator Cell_Locator;

void SetupComboTab(wxPanel *panel)
{
	// Create Animation controls, we don't want to reference to anim controls in Keyboard tab,
	// since they have different behavior and events.
	for (int i = 0; i < 24; ++i)
	{
		// wxAnimation anim = GUI_Controls.animCtrl[i]->GetAnimation();
		GUI_Controls.psComboButtons[i] = new CPS_Anim(panel, ID_BTN2 + i);
		GUI_Controls.psComboButtons[i]->SetAnimation(GUI_Controls.animCtrl[i]->GetAnimation());
		GUI_Controls.psComboButtons[i]->SetIndex(i);
		GUI_Controls.psComboButtons[i]->SetName(PS_LABEL[i].name);
		GUI_Controls.psComboButtons[i]->Connect((ID_BTN2 + i), wxEVT_LEFT_UP, wxMouseEventHandler(CPS_Anim::OnClickAnimInKeyboardTab));
		if (i >= 16)		// Play Analog Stick animation by default
			GUI_Controls.psComboButtons[i]->Play();
		GUI_Controls.psComboButtons[i]->SetToolTip(PS_LABEL[i].name);
	}

	// Creating Layout
	CComboGrid *comboGrid;
	wxSize largestButtonSize;
	wxString strArrPadChoices[2] = { "Pad 1", "Pad 2" };
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
	{
		wxStaticBoxSizer *topLevelSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Key Combinations (COMBOs)");
		{
			wxStaticBoxSizer *stcComboEditorSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "COMBO Editor");
			{
				GUI_Controls.btnNewCombo = new CButton(panel, wxID_ANY, "New Combo");
				GUI_Controls.btnDeleteCombo = new CButton(panel, wxID_ANY, "Delete Combo");
				GUI_Controls.btnRenameCombo = new CButton(panel, wxID_ANY, "Rename Combo");
				stcComboEditorSizer->Add(GUI_Controls.btnNewCombo);
				stcComboEditorSizer->AddSpacer(5);
				stcComboEditorSizer->Add(GUI_Controls.btnDeleteCombo);
				stcComboEditorSizer->AddSpacer(5);
				stcComboEditorSizer->Add(GUI_Controls.btnRenameCombo);
				GUI_Controls.btnNewCombo->Bind(wxEVT_LEFT_UP, ::OnClickNewCombo);
				GUI_Controls.btnDeleteCombo->Bind(wxEVT_LEFT_UP, ::OnClickDeleteCombo);
				GUI_Controls.btnRenameCombo->Bind(wxEVT_LEFT_UP, ::OnClickRenameCombo);
			}
			wxStaticBoxSizer *stcComboNameSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "COMBO Name");
			{
				GUI_Controls.cmbComboName = new wxComboBox(panel, wxID_ANY, wxEmptyString,
					wxDefaultPosition, wxSize(200, 25), 0, 0, wxCB_READONLY | wxCB_SORT);
				GUI_Controls.cmbComboName->SetBackgroundColour(wxColor(66, 66, 66));	// Dark Grey
				GUI_Controls.cmbComboName->SetForegroundColour(wxColor("White"));
				GUI_Controls.cmbComboName->Bind(wxEVT_COMBOBOX, ::OnChangeComboName);
				GUI_Controls.cmbComboName->Bind(wxEVT_KEY_UP, ::OnChangeComboNameKey);
				stcComboNameSizer->Add(GUI_Controls.cmbComboName, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
			}
			wxStaticBoxSizer *stcComboKeySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "KEY");
			{
				GUI_Controls.lblComboKey = new wxStaticText(panel, wxID_ANY, "NONE", wxDefaultPosition, wxSize(120, 20), wxTE_READONLY);
				GUI_Controls.lblComboKey->SetWindowStyle(wxTE_CENTER);
				GUI_Controls.lblComboKey->SetBackgroundColour(wxColor(66, 66, 66));
				GUI_Controls.lblComboKey->SetForegroundColour(wxColor("White"));
				GUI_Controls.lblComboKey->Bind(wxEVT_LEFT_UP, OnClickComboKey);	// Get a key
				GUI_Controls.lblComboKey->Bind(wxEVT_RIGHT_UP, OnClickComboKey);	// Delete the key
				stcComboKeySizer->Add(GUI_Controls.lblComboKey, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
			}
			wxStaticBoxSizer *stcDefaultDelaySizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Delay");
			{
				GUI_Controls.spnDefaultDelay = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
					wxSize(65, 25), 16896L, 1, 99999, 3); /* Min 1, Max 99999, Default 3 */
				GUI_Controls.spnDefaultDelay->SetBackgroundColour(wxColor(66, 66, 66));
				GUI_Controls.spnDefaultDelay->SetForegroundColour(wxColor("White"));
				stcDefaultDelaySizer->Add(GUI_Controls.spnDefaultDelay);
			}

			topLevelSizer->Add(stcComboEditorSizer, 0, wxALIGN_CENTER, 5);
			topLevelSizer->AddSpacer(10);
			topLevelSizer->Add(stcComboNameSizer, 1, wxALIGN_CENTER | wxLEFT | wxRIGHT, 5);
			topLevelSizer->Add(stcDefaultDelaySizer, 0, wxALIGN_CENTER, 5);
			topLevelSizer->AddSpacer(10);
			topLevelSizer->Add(stcComboKeySizer, 0, wxALIGN_CENTER, 5);
		}
		wxBoxSizer *midLevelSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticBoxSizer *comboGridSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "COMBOs Viewer");
			{
				// Virtual grid
				comboGrid = new CComboGrid(panel, wxID_ANY, wxPoint(1, 1), wxSize(665, 390));
				// Complex table holds various data in a cell accessed via grid
				CTableBase *tableBase = new CTableBase();
				// We need pointers to the grid and table so we can read/save their values
				GUI_Controls.virtualGrid = comboGrid;
				GUI_Controls.tableBaseGrid = tableBase;
				comboGrid->SetTable(tableBase, true);
				// Setup attributes
				
				wxGridCellAttr *attrDelayColumn = new wxGridCellAttr;
				comboGrid->InsertCols(0, MAX_COLUMNS);		// 0: Delay, 1-20 Buttons (columns# fixed, rows# not)
															// Set Column 0 attr, the range of acceptable numbers from 1 to 99999 (delay values)
				attrDelayColumn->SetEditor(new wxGridCellNumberEditor(1, 99999));
				attrDelayColumn->SetBackgroundColour(wxColor(66, 66, 66));
				attrDelayColumn->SetTextColour(wxColor(255, 255, 255));
				attrDelayColumn->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false));
				attrDelayColumn->SetAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
				attrDelayColumn->SetReadOnly(false); // only first column is editable
													 // Set ReadOnly attr to a column (from 1-20)
				comboGrid->SetColAttr(0, attrDelayColumn);					// column 0: Delay
				for (int i = 1; i < comboGrid->GetNumberCols(); ++i)		// column 1-20, PS2 buttons images (no text)
				{
					wxGridCellAttr *attrReadOnly = new wxGridCellAttr;
					attrReadOnly->SetReadOnly(true);
					// prevents overflow of text to next cell (just in case we use it to store hidden data)
					attrReadOnly->SetOverflow(false);
					comboGrid->SetColAttr(i, attrReadOnly);
				}
				comboGrid->SetSelectionBackground(wxColor("#990000"));	// Crimson -bloody red- :)
				comboGridSizer->Add(comboGrid, 0, 0, 5);
			}
			wxBoxSizer *actionAndButtonSizer = new wxBoxSizer(wxVERTICAL);
			{
				wxStaticBoxSizer *editComboSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Action Editor");
				{
					GUI_Controls.btnNewAction = new CButton(panel, wxID_ANY, "New Action");
					GUI_Controls.btnDeleteLastAction = new CButton(panel, wxID_ANY, "Delete Last Action");
					GUI_Controls.btnInsertActions = new CButton(panel, wxID_ANY, "Insert Actions");
					GUI_Controls.btnInsertInbetweenAction = new CButton(panel, wxID_ANY, "Insert Inbetween Actions");
					GUI_Controls.btnDeleteSelectedActions = new CButton(panel, wxID_ANY, "Delete Selected Actions");
					GUI_Controls.btnDeleteButton = new CButton(panel, wxID_ANY, "Delete Selected Button");
					
					largestButtonSize = GUI_Controls.btnInsertInbetweenAction->GetSize();
					GUI_Controls.btnNewAction->SetMinSize(largestButtonSize);
					GUI_Controls.btnDeleteLastAction->SetMinSize(largestButtonSize);
					GUI_Controls.btnInsertActions->SetMinSize(largestButtonSize);
					GUI_Controls.btnDeleteSelectedActions->SetMinSize(largestButtonSize);
					GUI_Controls.btnDeleteButton->SetMinSize(largestButtonSize);

					GUI_Controls.btnNewAction->Bind(wxEVT_LEFT_UP, OnClickNewAction);
					GUI_Controls.btnDeleteLastAction->Bind(wxEVT_LEFT_UP, OnClickDeleteLastAction);
					GUI_Controls.btnInsertInbetweenAction->Bind(wxEVT_LEFT_UP, OnClickInsertInbetweenAction);
					GUI_Controls.btnInsertActions->Bind(wxEVT_LEFT_UP, OnClickInsertAction);
					GUI_Controls.btnDeleteSelectedActions->Bind(wxEVT_LEFT_UP, OnClickDeleteSelectedActions);
					GUI_Controls.btnDeleteButton->Bind(wxEVT_LEFT_UP, OnClickDeleteButton);

					editComboSizer->Add(GUI_Controls.btnNewAction, 0, wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteLastAction, 0, wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnInsertActions, 0, wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnInsertInbetweenAction, 0, wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteSelectedActions, 0, wxALL | wxALIGN_CENTER, 5);
					editComboSizer->AddSpacer(5);
					editComboSizer->Add(GUI_Controls.btnDeleteButton, 0, wxALL | wxALIGN_CENTER, 5);
				}
				wxStaticBoxSizer *whichPadSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Which Pad?");
				{
					GUI_Controls.cmbWhichPad = new wxComboBox(panel, wxID_ANY, strArrPadChoices[0], wxDefaultPosition,
						wxDefaultSize, 2, strArrPadChoices, wxCB_READONLY | wxCB_SORT);
					GUI_Controls.cmbWhichPad->SetBackgroundColour(wxColor(66, 66, 66));	// Dark Grey
					GUI_Controls.cmbWhichPad->SetForegroundColour(wxColor("White"));
					GUI_Controls.cmbWhichPad->SetMinSize(largestButtonSize);
					whichPadSizer->Add(GUI_Controls.cmbWhichPad, 0, wxALL | wxALIGN_CENTER, 5);
				}
				wxStaticBoxSizer *sensitivitySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Button Sensitivity");
				{
					/* Min 0, Max 255. Default 255 for buttons. Max is 0 (Up/Left) or 255 (Right/Down) for analogs */
					GUI_Controls.spnSensitivity = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
						wxDefaultSize, 16896L, 0, 255, 0);	// Initial is 0, will be changed depending on button selected
					GUI_Controls.spnSensitivity->SetBackgroundColour(wxColor(66, 66, 66));
					GUI_Controls.spnSensitivity->SetForegroundColour(wxColor("White"));
					GUI_Controls.spnSensitivity->SetMinSize(largestButtonSize);
					GUI_Controls.spnSensitivity->Bind(wxEVT_SPINCTRL, ::OnChangeSensitivity);
					sensitivitySizer->Add(GUI_Controls.spnSensitivity, 0, wxALL | wxALIGN_CENTER, 5);
				}
				actionAndButtonSizer->Add(editComboSizer);
				actionAndButtonSizer->Add(whichPadSizer);
				actionAndButtonSizer->Add(sensitivitySizer);
			}

			midLevelSizer->Add(comboGridSizer);
			midLevelSizer->Add(actionAndButtonSizer, 1, wxALIGN_CENTER, 5);
		}
		wxStaticBoxSizer *lowLevelSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "PSX/PS2 Buttons");
		{
			wxBoxSizer *lowLevelContainerSizer = new wxBoxSizer(wxHORIZONTAL);
			{
				wxStaticBoxSizer *stcL1L2Sizer = new wxStaticBoxSizer(wxVERTICAL, panel, "L1 && L2");
				{
					wxGridSizer *L1L2Sizer = new wxGridSizer(3, 1, 0, 0);
					{
						L1L2Sizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::L1]);
						L1L2Sizer->AddSpacer(IMG_WIDTH);
						L1L2Sizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::L2]);
						stcL1L2Sizer->Add(L1L2Sizer);
					}
				}
				wxStaticBoxSizer *stcDpadArrowSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "D-Pad Arrows");
				{
					wxGridSizer *dpadArrowSizer = new wxGridSizer(3, 3, 0, 0);
					{
						dpadArrowSizer->AddSpacer(IMG_WIDTH);
						dpadArrowSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::UP]);
						dpadArrowSizer->AddSpacer(IMG_WIDTH);
						dpadArrowSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::LEFT]);
						dpadArrowSizer->AddSpacer(IMG_WIDTH);
						dpadArrowSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::RIGHT]);
						dpadArrowSizer->AddSpacer(IMG_WIDTH);
						dpadArrowSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::DOWN]);
						dpadArrowSizer->AddSpacer(IMG_WIDTH);
						stcDpadArrowSizer->Add(dpadArrowSizer);
					}
				}
				wxStaticBoxSizer *stcLeftAnalogSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Left Analog Stick");
				{
					wxGridSizer *leftAnalogSizer = new wxGridSizer(3, 3, 0, 0);
					{
						leftAnalogSizer->AddSpacer(IMG_WIDTH);
						leftAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::LANALOG_UP]);
						leftAnalogSizer->AddSpacer(IMG_WIDTH);
						leftAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::LANALOG_LEFT]);
						leftAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::L3]);
						leftAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::LANALOG_RIGHT]);
						leftAnalogSizer->AddSpacer(IMG_WIDTH);
						leftAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::LANALOG_DOWN]);
						leftAnalogSizer->AddSpacer(IMG_WIDTH);
						stcLeftAnalogSizer->Add(leftAnalogSizer);
					}
				}
				wxStaticBoxSizer *stcStartSelectSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Select & Start");
				{
					wxGridSizer *startSelectSizer = new wxGridSizer(3, 3, 0, 0);
					{
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::SELECT]);
						startSelectSizer->AddSpacer(IMG_WIDTH);
						startSelectSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::START]);
					}
					stcStartSelectSizer->Add(startSelectSizer);
				}
				wxStaticBoxSizer *stcRightAnalogSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Right Analog Stick");
				{
					wxGridSizer *rightAnalogSizer = new wxGridSizer(3, 3, 0, 0);
					{
						rightAnalogSizer->AddSpacer(IMG_WIDTH);
						rightAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::RANALOG_UP]);
						rightAnalogSizer->AddSpacer(IMG_WIDTH);
						rightAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::RANALOG_LEFT]);
						rightAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::R3]);
						rightAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::RANALOG_RIGHT]);
						rightAnalogSizer->AddSpacer(IMG_WIDTH);
						rightAnalogSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::RANALOG_DOWN]);
						rightAnalogSizer->AddSpacer(IMG_WIDTH);
					}
					stcRightAnalogSizer->Add(rightAnalogSizer);
				}
				wxStaticBoxSizer *stcTriCirCroSqrSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Digital Buttons");
				{
					wxGridSizer *triCirCroSqrSizer = new wxGridSizer(3, 3, 0, 0);
					{
						triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
						triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::TRIANGLE]);
						triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
						triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::SQUARE]);
						triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
						triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::CIRCLE]);
						triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
						triCirCroSqrSizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::CROSS]);
						triCirCroSqrSizer->AddSpacer(IMG_WIDTH);
					}
					stcTriCirCroSqrSizer->Add(triCirCroSqrSizer);
				}
				wxStaticBoxSizer *stcR1R2Sizer = new wxStaticBoxSizer(wxVERTICAL, panel, "R1 && R2");
				{
					wxGridSizer *R1R2Sizer = new wxGridSizer(3, 1, 0, 0);
					{
						R1R2Sizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::R1]);
						R1R2Sizer->AddSpacer(IMG_WIDTH);
						R1R2Sizer->Add(GUI_Controls.psComboButtons[(int)PS2BUTTON::R2]);
					}
					stcR1R2Sizer->Add(R1R2Sizer);
				}

				lowLevelContainerSizer->Add(stcL1L2Sizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcDpadArrowSizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcLeftAnalogSizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcStartSelectSizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcRightAnalogSizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcTriCirCroSqrSizer);
				lowLevelContainerSizer->AddSpacer(5);
				lowLevelContainerSizer->Add(stcR1R2Sizer);
				lowLevelContainerSizer->AddSpacer(5);
			}
			lowLevelSizer->Add(lowLevelContainerSizer);
		}
		parentSizer->Add(topLevelSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
		parentSizer->Add(midLevelSizer, 1, wxALIGN_CENTER, 5);
		parentSizer->Add(lowLevelSizer, 0, wxALIGN_CENTER, 2);
		parentSizer->AddSpacer(30);

		panel->SetSizerAndFit(parentSizer);
		panel->SetMinClientSize(parentSizer->GetSize());
		panel->GetParent()->ClientToWindowSize(panel->GetClientSize());
	}
	// Done!

	// finishing touches
	wxToolTip *ttpSpnSensitivity = new wxToolTip("For Buttons: Maximum value for buttons is 255, and Minimum is 0.\nDefault is Maximum.\n\n"
		"For both Analog Sticks: Maximum is 0 for LEFT/UP, while Maximum is 255 for DOWN/RIGHT. Minimum is 128 (stick is released or not moving). "
		"Default is Maximum whether it is 0 or 255 for Analog Sticks.\n\n"
		"You have to select a button before you can modify its own default sensitivity.");
	ttpSpnSensitivity->SetDelay(500);		// 0.5 second
	ttpSpnSensitivity->SetAutoPop(30000);	// 30 seconds
	wxToolTip *ttpSpnDefaultDelay = new wxToolTip("Number of frames the current Action will be repeated (executed).");
	ttpSpnDefaultDelay->SetDelay(500);		// 0.5 second
	ttpSpnDefaultDelay->SetAutoPop(30000);	// 30 seconds
	wxToolTip *ttpLblComboKey = new wxToolTip("Left-Click: And then 'press any key' to assign it to the current Combo.\n"
		"Right-Click: To erase the configured Key.");
	ttpLblComboKey->SetDelay(500);			// 0.5 second
	ttpLblComboKey->SetAutoPop(30000);		// 30 seconds
	GUI_Controls.spnSensitivity->SetToolTip(ttpSpnSensitivity);
	GUI_Controls.spnDefaultDelay->SetToolTip(ttpSpnDefaultDelay);
	GUI_Controls.lblComboKey->SetToolTip(ttpLblComboKey);

	comboGrid->DisableDragColSize();			// Prevent mouse from resizing rows and columns
	comboGrid->DisableDragRowSize();

	comboGrid->SetColFormatNumber(0);			// first column accepts integer numbers only
	comboGrid->SetColLabelValue(0, "Delay");

	comboGrid->SetCellHighlightPenWidth(3);		// for Delay
	comboGrid->SetCellHighlightROPenWidth(3);	// Thicker black border around selected "read only" cell

	comboGrid->SetDoubleBuffered(true);			// If supported by platform, it will be enabled

	// Set timer to re-adjust animation sync for Analog Sticks
	GUI_Controls.mainFrame->tmrAnimate->Start(30000); // 30 seconds

	// Set label and width
	for (int i = 1; i < comboGrid->GetNumberCols(); ++i)
	{
		wxString str = wxString::Format("#%d", i);
		comboGrid->SetColLabelValue(i, str);
		comboGrid->SetColSize(i, IMG_WIDTH);
		comboGrid->SetColLabelValue(i, wxString::Format("#%d", i));
	}

	comboGrid->SetRowLabelSize(IMG_WIDTH + 20);

	// Handle mouse clicks over grid to relocate the cell)
	comboGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, ::OnClickComboGrid);
	GUI_Controls.virtualGrid->GetGridWindow()->Bind(wxEVT_MOTION, OnMouseMoveOverGrid);

	// Associate Cell Locator with this grid
	Cell_Locator.SetGrid(comboGrid);

	comboGrid->Update();
	comboGrid->Refresh();
}

// Save the Grid to the Combos container
void SaveGridToCombo(wxString &strUserInput)
{
	// Get the key assigned for the combo
	wxString keyName = GUI_Controls.lblComboKey->GetLabel();
	unsigned char keyValue = 0;
	if (keyName == "NONE")
		keyValue = 0;
	else
	{
		for (int i = 0; i < (sizeof(DIK_KEYCODES) / sizeof(*DIK_KEYCODES)); ++i)
		{
			wxString name = DIK_KEYCODES[i].name;
			name = name.substr(4, name.length());
			if (name == keyName)
			{
				keyValue = DIK_KEYCODES[i].keyValue;
				break;
			}
		}
	}

	// Get which pad is assigned for the combo
	int pad = (GUI_Controls.cmbWhichPad->GetStringSelection() == "Pad 1") ? 0 : 1;

	// Save Current Combo (if not already saved)
	// when saving, Check to see if the combo exist, if it does, use the same combo otherwise add a new one.

	// Check to see if we are modifying the same COMBO, if yes, erase it so we can overwrite it or start a new one
	for (std::vector<CCombo *>::iterator it = Configurations.Combos.begin(); it != Configurations.Combos.end(); ++it)
	{
		// If the COMBO already exists, erase it. I know the container should be list instead of vector
		// erase operation is expensive, I might change it later
		if ((*it)->GetName() == GUI_Controls.cmbComboName->GetStringSelection())
		{
			delete *it;
			Configurations.Combos.erase(it);
			break;
		}
	}

	if (GUI_Controls.virtualGrid->GetNumberRows() > 0)
	{
		// Save the COMBO from the grid (before clearing the grid) into Combos container
		// if it was erased above, it will be recreated here
		CCombo *curCombo = new CCombo;
		curCombo->SetKey((int)keyValue);
		curCombo->SetName(strUserInput);
		curCombo->SetPad(pad);
		for (int row = 0; row < GUI_Controls.virtualGrid->GetNumberRows(); ++row)
		{
			CAction *action = new CAction;
			CCellValue *val;
			// for column 0, CCellValue's buttonName = Action Delay, while all other members = -1
			val = (CCellValue *)GUI_Controls.virtualGrid->GetTable()->GetValueAsCustom(row, 0, "");
			long int delay;
			val->buttonName.ToLong(&delay);
			/* GUI_Controls.virtualGrid->GetCellValue(row, 0).ToLong(&delay); */
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
