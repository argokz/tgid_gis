// FormDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
//#include "DialogNew.h"
#include "DialogNew2.h"
#include "FormDialog2.h"

//#include "grid/msflexgrid.h"

#include "InputDia.h"


#include "win.h"
#include "MyEdit.h"
//#include "colorbtn.h"
#include "ClrButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormDialog2

IMPLEMENT_DYNCREATE(CFormDialog2, CFormView)

static int m_id_null = 0;

void initDialog()
{
  m_id_null = 0;
}

void CFormDialog2::reset()
{
  for (int i = 0; i < m_n_control; i++) {
    m_control[i].m_height = 0;
    if (m_control[i].m_wnd) m_control[i].m_wnd->DestroyWindow();
    delete m_control[i].m_wnd;
    m_control[i].m_wnd = 0;
  }
  m_n_control = 0;
}


CFormDialog2::CFormDialog2()
//  : CFormView(CFormDialog2::IDD)
  : CFormView((LPCTSTR)NULL)
{
  //{{AFX_DATA_INIT(CFormDialog2)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_bSizedBefore = FALSE;

//  m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DIALOGTYPE));

  myFont = NULL;
  myFontB = NULL;
  myFontI = NULL;

  m_n_control = 0;
  m_currentC = NULL;
  is_font = 0;
  m_isOpen = 0;

  m_width = 0;
  m_height = 0;

  m_font_h = 0;

  m_id_null ++;
  m_id = m_id_null;

  m_mdb = "";
  m_fn = "";

}

CFormDialog2::~CFormDialog2()
{
  for (int i = 0; i < m_n_control; i++) {
    delete m_control[i].m_wnd;
  }
  delete myFont;
  delete myFontB;
  delete myFontI;
}

void CFormDialog2::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFormDialog2)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFormDialog2, CFormView)
  //{{AFX_MSG_MAP(CFormDialog2)
  ON_WM_SIZE()
  ON_WM_ERASEBKGND()
//  ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
  ON_MESSAGE(WM_CALENDAR, OnCalendar)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormDialog2 diagnostics

#ifdef _DEBUG
void CFormDialog2::AssertValid() const
{
  CFormView::AssertValid();
}

void CFormDialog2::Dump(CDumpContext& dc) const
{
  CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormDialog2 message handlers

void CFormDialog2::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  // TODO: add draw code here
}

BOOL CFormDialog2::OnEraseBkgnd(CDC* pDC)
{
//  return FALSE;
  CBrush* pOldBrush;

  CBrush backBrush;
//  backBrush.CreateSolidBrush(::GetSysColor(COLOR_3DLIGHT));
  backBrush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE)); // 4
  backBrush.UnrealizeObject();

  CRect rectClient;
  GetClientRect(rectClient);
  rectClient.InflateRect(10, 10, 10, 10);

  pOldBrush = pDC->SelectObject(&backBrush);
  pDC->Rectangle(rectClient);

  pDC->SelectObject(pOldBrush);
  return TRUE;
}


bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);


void setGridDB(CEditGrid2 *grid, const char *mdb, const char *tn, const char *q1, const char *val)
{
  CString q = q1;

  q.Replace("$", val);

  grid->m_tn = tn;
  grid->m_q = q;
  grid->m_file = mdb;

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
//    ret = editQ_2(wnd, dbf, tn, q1, nom1, q2, nom2, readOnly, cap, idd);

//  CInputDialog inD(AfxGetMainWnd( ), "", "?", q);
//  inD.DoModal();


    if (dbf.openTable(q)) {

      int nc = dbf.NFlds();
      int row = 1;

      grid->SetCols(nc);
      grid->SetRows(dbf.NRecs()+1);

      for (int c = 0; c < nc; c++) {
        CString str = dbf.read(c+1);
        grid->SetColWidth(c, 1800);
      }

//      grid->SetColWidth(1, 500);
      grid->SetColWidth(0, 500);

      if (!dbf.rs->IsEOF()) {

        dbf.rs->MoveFirst();

        while ( !dbf.rs->IsEOF() ) {

          for (int c = 0; c < nc; c++) {
            CString str = dbf.read(c+1);
            grid->SetTextMatrix(row, c, str);
          }

          dbf.rs->MoveNext();

          row++;
        }
      }
      dbf.closeTable();
    }

    dbf.closeMDB();
  }
}

CString getComboValue(CControl *cnt, CString v)
{
  int j, nf = (cnt->tok.size()-2)/2;

  for ( j = 0; j < nf; j++) {
    CString v1 = cnt->tok[2+j*2];
    CString v2 = cnt->tok[2+j*2+1];
    CString v = cnt->m_value;

    v.TrimRight();
    v1.TrimRight();
    v2.TrimRight();

    if (v1 == v) {
      return v2;
    }
  }
  return v;
}

void CFormDialog2::Init2()
{
//  SetScaleToFitSize(CSize(1, 1));

  CRect rect;
  GetClientRect(rect);
  setSize(rect.Width(), rect.Height());

  for (int i = 0; i < m_n_control; i++) {
    CWnd *edit = m_control[i].m_wnd;
    
    if (m_control[i].is_visible) {
      edit->ShowWindow(SW_SHOW);
      edit->EnableWindow(TRUE);
    }
    else {
      edit->ShowWindow(SW_HIDE);
      edit->EnableWindow(FALSE);
    }
  }
}

void CFormDialog2::Init3()
{
  for (int i = 0; i < m_n_control; i++) {
    int id = m_control[i].m_id;
    int typ = m_control[i].m_typ;
    int len = m_control[i].m_len;
    CWnd *edit = m_control[i].m_wnd;

    if (edit) {
      switch (m_control[i].m_ctrl) {
      case TIP_EDIT :
      case TIP_DATE :
        break;
      case TIP_READONLY :
        break;
      case TIP_CHECKBOX :
        break;
      case TIP_TABLE :
        {
          CEditGrid2 *grid = (CEditGrid2 *) edit;

          CString tn = m_control[i].m_tn;
          CString q = m_control[i].m_q;
          CString fn = m_control[i].m_fn;
          CString val = m_control[i].m_value;

          CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();

          setGridDB(grid, parent->getDB()->m_strFileName, tn, q, val);
        }
        break;
/*
      case TIP_PZGRAPH :
        {
          CEditGrid2 *grid = (CEditGrid2 *) edit;

          CString q = m_control[i].m_q;
          CString fn = m_control[i].m_fn;
          CString val = m_control[i].m_value;

          CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();

          setGridDB(grid, parent->getDB()->m_strFileName, q, val);
        }
        break;
*/
      case TIP_MEROPR :
        {
/*
          CEditGrid2 *grid = (CEditGrid2 *) edit;

          CString q = m_control[i].m_q;
          CString fn = m_control[i].m_fn;
          CString val = m_control[i].m_value;

          CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();

          setGridDB(grid, parent->getDB()->m_strFileName, q, val);
*/
        }
        break;
      case TIP_COMBO :
        {
          if (m_control[i].m_is_edit) {
            CComboBox *combo = (CComboBox *) edit;
            int j, cur = -1, nf = (m_control[i].tok.size()-2)/2;

            for ( j = 0; j < nf; j++) {
              CString v1 = m_control[i].tok[2+j*2];
              CString v2 = m_control[i].tok[2+j*2+1];
              CString v = m_control[i].m_value;

              v.TrimRight();
              v1.TrimRight();
              v2.TrimRight();

              if (v1 == v) cur = j;
              combo->AddString(v2);
            }
            combo->SetCurSel(cur);
          }
        }
        break;

      case TIP_COMBOEDIT :
        {
          CComboBox *combo = (CComboBox *) edit;
          int j, cur = -1, nf = (m_control[i].tok.size()-2)/2;

          for ( j = 0; j < nf; j++) {
            CString v1 = m_control[i].tok[2+j*2];
            CString v2 = m_control[i].tok[2+j*2+1];
            CString v = m_control[i].m_value;

            v.TrimRight();
            v1.TrimRight();
            v2.TrimRight();

            if (v1 == v) cur = j;
            combo->AddString(v2);
          }
//          combo->SetCurSel(cur);
        }

        break;
      case TIP_STATIC :
        break;
      case TIP_PODPIS :
        break;
      case TIP_BUTTON :
        break;
      }
    }
  }
}


