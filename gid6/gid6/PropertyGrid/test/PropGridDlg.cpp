// PropGridDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "..\..\PropertyGrid2.h"
#include "PropGridDlg.h"
#include ".\propgriddlg.h"

#include "..\crack.h"

#include "..\wm_user.h"


void SaveFont(const TCHAR *szSection, const LOGFONT &lf, COLORREF color);
bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridData::CGridData(CString _name, CString _val)
{
  table = "";
  id = 1;
  name = _name;
  label = _name;
  help = _name;
  typ = adBSTR;

//  value;
//  typ;
//  DefinedSize;
  NumericScale = 0;
  Precision = 0;
//  table_num;
  isList = false;
//  q;
}



CPropGridDlg::CPropGridDlg(CWnd* pParent, fun_prop fun_proc, void *param, const char *table, const char *capt, TYP_OF_P typ_of_p, void *param2, bool shift)
: CDialog(CPropGridDlg::IDD, pParent)
{
    m_par = pParent;
    m_send_value = WM_USER_ADO_UPDATED;
    m_typ_of_p = typ_of_p;
    m_param2 = param2;

    m_shift = shift;

//  m_par = NULL;

  m_table = table;

  m_resizing_gutter2 = false;
  m_b = 80;
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_str_status = _T("");

  m_fun = fun_proc;
  m_param = param;

  m_strCaption = capt;

  if ( capt ) {
    m_strCaption = capt;
  }
  m_szSection = "PropertyGrid";
}

void CPropGridDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_GRID, m_ctrlGrid);
  DDX_Control(pDX, IDC_STATUS, m_status);
  DDX_Text(pDX, IDC_STATUS, m_str_status);
}

BEGIN_MESSAGE_MAP(CPropGridDlg, CDialog)
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_WM_DESTROY()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_SYSCOMMAND()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
  ON_MESSAGE(WM_PG_FOCUSCHANGED, OnFocusChanged)

  ON_BN_CLICKED(IDC_A, OnBnClickedA)
  ON_BN_CLICKED(IDC_Z, OnBnClickedZ)
  ON_BN_CLICKED(IDC_D, OnBnClickedD)

  ON_BN_CLICKED(IDC_COLLAPSE_ALL, OnBnClickedCollapseAll)
  ON_BN_CLICKED(IDC_EXPAND_ALL, OnBnClickedExpandAll)
  ON_BN_CLICKED(IDC_CUSTOM_COLORS, OnBnClickedCustomColors)
  ON_BN_CLICKED(IDC_CATEGORIZED, OnBnClickedCategorized)
  ON_BN_CLICKED(IDC_ALPHABETICAL, OnBnClickedAlphabetical)
  ON_BN_CLICKED(IDC_NOSORT, OnBnClickedNosort)

  ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
  
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CPropGridDlg::OnMove(int x, int y) 
{
  CDialog::OnMove(x, y);
}

void CPropGridDlg::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);

  CRect sz, sz1, sz2, sz3;

  GetClientRect(sz);

  sz.bottom -= 40;

  sz.DeflateRect(1,1);

  m_sz = sz;

  sz2 = sz;
  sz3 = sz;

double getScreenMas();

  double mas = getScreenMas();


  sz2.top = 30*mas;
  sz2.bottom -= m_b;
  
  sz3.top = sz2.bottom;

  sz2.DeflateRect(1,1);
  sz3.DeflateRect(1,1);


  HDWP hDwp = BeginDeferWindowPos(2);
//  hDwp = DeferWindowPos(hDwp, m_grid.m_hWnd, NULL, 0, 0, cx, cy-40, SWP_NOZORDER | SWP_NOZORDER);

  if (::IsWindow(m_ctrlGrid.m_hWnd) )
