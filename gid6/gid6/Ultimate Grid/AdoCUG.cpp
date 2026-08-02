/////////////////////////////////////////////////////////////////////////////
//  Skeleton Class for a Derived CUGCtrl class

#include "stdafx.h"
//#include "resource.h"
#include "AdoCug.h"
#include "ADOImpDatasource.h"
#include "gidrView.h"
#include "resource.h"
#include <regex>

#include "wm_user.h"


int IsFile(const TCHAR* fN);
CString argpath();


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(AdoCug,CUGCtrl)
  //{{AFX_MSG_MAP(AdoCug)
  ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
  ON_MESSAGE(WM_PG_DATESELCHANGED, OnDateChanged)
  ON_COMMAND(ID_EXCEL, OnExcel)
  ON_COMMAND(ID_HIDE_COLUMN, OnHideColumn)
  ON_COMMAND(ID_EDIT, OnEdit)



  ON_COMMAND(ID_MOVE_TO, OnMoveTo)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


bool isVTEdit(VIRT_DATA type)
{
  if (type == virt_data_edit_add) return true;
  if (type == virt_data_edit_add_del) return true;
  if (type == virt_data_edit) return true;
  if (type == virt_data_edit_del) return true;

  return false;
}


bool isVTAdd(VIRT_DATA type)
{
  if (type == virt_data_edit_add) return true;
  if (type == virt_data_edit_add_del) return true;

  return false;
}


bool isVTDel(VIRT_DATA type)
{
  if (type == virt_data_edit_add_del) return true;
  if (type == virt_data_del) return true;
  if (type == virt_data_edit_del) return true;
  
  return false;
}

bool isVTEdit2(VIRT_DATA type)
{
  if (type == virt_data_edit        ) return true;
  if (type == virt_data_node        ) return true;
  if (type == virt_data_line        ) return true;
  if (type == virt_data_edit_add_del) return true;

  return false;
}





bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);

/////////////////////////////////////////////////////////////////////////////
// Standard AdoCug construction/destruction
AdoCug::AdoCug()
{
    m_typ = virt_data_unknown;
    m_tn = "Table2";
    m_curSortCol = -1;
    m_sortDir = UGCT_SORTARROWDOWN;

//    LOGFONT lf;
//    COLORREF color;

//    if (LoadFont2(_T("AdoSampleFont"), lf, color)) {
//        m_defFont.CreateFontIndirect(&lf);
//    }
//    else {
        m_defFont.CreatePointFont( 90, _T("Tahoma"));
//    }

    m_is_edit2 = false;
    m_is_edit = false;
    m_is_add = false;
    m_is_del = false;

    m_head = NULL;

    m_row = -1;

    m_calCtrl = NULL;
}

void AdoCug::setEdit(bool b) 
{ 
  m_is_edit = b; 

  m_GI->m_colInfo[0].dataSource->SetOption(UG_ADO_DS_SET_EDITMODE, b, 0);
};


void AdoCug::setEdit2(bool b) 
{ 
  m_is_edit2 = b; 

  m_GI->m_colInfo[0].dataSource->SetOption(UG_ADO_DS_SET_EDITMODE_2, b, 0);
};


void AdoCug::setAdd (bool b) 
{ 
  m_is_add = b; 
  m_GI->m_colInfo[0].dataSource->SetOption(UG_ADO_DS_USE_GHOSTROW, b, 0);
};

void AdoCug::setDel (bool b) 
{ 
  m_is_del = b; 
  m_GI->m_colInfo[0].dataSource->SetOption(UG_ADO_DS_SET_DELETEMODE, b, 0);
};



AdoCug::~AdoCug()
{
}
/////////////////////////////////////////////////////////////////////////////
//  OnSetup
//    This function is called just after the grid window 
//    is created or attached to a dialog item.
//    It can be used to initially setup the grid
void AdoCug::OnSetup()
{
//  AddTab( _T("Лист 1"), 1 );
//  SetTabWidth( 200 );

  PrintSetOption(UG_PRINT_TOPHEADING, TRUE);
  PrintSetOption(UG_PRINT_LEFTMARGIN, 10);
  PrintSetOption(UG_PRINT_RIGHTMARGIN, 10);
  PrintSetOption(UG_PRINT_TOPMARGIN, 10);
  PrintSetOption(UG_PRINT_BOTTOMMARGIN, 10);

  SetLockRowHeight();

  m_GI->m_dragCol = 0;
  m_GI->m_dragRow = 0;
}