void CFormDialog2::OnInitialUpdate() 
{
  CFormView::OnInitialUpdate();

  LOGFONT lf;
  unsigned long color;

  CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();


  if (LoadFont2(parent->m_font_save_name, lf, color)) {
    m_lf = lf;
    is_font = TRUE;
  }

  Init();
  Init2();

  SetScaleToFitSize(CSize(1, 1));

  if (m_width < 0) m_width = 0;
  if (m_height < 0) m_height = 0;

  SetScrollSizes(MM_TEXT, CSize(m_width, m_height));

  Init3();

  map_field.clear();

  int qq = GetDlgCtrlID();

//  m_calendar.Create( CS_DBLCLKS | WS_TABSTOP | WS_CHILD| WS_VISIBLE | WS_THICKFRAME | WS_CLIPSIBLINGS,
  m_calendar.Create( CS_DBLCLKS | WS_TABSTOP | WS_CHILD| WS_VISIBLE | WS_THICKFRAME | WS_CLIPSIBLINGS,
    CRect(0,0,0,0), this, GetDlgCtrlID()+1);
  m_calendar.ShowWindow(SW_HIDE);

  m_isOpen = 1;

}

CControl *CFormDialog2::addControlObj(int ctrl, OBJ obj)
{
  if (m_n_control >= 10240) return NULL; 

  m_control[m_n_control].m_cod = obj.name;
  m_control[m_n_control].m_ctrl = TIP_PODPIS2;

  switch(obj.ID) {
  case ID_TLabel:
    m_control[m_n_control].m_ctrl = TIP_PODPIS2;
    break;
  case ID_TButton:
    m_control[m_n_control].m_ctrl = TIP_BUTTON2;
    m_control[m_n_control].m_podp = obj.Caption;
    break;
  case ID_TEdit:
  case ID_TEditFile:
  case ID_TEditFilePic:
  case ID_TEditFileDoc:
  case ID_TEditDir:
  case ID_TEditPicture:
    m_control[m_n_control].m_ctrl = TIP_EDIT;
    break;
  case ID_TDbTable:
    m_control[m_n_control].m_ctrl = TIP_TABLE;
    break;
  case ID_TComboEdit:
    m_control[m_n_control].m_ctrl = TIP_COMBOEDIT;
    break;
  case ID_TStringGrid:
    m_control[m_n_control].m_ctrl = TIP_EDIT;
    break;
  case ID_TCheckBox:
    m_control[m_n_control].m_ctrl = TIP_CHECKBOX2;
    break;
  case ID_TGroupBox:
    m_control[m_n_control].m_ctrl = TIP_GROUPBOX;
    break;
  case ID_TPanel:
    m_control[m_n_control].m_ctrl = TIP_GROUPBOX;
    m_control[m_n_control].m_ctrl = TIP_EDIT;
    m_control[m_n_control].m_ctrl = TIP_BUTTON2;
    m_control[m_n_control].m_podp = obj.Caption;
    break;
  default:
    return NULL;
  }


//  memset(&m_control[m_n_control], 0, sizeof(CControl));

//  m_control[m_n_control].tok = 
  m_control[m_n_control].m_wnd = NULL;

//  m_control[m_n_control].m_ctrl = TIP_PODPIS;
  m_control[m_n_control].m_fieldName = obj.Caption;
  m_control[m_n_control].m_fn = obj.Caption;
  
  m_control[m_n_control].m_id = m_n_control + 1000*m_id;

  int q = 1;

  m_control[m_n_control].m_left = obj.Left*q;
  m_control[m_n_control].m_top = obj.Top*q;
  m_control[m_n_control].m_width = obj.Width*q;
  m_control[m_n_control].m_height = obj.Height*q;

//  m_control[m_n_control].m_row = row;
//  m_control[m_n_control].m_col = col;
//  m_control[m_n_control].m_cols = cols;
  m_control[m_n_control].m_form = this;

  CFrameWnd *frame = (CFrameWnd *) GetParent();

  if (frame->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
  }

  m_control[m_n_control].m_frame = frame;
  map_control[m_control[m_n_control].m_id] = &m_control[m_n_control];

  m_n_control ++;

  return &m_control[m_n_control-1];
}


CControl *CFormDialog2::addControlObj2(int ctrl)
{
  if (m_n_control >= 10240) return NULL; 

  m_control[m_n_control].m_wnd = NULL;
  m_control[m_n_control].m_id = m_n_control + 1000*m_id;

  int q = 1;

  m_control[m_n_control].m_ctrl = ctrl;
  m_control[m_n_control].m_form = this;

  CFrameWnd *frame = (CFrameWnd *) GetParent();

  if (frame->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
  }

  m_control[m_n_control].m_frame = frame;
  map_control[m_control[m_n_control].m_id] = &m_control[m_n_control];

  m_n_control ++;

  return &m_control[m_n_control-1];
}



CControl *CFormDialog2::addControl(int ctrl, const char *fn, int row, int col, int cols)
{
  if (m_n_control >= 10240) return NULL; 

//  memset(&m_control[m_n_control], 0, sizeof(CControl));

//  m_control[m_n_control].tok = 
  m_control[m_n_control].m_wnd = NULL;
  m_control[m_n_control].m_ctrl = ctrl;
  m_control[m_n_control].m_fieldName = fn;
  m_control[m_n_control].m_fn = fn;
  m_control[m_n_control].m_id = m_n_control + 1000*m_id;
  m_control[m_n_control].m_row = row;
  m_control[m_n_control].m_col = col;
  m_control[m_n_control].m_cols = cols;
  m_control[m_n_control].m_form = this;

  CFrameWnd *frame = (CFrameWnd *) GetParent();


  if (frame->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
  }

  m_control[m_n_control].m_frame = frame;
  map_control[m_control[m_n_control].m_id] = &m_control[m_n_control];
  m_n_control ++;
  return &m_control[m_n_control-1];
}

