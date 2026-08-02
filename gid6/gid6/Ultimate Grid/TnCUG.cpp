/***************************************************
****************************************************
Skeleton Class for a Derived TpCug
****************************************************
****************************************************/

#include "stdafx.h"
#include "resource.h"
#include "TnCug.h"

#include "ado.h"

int mon_days(int i, int y);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define N_TN 1
#define N14 14

BEGIN_MESSAGE_MAP(TpCug,CUGCtrl)
  //{{AFX_MSG_MAP(TpCug)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/***************************************************
****************************************************/
TpCug::TpCug()
{
    n_ist = 0;
    m_defFont.CreatePointFont( 90, _T("Arial"));
    m_m4 = 4;
    m_m10 = 10;

    m_c0 = 4;
    m_dc = 4;

}
/***************************************************
****************************************************/
TpCug::~TpCug()
{
  UGXPThemes::CleanUp();
}

/***************************************************
OnSetup
  This function is called just after the grid window 
  is created or attached to a dialog item.
  It can be used to initially setup the grid
****************************************************/
void TpCug::OnSetup()
{
  // setup rows and columns


/*
  // setup headings text
  QuickSetText (0,-2,"Date");
  QuickSetText (1,-2,"Number");
  QuickSetText (1,-1,"Type");
  QuickSetText (2,-2,"Payee");
  QuickSetText (2,-1,"Account");
  QuickSetText (3,-1,"Memo");
  QuickSetText (4,-2,"Deposit");
  EnableExcelBorders (FALSE);
  EnableCellOverLap (FALSE);

  // populate the first row group with data
  QuickSetText( 0, 0, "10/10/2001" );
  QuickSetText( 1, 0, "12203" );
  QuickSetText( 1, 1, "CHK" );
  QuickSetText( 2, 0, "Bell" );
  QuickSetText( 2, 1, "Phone" );
  QuickSetText( 4, 0, "45.50" );
*/
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
void TpCug::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed){
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
void TpCug::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed){
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
void TpCug::OnCharDown(UINT *vcKey,BOOL processed)
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
void TpCug::OnGetCell(int col,long row,CUGCell *cell)
{
  // set border color for each cell
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
/***************************************************
OnCellTypeNotify
  This message is sent from a cell type , message
  depends on the cell type - check the information
  on the cell type classes
  - The ID of the cell type is given
  Returns TRUE
****************************************************/
int TpCug::OnCellTypeNotify(long ID,int col,long row,long msg,long param){
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
int TpCug::OnEditStart(int col, long row,CWnd **edit)
{
  if (col == 0) return 0;
  if (row == N14) return 0;

  return TRUE;
}
/***************************************************
OnEditContinue this is send when editing is just passed focus to another cell
****************************************************/
int TpCug::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow){
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
void TpCug::OnDrawFocusRect(CDC *dc,RECT *rect){

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
COLORREF TpCug::OnGetDefBackColor(int section){
//  if(section == UG_GRID)
//    return GetSysColor(COLOR_WINDOW);
//  else
    return GetSysColor(COLOR_3DSHADOW  ); 
}


CString mon_name(int i);

bool readTG(CAdoFile *ado);
bool getTG(long hSourceID, double tn, double &t1, double &t2, double &t3, double &tv);
bool getTG_tn1_tn2(long hSourceID, double &tn1, double &tn2);


bool TpCug::init(map<int, CString> &map_ist, int _m4, int _m10, const char *db)
{
    m_m4 = _m4;
    m_m10 = _m10;

    CString sql;
    CAdoFile *ado = getAdo(db);

    m_ado = ado;

//    ResetAll(true);
    
    long rr = GetNumberRows();

    if (rr) {
      for (long r = 0; r < rr; r++) {
          DeleteRow(r);
      }
      DeleteRow(-3);
      DeleteRow(-2);
      DeleteRow(-1);
    }




    if (!ado) return false;

    m_map_ist = map_ist;

    n_ist = m_map_ist.size();

    map<int, CString>::const_iterator it = m_map_ist.begin();

    readTG(ado);

    CString tn = "heatLoss";

    int rows = 13 + 2;
    int cols = 4 + 4 * n_ist;

    SetNumberRows(0, TRUE);
    SetNumberCols(0, TRUE);


    SetNumberRows(rows);
    SetNumberCols(cols);

    // initalize masked edit control, font, and border style
//    m_pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
//    SetDefFont(&m_defFont);

    // setup headings layout
    SetSH_Width(0);
    SetTH_NumberRows(4);
    SetTH_Height(110);
    SetTH_RowHeight(-4, 30);
    SetTH_RowHeight(-3, 30);
    SetTH_RowHeight(-2, 30);
    SetTH_RowHeight(-1, 20);

    JoinCells(0, -4, 0, -2);
    JoinCells(1, -4, 3, -4);

    JoinCells(1, -3, 1, -2);
    JoinCells(2, -3, 2, -2);
    JoinCells(3, -3, 3, -2);

    JoinCells(4, -4, 4 + 4 * n_ist - 1, -4);

    int c0 = m_c0;

    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++) {
        JoinCells(c0, -3, c0 + 4 - 1, -3);

        CString s = it->second;

        QuickSetText(c0, -3, s);
        QuickSetText(c0, -2, "ПВ");
        QuickSetText(c0 + 1, -2, "ОВ");
        QuickSetText(c0 + 2, -2, "Подп.");
        QuickSetText(c0 + 3, -2, "ч.");
        c0 += m_dc;
    }

    for (int c = 0; c < cols; c++) {
        CString s;
        s.Format("%d", c + 1);
        QuickSetText(c, -1, s);
    }

    QuickSetText(0, -4, "Месяц");
    QuickSetText(1, -4, "Температура окружающей среды, C");
    QuickSetText(1, -3, "наружн. воздуха");
    QuickSetText(2, -3, "воздуха в техподв.");
    QuickSetText(3, -3, "грунта");

    if (n_ist) {
//        QuickSetText(4, -4, "Температура воды, C");
        QuickSetText(4, -4, "Параметры работы");
//        QuickSetText(4 + 3 * n_ist, -4, "Количество часов работы, ч");
    }

    CString mon_name(int i);

    sql.Format("SELECT * FROM %s", tn);


    if (ado->openTable0(sql)) {
        while (!ado->isEOF()) {
            long r = ado->read_long("r");
            if (r >= 0) {
              double d1 = ado->read_double("tn");            QuickSetNumber(1, r, d1);
              double d2 = ado->read_double("tpod");          QuickSetNumber(2, r, d2);
              double d3 = ado->read_double("tgr");           QuickSetNumber(3, r, d3);
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }

    sql = "SELECT * FROM heatLossSource";

    if (ado->openTable0(sql)) {
        while (!ado->isEOF()) {
            long r =             ado->read_long("r");
            long id =            ado->read_long("id"); 
            long heatSourceID =  ado->read_long("heatSourceID"); 
            long m =             ado->read_long("m"); 
            long sezon =         ado->read_long("sezon"); 
            double tx =          ado->read_double("tx");  
            double tgP =         ado->read_double("tgP");  
            double tgO =         ado->read_double("tgO");  
            double length =      ado->read_double("length");

            int c0 = m_c0;
            int i = 0;
            for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++) {
                int id = it->first;

                if (id == heatSourceID) {
                    QuickSetNumber(c0, r, tgP);
                    QuickSetNumber(c0+1, r, tgO);
                    QuickSetNumber(c0+2, r, tx);
                    QuickSetNumber(c0+3, r, length);
                }

                i++;
                c0 += m_dc;
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }

    int r = 0;

    for (int m = 1; m <= 12; m++) {
      if (m == m_m4) {
        QuickSetText(0, r++, mon_name(m) + " (з)");
        QuickSetText(0, r++, mon_name(m) + " (л)");
      }
      else if (m == m_m10) {
        QuickSetText(0, r++, mon_name(m) + " (л)");
        QuickSetText(0, r++, mon_name(m) + " (з)");
      }
      else {
        QuickSetText(0, r++, mon_name(m));
      }
    }

    QuickSetText(0, r, "Ср.год");

    LockColumns(1);
    LockRows(-1);

    return true;
}

double get_double(const char *s)
{
    double d = 0;
    CString ss = s;

    ss.Replace(",", ".");
    ss.Replace(" ", "");
    ss.Replace("\xA0", "");
    
    if (sscanf(ss, "%lg", &d) == 1) {
        return d;
    }
    return 0;
}

void TpCug::sred(int col, long row, double d)
{
    double dd = 0;

    for (int r = 0; r < 14; r++) {
        double d1 = 0;
        CString s;
        if (r == row) {
            d1 = d;
        }
        else {
            if (QuickGetText(col, r, &s) == UG_SUCCESS) {
                d1 = get_double(s);
            }
        }
        dd += d1;
    }
    QuickSetNumber(col, N14, dd / 14);
    RedrawCell(col, row);
}


int TpCug::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR str,BOOL cancelFlag)
{
    double d = get_double(str);

    sred(col, row, d);

    QuickSetNumber(col, row, d);
    RedrawCell(col, row);

    if (col == 1) {

        int c0 = m_c0;

        for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++) {
            double t1, t2, t3, tv;
            int id = it->first;
            if (getTG(id, d, t1, t2, t3, tv)) {
                QuickSetNumber(c0, row, t1);
                sred(c0, row, t1);
                RedrawCell(c0, row);

                QuickSetNumber(c0+1, row, t2);
                sred(c0+1, row, t2);
                RedrawCell(c0+1, row);
            }
            c0 += m_dc;
        }

        
       QuickSetNumber(col, row, d);
       RedrawCell(col, row);
    }

    Invalidate();

    return 1;
}

bool TpCug::QuickGetDouble(int col, int row, double &d)
{
    CString s;
    d = 0;
    if (QuickGetText(col, row, &s) == UG_SUCCESS) {
       d = get_double(s);
       return true;
    }
    return false;
}

bool TpCug::save_row(int r, int m, int sezon)
{
    CString tn = "heatLoss";

    m_ado->AddNew(tn);

    double d;

    m_ado->write("m", (long)m);
    m_ado->write("r", (long)r);
    m_ado->write("sezon", (long)sezon);

    if (QuickGetDouble(1, r, d)) m_ado->write("tn", d);
    if (QuickGetDouble(2, r, d)) m_ado->write("tpod", d);
    if (QuickGetDouble(3, r, d)) m_ado->write("tgr", d);

    m_ado->Update();

    save_row_ist(r, m, sezon);

    return true;
}

bool TpCug::save_row_ist(int r, int m, int sezon)
{
    CString tn = "heatLossSource";

    int i = 0;
    int c0 = m_c0;

    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++) {
        double d;
        QuickGetDouble(c0, r, d);

        m_ado->AddNew(tn);

        m_ado->write("heatSourceID", (long)it->first);
        m_ado->write("r", (long)r);
        m_ado->write("m", (long)m);
        m_ado->write("sezon", (long)sezon);

        if (QuickGetDouble(c0, r, d)) m_ado->write("tgP", d);
        if (QuickGetDouble(c0 + 1, r, d)) m_ado->write("tgO", d);
        if (QuickGetDouble(c0 + 2, r, d)) m_ado->write("tx", d);
        if (QuickGetDouble(c0 + 3, r, d)) m_ado->write("length", d);

        m_ado->Update();

        i++;
        c0 += m_dc;
    }

    return true;
}

bool TpCug::save_row0(int r, int m, int sezon)
{
    CString tn = "heatLoss";

    m_ado->AddNew(tn);
    m_ado->write("m", (long)m);
//    m_ado->write("mon", mon);
    m_ado->Update();

    return true;
}



bool TpCug::save()
{
    CString tn = "heatLoss";

    CString q;

    long affected;


    m_ado->Execute("DELETE FROM heatLossSourceList", &affected);

    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++)
    {
        m_ado->AddNew("heatLossSourceList");
        m_ado->write("id", (long)it->first);
        m_ado->Update();
    }


    q.Format("DELETE FROM %s", tn);
    if (m_ado->Execute(q, &affected)) {
    }



    CString par = "";

    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++) 
    {
        CString s;
        s.Format("%d", it->first);
        if (par != "") par += ",";
        par += s;
    }


    if (par != "") {
        q.Format("DELETE FROM heatLossSource WHERE heatSourceID in (%s)", par);
        m_ado->Execute(q, &affected);
    }

    int r = 0;
    for (int m = 1; m <= 12; m++) {
        CString mon = mon_name(m);

        if (m == m_m4) {
            save_row(r++, m, 1);
            save_row(r++, m, 2);

        }
        else if (m == m_m10) {
            save_row(r++, m, 2);
            save_row(r++, m, 1);

        } 
        else {
            int sezon = (m_m4 < m && m < m_m10) ? 2 : 1;
            save_row(r++, m, sezon);
        }
    }

//    save_row(r++, 0, 0, "Ср.год");


  return true;
}


void TpCug::setTG(int m4, int d4, int m10, int d10)
{
    int c0 = m_c0;

    CString s = "";


    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++)
    {
        long hSourceID = it->first;
        double t1, t2, t3, tv;
        if (!getTG(hSourceID, 0, t1, t2, t3, tv)) {
            if (s != "") s += "\n";
            s += it->second;
        }
    }

    if (s != "") {
      AfxMessageBox("Не заан температурный график в источниках :\n"+s, MB_OK|MB_ICONINFORMATION);
    }



    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++)
    {
        long hSourceID = it->first;

        for (int r = 0; r < N14; r++) {
            double tn = 0;
            CString s;
            double t1, t2, t3, tv;

            if (QuickGetText(N_TN, r, &s) == UG_SUCCESS) {
                tn = get_double(s);
                if (getTG(hSourceID, tn, t1, t2, t3, tv)) {
                  QuickSetNumber(c0, r, t1);
                  QuickSetNumber(c0+1, r, t2);
                }
            }
        }
        c0 += m_dc;
    }
    set_length(m4, d4, m10, d10);


    Invalidate();
}