/////////////////////////////////////////////////////////////////////////////
//  OnSheetSetup  
//    This notification is called for each additional sheet that the grid
//    might contain, here you can customize each sheet in the grid.
//  Params:
//    sheetNumber - idndex of current sheet
//  Return:
//    <none>
void AdoCug::OnSheetSetup( int )
{
  // add the sort arrow celltype
  AddCellType( &m_sortArrow );
  // create and set default font
  SetDefFont( &m_defFont );
  // enable popup menus
  EnableMenu( TRUE );
  SetMultiSelectMode( TRUE );
}


CFont *AdoCug::GetFontNormal()
{
    return &m_defFont;
}


/////////////////////////////////////////////////////////////////////////////
//  OnHitBottom
//    This notification allows for dynamic row loading, it will be called
//    when the grid's drawing function has hit the last row.  It allows the grid
//    to ask the datasource/developer if there are additional rows to be displayed.
//  Params:
//    numrows   - known number of rows in the grid
//    rowspast  - number of extra rows that the grid is looking for in the datasource
//    rowsfound - number of rows actually found, usually equal to rowspast or zero.
//  Return:
//    <none>
void AdoCug::OnHitBottom(long numrows,long rowspast,long rowsfound)
{
  UNREFERENCED_PARAMETER(numrows);
  UNREFERENCED_PARAMETER(rowspast);
  UNREFERENCED_PARAMETER(rowsfound);
  // used by the datasources
  if ( rowsfound > 0 )
  {
    SetNumberRows( numrows + rowsfound, FALSE );
  }
}

bool SetCellValue(CUGCell &cell, CString s)
{
    switch (cell.GetDataType()) {

    case UGCELLDATA_STRING:
        cell.SetText(s);
        break;
    case UGCELLDATA_NUMBER:
        if (cell.GetNumberDecimals() == 0) {
            long d;
            if (sscanf(s, "%d", &d) == 1) {
                cell.SetNumber(d);
            }
        }
        else {
            double g;
            if (sscanf(s, "%lg", &g) == 1) {
                cell.SetNumber(g);
            }

        }
        break;
/*
#define UGCELLDATA_BOOL     3
#define UGCELLDATA_TIME     4
#define UGCELLDATA_CURRENCY   5

    case :
        break;
*/
    }

    return true;
}


bool viewVirtualDlg(CWnd *wnd, const char *_bd, const char *q, long &id, const char *cap, CString & text, bool fnd = false, bool add = false, bool edit = false, bool del = false, CString tn = "", bool chk = false);
int getTableLookup(CWnd *wnd, const char *_db, const char *tn, CString &s);



void AdoCug::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
    if (updn) {
        GetOwner()->SendMessage(WM_LCLICK, row);
    }
}


CString GetPath(CString path);


CString getFileDialog(CWnd *wnd, CString defext, CString path, CString filter, CString profile, CString pattern)
{
//    path += "\\";

    CString out = "";

//    CString path0 = AfxGetApp()->GetProfileString(szSection, profile, path)+"\\";
    CString path0 = path + "\\";

    CFileDialog fd(TRUE, defext, path0, OFN_HIDEREADONLY,
//        filter, wnd);
    filter, 0);

    CStringW pw(path0);

    if (path != "") {
        fd.AddPlace((LPCWSTR)pw);
    }   

    if (fd.DoModal() == IDOK) {
        out = fd.GetPathName();
        if (path != "") {
            char szOut[MAX_PATH] = "";
            PathRelativePathTo(szOut, path0, FILE_ATTRIBUTE_DIRECTORY, out, FILE_ATTRIBUTE_NORMAL);
            CString path0 = GetPath(out);
            AfxGetApp()->WriteProfileString(szSection, profile, path0);

            out = szOut;

            if (!strncmp(szOut, ".\\", 2)) {
                out = &szOut[2];
            }
        }
    }

    return out;
}

int IsprDoc(CString doc_name);


