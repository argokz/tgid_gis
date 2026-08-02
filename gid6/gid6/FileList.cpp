// FileList.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "FileList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileList dialog


CFileList::CFileList(CWnd* pParent, set <CString> *set_db, int is_geo)
  : CDialog(CFileList::IDD, pParent)
{
  //{{AFX_DATA_INIT(CFileList)
  //}}AFX_DATA_INIT

  m_set_db = set_db;
  m_is_geo = is_geo;
}


void CFileList::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFileList)
  DDX_Control(pDX, IDC_LIST1, m_list);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileList, CDialog)
  //{{AFX_MSG_MAP(CFileList)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_DELETE, OnDelete)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileList message handlers

void CFileList::OnAdd()
{
  CString podp;

  if (m_is_geo) {
    podp = _TR("Файлы ArcMap (*.mdb)|*.mdb|Файлы ArcMap (*.shp)|*.shp|");
  }
  else {
    podp = _TR("Исходные файлы (*.mdb)|*.mdb");
  }


  CFileDialog fd(TRUE, _T("MDB"), NULL,
    OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT,
    podp, this);

  DWORD MAXFILE = 2562*10; //2562 is the max
  fd.m_ofn.nMaxFile = MAXFILE;
  char* pc = new char[MAXFILE];
  fd.m_ofn.lpstrFile = pc;
  fd.m_ofn.lpstrFile[0] = NULL;

  if (fd.DoModal() == IDOK) {
    POSITION pos = fd.GetStartPosition();

    while (pos) {
      CString str = fd.GetNextPathName( pos );
      if (m_list.FindString( -1, str) == LB_ERR) {
         m_list.AddString(str);
//         m_list.SetCheck(m_list.FindString( -1, str) , 1);
      }
    }
  }
}

void CFileList::OnDelete()
{
  int k;
  CString s;

  k = m_list.GetCurSel();
  if (k >= 0) {
    m_list.GetText(k, s);
    m_list.DeleteString(k);
    m_list.SetCurSel(max(0, k-1));
  }
}

BOOL CFileList::OnInitDialog()
{
  CDialog::OnInitDialog();

  int i = 0;
  set <CString>::const_iterator it = m_set_db->begin();
  for (; it != m_set_db->end(); ++it ) {
    m_list.AddString(*it);
//    m_list.SetCheck(i++, 1);
  }

  if (m_is_geo) {
    SetWindowText(_TR("Список цифровых карт"));
  }
  else {
    SetWindowText(_TR("Список дополнительных схем"));
  }


  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileList::OnOK()
{
  CString str;

  m_set_db->clear();

  for (int i = 0; i < m_list.GetCount(); i++) {
    m_list.GetText(i, str);
    m_set_db->insert(str);
  }


  CDialog::OnOK();
}
