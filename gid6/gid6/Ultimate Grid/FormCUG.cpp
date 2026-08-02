/***************************************************
****************************************************
Skeleton Class for a Derived FormCug
****************************************************
****************************************************/

#include "stdafx.h"
#include "resource.h"
#include "FormCug.h"

#include "ado.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(FormCug,CUGCtrl)
  //{{AFX_MSG_MAP(FormCug)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/***************************************************
****************************************************/
FormCug::FormCug()
{
    n_ist = 0;
    m_defFont.CreatePointFont( 90, _T("Arial"));

    m_section = "FormCug";
}
/***************************************************
****************************************************/
FormCug::~FormCug()
{
  UGXPThemes::CleanUp();
}

/***************************************************
OnSetup
  This function is called just after the grid window 
  is created or attached to a dialog item.
  It can be used to initially setup the grid
****************************************************/
void FormCug::OnSetup()
{
}

/***************************************************
OnLClicked
  Sent whenever the user clicks the left mouse
  button within the grid
  this message is sent when the button goes down
  then again when the button goes up

  'col' and 'row' are negative if the area clicked
  in is not a valid cell
  'rect' the rectangle of the cell that was clicked in
  'point' the point where the mouse was clicked
  'updn'  TRUE if the button is down FALSE if the
          button just when up
****************************************************/
void FormCug::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed){
  if (row % 2 && col == 0)
    GotoCell (col, row-1);
  else
    GotoCell (col,row);
}
/***************************************************
OnDClicked
  Sent whenever the user double clicks the left mouse
  button within the grid

  'col' and 'row' are negative if the area clicked
  in is not a valid cell
  'rect' the rectangle of the cell that was clicked in
  'point' the point where the mouse was clicked
****************************************************/
void FormCug::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed){
  // when mouse is double clicked on a individual cell, start editing mode
  StartEdit();
}        

/////////////////////////////////////////////////////////////////////////////
//  OnCharDown
//    Sent when grid received a WM_CHAR message, usually as a result
//    of a user pressing any key that represents a printable characrer.
//  Params:
//    vcKey   - virtual key code of the key user has pressed
//    processed - indicates if current event was processed by other control in the grid.
//  Return:
//    <none>
void FormCug::OnCharDown(UINT *vcKey,BOOL processed)
{
  UNREFERENCED_PARAMETER(processed);

  StartEdit( *vcKey );
}

/***************************************************
OnGetCell
  This message is sent everytime the grid needs to
  draw a cell in the grid. At this point the cell
  class has been filled with the information to be
  used to draw the cell. The information can now be
  changed before it is used for drawing
****************************************************/

#if 0
void FormCug::OnGetCell(int col,long row,CUGCell *cell)
{// set border color for each cell
  cell->SetBorderColor (&m_pen);

  if (row < 0) {           // top heading
    cell->SetBorder (UG_BDR_RAISED);
    cell->SetCellTypeEx(UGCT_NORMALMULTILINE); // Apply extended style to the cell
  }
  else if (row > 0) {   // lower row
    if (col > 0) {
//        cell->SetCellType (UGCT_DROPLIST);
    }
  }
}
#endif

void FormCug::OnGetCell(int col, long row, CUGCell *cell)
{
    if (row == -1) {
    switch(col) {
    case 0: 
        cell->SetText("Наименование");
        break;
    case 1: 
        cell->SetText(">=");
        break;
    case 2: 
        cell->SetText("<=");
        break;
    case 3: 
        cell->SetText("Ед.изм.");
        break;
      
    }
    return;
    }

  
    FormRow r;

    if (row >= 0 && row < m_row.size()) {
        r = m_row[row];
    }
    else {
        AfxMessageBox("Ошибка", MB_OK|MB_ICONINFORMATION);
        return;
    }



    if (col == 0)
    { // set default values to the top heading
      //    cell->SetCellType( m_sortArrow.GetID());
      //    cell->SetCellTypeEx( m_sortDir );

        cell->SetText(r.label);
    }
    if (col == 3) {
        cell->SetText(r.razmer);
    }



    if (row < 0) {
        cell->SetCellTypeEx(UGCT_NORMALMULTILINE); // Apply extended style to the cell
    }
    else {
//        cell->SetCellTypeEx(UGCT_NORMALELLIPSIS); // Apply extended style to the cell
        cell->SetDataType( UGCELLDATA_STRING);

        if (col > 0) {
               
            double d;
            CString val = r.m_val[col - 1];
            if (sscanf(val, "%lg", &d) == 1) {
                cell->SetDataType(UGCELLDATA_NUMBER);
                cell->SetNumber(d);
            }
        }
    }
}