/***************************************************
OnRClicked
	This function is called when the right mouse 
	button is clicked over a cell using this cell
	type.
Params:
	col - column that was clicked in
	row - row that was clicked in
	updn - TRUE if the mouse button just went down
		 - FALSE if the mouse button just went up
	rect - rectangle of the cell that was clicked in
	point - point where the mouse was clicked
Return:
	TRUE - if the event was processed
	FALSE - if the event was not
****************************************************/
void AdoCug::OnRClicked(int col, long row, int updn, RECT* rect, POINT* point, BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);

    m_row = row;

    CMenu menu;

	VERIFY(menu.CreatePopupMenu());

	menu.AppendMenu(MF_STRING, ID_EXCEL, "Экспорт в MS Excel");
    menu.AppendMenu(MF_STRING, ID_HIDE_COLUMN, "Скрыть/показать столбцы");


    Klassif *kls = m_geofile->m_kl_list->findKlN(m_tn);
    if (kls) {
        menu.AppendMenu(MF_STRING, ID_MOVE_TO, "Перейти к объекту на схеме");
    }

    CGidrView *pView = getView();

    if (pView && pView->isEditGid()) {
        if (kls && isEditGeo(kls->nazv, false)) {
            menu.AppendMenu(MF_STRING, ID_EDIT, "Редактирование объекта");
        }
    }

//    menu.AppendMenu(MF_STRING, ID_EXCEL, m_tn);

    CPoint pt;
    GetCursorPos(&pt);

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);


