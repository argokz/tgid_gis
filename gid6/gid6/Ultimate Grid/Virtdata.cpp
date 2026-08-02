
#include "stdafx.h"
#include "WINNLS.H"
#include "UGCtrl.h"
#include "UGStrOp.h"

#include "Virtdata.h"

#include "virtualdata.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***************************************************
****************************************************/
CVirtData::CVirtData()
{
  m_data = NULL;
  m_nFetchedStarting = 0;
  m_pcRowsObtained = 0;
  m_nMinFetchRows = 50; // MIN_FETCH_ROWS;
}
/***************************************************
****************************************************/
CVirtData::~CVirtData(){
  // could ask user to commit or abort trans here?
  Close();
}
/***************************************************
****************************************************/

BOOL CVirtData::IsOpen()
{
  return TRUE;
}


/***************************************************
****************************************************/

#if 0 

int CVirtData::GetOption(int option,long param1,long param2){
  switch (option) {
  case UGDAO_OPT_GHOSTROWMODE:
    return (int) m_bGhostRowMode;
    break;
  case UGDAO_OPT_ALLOWEDITS:
    return (int) m_bEditAllowed;
    break;
  case UGDAO_OPT_NUMBERSIDEHDG:
    return (int) m_bNumberSideHdg;
    break;
  default:
    break;
  }
  return UG_SUCCESS;
}
#endif

/***************************************************
  This is the point at which we update the buffered edit...
****************************************************/
int CVirtData::OnCanMove(int oldcol,long oldrow,int newcol,long newrow){

  return TRUE;
}

/***************************************************
****************************************************/
BOOL CVirtData::CanEditCell(int col, long row){
  return FALSE;
}

/****************************************************
****************************************************/
int CVirtData::StartTransaction()
{
  return UG_SUCCESS;
}
/****************************************************
****************************************************/
int CVirtData::CancelTransaction()
{
  return UG_SUCCESS;
}
/****************************************************
****************************************************/
int CVirtData::FinishTransaction()
{
  return UG_SUCCESS;
}


/***************************************************
****************************************************/
int CVirtData::GetCell(int col, long row, CUGCell *cell)
{
    //  TRACE("col = %d, row = %ld\n",col,row);
    // column headings
    if (col == -1)  // we can do nothing with row headings or corner
    {
        if (row >= 0)
        {
            cell->SetNumberDecimals(0);
            cell->SetNumber(row + 1);
        }
        return UG_NA;
    }
    if (row == -1)
    {  // supply field names for column headings 
        CString str;
        GetColName(col, &str);
        cell->SetText(str);
    }
    else
    {
        if (m_data) {
            int first = m_ctrl->GetTopRow();
            int last = m_ctrl->GetBottomRow();

            if (last > first + 100) last = first + 100;

            if (first <= row && row <= last) {
              CString str = m_data->GetItemText(col, row, first, last+1);
              cell->SetText(str);
            }
        }
    }

    return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CVirtData::SetCell(int col,long row,CUGCell *cell)
{
  return UpdateCurrentRecord(); 
}
/***************************************************
CVirtData::GetNumRows
// The code below implements GetNumRows for use with SetGridUsingDatasource()
// of CUGCtrl. It returns either 0 or 1 and relies on the OnHitBottom mechanism
// to tell the grid there are more records to be had.  Use of this
// method assumes the following code in the OnHitBottom method of your 
// CUGCtrl derived class:
//  
//  if(rowsfound >0){
//    SetNumberRows(numrows+rowsfound,FALSE);
//  
// DO NOT rely on this method to tell you the complete
// count of records.  Use GetNumRowsComplete (which can
// be slow).  
****************************************************/
long CVirtData::GetNumRows()
{
//  FetchNewRows( 0 );

  return m_data->GetCount();

//  return 1;   // assume virtual loading through the OnHitBottom
          // mechanism 
}
/***************************************************
  GetNumRowsComplete() 

    This is not in the base class.  Use this if
  you must know the record count - it may be
  slow in certain situations.  Internally, only the 
  FindNext() method uses GetNumRowsComplete().
  
    Returns count of records or -1 (UG_NA) if an error 
  occurred... candidate for a GetLastError type call...
****************************************************/
long CVirtData::GetNumRowsComplete() 
{
    if (m_data) {
        return m_data->GetCount();
    }
    
    return 0;
}

/***************************************************
****************************************************/
int CVirtData::GetNumCols()
{
  if (m_data) return m_data->GetNFlds();
  return 1;
}
/***************************************************
****************************************************/
int CVirtData::GetColName(int col,CString * str)
{
  (*str).Format("¹%d", col);

  if (m_data) {
    *str = m_data->GetFieldTitle(col);
  }

  return UG_SUCCESS;
}
/***************************************************

  0-string 1-bool 2-short 3-long 4-float 
  5-double 6-currency 7-date 8-time
  8-memo 9- blob 10-ole  11-other
****************************************************/
int CVirtData::GetColType(int col,int *type)
{
  *type = 0;
  return UG_SUCCESS;
}
                          
/***************************************************
****************************************************/
int CVirtData::OnHitBottom(long numrows, long rowspast, long *rowsfound)
{
    *rowsfound = 0;

    if (m_data) {
        int nr = m_data->GetCount();

        CString str = m_data->GetItemText(0, numrows, numrows, numrows + rowspast + 2);
        if (numrows + rowspast < nr) {
            *rowsfound = rowspast;
        }
        else if (numrows >= nr) {
            *rowsfound = 0;
        }
        else {
            *rowsfound = numrows + rowspast - nr;
        }
    }
    return UG_SUCCESS;
}

/***************************************************
****************************************************/
int CVirtData::FindFirst(CString *string, int *col,long *row, long flags)
{
  return FindNext(string, col, row, flags);
}

/***************************************************
****************************************************/
int CVirtData::FindNext(CString *string, int *col, long *row, int flags)
{
  int  retval = UG_SUCCESS;

  return retval;
}

/***************************************************
flags - UG_SORT_ASSENDING or UG_SORT_DESCENDING
****************************************************/
int CVirtData::SortBy(int col,int flags)
{
  return SortBy(&col,1,flags);
}

/***************************************************
****************************************************/
int CVirtData::SortBy(int *cols,int num,int flags)
{
//  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

  if (m_data) {
//    *str = m_data->GetFieldName(col);
    m_data->SortBy(cols, num, flags);
  }

  return UG_SUCCESS;
}
 

/***************************************************
****************************************************/
int CVirtData::Reset()
{

  return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CVirtData::AppendRow()
{

  // don't append rows if edit buffer active...
  
  return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CVirtData::AppendRow(CUGCell *cellList,int numCells)
{
  return UG_SUCCESS;
}
/***************************************************
****************************************************/
int CVirtData::DeleteRow(long row)
{
  return UG_SUCCESS;
}
/****************************************************
****************************************************/

void CVirtData::ClearGhostRow() 
{
}

/***************************************************
****************************************************/

void CVirtData::AllocateGhostRow() 
{
}

/***************************************************
****************************************************/
int CVirtData::PopulateGhostRow() 
{
  return UG_SUCCESS;
}

// Edit notifications...
/***************************************************
****************************************************/
int CVirtData::OnEditStart(int col, long row,CWnd **edit)
{
  return TRUE;
}

/***************************************************
  A temporary fix - try to add a row that will get 
  past non-zero length and required fields so that 
  we can supply the user with a complete row to 
  edit.
****************************************************/
int CVirtData::AddDefaultRow() 
{
  return UG_SUCCESS;
}

/***************************************************
****************************************************/
int CVirtData::OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey)
{
  return TRUE;
}
/***************************************************
****************************************************/
int CVirtData::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
  return TRUE;
}
/***************************************************
****************************************************/
int CVirtData::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow)
{
  return TRUE;
}