CControl *CFormDialog2::getControlC(int id)
{
  map<int, CControl*>::const_iterator it = map_control.find(id);

  if (it != map_control.end()) {
    return it->second;
  }

  return NULL;
}

void CFormDialog2::nextRow()
{
  if (m_n_control > 0) m_control[m_n_control-1].m_next = false;
}

void CFormDialog2::Init()
{
  if (myFont) delete myFont;
  if (myFontB) delete myFontB;
  if (myFontI) delete myFontI;

  myFont = new CFont();
  myFontB = new CFont();
  myFontI = new CFont();

  if (is_font) {
    if (m_font_h) m_lf.lfHeight = m_font_h;
    
    myFont->CreateFontIndirect(&m_lf);

    m_lf.lfWeight = 700;
    myFontB->CreateFontIndirect(&m_lf);

    m_lf.lfWeight = 400;
    m_lf.lfItalic = 1;
    
    myFontI->CreateFontIndirect(&m_lf);

    m_lf.lfItalic = 0;
  }
  else {
//    int m_font_h = 18;

    int font_h = 18;
    if (m_font_h) font_h = m_font_h;

    myFont->CreateFont( font_h, 0, 0, 0, FW_NORMAL, false, false,
          0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, _T("MS Sans Serif") );
    myFontB->CreateFont( font_h, 0, 0, 0, FW_NORMAL, false, false,
          0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, _T("MS Sans Serif") );
    myFontI->CreateFont( font_h, 0, 0, 0, FW_NORMAL, false, false,
          0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, _T("MS Sans Serif") );
  }




  BOOL ret = 0;

  for (int i = 0; i < m_n_control; i++) {
    int id = m_control[i].m_id;
    int typ = m_control[i].m_typ;
    int len = m_control[i].m_len;

    int bk_color = m_control[i].m_bkColor;

    if (m_control[i].m_is_edit) bk_color = 0xFFFFFF;
    if (bk_color == 0xFFFFFF && m_control[i].m_ctrl == TIP_PODPIS) bk_color = 0xF0F0F0;

    switch (m_control[i].m_ctrl) {
    case TIP_EDIT :
    case TIP_READONLY :
      {
        CMyEdit *edit = new CMyEdit;
        m_control[i].m_wnd = edit;

        DWORD qq = ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;

        if (m_control[i].m_typ == dbMemo && !m_control[i].m_is_file || m_control[i].m_rows > 1) {
          qq = ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER  | WS_VSCROLL;
        }
        if (m_control[i].m_isText) {
          qq = ES_MULTILINE | ES_WANTRETURN | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER ;
        }

        ret = edit->CreateEx(WS_EX_CLIENTEDGE, "EDIT","", qq, 0, 0, 1, 1, this->GetSafeHwnd(), (HMENU)id);
        if (ret)
        {
          if (typ == dbText) {
            edit->SetLimitText(len);
          }
          edit->SetReadOnly(!m_control[i].m_is_edit);
          edit->SetWindowText(m_control[i].m_value);
          edit->SetBkColor(bk_color);
        }
      }
      break;
    case TIP_DATE :
      {
        CMyEdit *edit = new CMyEdit;
        m_control[i].m_wnd = edit;

        DWORD qq = ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;

        ret = edit->CreateEx(WS_EX_CLIENTEDGE, "SysDateTimePick32","", qq, 0, 0, 1, 1, this->GetSafeHwnd(), (HMENU)id);
        if (ret)
        {
          if (typ == dbText) {
            edit->SetLimitText(len);
          }
          edit->SetReadOnly(!m_control[i].m_is_edit);
          edit->SetWindowText(m_control[i].m_value);
        }
      }
      break;
    case TIP_COMBO :
      {
        if (m_control[i].m_is_edit) {
          CMyComboBox *combo = new CMyComboBox;
          if (combo) {
            m_control[i].m_wnd = combo;
            ret = combo->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,  CRect(0, 0, 1, 1), this, id);
  //          combo->EnableWindow(m_control[i].m_is_edit);
//            combo->SetTextColor(0xFF);
            combo->SetBkColor(bk_color);
          }
        }
        else {
          CMyEdit *edit = new CMyEdit;
          m_control[i].m_wnd = edit;

          DWORD qq = ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER ;

          ret = edit->CreateEx(WS_EX_CLIENTEDGE, "EDIT","", qq, 0, 0, 1, 1, this->GetSafeHwnd(), (HMENU)id);
          if (ret)
          {
            edit->SetReadOnly(TRUE);
            edit->SetWindowText(getComboValue(&m_control[i], m_control[i].m_value));
            edit->SetBkColor(bk_color);
          }
        }
      }
      break;
    case TIP_COMBOEDIT :
      {
        CMyComboBox *combo = new CMyComboBox;
        if (combo) {
          m_control[i].m_wnd = combo;
          ret = combo->Create(CBS_DROPDOWN | WS_VSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,  CRect(0, 0, 1, 1), this, id);
          combo->EnableWindow(m_control[i].m_is_edit);
          combo->SetWindowText(m_control[i].m_value);
          combo->SetBkColor(bk_color);
//          combo->EnableWindow(m_control[i].m_is_edit);
        }
      }
      break;
    case TIP_CHECKBOX :
      {
        CButton *check = new CButton;
        m_control[i].m_wnd = check;
        ret = check->Create("", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY ,  CRect(0, 0, 1, 1), this, id);
        check->SetCheck(m_control[i].m_value == "TRUE");
//        check->SetWindowText(m_control[i].m_value);
        check->EnableWindow(m_control[i].m_is_edit);
      }
      break;
    case TIP_CHECKBOX2 :
      {
        CButton *check = new CButton;
        m_control[i].m_wnd = check;
        ret = check->Create("", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY ,  CRect(0, 0, 1, 1), this, id);
        check->SetCheck(m_control[i].m_value == "TRUE");
        check->SetWindowText(m_control[i].m_podp);
        check->EnableWindow(m_control[i].m_is_edit);
      }
      break;
    case TIP_TABLE :
      {
        CEditGrid2 *grid = new CEditGrid2;
        if (grid) {
          m_control[i].m_wnd = grid;
          ret = grid->Create("", WS_TABSTOP | WS_CHILD | WS_VISIBLE  ,  CRect(0, 0, 1, 1), this, id);
          if (ret) {
//            grid->
            grid->EnableWindow(m_control[i].m_is_edit);
          }
        }
      }
      break;
    case TIP_PZGRAPH :
      {
        CEditGrid2 *grid = new CEditGrid2;
        if (grid) {
          m_control[i].m_wnd = grid;
          ret = grid->Create("", WS_TABSTOP | WS_CHILD | WS_VISIBLE ,  CRect(0, 0, 1, 1), this, id);
          if (ret) {
            grid->EnableWindow(m_control[i].m_is_edit);
          }
        }
      }
      break;
    case TIP_PODPIS :
    case TIP_STATIC :
      if (1) {
        CMyEdit *edit = new CMyEdit;
        m_control[i].m_wnd = edit;

//        DWORD qq = ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_MULTILINE;
//        DWORD qq = WS_CHILD | WS_VISIBLE | WS_BORDER ;
        DWORD qq = ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER ;

        qq = ES_MULTILINE | WS_CHILD | WS_VISIBLE;


        ret = edit->CreateEx(WS_EX_CLIENTEDGE, "EDIT","", qq, 0, 0, 1, 1, this->GetSafeHwnd(), (HMENU)id);
        
        
        if (ret)
        {
//          edit->SetLimitText(100);
          edit->SetReadOnly(TRUE);
          edit->SetWindowText(m_control[i].m_fieldName);
          edit->SetBkColor(bk_color);
        }
      }

      else {
        CMyStatic *edit = new CMyStatic;
        m_control[i].m_wnd = edit;
        
        ret = edit->Create(m_control[i].m_fieldName, ES_MULTILINE | WS_CHILD | WS_VISIBLE,  CRect(0, 0, 1, 1), this, id);
        
        if (ret)
        {
//          edit->SetLimitText(100);
//          edit->SetReadOnly(TRUE);
          edit->SetWindowText(m_control[i].m_fieldName);
          edit->SetBkColor(bk_color);
        }
      }
      break;

    case TIP_PODPIS2 :
      {
        CStatic *stat = new CStatic;
        m_control[i].m_wnd = stat;

        ret = stat->Create(m_control[i].m_fieldName, WS_CHILD | WS_VISIBLE,  CRect(0, 0, 1, 1), this, id);

/*
        CMyEdit *edit = new CMyEdit;
        m_control[i].m_wnd = edit;

//        DWORD qq = ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_MULTILINE;
        DWORD qq = ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER ;
//        DWORD qq = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER ;

        ret = edit->CreateEx(WS_EX_CLIENTEDGE, "EDIT","", qq, 0, 0, 1, 1, this->GetSafeHwnd(), (HMENU)id);
        if (ret)
        {
          edit->SetReadOnly(TRUE);
          edit->SetWindowText(m_control[i].m_fieldName);
        }
*/        
      }
      break;

    case TIP_GROUPBOX :
      {
        CButton *button = new CButton();
        m_control[i].m_wnd = button;
        ret = button->CreateEx(WS_EX_TRANSPARENT, "Button", m_control[i].m_fieldName, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, CRect(0, 0, 1, 1), this, id);
      }
      break;
    case TIP_BUTTON :
      {
        CButton *button = new CButton;
        m_control[i].m_wnd = button;
        ret = button->Create("...", WS_CHILD | WS_VISIBLE,  CRect(0, 0, 1, 1), this, id);
      }
      break;
    case TIP_BUTTON2 :
      {
/*
        CMyButton *button = new CMyButton;
        m_control[i].m_wnd = button;
        ret = button->Create(m_control[i].m_podp, WS_CHILD | WS_VISIBLE,  CRect(0, 0, 1, 1), this, id);
        button->SetBkColor(bk_color);
*/

        CClrButton *button = new CClrButton;
        m_control[i].m_wnd = button;

//        button->SetBkColor(bk_color);
        ret = button->Create(m_control[i].m_podp, BS_OWNERDRAW |  WS_CHILD | WS_VISIBLE,  CRect(0, 0, 1, 1), this, id);

        button->SetColor(0, bk_color);

      }
      break;
    }

    if (!ret) {
      DestroyWindow();
      return;
    }
    
    if (m_control[i].m_wnd) {
      if (m_control[i].m_bold) {
        m_control[i].m_wnd->SetFont(myFontB);
      }
      else if (m_control[i].m_italic) {
        m_control[i].m_wnd->SetFont(myFontI);
      }
      else {
        m_control[i].m_wnd->SetFont(myFont);
      }

    }
  }
}