//      m_ctrlGrid.MoveWindow(sz2);
      hDwp = DeferWindowPos(hDwp, m_ctrlGrid.m_hWnd, NULL, sz2.left, sz2.top, sz2.Width(), sz2.Height(), SWP_NOZORDER | SWP_NOZORDER);

  if (::IsWindow(m_status.m_hWnd) )
//      m_status.MoveWindow(sz3);
      hDwp = DeferWindowPos(hDwp, m_status.m_hWnd, NULL, sz3.left, sz3.top, sz3.Width(), sz3.Height(), SWP_NOZORDER | SWP_NOZORDER);

  CRect rect;

  for (CWnd *wnd = GetTopWindow(); wnd; wnd = wnd->GetNextWindow()) {
      int id = wnd->GetDlgCtrlID();
//      if (id == IDOK || id == IDCANCEL || id == IDC_COPY) {
//      if (id == IDOK || id == IDCANCEL || id == IDC_COPY) {


      if (id != IDC_ALPHABETICAL && id != IDC_CATEGORIZED && id != IDC_COLLAPSE_ALL && id != IDC_CUSTOM_COLORS && id != IDC_EXPAND_ALL && id != IDC_GRID && id != IDC_NOSORT && id != IDC_STATIC && id != IDC_STATUS ) {
          wnd->GetWindowRect(rect);
          ScreenToClient(&rect);
          hDwp = DeferWindowPos(hDwp, wnd->m_hWnd, NULL, rect.left, cy - rect.Height() - 5, -1, -1, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
      }
  }
  EndDeferWindowPos(hDwp);
}

// CPropGridDlg message handlers


HITEM CPropGridDlg::addButton(HSECTION hs, const char *label, const char *fun, int id)
{
  bool editable = true;
  bool undefined = false;
  CString lb = label;

  HITEM hi = -1;
  hi = m_ctrlGrid.AddIntegerItem(hs, lb, (long)id, "", editable, undefined);
  m_ctrlGrid.SetItemFun(hi, CString(fun));
  m_ctrlGrid.SetItemButtom(hi);

  return hi;
}