double sredn1(double t_mart, double t_apr, int d_apr_zima, int m4) 
{
  int d30 = mon_days(m4, 1);
  
  double t_1apr = (t_mart+t_apr)/2;
  return (t_1apr*d_apr_zima+t_apr*(d30-d_apr_zima))/d30;
}

double sredn2(double t_apr, double t_mai, int d_apr_zima, int m4) 
{
  int d30 = mon_days(m4, 1);

  double t_1mai = (t_apr+t_mai)/2;
  return (t_1mai*(d30-d_apr_zima)+t_apr*d_apr_zima)/d30;
}



void TpCug::set_length(int m4, int d4, int m10, int d10)
{
    int c0 = m_c0;

    for (map<int, CString>::const_iterator it = m_map_ist.begin(); it != m_map_ist.end(); it++)
    {
        long hSourceID = it->first;

        int r = 0;

        for (int m = 1; m <= 12; m++) {
          int sezon = (m4 < m && m < m10) ? 2 : 1;
          
          int md = mon_days(m, 1);

          if (m == m4) {
            QuickSetNumber(c0+3, r++, d4);
            QuickSetNumber(c0+3, r++, md-d4);
          }
          else if (m == m10) {
            QuickSetNumber(c0+3, r++, d10-1);
            QuickSetNumber(c0+3, r++, md-d10+1);
          } 
          else {
            QuickSetNumber(c0+3, r++, md);
          }
        }
        c0 += m_dc;
    }
    Invalidate();
}