//	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//  OnDClicked
//    Sent whenever the user double clicks the left mouse button within the grid
//  Params:
//    col, row  - coordinates of a cell that received mouse click event
//    processed - indicates if current event was processed by other control in the grid.
//    rect    - represents the CDC rectangle of cell in question
//    point   - represents the screen point where the mouse event was detected
//  Return:
//    <none>
void AdoCug::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(row);
  UNREFERENCED_PARAMETER(*rect);
  UNREFERENCED_PARAMETER(*point);
  UNREFERENCED_PARAMETER(processed);
  if ((regex_match(m_tn.GetString(), regex("Тип (арматуры|назначения|конструкции|конструкции камеры|исполнения|месторасположения|материала|характеристики)")))) {
      if (col == 2) {
          long id;
          CString text;
          bool ret = viewVirtualDlg(this, "AstanaGID",
              "select id, name as 'tblName', rus_name as 'Объект' from listPtsTables",
              id, "Объекты ПТС", text, false, false, false, false, "", true);
          if (ret) {
              CUGCell cell;
              GetCell(col + 1, row, &cell);
              SetCellValue(cell, text);
              SetCell(col + 1, row, &cell);
              ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->UpdateNow();
              Refresh2();
          }
          return;
      }
      else if (col == 3) {
          return;
      }
      
  }
  else
  if ((regex_match(m_tn.GetString(), regex("Ответственные")))) {
      if (col == 2) {
          long id;
          CString text;
          bool ret = viewVirtualDlg(this, "AstanaGID",
              "select id, znachenie as 'Должность' from dolzhnosti_es",
              id, "Должности", text, false, false, false, false, "", false);
          if (ret) {
              CUGCell cell;
              CString str;
              str.Format("%d", id);
              GetCell(col+1, row, &cell);
              SetCellValue(cell, str);
              SetCell(col+1, row, &cell);
              ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->UpdateNow();
              Refresh2();
          }
          return;
      }
      else if (col == 3) {
          return;
      }

  }
  if (m_head) {
      const AdoLookup *look = m_head->getLookup(col);
      if (look) {
          CString s;
          long id;
          CString text;
          bool ret = viewVirtualDlg(this, look->db, look->getQ(), id, look->rus_name, text);

          if (ret) {
                CUGCell cell;
                GetCell(col-1, row, &cell);
                cell.SetNumber(id);
                SetCell(col-1, row, &cell);

                ((CADOImpDatasource*)GetDataSource( GetDefDataSource()))->UpdateNow();
                Refresh2();

          }
          return;
      }
      look = m_head->getTable(col);
      if (look) {
          long id;
          CString text;
          bool ret = viewVirtualDlg(this, look->db, look->getQ(), id, look->tn, text);

          if (ret) {
                CUGCell cell;
                GetCell(col, row, &cell);
                SetCellValue(cell, text);
                SetCell(col, row, &cell);
          }
      }
      AdoDocs *docs = m_head->getDocs(col);
      if (docs) {
void view_documents(CWnd *wnd, CString tab1, CString tab2, CString tn1, int id);

            CUGCell cell;
            GetCell(col, row, &cell);
            int id = atoi(cell.GetText());

            view_documents(this, docs->tab1, docs->tab2, "", id);
            return;
      }


      AdoFile *file = m_head->getFile(col);
      if (file) {
            CUGCell cell;
            GetCell(col, row, &cell);
            CString text = cell.GetText();

            CString path = AfxGetApp()->GetProfileString(szSection, file->s3, "");



            if (text != "") {
                CString p2 = path+"\\"+text;

                if (path == "") {
                    if (!IsFile(text)) {
                        CString s;
                        s.Format("Задайте каталог хранения документов");
                        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                        return;
                    }
                    p2 = text;
                }

                if (IsFile(p2)) {
                    HINSTANCE hi = ShellExecute(this->m_hWnd, "open", p2, NULL, NULL, SW_SHOWNORMAL);
                }
                else {
                    CString s;
                    s.Format("Не найден файл %s", p2);
                    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                }
            }
            else {
                CString p = getFileDialog(this, file->s1, path, file->s2, "AdoCUG", file->pattern);

                if (p != "") {
                    if (p.Left(0) == "." && path != "") {
                        CString s;
                        s.Format("Файл должен находиться в каталоге %s", path);
                        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                    }
                    else {
                        SetCellValue(cell, p);
                        SetCell(col, row, &cell);
                        ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->UpdateNow();
                        Refresh2();

                        CString pat, pp;
                        pp = path + "\\" + p;
                        pat.Format("%s\\pat\\%s", argpath(), file->pattern);

                        if (pat != "" && !IsFile(pp)) {
                            CopyFile(pat, pp, 0);
                            if (IsFile(pp)) {
                                IsprDoc(pp);
                                HINSTANCE hi = ShellExecute(this->m_hWnd, "open", pp, NULL, NULL, SW_SHOWNORMAL);
                            }
                        }

                    }
                }
            }
            return;
      }
      else if (m_head->isDate(col)) {
          CPoint pt;
          GetCursorPos(&pt);
          CUGCell cell;
          GetCell(col, row, &cell);
          CString text = cell.GetText();
          SYSTEMTIME t;

          CRect rc(pt.x, pt.y, pt.x+100, pt.y+100);
          
          delete m_calCtrl;

          m_calCtrl = new CPropertyGridMonthCalCtrl;
          CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*)m_calCtrl;
          mc->CreateEx(0, MONTHCAL_CLASS, NULL, WS_POPUP | WS_BORDER, rc, GetParent(), 0);
          memset(&t, 0, sizeof(t));

          if (sscanf(text, "%hd.%hd.%hd", &t.wDay, &t.wMonth, &t.wYear) == 3) {
              mc->SetCurSel(&t);
          }
          mc->SetOwner(this);
          mc->SizeMinReq();

          // now position it
          CRect rc2;
          mc->GetWindowRect(&rc2);
          rc2.OffsetRect(rc.right - rc2.right, 0);
          mc->SetWindowPos(NULL, rc2.left, rc2.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
          m_row_cur = row;
          m_col_cur = col;


          return;
      }
      else {
            CUGCell cell;
            GetCell(col, row, &cell);
            if (cell.GetDataType() == UGCELLDATA_TIME) {
/**
                // the calendar rect
                CRect rc(200, 200, 500, 300);
//                ClientToScreen(&rc);

                // create it
                m_control = new CPropertyGridMonthCalCtrl;
                CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*)m_control;
                mc->CreateEx(0, MONTHCAL_CLASS, NULL, WS_POPUP | WS_BORDER, rc, GetParent(), 0);
                mc->SetCurSel(pItem->m_dtValue);
                mc->SetOwner(this);
                mc->SizeMinReq();
*/
            }
      }
      
  }
  else {
  }


  StartEdit();
  GetOwner()->SendMessage(WM_LDBL, row);
}   