HITEM CPropGridDlg::addDataGrid(HSECTION hs, CGridData &dg)
{
  CString sv = CCrack::strVARIANT2(dg.value);

//  CString lb0;
//  lb0.Format("%s %d %s", dg.table, dg.id, dg.label);
//  CString lb(lb0);

  m_ctrlGrid.SetName(m_table);

  CString lb(dg.label);

  bool undefined = false;

  if (dg.value.vt == VT_NULL) undefined = true;
  bool editable = true;

  CString frm = "";
  
  double dv = 0;
  HITEM hi = -1;

  switch (dg.typ) {
    case adEmpty :
    case adSmallInt :
      hi = m_ctrlGrid.AddIntegerItem(hs, lb, (long)V_I2(&dg.value), "", editable, undefined);
      break;
    case adTinyInt :
      hi = m_ctrlGrid.AddBoolItem(hs, lb, (bool)V_I2(&dg.value), editable, undefined);
      break;
    case adInteger :
      hi = m_ctrlGrid.AddIntegerItem(hs, lb, (long)V_I4(&dg.value), "", editable, undefined);
      break;
    case adBigInt :
    case adUnsignedTinyInt :
    case adUnsignedSmallInt :
    case adUnsignedInt :
    case adUnsignedBigInt :
    case adSingle :
    case adDouble :
           if (dg.value.vt == VT_R4) dv = (double)V_R4(&dg.value);
      else if (dg.value.vt == VT_R8) dv = (double)V_R8(&dg.value);

      if (dg.NumericScale != 255 && dg.NumericScale != 0) {
        frm.Format(_T("%%.%dlf"), dg.NumericScale);
      }

      hi = m_ctrlGrid.AddDoubleItem(hs, lb, dv, (const char*)frm, editable, undefined);
      break;
    case adCurrency :
        {
        COleCurrency cy(dg.value);

        hi = m_ctrlGrid.AddCurrencyItem(hs, lb, cy, "%%.2f");

/*

//        CString s = cy.Format();
        hi = m_ctrlGrid.AddStringItem(hs, lb, s);
        
        m_ctrlGrid.SetItemLength(hi, 20);

        CItem* pItem = FindItem(it);
        if (pItem) pItem->m_options.push_back(format);
*/


        //                dv = (double)V_I4(&dg.value) / 10000.;
//            hi = m_ctrlGrid.AddDoubleItem(hs, lb, dv, "%.2lf", editable, undefined);
        }
        break;
    case adDecimal :
    case adNumeric :
      break;
    case adBoolean :
      hi = m_ctrlGrid.AddBoolItem(hs, lb, (bool) V_BOOL(&dg.value), editable, undefined);
    case adError :
    case adUserDefined :
    case adVariant :
    case adIDispatch :
    case adIUnknown :
    case adGUID :
      break;
    case adDate :
    case adDBDate :
    case adDBTime :
    case adDBTimeStamp :
      hi = m_ctrlGrid.AddDateItem(hs, lb, COleDateTime(dg.value), "%d.%m.%Y", editable, undefined);
      break;
    case adBSTR :
    case adChar :
    case adVarChar :
    case adLongVarChar :
    case adWChar :
    case adVarWChar :
    case adLongVarWChar :

      if (dg.typ == adLongVarWChar || dg.typ == adLongVarChar || dg.DefinedSize == -1) {
        hi = m_ctrlGrid.AddTextItem(hs, lb, CString(sv));
      }
      else {
        hi = m_ctrlGrid.AddStringItem(hs, lb, CString(sv));
      }
      break;

    case adBinary :
    case adVarBinary :
    case adLongVarBinary :
    case adChapter :
    case adFileTime :
    case adPropVariant :
    case adVarNumeric :
//      case adArray :
    default:
//        m_ctrlGrid.AddStringItem(hs, CString(dg.label), CString(sv));
      break;
  }


  if (hi == (HITEM)-1) {
    hi = m_ctrlGrid.AddStringItem(hs, lb, CString("??????????????????"));
  }
  else {
   m_ctrlGrid.SetItemName(hi, (CString) dg.name);
    if (dg.typ == adCurrency) {
        m_ctrlGrid.SetItemLength(hi, 20);
    }
    else {
        m_ctrlGrid.SetItemLength(hi, dg.DefinedSize);
    }
  }

  return hi;
}

HSECTION CPropGridDlg::addSection(const char *tn, bool collapsed, int number)
{
  HSECTION hs = m_ctrlGrid.AddSection(tn, collapsed, -1, number);
  return hs;
}

