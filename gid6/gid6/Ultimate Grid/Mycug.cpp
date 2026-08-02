/***************************************************
****************************************************
Skeleton Class for a Derived MyCug
****************************************************
****************************************************/

#include "stdafx.h"
//#include "uglibsel.h"
#include "resource.h"
#include "MyCug.h"

#include "wm_user.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(MyCug,CUGCtrl)
  //{{AFX_MSG_MAP(MyCug)
  ON_WM_LBUTTONDBLCLK()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/***************************************************
****************************************************/
MyCug::MyCug()
{
    m_tn = "Table";
    m_curSortCol = -1;
    m_sortDir = UGCT_SORTARROWDOWN;
    m_index = 0;
    m_defFont.CreatePointFont( 90, _T("Arial"));
}
/***************************************************
****************************************************/
MyCug::~MyCug()
{
  UGXPThemes::CleanUp();
}

/***************************************************
OnSetup
  This function is called just after the grid window 
  is created or attached to a dialog item.
  It can be used to initially setup the grid
****************************************************/
void MyCug::setup()
{
  m_index = AddDataSource(&m_data);
}

void MyCug::OnSetup()
{
  // initialize member variables
  CString temp;
  CUGCell cell;

  m_index = AddDataSource(&m_data);

  m_fontHeading.CreateFont(16,0,365,450,FW_EXTRABOLD,FALSE,TRUE,0,0,0,0,0,0,"Arial");


  int nRows = 5;
  int nCols= 4;

  SetNumberRows(nRows);
  SetNumberCols(nCols);

#if 0

  SetTH_Height(60);     // adjust height
  
  for(y = 0;y < nRows;y++){
    for(x = 0; x < nCols; x++){
//      wsprintf(buf,"%d",x*y);
      if (x % 2)
        QuickSetFont(x, y, &m_fontGrid);
      else
        QuickSetFont(x, y, &m_fontCool);
//      QuickSetText(x,y,buf);
    }
  }



  for(x=0; x<nCols; x++){
    QuickSetFont(x,-1, &m_fontHeading);
    wsprintf(buf,"Col %d",x);
    QuickSetText(x,-1,buf);
    QuickSetAlignment(x,-1,UG_ALIGNBOTTOM);
  }

#endif


//    SetDefDataSource(index);
//    SetGridUsingDataSource(index);

/*
    
    int cols = GetNumberCols();
    int rows = GetNumberRows();

  // set up rows and cols...
  SetNumberRows(rows);
  SetNumberCols(cols);
*/
}


bool MyCug::init(CVirtListData *data)
{
  // initialize member variables
  CString temp;
  CUGCell cell;

  SetLockRowHeight();

  SetDefFont( &m_defFont );

  m_data.init(data);

  SetDefDataSource(m_index);
  SetGridUsingDataSource(m_index);

  int cols = GetNumberCols();
  int rows = GetNumberRows();

  // set up rows and cols...
  SetNumberRows(rows);
  SetNumberCols(cols);

  for (int col = 0; col < cols; col++) {
    CString p2;
    p2.Format("%d", col);
    int width = AfxGetApp()->GetProfileInt(m_section, p2, -1);

    if (width >= 0) {
      SetColWidth(col, width);
    }
  }

  int height = AfxGetApp()->GetProfileInt(m_section, "row",  -1);

  if (height >= 0) {
    SetRowHeight(-1, height);
  }

  int height1 = AfxGetApp()->GetProfileInt(m_section, "row1",  -1);

  if (height1 >= 0) {
    SetRowHeight(1, height);
  }


  PrintSetOption(UG_PRINT_TOPHEADING, TRUE);
  PrintSetOption(UG_PRINT_LEFTMARGIN, 10);
  PrintSetOption(UG_PRINT_RIGHTMARGIN, 10);
  PrintSetOption(UG_PRINT_TOPMARGIN, 10);
  PrintSetOption(UG_PRINT_BOTTOMMARGIN, 10);

  return true;
}