/////////////////////////////////////////////////////////////////////////////
//  OnTH_LClicked
//    Sent whenever the user clicks the left mouse button within the top heading
//    this message is sent when the button goes down then again when the button goes up
//  Params:
//    col, row  - coordinates of a cell that received mouse click event
//    updn    - is TRUE if mouse button is 'down' and FALSE if button is 'up'
//    processed - indicates if current event was processed by other control in the grid.
//    rect    - represents the CDC rectangle of cell in question
//    point   - represents the screen point where the mouse event was detected
//  Return:
//    <none>
void AdoCug::OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
  UNREFERENCED_PARAMETER(row);
  UNREFERENCED_PARAMETER(*rect);
  UNREFERENCED_PARAMETER(*point);
  UNREFERENCED_PARAMETER(processed);

  if ( !updn )
    return;

  BeginWaitCursor();
  
  if ( col == m_curSortCol )
  {
    if ( m_sortDir == UGCT_SORTARROWDOWN )
    {
      m_sortDir = UGCT_SORTARROWUP;
      SortBy( col, UG_SORT_DESCENDING );
    }
    else
    {
      m_sortDir = UGCT_SORTARROWDOWN;
      SortBy( col, UG_SORT_ASCENDING );
    }
  }
  else
  {
    m_curSortCol = col;
    m_sortDir = UGCT_SORTARROWDOWN;
    SortBy( col, UG_SORT_ASCENDING );
  }

  AdjustComponentSizes();
  EndWaitCursor();
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
void AdoCug::OnCharDown(UINT *vcKey,BOOL processed)
{
  UNREFERENCED_PARAMETER(processed);

  StartEdit( *vcKey );
}
  
/////////////////////////////////////////////////////////////////////////////
//  OnGetCell
//    This message is sent everytime the grid needs to
//    draw a cell in the grid. At this point the cell
//    object has been populated with all of the information
//    that will be used to draw the cell. This information
//    can now be changed before it is used for drawing.
//  Warning:
//    This notification is called for each cell that needs to be painted
//    Placing complicated calculations here will slowdown the refresh speed.
//  Params:
//    col, row  - coordinates of cell currently drawing
//    cell    - pointer to the cell object that is being drawn
//  Return:
//    <none>
void AdoCug::OnGetCell(int col, long row, CUGCell *cell)
{
    if (row < 0 && m_head) {

        for (list<P4>::const_iterator it = m_head->list_p4.begin(); it != m_head->list_p4.end(); it++) {
            P4 p4 = *it;

            int r1 = -(m_head->rows - p4.r1);
            int r2 = -(m_head->rows - p4.r2);

            if (p4.c1 <= col && col <= p4.c2 && r1 <= row && row <= r2) {
                cell->SetJoinInfo(FALSE, p4.c1 - col, r1 - row);
            }

            if (p4.c1 == col && r1 == row) {
                cell->SetJoinInfo(TRUE, p4.c2 - p4.c1, p4.r2 - p4.r1);
            }
        }
    }


    if (col >= 0 && col == m_curSortCol && row == -1)
    { // set default values to the top heading
        cell->SetCellType(m_sortArrow.GetID());
        cell->SetCellTypeEx(m_sortDir);
    }
    if (row < 0) {
        cell->SetCellTypeEx(UGCT_NORMALMULTILINE); // Apply extended style to the cell
    }
    else {
//        cell->SetCellTypeEx(UGCT_NORMALELLIPSIS); // Apply extended style to the cell
    }
}