// functions related to the edit row...


/***************************************************
****************************************************/
void CVirtData::ClearEditRow() 
{
}

/***************************************************
****************************************************/
void CVirtData::AllocateEditRow() 
{
}


/***************************************************
****************************************************/
void CVirtData::ReadEditRow(long row) 
{
}

/***************************************************
****************************************************/
int CVirtData::UpdateFromEditRow()
{
  return UG_SUCCESS;
}

/***************************************************
****************************************************/
void CVirtData::CancelEdit() 
{
}

/***************************************************
****************************************************/
int CVirtData::GotoRecord(long row) 
{
  return UG_SUCCESS;
}

/***************************************************
****************************************************/
int CVirtData::SetFieldValueFromCell(int col, CUGCell *cell)
{
  // given a cell: determine the type, get the value into an
  // ole variant, and set the field value.  Assume we are 
  // positioned on the correct record...
  return UG_SUCCESS;
}


/***************************************************
****************************************************/
int CVirtData::UpdateCurrentRecord() 
{
  // assume setfieldvalue called....
  return UG_SUCCESS;
}

int CVirtData::CheckPosition() 
{
  // a silly function - this will change or be obsolete...
  
  return UG_SUCCESS;
}

void CVirtData::VerifyPosition()
{
}


bool CVirtData::init(CVirtListData *data)
{
  m_data = data;

  return true;
}



int CVirtData::FetchNewRows( long row )
{
  // Determine howmany rows are currently visible on the screen, use the window
  // size and default row size as a guide.
  CRect rect;
  m_ctrl->GetWindowRect( rect );
  long nVisibleRows = (rect.bottom - rect.top) / m_ctrl->m_GI->m_defRowHeight;
  // nStartingDelta determines the position change in relation to current DB position
  long nStartingDelta = m_nFetchedStarting;

  // find out how many rows need to be fetched
  m_nMaxFetchRows = nVisibleRows * 3;

  // make sure that enough rows are fetched at one time
  if ( m_nMaxFetchRows < m_nMinFetchRows )
    m_nMaxFetchRows = m_nMinFetchRows;

  // reset starting row for the fetch rows buffer
  m_nFetchedStarting = row - ( m_nMaxFetchRows / 2 );

  m_pcRowsObtained += 1;

  return UG_SUCCESS;
}