#if 0
BOOL CPropGridDlg::Populate(const char *tn, list<CGridData> &lst)
{
  if (lst.size() == 0) return FALSE;
  
  // basic items
  HSECTION hs = m_ctrlGrid.AddSection(tn, false, -1);

  list<CGridData>::const_iterator it = lst.begin();

  for (; it != lst.end(); it++) {
    CGridData dg = *it;
    addDataGrid(hs, dg);
  }
  Invalidate();

/*

  // basic items
  HSECTION hs = m_ctrlGrid.AddSection("Первый пункт");
  m_ctrlGrid.AddStringItem(hs, "Тут что-то такое", "И тут что-то такое");

  
  hs = m_ctrlGrid.AddSection("Basic Items");
  m_ctrlGrid.AddStringItem(hs, "String", "A single line CString item");
  m_ctrlGrid.AddTextItem(hs, "Text", "A multi line text item.\r\nSecond line...");
  m_ctrlGrid.AddStringItem(hs, "Disabled item", "A disabled item", false);
  m_ctrlGrid.AddIntegerItem(hs, "Integer", 10);
  m_ctrlGrid.AddIntegerItem(hs, "Formatted Integer", 8, "%d inches");
  m_ctrlGrid.AddDoubleItem(hs, "Double", 7.33);
  m_ctrlGrid.AddDoubleItem(hs, "Formatted Double", 10.25, "%gmm");
  m_ctrlGrid.AddBoolItem(hs, "Boolean", true);

  // custom items
  hs = m_ctrlGrid.AddSection("Custom Items");
  m_ctrlGrid.AddCustomItem(hs, "Custom In-place Edit", &m_item_serial);
  m_ctrlGrid.AddCustomItem(hs, "Custom Modal Edit", &m_item_rect);
  m_ctrlGrid.AddCustomItem(hs, "Ccustom Dropdown Edit", &m_item_tree);
  m_ctrlGrid.AddCustomItem(hs, "Full Custom Edit", &m_item_gradient);

  // more items inserted before the custom ones
  hs = m_ctrlGrid.AddSection("More Items", true, hs);

  // a combo
  vector<CString> v;
  v.push_back("English");
  v.push_back("French");
  v.push_back("German");
  v.push_back("Spanish");
  m_ctrlGrid.AddComboItem(hs, "Combo", v, 2, true);

  // some "simple"    
  m_ctrlGrid.AddDateItem(hs, "Date", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddDateItem(hs, "Formatted Date", COleDateTime::GetCurrentTime(), "%A, %d %B %Y");
  m_ctrlGrid.AddDateTimeItem(hs, "Date Time", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddColorItem(hs, "Color", RGB(255,156,12));
  m_ctrlGrid.AddFileItem(hs, "File", "C:\\AUTOEXEC.BAT", "All Files (*.*)|*.*||");
  m_ctrlGrid.AddFolderItem(hs, "Folder", "C:\\", "Select a folder");

  // a font
  LOGFONT lf;
  CFont font;
  font.CreatePointFont(80, "Tahoma");
  font.GetLogFont(&lf);
  m_ctrlGrid.AddFontItem(hs, "Font", lf, true);

  // customization
  hs = m_ctrlGrid.AddSection("Grid customization");
  m_hItemShade = m_ctrlGrid.AddBoolItem(hs, "Shade titles", m_ctrlGrid.GetShadeTitles());
  m_hItemLines = m_ctrlGrid.AddBoolItem(hs, "Draw lines", m_ctrlGrid.GetDrawLines());
  m_hItemGutter = m_ctrlGrid.AddBoolItem(hs, "Draw gutter", m_ctrlGrid.GetDrawGutter());
  m_hItemFocusDis = m_ctrlGrid.AddBoolItem(hs, "Focus disabled items", m_ctrlGrid.GetFocusDisabled());

  // stuff
  m_ctrlGrid.SetOkCancelStrings("OK", "Annuler");
  m_ctrlGrid.SetDateTimeStrings("Date", "Heure");

*/
  // 
//  ((CButton*)GetDlgItem(IDC_CATEGORIZED))->SetCheck(1);

  return TRUE;
}

#endif


static int n_prop_dlg = 0;
static int d_bx0 = 40, d_by0 = 100;
static int d_bx = d_bx0, d_by = d_by0;