/////////////////////////////////////////////////////////////////////////////
//  OnMenuCommand
//    This notification is called when the user has selected a menu item
//    in the pop-up menu.
//  Params:
//    col, row - the cell coordinates of where the menu originated from
//    setcion - identify for which portion of the gird the menu is for.
//          possible sections:
//            UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//            UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//    item - ID of the menu item selected
//  Return:
//    <none>
void AdoCug::OnMenuCommand(int col,long row,int section,int item)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(section);

  switch( item )
  {
  case 1001:
    InsertRow( row );
    break;
  case 1002:
    AppendRow();
    break;
  case 1003:
    DeleteRow( row );
    break;

  case 1004 :  //Редактировать
    EditRow(row);
    break;
      
  case 1006:
    ((CADOImpDatasource*)GetDataSource( GetDefDataSource()))->RefreshData();
    RedrawAll();
    break;
  case 1007:
    if (((CADOImpDatasource*)GetDataSource( GetDefDataSource()))->UpdateNow() != UG_SUCCESS )
      ((CADOImpDatasource*)GetDataSource( GetDefDataSource()))->CancelUpdates();

    RedrawAll();
    break;
  case 1008:
    ((CADOImpDatasource*)GetDataSource( GetDefDataSource()))->CancelUpdates();
    RedrawAll();
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  OnMenuStart
//    Is called when the pop up menu is about to be shown
//  Params:
//    col, row  - the cell coordinates of where the menu originated from
//    setcion   - identify for which portion of the gird the menu is for.
//          possible sections:
//            UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//            UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//  Return:
//    TRUE - to allow menu to show
//    FALSE - to prevent the menu from poping up
int AdoCug::OnMenuStart(int col,long row,int section)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(row);

  EmptyMenu();

  if ( section == UG_GRID )
  {
//      AddMenuItem( 1006, _T("Обновить данные"));
//      AddMenuItem( 1007, _T("Commit changes"));
//      AddMenuItem( 1008, _T("Cancel changes"));
  }
  else if ( section == UG_SIDEHEADING )
  {
      if (m_is_edit2) {
          AddMenuItem( 1004, _T("Редактировать"));
//          AddMenuItem( 1001, _T("Вставить строку"));
      }
//      if (m_is_edit) {
//          if (m_is_add) AddMenuItem( 1002, _T("Добавить строку"));
          if (m_is_del) AddMenuItem( 1003, _T("Удалить строку"));
//      }
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//  OnTabSelected
//    Called when the user selects one of the tabs on the bottom of the screen
//  Params:
//    ID  - id of selected tab
//  Return:
//    <none>
void AdoCug::OnTabSelected(int ID)
{
  SetSheetNumber( ID - 1 );
}



void AdoCug::OnColSized(int col,int *width)
{
  UNREFERENCED_PARAMETER(col);
  UNREFERENCED_PARAMETER(*width);

  CString p2;
  p2.Format("%d", col);
  AfxGetApp()->WriteProfileInt(m_section, p2, *width);
}


int AdoCug::OnTopHdgSized(int *height)
{
  UNREFERENCED_PARAMETER(*height);

  AfxGetApp()->WriteProfileInt(m_section, "row", *height);

  return TRUE;
}

void AdoCug::OnRowSized(long row, int* height)
{
    UNREFERENCED_PARAMETER(row);
    UNREFERENCED_PARAMETER(*height);

    AfxGetApp()->WriteProfileInt(m_section, "row1", *height);
}


double getScreenMas();

void AdoCug::setName(const char *tn, VIRT_DATA typ)
{
    m_tn = tn;
    m_typ = typ;

    m_section.Format("%s\\%s\\%s", szSection, "UltimateTable", m_tn);

    int height = AfxGetApp()->GetProfileInt(m_section, "row",  -1);

    if (height >= 0) {
      SetRowHeight(-1, height);
    }
    else {
      SetRowHeight(-1, 100);
    }

   int height1 = AfxGetApp()->GetProfileInt(m_section, "row1", -1);
   if (height1 >= 0) {
       SetRowHeight(1, height1);
   }
   else {
       SetRowHeight(1, 40);
   }



//    CADOImpDatasource *ado_s = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

    int cols = GetNumberCols();

    for (int col = 0; col < cols; col++) {
      CString p2;
      p2.Format("%d", col);

      int width0 = -1;

      if (m_head) {

double getScreenMas();

            double mas = getScreenMas();

            map<int, int>::const_iterator it = m_head->width.find(col);
            if (it != m_head->width.end()) {
                width0 = it->second*mas;
            }
      }

      int width = AfxGetApp()->GetProfileInt(m_section, p2, width0);

      if (m_head && !m_head->save && width0 != -1) {
          width = width0;
      }


      CString s = "";

//      if (ado_s->GetColName(col, &s) == UG_SUCCESS) {
//      }

//      if (s.Find("__") == 0) width = -1;

      if (width == -2) {
          SetColWidth(col, 0);
      }
      else if (width == -1) {
          BestFit(col, col, 20, UG_BESTFIT_TOPHEADINGS | UG_BESTFIT_AVERAGE);
          //          BestFit(col, col, 20, 0);
      }
      else {
          SetColWidth(col, width);
      }
    }

    setEdit(isVTEdit(typ));
    setAdd (isVTAdd(typ));
    setDel (isVTDel(typ));

    if (isVTEdit2(typ)) {
        setEdit2(true); 
    }

    if (m_head) {
        SetTH_NumberRows(m_head->rows);

        int height = 0;
        for (int i = 0; i < m_head->rows; i++) {
          int hh = -1;
          
          if (m_head) {
                map<int, int>::const_iterator it = m_head->height.find(i);
                if (it != m_head->height.end()) {
                    hh = it->second;
                }
          }


          if (hh != -1) {
              height += hh;
              SetTH_RowHeight(i - m_head->rows, hh);
          }
        }

        if (height > 0) {
            SetTH_Height(height);
        }

        EnableJoins(TRUE);
    }

}


#include "Filtr3.h"

void AdoCug::HideColumn()
{
  int cols = GetNumberCols();

  CADOImpDatasource *ado_s = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

  CFiltr3 dlg(this, "Укажите отображаемые столбцы таблицы");

  for (int col = 1; col < cols; col++) {
      CString s;

      if (ado_s->GetColName(col, &s) == UG_SUCCESS) {
          int w = GetColWidth(col);

          CString s2 = ado_s->getFullName1(s);

//          dlg.Add(s+" : "+s2, col, w > 0);
          dlg.Add(s2, col, w > 0);
      }
  }

  if (dlg.DoModal() == IDOK) {
    for (int col = 1; col < cols; col++) {
        bool on = dlg.isOn(col);
        int w = GetColWidth(col);
        if (w > 0 && !on) {
          SetColWidth(col, 0);
          w = 0;
        }
        else if (w == 0 && on) {
          SetColWidth(col, 100);
          w = 100;
        }

        CString p2;
        p2.Format("%d", col);
        AfxGetApp()->WriteProfileInt(m_section, p2, w);
    }
    Invalidate();
  }
}

void AdoCug::OnDataSourceNotify(int ID,long msg,long param)
{
  if (msg == UG_ADO_DS_OPEN_CONN) {
  }
}


#include "ado.h"
#include "GeoFile.h"

#include "ado2.h"


int AdoCug::EditRow(long row)
{
    if (row < 0 || row >= m_GI->m_numberRows)
        return UG_ERROR;

    CADOImpDatasource *data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

    long id = -1;

    CUGCell cell;
    data->GetCell(0, row, &cell);
    CString txt;

    txt = cell.GetText();

    if (txt != "") {
        cell.GetNumber(&id);
    }
    CGidrView *pView = getView();

    switch (m_typ) {
      case virt_data_geo :
      case virt_data_geo_big_line :
      {
          Klassif *kls = m_geofile->m_kl_list->findKlN(m_tn);
          if (kls) {
                ado_info(this, kls->m_ado, m_tn, id, kls->RusName(), TP_ZERO, NULL, false);
          }
      }
      break;
      case virt_data_node :
      {
          if (pView) {
                CNode2 *n = pView->m_cxema->m_graph->find(id);
                if (n) pView->info(pView->m_cxema, n, NULL);
          }
      }
      break;
      case virt_data_line :
      {
          if (pView) {
                CLINE2 *l = pView->m_cxema->m_graph->findLine(id);
                if (l) pView->info(pView->m_cxema, NULL, l);
          }
      }
      break;
      case virt_data_node_out:
      case virt_data_line_out:
      {
          if (pView) {
//              CLINE2 *l = pView->m_cxema->m_graph->findLine(id);
//              if (l) pView->info(pView->m_cxema, NULL, l);
          }
      }
      break;
      default :
          {
                CADOImpDatasource *data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());
                CAdoFile *ado = getAdo(data->m_sDbName);
                if (ado) ado_info(this, ado, m_tn, id, m_tn, TP_ZERO, NULL, false);
          }
          break;

    }

    return 0;
}


LRESULT AdoCug::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    CADOImpDatasource *data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

//   if (data->UpdateNow() != UG_SUCCESS)
//        data->CancelUpdates();
//
    data->Refresh2();

    RedrawAll();
    Invalidate();
    return 0;
}