BOOL CFormDialog2::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
  DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
//  ENSURE(pParentWnd != NULL);
  ASSERT_KINDOF(CFrameWnd, pParentWnd);

  dwStyle &= (~WS_BORDER);
//  dwStyle &= (~WS_CLIPSIBLINGS);

  if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle | WS_CLIPCHILDREN,
    rect, pParentWnd, nID, pContext))
  {
    return FALSE;
  }

  return TRUE;
}

CSize CFormDialog2::getSize(const char *text)
{
  CSize size;
  CFont *oldfont; 
  CClientDC dc(NULL);


  oldfont = dc.SelectObject(myFont);
  size = dc.GetTextExtent(text, lstrlen(text));

  dc.SelectObject(oldfont);

  return size;
}

int CFormDialog2::get_c2(int i)
{
  int c2 = m_control[i].m_col+1;

  for (int j = i+1; m_control[j].m_ctrl != TIP_PODPIS && j < m_n_control; j++) {
    c2 = m_control[j].m_col;
  }
//  if (j < m_n_control) c2 = m_control[j].m_col;

  return c2;

}

#define X1_3 1
#define Y1_3 1.3


int CFormDialog2::getWidth(CControl *cnt)
{
  int width = cnt->m_width;
  int w = width;

  int j, nf = (cnt->tok.size()-2)/2;

  for ( j = 0; j < nf; j++) {
    CString v2 = cnt->tok[2+j*2+1];
    v2.TrimRight();
    
    int ww = getSize(v2).cx+4;

    if (ww > w) {
      w = ww;
    }
  }
  return w;
}

void moveWnd(CWnd *wnd, int left, int top, int width, int height)
{
  UINT flags = SWP_NOSENDCHANGING |SWP_NOACTIVATE | SWP_NOZORDER;


  CRect rect;
  WINDOWPLACEMENT pl;

  wnd->GetWindowRect(rect);
  wnd->GetWindowPlacement(&pl);

  if (pl.rcNormalPosition.left != left || pl.rcNormalPosition.top != top || pl.rcNormalPosition.right-pl.rcNormalPosition.left != width /*|| pl.rcNormalPosition.bottom-pl.rcNormalPosition.top != height*/) {
    wnd->ShowWindow(SW_HIDE);  
    wnd->SetWindowPos(NULL, left, top, width, height, flags);
    wnd->ShowWindow(SW_SHOW);  
  }
}


void CFormDialog2::setSizeD(int cx, int cy)
{
  int i;

  CClientDC screenDC(NULL);
  CFont *oldFont = screenDC.SelectObject(myFont);

  m_width  = 10; 
  m_height = 10; 

  if (oldFont) {

    for (i = 0; i < m_n_control; i++) {
      CWnd *edit = m_control[i].m_wnd;
      int left = m_control[i].m_left;
      int top = m_control[i].m_top;
      int width = m_control[i].m_width;
      int height = m_control[i].m_height;

      if (m_control[i].m_ctrl == TIP_COMBO || m_control[i].m_ctrl == TIP_COMBOEDIT) {
        CComboBox *combo = (CComboBox *) edit;

        int w = getWidth(&m_control[i]);
        if (w > width) {
          combo->SetDroppedWidth(w);
        }
        int hh = height;
        if (m_control[i].m_is_edit) {
          hh = 400;
        }
        moveWnd(combo, left, top, width, hh);
      }
      else {
        moveWnd(edit, left, top, width, height);
      }

      if (left+width > m_width) m_width = left+width; 
      if (top+height > m_height) m_height = top+height; 
    }
  }
}

