// TuItogInit.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "TuItogInit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTuItogInit dialog


CTuItogInit::CTuItogInit(int y, CWnd* pParent /*=NULL*/)
  : CDialog(CTuItogInit::IDD, pParent)
{
  //{{AFX_DATA_INIT(CTuItogInit)
  m_coef24 = 2.4;
  m_y1 = y-4;
  m_y2 = y;
  m_y_ist = y;
  m_date = COleDateTime::GetCurrentTime();
  //}}AFX_DATA_INIT

  CString ss = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");
  m_coef24 = atof(ss);
}

void CTuItogInit::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTuItogInit)
  DDX_Control(pDX, IDC_SPIN3, m_s3);
  DDX_Control(pDX, IDC_SPIN2, m_s2);
  DDX_Control(pDX, IDC_SPIN1, m_s1);
  DDX_Text(pDX, IDC_EDIT3, m_y1);
  DDV_MinMaxInt(pDX, m_y1, 2000, 2030);
  DDX_Text(pDX, IDC_EDIT4, m_y2);
  DDV_MinMaxInt(pDX, m_y2, 2000, 2030);
  DDX_Text(pDX, IDC_EDIT2, m_y_ist);
  DDV_MinMaxInt(pDX, m_y_ist, 2000, 2030);
  DDX_DateTimeCtrl(pDX, IDC_DATE, m_date);
  DDX_Text(pDX, IDC_COEF24, m_coef24);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTuItogInit, CDialog)
  //{{AFX_MSG_MAP(CTuItogInit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTuItogInit message handlers


BOOL CTuItogInit::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_s1.SetRange(2000, 2030);
  m_s2.SetRange(2000, 2030);
  m_s3.SetRange(2000, 2030);
  // TODO: Add extra initialization here


  UpdateData(TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CTuItogInit::OnOK() 
{
  UpdateData(TRUE);

  CString ss;
  ss.Format("%g", m_coef24);

  AfxGetApp()->WriteProfileString(szSection, "coef24", ss);

  CDialog::OnOK();
}