void AdoCug::UpdateNow()
{
   CADOImpDatasource *data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

   if (data->UpdateNow() != UG_SUCCESS) {
        data->CancelUpdates();
   }
}


void AdoCug::Refresh2()
{
    CADOImpDatasource *data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

    data->Refresh2();

    RedrawAll();
    Invalidate();
}



/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void AdoCug::RefreshData() 
{
/**
    int nSheetNumber =GetSheetNumber();
  _tagDSList *pTempItem = m_pDSList;
  
  for( int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex ++ )
    pTempItem = pTempItem->next;

  if ( pTempItem == NULL )
    return;

  pTempItem->m_dsADO->RefreshData();
  RedrawAll();
/**/


  BeginWaitCursor();
  
//  m_GI->m_defDataSource-> Refresh2();

  CADOImpDatasource* data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());

  data->Refresh2();

  AdjustComponentSizes();
  EndWaitCursor();
}


void AdoCug::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
  GetOwner()->SendMessage(WM_TABLE_CHANGE, 0, 0L);
}



void AdoCug::setHead(AdoHead *h)
{
    m_head = h;
}


void AdoCug::showAll(bool show_all)
{
//    long l1 = GetNumberRows();
    ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->ShowAll(show_all);
//    long l = GetNumberRows();

//    SetNumberRows(l , FALSE);
    Refresh2();
}