void CFormDialog2::setSize(int cx, int cy)
{
  int w, i, j;
  m_width = 0;
  m_height = 0;
  CSize size;

  int col, row;
  int cols;

  if (m_n_control <= 0) return;

  for (i = 0; i < m_n_control; i++) {
    if (m_control[i].m_height > 1) {
      setSizeD(cx, cy);
      return;
    }
  }

  int WIDTH_ROW[N256];
  int WIDTH_COL[N256];

  size = getSize("8");

  double DD = 2;
  double D2 = 0;

  double D8 = size.cx;

  double D88 = getSize("8888888888888888888888888888888888888 8888888").cx;
  double D888 = getSize("8888888888888888888888888888888888888 8888888 8888888888888888888888888").cx;

  size = getSize(m_control[0].m_fieldName);
  DD = (double) size.cy / 20;

  for (i = 0; i < N256; i++) WIDTH_ROW[i] = size.cy*Y1_3+D2*DD;
  for (i = 0; i < N256; i++) WIDTH_COL[i] = 0;

  int W_BUTTON = 20*DD;

  {
    CClientDC screenDC(NULL);

    CFont *oldFont = screenDC.SelectObject(myFont);
   
    if (oldFont) {
      for (i = 0; i < m_n_control; i++) {

        cols = m_control[i].m_cols; 
        col = m_control[i].m_col;
        row = m_control[i].m_row;

        int w = 0;
        int h = size.cy*Y1_3+DD*2;

        if (m_control[i].m_ctrl == TIP_COMBO || m_control[i].m_ctrl == TIP_COMBOEDIT) {
          int j, cur = -1, nf = (m_control[i].tok.size()-2)/2;

          w = D888;

          for ( j = 0; j < nf; j++) {
            CString v2 = m_control[i].tok[2+j*2+1];
            v2.TrimRight();
            
            int ww = getSize(v2).cx;

            if (ww > w) {
              w = ww;
            }
          }
        }

        if (m_control[i].m_ctrl == TIP_PODPIS || m_control[i].m_ctrl == TIP_EDIT || m_control[i].m_ctrl == TIP_DATE || m_control[i].m_ctrl == TIP_CHECKBOX) {
          if (m_control[i].m_typ == dbMemo && !m_control[i].m_is_file) {
            CSize sz = screenDC.GetTextExtent("8");
            w = sz.cx*1.1*24;
            h = sz.cy*Y1_3*3+DD*2;
          }
          else {
            CString val = "888888888888";
            if (m_control[i].m_ctrl == TIP_PODPIS) {
              val = m_control[i].m_fn;
            }
            else {
              for (int j = 12; j < m_control[i].m_len && j < 40; j++) {
                val += "8";
              }
            }

            if (m_control[i].m_typ == dbDouble ) {
              val = "88888.888";
            }
            if (m_control[i].m_typ == dbDate ) {
              val = "88:88:88:88888888";
            }
            CSize sz = screenDC.GetTextExtent(val);

            w = sz.cx*X1_3;

            if ((m_control[i].m_typ == dbMemo && !m_control[i].m_is_file) || m_control[i].m_isText) {
               h = sz.cy*Y1_3*3+DD*2;
            }
          }
        }

        if (w > D88) {
          if (m_control[i].m_ctrl == TIP_PODPIS) {
            int nn = (w+D88-1)/D88;
            h = max(h, size.cy*Y1_3*nn+DD*2);
          }
          w = D88;
        }

//        w += D8*4;
        w += D8*2;

        for (int j = 0; j < cols; j++) {
          if (w/cols > WIDTH_COL[col+j]) {
            WIDTH_COL[col+j] = w/cols;
          }
        }

        if (h > WIDTH_ROW[row]) {
          WIDTH_ROW[row] = h;
        }
      }
      screenDC.SelectObject(oldFont);
    }
  }

  int col_max = 0;
  int row_max = 0;

  for (i = 0; i < m_n_control; i++) {
    cols = m_control[i].m_cols; 
    col = m_control[i].m_col;
    row = m_control[i].m_row;
    
    int id = m_control[i].m_id;
    int typ = m_control[i].m_typ;
    int len = m_control[i].m_len;
    CWnd *edit = m_control[i].m_wnd;

    int x = 0*DD;
    int y = 0*DD;

    col_max = max(col_max, col);
    row_max = max(row_max, row);

    for  (j = 0; j < row; j++) y += WIDTH_ROW[j]; 
    for  (j = 0; j < col; j++) x += WIDTH_COL[j]; 

    if (edit) {
      w = 0;
      for (j = 0; j < cols; j++) {
        w += WIDTH_COL[col+j];
      }
      int h = size.cy*Y1_3+DD*2-1+1;

      if ((m_control[i].m_typ == dbMemo && !m_control[i].m_is_file) || m_control[i].m_isText || m_control[i].m_ctrl == TIP_PODPIS) {
        h = WIDTH_ROW[row];
      }

      if (m_control[i].m_ctrl == TIP_EDIT) {
        if (m_control[i].m_next && i+1 < m_n_control && m_control[i+1].m_ctrl == TIP_BUTTON) w -= W_BUTTON;
      }
      if (m_control[i].m_ctrl == TIP_BUTTON) {
        x += (w - W_BUTTON - DD);
        w = W_BUTTON;
      }
      if (m_control[i].m_ctrl == TIP_TABLE) {
        moveWnd(edit, 0, 0, cx, cy);
      }
      else if (m_control[i].m_ctrl == TIP_PZGRAPH) {
        moveWnd(edit, 0, 0, cx, cy);
      }
      else if (m_control[i].m_ctrl == TIP_COMBO && m_control[i].m_is_edit) {
        CComboBox *combo = (CComboBox *) edit;
        int w1 = getWidth(&m_control[i]);
        if (w1 > w) {
          combo->SetDroppedWidth(w1);
        }
        moveWnd(combo, x, y, w-D2*DD, 200*DD);
      }
      else if (m_control[i].m_ctrl == TIP_COMBOEDIT && m_control[i].m_is_edit) {
        CComboBox *combo = (CComboBox *) edit;
        int w1 = getWidth(&m_control[i]);
        if (w1 > w) {
          combo->SetDroppedWidth(w1);
        }
        moveWnd(combo, x, y, w-DD, 200*DD);
      }
      else if (m_control[i].m_ctrl == TIP_CHECKBOX) {
        moveWnd(edit, x, y, w-DD, h);
      }
      else {
        moveWnd(edit, x, y, w-DD, h);
      }
    }
  }

  m_width = 10;
  m_height = 10;

  for  (j = 0, m_width = 4*DD; j <= col_max; j++) m_width  += WIDTH_COL[j]; 
  for  (j = 0, m_height = 4*DD; j <= row_max; j++) m_height += WIDTH_ROW[j]; 
}

