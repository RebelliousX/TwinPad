#ifndef __COMBO_GRID_H_
#define __COMBO_GRID_H_

#include "wx/grid.h"

#ifndef WX_PRECOM
	#include "wx/wx.h"
#else
	#include "wx/wxprec.h"
#endif

#include <vector>

class CCellValue
{
public:
	wxString resourceFile;
	int buttonValue;
};

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
public:
	CTableBase() { rows = cols = 0; }
	virtual int GetNumberRows() { return rows; }
	virtual int GetNumberCols() { return cols; }
	virtual bool IsEmptyCell(int row, int col) { return GetValue(row, col).empty(); }
	//These two functions I have to override since they are pure virtuals. I don't need them for TwinPad.
	virtual void SetValue(int row, int col, const wxString &val) 
		{ customCellValue[row][col].resourceFile = val; /*Should Not be called*/ }
	virtual wxString GetValue(int row, int col) 
		{ return customCellValue[row][col].resourceFile; /*Should Not be called*/ }
	
	virtual void  SetValueAsCustom( int row, int col, const wxString& typeName, void* value )
	{
		customCellValue[row][col].buttonValue  = ((CCellValue *) value)->buttonValue;
		customCellValue[row][col].resourceFile = ((CCellValue *) value)->resourceFile;
	}
	virtual void* GetValueAsCustom( int row, int col, const wxString &typeName) { return &customCellValue[row][col]; }
	
	virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 ) 
	{
		rows += numRows;
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, pos, numRows);
		GetView()->ProcessTableMessage( msg );
		std::vector<CCellValue> temp;
		customCellValue.insert(customCellValue.begin() + pos, temp);
		customCellValue[pos].resize(19);
		//ResizeCustomCellValueMatrix(rows, cols);
		return true;
	}
	virtual bool DeleteRows(size_t pos = 0, size_t numRows = 1) 
	{
		rows -= numRows;
		if (rows < 0) rows = 0;
		
		for (int i = 0; i < this->GetColsCount(); ++i)
			this->SetValue(pos, i, wxString::Format(":) %d", i));
		
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, numRows);
		GetView()->ProcessTableMessage(msg);
		customCellValue.erase(customCellValue.begin() + pos);
		//ResizeCustomCellValueMatrix(rows, cols);
		return true;
	}
	virtual bool InsertCols( size_t pos = 0, size_t numCols = 1) 
	{
		cols += numCols;
		wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_COLS_INSERTED, 0, numCols);
		GetView()->ProcessTableMessage( msg );
		colsLabels.resize(cols);
		ResizeCustomCellValueMatrix(rows, cols);
		return true;
	}
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
	virtual wxString GetColLabelValue(int col) 
	{
		return colsLabels[col];
	}
	virtual wxString GetTypeName(int row, int col) 
	{
		if (col > 1) 
			return wxGRID_VALUE_STRING; //"Custom"; 
		else 
			return wxGRID_VALUE_STRING; //"integer";

		return wxEmptyString; //Should not get here.
	}

	void ResizeCustomCellValueMatrix(int row, int col)
	{
		CCellValue dummyValue;
		dummyValue.buttonValue = -1;	//-1 is not valid PS2 button unlike 0. (0 is L2)
		dummyValue.resourceFile = wxEmptyString;

		//Can't resize from 0 to 0, also row & col can't be negative :p
		if (row <= 0) row = 1;
		if (col <= 0) 
		{
			col = 1;
			//Special case, handle Delay value in first column (#0)
			dummyValue.buttonValue = -1;
			dummyValue.resourceFile = "1";	//Default Delay value for elements in column # 0.
		}

		//resize custemCellValue matrix and initialize with dummy value
		customCellValue.resize(row);
		for(int i = 0; i < row; ++i)
			customCellValue[i].resize(col, dummyValue);
	}

public:
	std::vector<std::vector<CCellValue>> customCellValue;

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

class CComboCellRenderer : public wxGridCellStringRenderer
{
public:
	virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected)
	{
		wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

		CCellValue *val = (CCellValue *) grid.GetTable()->GetValueAsCustom(row, col, wxGRID_VALUE_STRING);
		
		wxImage bmp(val->resourceFile, wxBITMAP_TYPE_GIF);

		dc.DrawBitmap(bmp, wxPoint(rect.x, rect.y));
	}
};

#endif