BOOL CPropGridDlg::OnInitDialog()
{
//  m_bModeless = FALSE;
//  m_nFlags |= WF_CONTINUEMODAL;

  CDialog::OnInitDialog();

  m_ctrlGrid.SetName(m_table);

  m_szSection.Format("PropGrid\\%s", m_table);


//  m_bModeless = TRUE;
//  m_nFlags &= ~WF_CONTINUEMODAL;

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon


  int col_BackColor=0x00CCFFFF;
  int col_BackColorBkg=0x009FB8C8;
  int col_BackColorFixed=0x006FB7FF;
  int col_BackColorSel=0x0000000D;
  int col_ForeColor=0x80000008;
  int col_ForeColorFixed=0x80000012;
  int col_ForeColorSel=0x8000000E;
  int col_GridColor=0x00C0C0C0;
  int col_GridColorFixed=0x00000000;

/*
  m_ctrlGrid.SetBackColor(col_BackColor);
  m_ctrlGrid.SetShadeColor(col_GridColor);
  m_ctrlGrid.SetTextColor(col_ForeColor);
  m_ctrlGrid.SetTitleColor(col_ForeColorFixed);
*/

  ((CButton*)GetDlgItem(IDC_CATEGORIZED))->SetCheck(1);

  int bx = 100, by = 100;

  int gutter_width = AfxGetApp()->GetProfileInt(m_szSection, "gutter_width", 200);
  m_ctrlGrid.SetGutterWidth(gutter_width);



//  m_resizing_gutter2 = AfxGetApp()->GetProfileInt(m_szSection, "resizing_gutter2", 500);

  d_bx0 = AfxGetApp()->GetProfileInt(m_szSection, "grid_prop_bx", d_bx0);
  d_by0 = AfxGetApp()->GetProfileInt(m_szSection, "grid_prop_by", d_by0);


  int cx = AfxGetApp()->GetProfileInt(m_szSection, "grid_prop_cx", 500);
  int cy = AfxGetApp()->GetProfileInt(m_szSection, "grid_prop_cy", 600);

  if (n_prop_dlg == 0) {
    d_bx = d_bx0; d_by = d_by0;
  }

  if (cx > 0 && cy > 0) {
//    SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER|SWP_NOMOVE);
    SetWindowPos(&wndTopMost,d_bx,d_by,cx,cy,SWP_NOZORDER);

    CWnd * wnd = GetParent();
    if (wnd) {
      CRect rc;
      GetClientRect(&rc);

      if (m_shift) {
        d_bx += 40;
        d_by += 20;

        if (d_bx > rc.Width()) d_bx = d_bx0;
        if (d_by > rc.Height()) d_by = d_by0;
      }
    }
  }

  SetWindowText(m_strCaption);


  CMenu* pSysMenu = GetSystemMenu(FALSE);
  pSysMenu->AppendMenu(MF_SEPARATOR);

  pSysMenu->AppendMenu(MF_STRING, ID_FONT, _T("Шрифт"));
  pSysMenu->AppendMenu(MF_STRING, ID_COPY_STR, _T("Копировать структуру таблицы в буфер"));

  CString ss = m_table;
  ss.MakeLower();

  if (ss == "pipesections") {
      pSysMenu->AppendMenu(MF_STRING, ID_COPY_HPS, _T("Копировать данные из Участков теплопроводов"));
      pSysMenu->AppendMenu(MF_STRING, ID_COPY_PTS, _T("Установить по умолчанию максимальные значения ПТС"));
  }

  m_status.SetFont(m_ctrlGrid.GetFontNormal());

  n_prop_dlg++;

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPropGridDlg::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPropGridDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CPropGridDlg::OnBnClickedOk()
{
  //CString strValue;
  //if (m_ctrlGrid.GetItemValue(m_hItem, strValue))
  //    AfxMessageBox(strValue.c_str(), MB_OK|MB_ICONINFORMATION);
  OnOK();
}

LRESULT CPropGridDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
/*
  if (wParam == m_hItemLines)
  {
    bool draw_lines;
    m_ctrlGrid.GetItemValue(m_hItemLines, draw_lines);
    m_ctrlGrid.SetDrawLines(draw_lines);
    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
  }
  else if (wParam == m_hItemShade)
  {
    bool shade_titles;
    m_ctrlGrid.GetItemValue(m_hItemShade, shade_titles);
    m_ctrlGrid.SetShadeTitles(shade_titles);
  }
  else if (wParam == m_hItemGutter)
  {
    bool draw_gutter;
    m_ctrlGrid.GetItemValue(m_hItemGutter, draw_gutter);
    m_ctrlGrid.SetDrawGutter(draw_gutter);
  }
  else if (wParam == m_hItemFocusDis)
  {
    bool focus_disabled;
    m_ctrlGrid.GetItemValue(m_hItemFocusDis, focus_disabled);
    m_ctrlGrid.SetFocusDisabled(focus_disabled);
  }
*/
  return 0;
}