/***************************************************
OnCellTypeNotify
  This message is sent from a cell type , message
  depends on the cell type - check the information
  on the cell type classes
  - The ID of the cell type is given
  Returns TRUE
****************************************************/
int FormCug::OnCellTypeNotify(long ID,int col,long row,long msg,long param){
  return TRUE;
}
/***************************************************
OnEditStart
  This message is sent whenever the grid is ready 
  to start editing a cell
  A return of TRUE allows the editing a return of
  FALSE stops editing
  Plus the properties of the CEdit class can be modified
****************************************************/
int FormCug::OnEditStart(int col, long row,CWnd **edit)
{
  if (col == 0) return 0;
  
  return TRUE;
}
/***************************************************
OnEditContinue this is send when editing is just passed focus to another cell
****************************************************/
int FormCug::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow){
  // used to navigate through the cells, while in editing mode, 
  // this function will set focus to new cell every time cell looses focus
  UNREFERENCED_PARAMETER(oldcol);
  UNREFERENCED_PARAMETER(oldrow);
  UNREFERENCED_PARAMETER(*newcol);
  UNREFERENCED_PARAMETER(*newrow);
  return TRUE;
}

/***************************************************
OnDrawFocusRect
****************************************************/
void FormCug::OnDrawFocusRect(CDC *dc,RECT *rect){

  //DrawExcelFocusRect(dc,rect);
  
  rect->bottom --;
  rect->right --;
  dc->DrawFocusRect(rect);
}
/***************************************************
  Purpose
  Params
  Return
****************************************************/
COLORREF FormCug::OnGetDefBackColor(int section){
//  if(section == UG_GRID)
//    return GetSysColor(COLOR_WINDOW);
//  else
    return GetSysColor(COLOR_3DSHADOW  ); 
}

int FormCug::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR str,BOOL cancelFlag)
{
    if (!cancelFlag) {
        if (0 <= row && row < m_row.size() && col >= 1) {
            m_row[row].m_val[col - 1] = str;
        }
    }
    return 1;
}


void FormCug::OnRowSized(long row, int* height)
{
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(*height);

    AfxGetApp()->WriteProfileInt(m_section, "row1", *height);
}


int FormCug::OnTopHdgSized(int *height)
{
  UNREFERENCED_PARAMETER(*height);

  AfxGetApp()->WriteProfileInt(m_section, "row", *height);

  return TRUE;
}

void FormCug::OnColSized(int col,int *width)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(*width);

  CString p2;
  p2.Format("col%d", col);

  AfxGetApp()->WriteProfileInt(m_section, p2, *width);
}



bool FormCug::init()
{
    int rows = m_row.size();
    int cols = 4;
    SetNumberRows(rows);
    SetNumberCols(cols);

    SetUniformRowHeight(TRUE);


//    BestFit(0, cols-1, rows, UG_BESTFIT_TOPHEADINGS);
    BestFit(0, 0, rows, UG_BESTFIT_TOPHEADINGS);
//    BestFit(0, cols-1, rows, UG_BESTFIT_AVERAGE);

   int height = AfxGetApp()->GetProfileInt(m_section, "row",  -1);
   if (height >= 0) {
     SetRowHeight(-1, height);
   }
   else {
     SetRowHeight(-1, 40);
   }

   int height1 = AfxGetApp()->GetProfileInt(m_section, "row1", -1);
   if (height1 >= 0) {
       SetRowHeight(1, height1);
   }
   else {
       SetRowHeight(1, 40);
   }


   int width;
   
   width= AfxGetApp()->GetProfileInt(m_section, "col1", 80);

   SetColWidth(1, width);

   width= AfxGetApp()->GetProfileInt(m_section, "col1", 80);
   
   SetColWidth(2, width);

   SetDefFont( &m_defFont );
    
//    SetNumberDecimals(1);

    return true;
}