void CFormDialog2::OnSize(UINT nType, int cx, int cy)
{
  CFormView::OnSize(nType, cx, cy);
  if (cx > 0 && cy > 0)
  {
    m_bSizedBefore = TRUE;
    CPoint pt(0,0);

//    SetScaleToFitSize(CSize(1, 1));
    
    SetScrollSizes(MM_TEXT, CSize(1, 1));
    pt = GetScrollPosition();

    if (pt.x != 0 || pt.y != 0) ScrollToPosition(CPoint(0,0));
    setSize(cx, cy);
 
    if (m_width > 0 && m_height > 0) {
      SetScrollSizes(MM_TEXT, CSize(m_width, m_height));
      if (pt.x != 0 || pt.y != 0) ScrollToPosition(pt);
    }
  }
}

void CFormDialog2::ScrollTo(CControl *control)
{

  if (!control) return;

  CRect rc;
  CRect rect;
  CWnd *pWnd;
//  if ( pWnd = GetDlgItem(nID%N_RANGE+N_STATS) ) {
  if ( pWnd = control->m_wnd ) {
    pWnd->GetWindowRect(rect);
    
    GetClientRect(rc);
    ScreenToClient(rect);

    if (!rc.PtInRect(rect.TopLeft()) || !rc.PtInRect(rect.BottomRight()))
    {
      CPoint pos = GetScrollPosition();
      BOOL bScrollUp = rect.top < 0 ;
      BOOL bScrollLeft = rect.left < 0 ;

      CPoint ptDevScroll = GetDeviceScrollPosition();
      
      int xx, yy;

      if (bScrollUp) {
        yy = rect.top + pos.y;
      }
      else {
        yy = pos.y + rect.bottom - rc.Height();
      }

      if (bScrollLeft) {
        xx = rect.left+pos.x;
      }
      else {
        xx = pos.x+rect.right-rc.Width();
      }

      ScrollToPosition(CPoint(xx, yy));
    }
  }
}

void CFormDialog2::OnSetFocus(CControl *control)
{
  m_currentC = control;
  ScrollTo(control);
}

void CFormDialog2::OnKillFocus(CControl *control)
{
  CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();

  parent->killFocus(control);
//  m_currentC = control;
//  ScrollTo(control);
}



void CFormDialog2::OnEnUpdate(CControl *control)
{
    CString val;

    if (!control) return;

    CEdit *edit;

    if (control->m_ctrl == TIP_EDIT || control->m_ctrl == TIP_COMBOEDIT  || control->m_ctrl == TIP_DATE ) {
      if (control->m_is_edit) {
        edit = (CEdit*) control->m_wnd;
        edit->GetWindowText(val); 

        UINT myLastSel = edit->GetSel();

        if (control->m_typ == dbInteger || control->m_typ == dbLong) {
          for (int i = 0; i < val.GetLength(); i++) {
            if (!isdigit(val[i])) {
              val = val.Left(i);
              edit->SetWindowText(val); 
              edit->SetSel(myLastSel);
              break;
            }
          }
        }
        if (control->m_typ == dbDouble) {
          for (int i = 0; i < val.GetLength(); i++) {
            if (!isdigit(val[i]) && val[i] != '.' && val[i] != '-' && val[i] != ',' && val[i] != 'e' && val[i] != 'E') {
              val = val.Left(i);
              edit->SetWindowText(val); 
              edit->SetSel(myLastSel);
              break;
            }
          }
        }
      }
    }
}

void CFormDialog2::OnEnChange(CControl *control)
{
    CString val;

    if (!control) return;

    CEdit *edit;

    if (control->m_ctrl == TIP_EDIT || control->m_ctrl == TIP_COMBOEDIT  || control->m_ctrl == TIP_DATE) {
      if (control->m_is_edit) {
        edit = (CEdit*) control->m_wnd;
        edit->GetWindowText(val); 

        chValue(control, val);
      }
    }
}


void CFormDialog2::CbnChange(CControl *control, const char *real_value)
{
  if (!control) return;

  chValue(control, real_value);

  for (int i = 0; i < m_n_control; i++) {
    CControl cn = m_control[i];

    if (cn.m_ctrl == TIP_COMBO) {
      if (cn.m_fn == control->m_fn) {
        if (control->m_wnd != cn.m_wnd) {
          if (cn.m_rjad == control->m_rjad) {
            setValue(&cn, real_value);
          }
        }
      }
    }
  }
}


void CFormDialog2::OnCbnChange(CControl *control)
{
  if (!control) return;

  CComboBox *combo = (CComboBox *) control->m_wnd;

  if (combo) {
    CString val;
    int cur = combo->GetCurSel();
    if (cur >= 0) {
      combo->GetLBText(cur, val); 

      int j,  nf = (control->tok.size()-2)/2;

      for ( j = 0; j < nf; j++) {
        CString v1 = control->tok[2+j*2];
        CString v2 = control->tok[2+j*2+1];
        CString v = control->m_value;

        v.TrimRight();
        v1.TrimRight();
        v2.TrimRight();

        if (v2 == val) {
          chValue(control, v1);

          for (int i = 0; i < m_n_control; i++) {
            CControl cn = m_control[i];
//            if (cn.m_ctrl == TIP_COMBO && cn.m_fn == control->m_fn && control->m_wnd != cn.m_wnd && cn.m_row == control->m_row) {
//            if (cn.m_ctrl == TIP_COMBO && cn.m_fn == control->m_fn && control->m_wnd != cn.m_wnd) {

            if (cn.m_ctrl == TIP_COMBO) {
              if (cn.m_fn == control->m_fn) {
                if (cn.m_tn == control->m_tn) {
                  if (control->m_wnd != cn.m_wnd) {
                    if (cn.m_rjad == control->m_rjad) {
                      setValue(&cn, v1);
                    }
                  }
                }
              }
            }
          }
          break;
        }
      }

/*

    for (int i = 0; i < m_n_control; i++) {
      CControl *c = &m_control[i];

//      if (c->m_fn == control->m_fn && c->m_tn == control->m_tn && c->m_rjad == control->m_rjad) {
      if (c->m_fn == control->m_fn && c->m_rjad == control->m_rjad && c != control) {
        setValue(c, v1);

//            CString s = getViewValue(val, control->mdb, control->table, control->id, control->fn_id);
//            control->m_real_value = val;
//            setValue(control, s, val);
      }
    }

*/



    }
  }
}

void CFormDialog2::OnBnClicked(CControl *control)
{
  CString str;

  if (control && (control->m_ctrl == TIP_BUTTON || control->m_ctrl == TIP_BUTTON2) && control->m_main && control->m_main->m_ctrl == TIP_EDIT) {

    CControl *mc = control->m_main;

    if (mc->m_typ == dbDate) {
      m_currentC = mc;
      CString val;
      mc->m_wnd->GetWindowText(val);
      m_calendar.InitCalendar(val, 1);

      RECT r;
      mc->m_wnd->GetWindowRect(&r);
      r.top = r.bottom;

      ScreenToClient(&r);

      m_calendar.PositionCalendar(r);     
    }
    else {
      CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();
      parent->Do(control);
    }
  }
  else if (control && (control->m_ctrl == TIP_BUTTON || control->m_ctrl == TIP_BUTTON2)) {
    CControl *mc = control->m_main;
    CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();
    parent->Do(control);
  }



  else if (control && control->m_ctrl == TIP_CHECKBOX || control && control->m_ctrl == TIP_CHECKBOX2 ) {
    CButton *check = (CButton *) control->m_wnd;
    if (check) {
      BOOL ch = check->GetCheck();
      chValue(control, ch == BST_CHECKED ? "TRUE" : "FALSE");
    }
  }
}