void TpCug::set_temp(const double *t, int m4, int d4, int m10, int d10)
{
    int r = 0;
    
    for (int m = 1; m <= 12; m++) {
      int sezon = (m_m4 < m && m < m_m10) ? 2 : 1;
      
      double t2 = t[m-1];
      double t1 = m >= 2 ? t[m-2] : t2;
      double t3 = m < 12 ? t[m] : t2;
      
      if (m == m_m4) {
        double tz = sredn1(t1, t2, d4, m4);
        double tl = sredn2(t2, t3, d4, m4);

        QuickSetNumber(1, r, tz);
        QuickSetNumber(2, r++, 5);
        QuickSetNumber(1, r, tl);
        QuickSetNumber(2, r++, 15);
      }
      else if (m == m_m10) {
        double tl = sredn1(t1, t2, d10-1, m10);
        double tz = sredn2(t2, t3, d10-1, m10);

        QuickSetNumber(1, r, tl);
        QuickSetNumber(2, r++, 15);
        QuickSetNumber(1, r, tz);
        QuickSetNumber(2, r++, 5);
      } 
      else {
        QuickSetNumber(1, r, t2);
        QuickSetNumber(2, r++, sezon == 1 ? 5 : 15);
      }
    }
    set_length(m4, d4, m10, d10);

    
    Invalidate();
}