void MyCug::OnHitBottom(long numrows,long rowspast,long rowsfound)
{
  if (rowsfound > 0) {
    SetNumberRows(numrows+rowsfound,FALSE);
  }
}


void MyCug::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
  GetOwner()->SendMessage(WM_LDBL, row);
}

void MyCug::OnSheetSetup(int)
{
    // add the sort arrow celltype
    AddCellType(&m_sortArrow);
    // create and set default font
    SetDefFont(&m_defFont);
    // enable popup menus
    EnableMenu(TRUE);
    SetMultiSelectMode(TRUE);
}

void MyCug::OnGetCell(int col,long row,CUGCell *cell)
{
  if ( col >= 0 && col == m_curSortCol && row == -1 )
  { // set default values to the top heading
    cell->SetCellType( m_sortArrow.GetID());
    cell->SetCellTypeEx( m_sortDir );
  }
//  if (row == -1) {
    cell->SetCellTypeEx(UGCT_NORMALMULTILINE); // Apply extended style to the cell
//  }
}

void MyCug::Sort(int col)
{
    BeginWaitCursor();

    if (col == m_curSortCol)
    {
        if (m_sortDir == UGCT_SORTARROWDOWN)
        {
            m_sortDir = UGCT_SORTARROWUP;
            SortBy(col, UG_SORT_DESCENDING);
        }
        else
        {
            m_sortDir = UGCT_SORTARROWDOWN;
            SortBy(col, UG_SORT_ASCENDING);
        }
    }
    else
    {
        m_curSortCol = col;
        m_sortDir = UGCT_SORTARROWDOWN;
        SortBy(col, UG_SORT_ASCENDING);
    }

    AdjustComponentSizes();
    EndWaitCursor();
}



void MyCug::OnTH_LClicked(int col, long row, int updn, RECT *rect, POINT *point, BOOL processed)
{
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(*rect);
    UNREFERENCED_PARAMETER(*point);
    UNREFERENCED_PARAMETER(processed);

    if (!updn) return;

    Sort(col);

}

int MyCug::OnCanSizeRow(long row)
{
  UNREFERENCED_PARAMETER(row);
//  if (row == -1) return TRUE;
  return FALSE;
}

void MyCug::OnColSized(int col,int *width)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(*width);

  CString p2;
  p2.Format("%d", col);
  AfxGetApp()->WriteProfileInt(m_section, p2, *width);
}



int MyCug::OnTopHdgSized(int *height)
{
  UNREFERENCED_PARAMETER(*height);

  AfxGetApp()->WriteProfileInt(m_section, "row", *height);

  return TRUE;
}

void MyCug::OnRowSized(long row, int* height)
{
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(*height);

    AfxGetApp()->WriteProfileInt(m_section, "row1", *height);
}


void MyCug::setName(const char *tn)
{
  m_tn = tn;
  m_section.Format("%s\\%s\\%s", szSection, "UltimateTable", m_tn);
}

#include "Filtr3.h"

void MyCug::HideColumn()
{
  int cols = GetNumberCols();

  CFiltr3 dlg(this, "”кажите отображаемые столбцы таблицы");

  for (int col = 1; col < cols; col++) {
      CString s;
      
      if (m_data.GetColName(col, &s) == UG_SUCCESS) {
          int w = GetColWidth(col);
          dlg.Add(s, col, w > 0);
      }
  }

  if (dlg.DoModal() == IDOK) {
    for (int col = 1; col < cols; col++) {
        bool on = dlg.isOn(col);
        int w = GetColWidth(col);
        if (w > 0 && !on) {
          SetColWidth(col, 0);
        }
        if (w == 0 && on) {
          SetColWidth(col, 100);
        }
    }
    Invalidate();
  }
}


void MyCug::Find()
{
  FindInAllCols(TRUE);
  FindDialog();
}