LRESULT CPropGridDlg::OnFocusChanged(WPARAM wParam, LPARAM lParam)
{
  int id = wParam;

  m_str_status = "";

  if (lParam) {
    m_str_status = (LPCSTR)lParam;
    m_str_status.Replace("\n", "\r\n");
  }

  UpdateData(FALSE);
  return 0;
}


void CPropGridDlg::OnBnClickedCollapseAll()
{
  m_ctrlGrid.ExpandCollapse();
}

void CPropGridDlg::OnBnClickedExpandAll()
{
  m_ctrlGrid.ExpandCollapse();
}

void CPropGridDlg::OnBnClickedCustomColors()
{
  CButton* pBtn = (CButton*) GetDlgItem(IDC_CUSTOM_COLORS);
  if (pBtn->GetCheck() == 0)
  {
    m_ctrlGrid.SetBackColor(GetSysColor(COLOR_WINDOW));
    m_ctrlGrid.SetShadeColor(GetSysColor(COLOR_3DFACE));
    m_ctrlGrid.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
    m_ctrlGrid.SetTitleColor(GetSysColor(COLOR_WINDOWTEXT));
  }
  else
  {
    m_ctrlGrid.SetBackColor(RGB(0xFF, 0xFF, 0xE0));
    m_ctrlGrid.SetShadeColor(RGB(0,187,94));
    m_ctrlGrid.SetTextColor(RGB(0,0,192));
    m_ctrlGrid.SetTitleColor(RGB(255,255,255));
  }
}

void CPropGridDlg::OnBnClickedCategorized()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_CATEGORIZED);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(TRUE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(TRUE);
}

void CPropGridDlg::OnBnClickedAlphabetical()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_ALPHABETICAL);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(FALSE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(FALSE);
}

void CPropGridDlg::OnBnClickedNosort()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_NOSORT);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(FALSE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(FALSE);
}

void CPropGridDlg::OnDestroy() 
{
  WINDOWPLACEMENT wpi;
  GetWindowPlacement(&wpi); 

  int gutter_width = m_ctrlGrid.GetGutterWidth();
  AfxGetApp()->WriteProfileInt(m_szSection, "gutter_width", gutter_width);
  
  AfxGetApp()->WriteProfileInt(m_szSection, "grid_prop_bx", wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(m_szSection, "grid_prop_by", wpi.rcNormalPosition.top);
  AfxGetApp()->WriteProfileInt(m_szSection, "grid_prop_cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(m_szSection, "grid_prop_cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);

  CDialog::OnDestroy();

  n_prop_dlg--;

  delete this;
}

void CPropGridDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  CDialog::OnLButtonDown(nFlags, point);
}

BOOL CPropGridDlg::PreTranslateMessage(MSG* pMsg) 
{
//  if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
//    if (pMsg->wParam != VK_RETURN) return TRUE;
//  }

  POINT point = pMsg->pt;
  ScreenToClient(&point);

  int m_b2 = m_sz.Height()-m_b;

  if ( WM_LBUTTONDOWN == pMsg->message )
  {
    if (abs(point.y-m_b2)<3)
    {
      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
      m_resizing_gutter2 = true;
      m_ptLast = point;
      SetCapture();
     }
  }
  else if ( WM_MOUSEMOVE == pMsg->message )
  {
    if (m_resizing_gutter2)
    {
      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));

      if (m_b2 != point.y && point.y > 45 && point.y < m_sz.Height() -10) {

        m_b = m_sz.Height()-point.y;

        CRect sz2 = m_sz;
        CRect sz3 = m_sz;

        sz2.top = 40;
        sz2.bottom = sz3.top = point.y;

        sz2.DeflateRect(2,2);
        sz3.DeflateRect(2,2);
        if (::IsWindow(m_ctrlGrid.m_hWnd) && ::IsWindow(m_status.m_hWnd) ) {
          m_ctrlGrid.SetRedraw(FALSE);
          m_status.SetRedraw(FALSE);
    
          m_ctrlGrid.MoveWindow(sz2);
          m_status.MoveWindow(sz3);

          m_ctrlGrid.SetRedraw(TRUE);
          m_status.SetRedraw(TRUE);
        }
        Invalidate();
      }
    }
    else {
      if (abs(point.y-m_b2)<3)::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
      else ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    }
  }
  
  else if ( WM_LBUTTONUP == pMsg->message )
  {
    if (m_resizing_gutter2)
    {
      ReleaseCapture();
      m_resizing_gutter2 = false;
      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    }
  }

  return CDialog::PreTranslateMessage(pMsg);
}

