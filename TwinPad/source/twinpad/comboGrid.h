#ifndef __COMBO_GRID_H_
#define __COMBO_GRID_H_

#include "wx/grid.h"
#include "wx/mstream.h"		//for wxMemoryInputStream

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include <vector>

#include "resources.h"

class CGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
	CGridCellAttrProvider() { m_attrCell = new wxGridCellAttr; }
	virtual ~CGridCellAttrProvider() { m_attrCell->DecRef(); } //See functions_gui.cpp, SetupComboTab() about IncRef

private:
    wxGridCellAttr *m_attrCell;
};

class CTableBase : public wxGridTableBase
{
	///-----------Note------------------
	//-----Delete a row in 2D vector: 
	//e.g: g[r][c] is a vector, g.erase( the beginning pointer of g 'g.begin()' + offset 'the row' to delete )
	//-----Delete a column in 2D vector:
	//e.g: g[r][c] is a vector, g[r].erase( the beginning pointer of g[r] 'g[r].begin() + offset 'the column' to delete )
	//--->Note: erase will destroy elements and force reallocation.

public:
	CTableBase() 
	{ 
		rows = cols = 0; 
		//Initializing EmptyCell, will be used to create empty grid cells  - or -
		//We use this to return a poninter to it if requesting data outside of grid, 
		//that is, negative row and column numbers
		EmptyCell.buttonName = "";
		EmptyCell.buttonSensitivity = -1;
		EmptyCell.buttonValue = -1;
	}

	virtual ~CTableBase() { }

	virtual int GetNumberRows() { return rows; }
	virtual int GetNumberCols() { return cols; }
	virtual bool IsEmptyCell(int row, int col) { return GetValue(row, col).empty(); }
	//Set Button's Name only, nothing else.
	virtual void SetValue(int row, int col, const wxString &val) { customCellValue[row][col].buttonName = val; }
	//Returns a string containing Button's Name only, nothing else.
	virtual wxString GetValue(int row, int col) { return customCellValue[row][col].buttonName; }
	//Accept a void pointer which will be casted to CCellValue structure internally.
	virtual void  SetValueAsCustom( int row, int col, const wxString& typeName, void* value )
	{
		customCellValue[row][col].buttonValue  = ((CCellValue *) value)->buttonValue;
		customCellValue[row][col].buttonSensitivity = ((CCellValue *)value)->buttonSensitivity;
		customCellValue[row][col].buttonName = ((CCellValue *) value)->buttonName;
	}
	//Return a void pointer to CCellValue structure, should be casted to " (CCellValue *) "
	virtual void* GetValueAsCustom(int row, int col, const wxString &typeName) 
	{ 
		//Special case, handle requests outside of grid, return an empty cell. e.g: Cell_Locator.SetLocation(-1,-1)
		if (row < 0 && col < 0)
			return &EmptyCell;

		//Valid request, i.e: row, col >= 0
		return &customCellValue[row][col]; 
	}
	
	//Insert rows at specified position, default is 1 row at position 0
	virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 ) 
	{
		rows += numRows;
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, pos, numRows);
		GetView()->ProcessTableMessage( msg );
		std::vector<CCellValue> dummyAction(19, EmptyCell);
		customCellValue.insert(customCellValue.begin() + pos, dummyAction);
		return true;
	}
	//Delete rows at specified position, default is 1 row at position 0
	virtual bool DeleteRows(size_t pos = 0, size_t numRows = 1)
	{
		rows -= numRows;
		if (rows < 0)
			rows = 0;
		wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, numRows);
		GetView()->ProcessTableMessage(msg);
		customCellValue.erase(customCellValue.begin() + pos);
		return true;
	}
	//Insert columns at the specified position, default is 1 column at position 0
	virtual bool InsertCols( size_t pos = 0, size_t numCols = 1) 
	{
		cols += numCols;
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_COLS_INSERTED, 0, numCols);
		GetView()->ProcessTableMessage( msg );
		colsLabels.resize(cols);
		ResizeCustomCellValueMatrix(rows, cols);
		return true;
	}
	//Delete columns at the specified position, default is 1 column at position 0
	virtual bool DeleteCols( size_t pos = 0, size_t numCols = 1) 
	{
		cols -= numCols;
		if (cols < 0) 
			cols = 0;
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_COLS_DELETED, 0, numCols);
		GetView()->ProcessTableMessage( msg );
		colsLabels.resize(cols);
		return true;
	}
	//Set the label of column number 'numCol'
	virtual void SetColLabelValue(int numCol, const wxString& label) 
	{
	   if(numCol < cols)
		  colsLabels[numCol] = label;
	   else
	   {
		  colsLabels.push_back(label);
		  wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_COLS_INSERTED, 0, numCol);
		  GetView()->ProcessTableMessage( msg );
		  cols++;
		  ResizeCustomCellValueMatrix(rows, cols);
	   }
	}
	//Get the label of column number 'numCol'
	virtual wxString GetColLabelValue(int numCol) { return colsLabels[numCol]; }
	virtual wxString GetTypeName(int row, int col) { return wxGRID_VALUE_STRING; }