LRESULT AdoCug::OnDateChanged(WPARAM wParam, LPARAM lParam)
{
    CString s;
    SYSTEMTIME t;

    CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*)m_calCtrl;
    mc->GetCurSel(&t);

    CUGCell cell;
    GetCell(m_col_cur, m_row_cur, &cell);
    cell.SetTime(0, 0, 0, t.wDay, t.wMonth, t.wYear);
    SetCell(m_col_cur, m_row_cur, &cell);

    ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->UpdateNow();
    Refresh2();

    if (m_calCtrl)
    {
        if (m_calCtrl->GetSafeHwnd())
            m_calCtrl->DestroyWindow();
        delete m_calCtrl;
        m_calCtrl = NULL;
    }

    return 0;
}


void findMs(int mag);
void findRs(int mag);


void AdoCug::OnDestroy()
{
    if (m_tn == "Участок МС" || m_tn == "Участок РС") {
        UpdateNow();
        findMs(-1);
        findRs(-1);
    }
}


long AdoCug::GetNumRowsComplete()
{
    long rows = -1;

    try {
        rows = ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->GetNumRowsComplete();
    }
    catch (...) {
        return -1;
    }

    return rows;
}


void AdoCug::OnMoveTo()
{
//    GetOwner()->SendMessage(ID_MOVE_TO2, m_row);
//    GetOwner()->SendMessage(ID_MOVE_TO, m_row);
    GetOwner()->SendMessage(WM_LDBL, m_row);
}

void AdoCug::OnExcel()
{
    int cols = GetNumberCols();

    if (cols <= 0) return;

    list<int> list_col;


    for (int col = 0; col < cols; col++) {
        int w = GetColWidth(col);
        if (w != 0) {
            list_col.push_back(col);
        }
    }

    ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->excel(list_col);
}

void AdoCug::OnEdit()
{
    CUGCell cell;
    GetCell(0, m_row, &cell);
    CString s = cell.GetText();
    s.Replace("\xA0", "");

    Klassif *kls = m_geofile->m_kl_list->findKlN(m_tn);

    if (kls) {
        CGidrView *pView = getView();

        if (pView && pView->isEditGid()) {
            int id = atoi(s);
//        prop = ado_info(this, kls->m_ado, m_tn, id, kls->RusName(), 3, NULL, autodelete);
            pView->GeoInfo(m_tn, id, false);
        }
    }


//    ((CADOImpDatasource*)GetDataSource(GetDefDataSource()))->excel(list_col);
//    AfxMessageBox(m_tn+" "+s);
}



void AdoCug::OnHideColumn()
{
    HideColumn();
}

int AdoCug::Execute(const char *q)
{
  CADOImpDatasource* data = (CADOImpDatasource*)GetDataSource(GetDefDataSource());
  return data->Execute(q);
}