void CPropGridDlg::OnOK()
{
    CString str;
    UpdateData(TRUE);

    m_ctrlGrid.save_profile();

    list<CItemChanged> lst;
    bool ch = m_ctrlGrid.getChanges(lst);

    if (!m_ctrlGrid.check(ch, false)) return;

    ch = m_ctrlGrid.getChanges(lst);

    if (ch) {
       if (m_fun && lst.size() > 0) {
           if (!m_fun(lst, m_param, m_send_value)) {
               return;
           }
       }
    }

    CWnd *wnd = m_par;
    if (wnd) {
//        wnd->SendMessage(m_send_value, (WPARAM)m_typ_of_p, (LPARAM)m_param2);
        wnd->SendMessage(m_send_value, (WPARAM)m_typ_of_p, (LPARAM)m_param);
    }

    DestroyWindow();
}

void CPropGridDlg::OnCancel() 
{
    m_ctrlGrid.save_profile();

//    if (!m_ctrlGrid.check(false, true)) return;

    CWnd *wnd = m_par;
    if (wnd) {
//        wnd->SendMessage(WM_USER_ADO_NOUPDATED, (WPARAM)m_typ_of_p, (LPARAM)m_param2);
        wnd->SendMessage(WM_USER_ADO_NOUPDATED, (WPARAM)m_typ_of_p, (LPARAM)m_param);
    }

    DestroyWindow();
}

void CPropGridDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    LOGFONT lf;
    switch (nID) {
    case ID_FONT:
    {
        CFont *font = m_ctrlGrid.GetFontNormal();
        if (font) {
            font->GetLogFont(&lf);
        }
        lf.lfCharSet = DEFAULT_CHARSET;

        CFontDialog fd(&lf, CF_EFFECTS | CF_SCREENFONTS);

        if (fd.DoModal() == IDOK) {
            fd.GetCurrentFont(&lf);
            SaveFont(_T("PropGridFont"), lf, fd.GetColor());
            m_ctrlGrid.InitFont();
            m_status.SetFont(m_ctrlGrid.GetFontNormal());
        }
        break;
    }
    case ID_COPY_STR:
        m_ctrlGrid.Print_struct();
        break;

    case ID_COPY_HPS:
        m_ctrlGrid.copyHps();
        break;

    case ID_COPY_PTS:
        m_ctrlGrid.copyPts();
        break;


    default:
        CDialog::OnSysCommand(nID, lParam);
        break;
    }
}
 
void CPropGridDlg::OnBnClickedCopy()
{
    m_ctrlGrid.Print();
}


void CPropGridDlg::OnBnClickedA()
{
  m_ctrlGrid.prop_fun2(IDC_A);
}

void CPropGridDlg::OnBnClickedZ()
{
  m_ctrlGrid.prop_fun2(IDC_Z);
}

void CPropGridDlg::OnBnClickedD()
{
  m_ctrlGrid.prop_fun2(IDC_D);
}
