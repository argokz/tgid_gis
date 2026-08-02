// MMenu.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "MMenu.h"
#include "InputDia.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMMenu dialog


//UINT nIDTemplate
//CMMenu::CMMenu(CWnd* pParent, int isEdit)
//  : CDialog(isEdit ? CMMenu::IDD2 : CMMenu::IDD, pParent)

  
CMMenu::CMMenu(CWnd* pParent, UINT nIDTemplate, const char *capt)
  : CDialog(nIDTemplate, pParent)
//  : CDialog(isEdit ? CMMenu::IDD2 : CMMenu::IDD, pParent)
{
  //{{AFX_DATA_INIT(CMMenu)
  m_Index = -1;
  //}}AFX_DATA_INIT

  m_strCaption = "";
  if ( capt ) {
    m_strCaption = capt;
  }

  n_multy = 1;

  m_text = "";
}


void CMMenu::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMMenu)
  DDX_Control(pDX, IDC_EDIT, m_Edit);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_LBIndex(pDX, IDC_LIST, m_Index);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMMenu, CDialog)
  //{{AFX_MSG_MAP(CMMenu)
  ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_BN_CLICKED(IDC_COPY, OnCopy)
//  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMenu message handlers


BOOL CMMenu::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  if (m_text != "") {
    CStatic *stat = (CStatic*) GetDlgItem(IDC_TEXT);

    if (stat) {
      stat->SetWindowText(m_text);
    }
  }

  for ( int i = 0; i < lst.GetSize(); i++ ) {
    m_List.AddString(lst[i]);
  }

  if (m_Index >= 0) {
    UpdateData(FALSE);
  }


  SetWindowText(m_strCaption);


  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CMMenu::CleanList()
{
//    UpdateData(TRUE);

    m_List.ResetContent();

    lst.RemoveAll();
    map_menu.clear();
    map_menu2.clear();
}



void CMMenu::ReadData()
{
  UpdateData(TRUE);
  int n = m_List.GetCurSel();
  m_strText = "";
  if (n >= 0) m_List.GetText(n, m_strText);

  m_List.ResetContent();

  for ( int i = 0; i < lst.GetSize(); i++ ) {
    m_List.AddString(lst[i]);
  }
  m_List.SelectString (-1, m_strText);

}




void CMMenu::OnDblclkList()
{
  ReadData();

  EndDialog(IDOK);
}


void CMMenu::OnOK() 
{
  OnDblclkList();

  CDialog::OnOK();
}

void CMMenu::OnChangeEdit() 
{
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.
  
  // TODO: Add your control notification handler code here
  
  int i;
  CString str, pic;
  char cpic[256];

  UpdateData(TRUE);


  m_Edit.GetLine(0, cpic, 255);
  cpic[m_Edit.LineLength(0)] = 0;
  pic = cpic;
  pic.MakeUpper();

  m_List.ResetContent();

   for ( i = 0; i < lst.GetSize(); i++ ) {
     CString str = lst[i];
     str.MakeUpper();

     if (strstr(str, pic)) m_List.AddString(lst[i]);
   }

   m_List.SetCurSel(0);

/*
  
  for ( k = 0; k < m_List.GetCount(); k++ ) {
    m_List.GetText(k, str);
    str.MakeUpper();
    if (strstr(str, pic)) break;
  }
  if (k < m_List.GetCount()) m_List.SetCurSel(k);
*/
}

HBRUSH CMMenu::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
  pDC->SetBkMode(TRANSPARENT);
  return m_Brush;
}

BOOL CMMenu::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);

  UpdateData(TRUE);


  if ( nID == IDC_DBALL || nID == IDC_DEL || /*nID == IDC_EDIT || */nID == IDC_PASSWORD) {
    ReadData();

    if ( nID == IDC_DBALL) {
        CleanList();
    }

    EndDialog(nID);
  }

  if ( nID == IDC_DEL_TU) {
    ReadData();
    if (m_strText != "") {
      CString str;
      str.Format(_TR("”далить данные за %s год?"), m_strText);
      if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        EndDialog(nID);
      }
    }
  }
  if ( nID == IDC_ADD_TU) {
//    CString text = "";
//    text.Format("%d", y+1);
//    CInputDialog inD(this, "", _TR("¬ведите год"), text);

//    if (inD.DoModal() == IDOK) {
//    }

//    long year = atoi(inD.m_strInput);

    EndDialog(nID);
  }
  

  return CDialog::OnCommand(wParam, lParam);
}


void CMMenu::OnCopy() 
{
  int k;
  CString str, ss;

  for ( k = 0; k < m_List.GetCount(); k++ ) {
    m_List.GetText(k, ss);
    str += ss;
    str += "\r\n";
  }

  if (!OpenClipboard())
  {
    AfxMessageBox(_T("Failed to open clipboard"));
    return;
  }
  if (!::EmptyClipboard())
  {
    AfxMessageBox(_T("Failed to empty clipboard"));
    ::CloseClipboard();
    return;
  }


  // allocate a (shared) global memory block for the data
  HGLOBAL hTextData = ::GlobalAlloc(GMEM_DDESHARE, str.GetLength()*sizeof(TCHAR));
  if (hTextData == NULL) return;

  // now copy the strings, terminate each with CR/LF
  LPTSTR lpOut = (LPTSTR)::GlobalLock(hTextData);
  ASSERT(lpOut != NULL);

  lstrcpy(lpOut, str);

  if (hTextData == NULL || ::SetClipboardData(CF_TEXT, hTextData) == NULL)
  {
    AfxMessageBox(_T("Failed to set clipboard data"));
    ::CloseClipboard();
    return;
  }

  if (!::CloseClipboard())
    AfxMessageBox(_T("Failed to close clipboard"));
}


void CMMenu::Add(const char *text, void *v)
{
  lst.Add(text);
  if (v) {
    map_menu[CString(text)] = v;
    map_menu2[lst.GetSize()-1] = v;
  }
}

void * CMMenu::getV(const char *text)
{
  map<CString, void*>::const_iterator it;

  it = map_menu.find(CString(text));

  if (it != map_menu.end()) {
    return it->second;
  }
  return NULL;
}


void * CMMenu::getV()
{
  return getV(m_strText);
}


void * CMMenu::getV2()
{
  map<int, void*>::const_iterator it = map_menu2.find(m_Index);

  if (it != map_menu2.end()) {
    return it->second;
  }
  return NULL;
}