void CFormDialog2::OnShiftF1()
{
  CControl *control = m_currentC;

  if (control) {
    CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();
    parent->Help(control->m_base, control->m_tn, control->m_fieldName);
  }
}

BOOL CFormDialog2::PreTranslateMessage(MSG* pMsg) 
{
//  if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
//    if (pMsg->wParam != VK_RETURN) return TRUE;
//  }


  if (pMsg->message==WM_KEYDOWN)
  {
    if (pMsg->wParam==VK_F4)
    {
      return PreTranslateInput(pMsg);
    }

    
    if (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE)
    {
      CControl *control = m_currentC;

      if (control && (control->m_ctrl == TIP_COMBO || control->m_ctrl == TIP_COMBOEDIT)) {
        if (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE) {
          CComboBox *combo = (CComboBox *) control->m_wnd;
          if (combo && combo->GetDroppedState()) {
          }
          goto QUIT;
        }
      }
      if (control && (control->m_typ == dbMemo && !control->m_is_file) && pMsg->wParam==VK_RETURN) {
      }
      else {
        CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();

        parent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
      }
    }
    if (pMsg->wParam==VK_DELETE)
    {
      CControl *control = m_currentC;

      if (control && (control->m_ctrl == TIP_COMBO || control->m_ctrl == TIP_COMBOEDIT)) {
        CComboBox *combo = (CComboBox *) control->m_wnd;

        if (combo) {
          combo->SetCurSel(-1);
          chValue(control, "");
        }
      }
    }
    if (pMsg->wParam==VK_F1)
    {

      int shift = GetKeyState(VK_SHIFT) & 0x80;
      if (shift) {
  //      AfxMessageBox("");
        CControl *control = m_currentC;
        if (control) {
          CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();
          parent->Help(control);
        }
      }
    }
  }

QUIT :

  // allow tooltip messages to be filtered
  if (CView::PreTranslateMessage(pMsg))
    return TRUE;

  // don't call IsDialogMessage if form is empty
  if (::GetWindow(m_hWnd, GW_CHILD) == NULL)
    return FALSE;

  // filter both messages to dialog and from children
  return PreTranslateInput(pMsg);
}

void CFormDialog2::chValue(CControl *control, const char *val)
{
  CString old_val = control->m_value;

  CDialogNew2 *parent = (CDialogNew2 *) GetParent()->GetParent();
  if (parent->chValue(control, val, old_val)) {
    
    if (val != old_val) {
      map_field[*control] = val;
    }
    control->m_new_value = val;
    control->m_real_value = val;
    
    parent->chValue2(control, val, old_val);
    control->m_value = val;
  }
}


BOOL CFormDialog2::setValue(const char *tn, const char *fn, const char *val)
{
  for (int i = 0; i < m_n_control; i++) {
    if (m_control[i].m_ctrl != TIP_PODPIS && m_control[i].m_ctrl != TIP_PODPIS2 && !strcmp(fn, m_control[i].m_fieldName)) {
      return setValue(&m_control[i], val);
    }
  }
  return FALSE;
}

BOOL CFormDialog2::setValue(CControl* control, const char *val)
{
  if (control->m_ctrl == TIP_EDIT || control->m_ctrl == TIP_COMBOEDIT || control->m_ctrl == TIP_DATE) {
    CEdit *edit = (CEdit *) control->m_wnd; 
    if (edit) {
      edit->SetWindowText(val);
      chValue(control, val);
      return TRUE;
    }
  }
  if (control->m_ctrl == TIP_CHECKBOX) {
    CButton *check = (CButton *) control->m_wnd; 
    if (check) {
      check->SetCheck(val == "TRUE");
      chValue(control, val);
      return TRUE;
    }
  }
  if (control->m_ctrl == TIP_COMBO) { ///////////////////////
    CComboBox *combo = (CComboBox *) control->m_wnd; 
    if (combo) {
      for (int i = 1; i < control->tok.size()/2; i ++) {
        CString qq1 =  control->tok[i*2];
        CString qq2 =  control->tok[i*2+1];
        if (qq1 == CString(val)) {
          if (control->m_is_edit) {
            combo->SetCurSel(i-1);
          }
          else {
            CEdit *edit = (CEdit *) control->m_wnd; 
            edit->SetWindowText(qq2);
          }
        }
      }
      chValue(control, val);
      return TRUE;
    }
  }
  return FALSE;
}


BOOL CFormDialog2::setValue(CControl* control, const char *val, const char *real)
{
  if (control->m_ctrl == TIP_EDIT || control->m_ctrl == TIP_COMBOEDIT || control->m_ctrl == TIP_DATE) {
    CEdit *edit = (CEdit *) control->m_wnd; 
    if (edit) {
      edit->SetWindowText(val);
      chValue(control, real);
      return TRUE;
    }
  }
  return FALSE;
}



CString CFormDialog2::getValue(CControl* control)
{
  CString val = "";

  if (control->m_ctrl == TIP_EDIT || control->m_ctrl == TIP_COMBOEDIT || control->m_ctrl == TIP_DATE) {
    CEdit *edit = (CEdit *) control->m_wnd; 
    if (edit) {
      edit->GetWindowText(val);
      return val;
    }
    else {
      return control->m_value;
    }
  }
  else if (control->m_ctrl == TIP_CHECKBOX || control->m_ctrl == TIP_CHECKBOX2) {
    CButton *check = (CButton *) control->m_wnd;
    if (check) {
      BOOL ch = check->GetCheck();
      val = ch == BST_CHECKED ? "Да" : "Нет";
    }
  }
  else if (control->m_ctrl == TIP_COMBO) {
    CComboBox *cmb = (CComboBox *) control->m_wnd; 
    if (cmb) {
      cmb->GetWindowText(val);
      return val;
    }
  }
  return val;
}

CControl *CFormDialog2::getControl(const char *fn)
{
  for (int i = 0; i < m_n_control; i++) {
    if (m_control[i].m_ctrl != TIP_PODPIS && m_control[i].m_ctrl != TIP_PODPIS2 && !strcmp(fn, m_control[i].m_fieldName)) {
      return &m_control[i];
    }
  }
  return NULL;
}

CControl *CFormDialog2::getControlR(const char *tn, const char *fn, int rjad)
{
  for (int i = 0; i < m_n_control; i++) {
    if (m_control[i].m_ctrl != TIP_PODPIS && m_control[i].m_ctrl != TIP_PODPIS2 && !strcmp(fn, m_control[i].m_fieldName) &&  m_control[i].m_rjad == rjad &&  m_control[i].m_tn == tn) {
      return &m_control[i];
    }
  }
  return NULL;
}

BOOL CFormDialog2::getValue(const char *fn, CString &val)
{
  CControl *cntrl = getControl(fn);

  if (cntrl) {
    val = getValue(cntrl);
    return TRUE;
  }
  return FALSE;
}

CString DateToString(const COleDateTime &Date);