protected:
	//Allocate/Deallocate memory for the grid
	void ResizeCustomCellValueMatrix(int row, int col)
	{
		//Can't resize from 0 to 0, also row & col can't be negative
		if (row <= 0) row = 1;
		if (col <= 0) col = 1;

		//resize custemCellValue matrix and initialize with dummy value
		customCellValue.resize(row);
		for(int i = 0; i < row; ++i)
			customCellValue[i].resize(col, EmptyCell);
	}

protected:
	std::vector<std::vector<CCellValue>> customCellValue;
	CCellValue EmptyCell;
private:
	int rows, cols;
	std::vector<wxString> colsLabels;
};

class CComboGrid : public wxGrid
{
public:
	CComboGrid(wxWindow *window, wxWindowID id, wxPoint &point, wxSize &size)
		: wxGrid(window, id, point, size)
	{ }
};

/// Custom Cell Renderer for draw images (not strings) on Cells
class CComboCellRenderer : public wxGridCellStringRenderer
{
public:
	virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected)
	{
		wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

		CCellValue *val = (CCellValue *) grid.GetTable()->GetValueAsCustom(row, col, wxGRID_VALUE_STRING);

		//erase current button
		if (val->buttonValue == -1)
		{
			//dc.Clear() will clear all subsequent buttons! Is it Reference Incremented? I don't know.
			//Dc.FloodFill() works ok. Grid needs to be updated and refreshed from caller not here to avoid
			//continous flicker to other buttons. wxWidgets has bizarre behavior!
			dc.FloodFill(wxPoint(rect.x, rect.y), grid.GetDefaultCellBackgroundColour());
			return;
		}

		//holds image data
		void *data = 0;
		size_t length;

		//Get image data and size based on index number
		GetImageData(data, &length, (unsigned) val->buttonValue);

		unsigned char *buffer = new unsigned char[length];
		memcpy(buffer, data, length);

		wxMemoryInputStream iStream(buffer, length);

		wxImage bmp;
		bmp.LoadFile(iStream, wxBITMAP_TYPE_GIF);
		
		dc.DrawBitmap(bmp, wxPoint(rect.x, rect.y));
	}
};

class CCellLocator
{
public:
	/*
	Initialize Cell Locator: Row 0, Col 1, Enabled and Background color is Green by default
	Note: You need to pass it a pointer to a grid first before using Cell Locator by calling
	SetGrid(CComboGrid *grid) 
	*/
	CCellLocator() : curRow(0), curCol(1), enabled(true), bgColor(wxColor(20, 190, 40)) { }

	//Should be only done once, once the mainGrid is initialized
	void SetGrid(CComboGrid *mainGrid) { grid = mainGrid; }
	
	void MoveToNextButton()
	{
		//remove previous cell background here
		if (curCol < grid->GetNumberCols() - 1)
		{
			setCurrentBGColor(grid->GetDefaultCellBackgroundColour());
			++curCol;
			setCurrentBGColor(wxColor(20, 190, 40));	//Green
			grid->MakeCellVisible(curRow, (curCol == 1) ? 0 : curCol);
			grid->Update();
			grid->Refresh();
			grid->SetFocus();
			grid->SetGridCursor(wxGridCellCoords(curRow, curCol));
			ModifySensitivity();
		}
		else if (curRow < grid->GetNumberRows() - 1)
		{
			MoveToNextAction();
		}
		else
		{
			grid->Update();
			grid->Refresh();
			grid->SetFocus();
			grid->SetGridCursor(wxGridCellCoords(curRow, curCol));
			ModifySensitivity();
		}
			
	}
	void MoveToNextAction()
	{
		setCurrentBGColor(grid->GetDefaultCellBackgroundColour());
		++curRow;
		curCol = 1;
		setCurrentBGColor(bgColor);
		grid->MakeCellVisible(curRow, (curCol == 1) ? 0 : curCol);
		grid->Update();
		grid->Refresh();
		grid->SetFocus();
		grid->SetGridCursor(wxGridCellCoords(curRow, curCol));
		ModifySensitivity();
	}

	void SetLocation(int iRow, int iCol) 
	{
		if (iCol == 0) return;	//don't allow moving into Delay column
		//remove current color before moving to another cell
		setCurrentBGColor(grid->GetDefaultCellBackgroundColour()); //Default is White, can be changed
		curRow = iRow;
		curCol = iCol;
		setCurrentBGColor(bgColor);
		//make cuurent cell visible, if it was the first button column, show Delay column too
		grid->MakeCellVisible(curRow, (curCol == 1) ? 0 : curCol);
		if(iRow != -1 || iCol != -1)
			grid->SetGridCursor(wxGridCellCoords(curRow, curCol));
		grid->Update();
		grid->Refresh();
		grid->SetFocus();
		ModifySensitivity();
	}

	//Get current location of Cell Locator on the grid
	void GetLocation(wxGridCellCoords &coords)
	{
		coords.SetCol(curCol);
		coords.SetRow(curRow);
	}
	
	/* Check to see if current location (rows) outside of table, if so, put cursor
	   at last row */
	void TestAndCorrectLocation()
	{
		if (curRow >= grid->GetNumberRows())
		{
			curRow = grid->GetNumberRows() - 1;
			curCol = 1;
			SetLocation(curRow, curCol);
		}
	}

private:
	int curRow, curCol;
	bool enabled;
	wxColor bgColor;
	CComboGrid *grid;

	void setCurrentBGColor(wxColor color)
	{
		grid->SetCellBackgroundColour(color, curRow, curCol);
	}
};

#endif