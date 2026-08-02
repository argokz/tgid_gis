// Input2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Input2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInput2 dialog


CInput2::CInput2(map <CString, CString> &map_ist, CWnd* pParent /*=NULL*/)
	: CDialog(CInput2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInput2)
	m_edit2 = "";
	m_n = -1;
	//}}AFX_DATA_INIT

  m_map_ist = map_ist;
}


void CInput2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInput2)
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_Text(pDX, IDC_EDIT2, m_edit2);
	DDV_MaxChars(pDX, m_edit2, 3);
	DDX_CBIndex(pDX, IDC_COMBO1, m_n);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInput2, CDialog)
	//{{AFX_MSG_MAP(CInput2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInput2 message handlers

BOOL CInput2::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
  map <CString, CString>::const_iterator it;
 
  it = m_map_ist.begin();

  for (; it != m_map_ist.end(); it++) {
    m_combo.AddString(it->first);
  }


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInput2::OnOK() 
{
  UpdateData(TRUE);
	
  m_edit1 = "";

  if (m_n >= 0) {
    m_combo.GetLBText(m_n, m_edit1 );
  }

	CDialog::OnOK();
}