LRESULT CFormDialog2::OnCalendar(WPARAM wParam, LPARAM lParam)
{
  DATE date = (DATE) lParam;
  CString str = DateToString(COleDateTime(date));
  
  CControl *control = m_currentC;

  if (control) {
    setValue(control->m_tn, control->m_fieldName, str);
  }

  return 0L;
}

void CFormDialog2::setFontH(int h)
{
  m_font_h = h;

  LOGFONT lf;

  lf.lfFaceName[0] = 0;
  lf.lfHeight = h;
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = 400;
  lf.lfItalic = 0;
  lf.lfUnderline = 0;
  lf.lfStrikeOut = 0;
  lf.lfCharSet = 204;
  lf.lfOutPrecision = 0;
  lf.lfClipPrecision = 0;
  lf.lfQuality = 0;
  lf.lfPitchAndFamily = 0;

  setFont(lf);
}

void CFormDialog2::setFont(LOGFONT lf)
{
  m_lf = lf;

  if (myFont) delete myFont;
  if (myFontB) delete myFontB;
  if (myFontI) delete myFontI;
  myFont = new CFont();
  myFontB = new CFont();
  myFontI = new CFont();
  myFont->CreateFontIndirect(&m_lf);
  m_lf.lfWeight = 700;
  myFontB->CreateFontIndirect(&m_lf);

  m_lf.lfWeight = 400;
  m_lf.lfItalic = 1;
  
  myFontI->CreateFontIndirect(&m_lf);

  m_lf.lfItalic = 0;

  for (int i = 0; i < m_n_control; i++) {
    if (m_control[i].m_wnd) {
      if (m_control[i].m_bold) {
        m_control[i].m_wnd->SetFont(myFontB);
      }
      else if (m_control[i].m_italic) {
        m_control[i].m_wnd->SetFont(myFontI);
      }
      else {
        m_control[i].m_wnd->SetFont(myFont);
      }

    }
  }

  CRect rect;
  GetClientRect(rect);
  setSize(rect.Width(), rect.Height());
}

BOOL CFormDialog2::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  HWND hWndCtrl = (HWND)lParam;
  UINT nID = LOWORD(wParam);
  UINT msg = HIWORD(wParam); // == EN_CHANGE

  CControl *control = getControlC(nID);

  if (control && m_isOpen) {

    if (control->m_typ == TIP_TABLE) {
      int qq;
      qq = 1;
    }
    switch(msg) {
    case EN_UPDATE : 
      OnEnUpdate(control);
      break;
    case EN_CHANGE : 
    case CBN_EDITCHANGE :
      OnEnChange(control);  
      break;
    case CBN_SELCHANGE : 
      OnCbnChange(control);  
      break;
    case EN_SETFOCUS :
    case CBN_SETFOCUS :
    case BN_SETFOCUS :
      OnSetFocus(control);  
      break;
    case EN_KILLFOCUS :
    case CBN_KILLFOCUS :
    case BN_KILLFOCUS :
      OnKillFocus(control);  
      break;
    case BN_CLICKED : 
      OnBnClicked(control);  
      break;
    }
  }


  if (nID != 0 && !::IsWindow(hWndCtrl)) return TRUE;


  return CFormView::OnCommand(wParam, lParam);
}

/*
HBRUSH CSplashDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 
    if(nCtlColor==CTLCOLOR_BTN)
    {
        pDC->SetBkMode(TRANSPARENT);
        hbr=m_brush;
    }
    return hbr;
}
*/


CString getViewValue(const char *val, const char *mdb1, const char *table, const char *id, const char *fn);

void CFormDialog2::setPS(const char *fn, const char *val, const char *mdb, const char *table, const char *id, const char *fn_id)
{
  for (int i = 0; i < m_n_control; i++) {
    CControl *control = &m_control[i];

    if (control->m_fn == fn) {
      CString s = getViewValue(val, control->mdb, control->table, control->id, control->fn_id);
      control->m_real_value = val;
      setValue(control, s, val);
    }
  }
}

void CFormDialog2::setXY(const int *row_h, const int *col_w)
{
  if (row_h) {
    for (int i = 0; i < N256; i++) {
       m_col_w[i] = col_w[i];
       m_row_h[i] = row_h[i];
    }
  }

  for (int i = 0; i < m_n_control; i++) {
    CControl *control = &m_control[i];
    if (control->is_visible) {
      if (control->m_ctrl == TIP_BUTTON) {
        CControl * cm = control->m_main;

        control->m_width =  18; 
        control->m_left =   cm->m_left+cm->m_width-control->m_width-2;

        control->m_top =    cm->m_top;
        control->m_height = cm->m_height-2; 

        cm->m_width -=  control->m_width; 

        control->m_top += 2;
        cm->m_width --;

        continue;
      }
      else {
        int w = 0, h = 0;

    //    if (0 <= control->m_row && control->m_row < 256) h = m_row_h[control->m_row];
    //    if (0 <= control->m_col && control->m_col < 256) w = m_col_w[control->m_col];

        int i, j, top = 0, left = 0;

        for (i = control->m_row, j = 0; j < control->m_rows ; i++, j++) {
          int h1 = 0;
          if (0 <= control->m_row && control->m_row < N256) h1 = m_row_h[i];
          if (h1 == 0) h1 = 24;
          h += h1;
        }

        for (i = control->m_col, j = 0; j < control->m_cols ; i++, j++) {
          int w1 = 0;
          if (0 <= control->m_col && control->m_col < N256) w1 = m_col_w[i];
          if (w1 == 0) w1 = 200;
          w += w1;
        }

        for (i = 0; i  < control->m_row; i++) {
          int h1 = 0;
          if (0 <= control->m_row && control->m_row < N256) h1 = m_row_h[i];
          if (h1 == 0) h1 = 24;
          top += h1;
        }

        for (i = 0; i  < control->m_col; i++) {
          int w1 = 0;
          if (0 <= control->m_col && control->m_col < N256) w1 = m_col_w[i];
          if (w1 == 0) w1 = 200;
          left += w1;
        }

        if (w == 0) w = 200;
        if (h == 0) h = 24;
    
        control->m_left = left;
        control->m_top = top;
        control->m_width = w;
        control->m_height = h;
      }
    }
  }
}
/*

CControl *CFormDialog2::Next(CControl *control)
{
  for (int i = 0; i < m_n_control; i++) {
    CControl *c = &m_control[i];
    if (c == control) {
      return &m_control[(i+1)%m_n_control];
    }
  }
  return NULL;
}

CControl *CFormDialog2::Prev(CControl *control)
{
  for (int i = 0; i < m_n_control; i++) {
    CControl *c = &m_control[i];
    if (c == control) {
      return &m_control[(i+m_n_control-1)%m_n_control];
    }
  }
  return NULL;
}
*/


CControl *CFormDialog2::Next(CControl *control, int n)
{
  for (int i = 0; i < m_n_control; i++) {
    CControl *c = &m_control[i];
    if (c == control) {
      return &m_control[(i+m_n_control+n)%m_n_control];
    }
  }
  return NULL;
}

CControl *CFormDialog2::Prev(CControl *control, int n)
{
  return Next(control, -1);
}
