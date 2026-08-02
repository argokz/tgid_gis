// OpcParam.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OpcParam.h"

#include "OpcTree.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpcParam dialog

extern COPCThread2 *pOPCThread;

void split(CString s, CString & s1, double &a)
{
  s1 = s;
  a = 1.;

  int k = s.Find("|");

  if (k >= 0) {
    s1 = s.Left(k);
    a = atof(s.Mid(k+1));
  }
}

CString merge(CString s1, double a)
{
  if (a == 1.) return s1;

  CString s;
  s.Format("%s|%g", s1, a);

  return s;
}


COpcParam::COpcParam(const CString &opc1, const CString &opc2, CWnd* pParent /*=NULL*/)
	: CDialog(COpcParam::IDD, pParent)
{

  m_nTimer = 0;
  m_pOPCThread = 0;

  
  //{{AFX_DATA_INIT(COpcParam)
	m_opc1 = opc1;
	m_opc2 = opc2;
	m_out1 = "";
	m_out2 = "";
	m_a1 = 1.0;
	m_a2 = 1.0;
	//}}AFX_DATA_INIT

  split(opc1, m_opc1, m_a1);
  split(opc2, m_opc2, m_a2);

}


void COpcParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpcParam)
	DDX_Text(pDX, IDC_EDIT1, m_opc1);
	DDX_Text(pDX, IDC_EDIT2, m_opc2);
	DDX_Text(pDX, IDC_EDIT_OUT1, m_out1);
	DDX_Text(pDX, IDC_EDIT_OUT2, m_out2);
	DDX_Text(pDX, IDC_A1, m_a1);
	DDX_Text(pDX, IDC_A2, m_a2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpcParam, CDialog)
	//{{AFX_MSG_MAP(COpcParam)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit)
	ON_EN_CHANGE(IDC_A1, OnChangeEdit)
	ON_EN_CHANGE(IDC_A2, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcParam message handlers

extern OPCServer *opc_server;


void COpcParam::OnButton1() 
{
  if (opc_server) {
    COpcTree opct(this, opc_server);
    if (opct.DoModal() == IDOK) {
      m_opc1 = opct.str;
      UpdateData(FALSE);
    }
  }
}

void COpcParam::OnButton2() 
{
  if (opc_server) {
    COpcTree opct(this, opc_server);
    if (opct.DoModal() == IDOK) {
      m_opc2 = opct.str;
      UpdateData(FALSE);
    }
  }
}

void COpcParam::OnOK() 
{
  UpdateData(TRUE);

  CString v1, v2;

  if (m_a1 != 1.) {
    m_opc1 = merge(m_opc1, m_a1);
  }
  if (m_a2 != 1.) {
    m_opc2 = merge(m_opc2, m_a2);
  }

  UpdateData(FALSE);
  
	CDialog::OnOK();
}

CString getOpcVar(const char *name, double a);


BOOL COpcParam::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  
  UpdateData(FALSE);

  m_nTimer = SetTimer(1, 1000, 0);

  m_pOPCThread = pOPCThread;

  if (m_pOPCThread) {
    m_pOPCThread->get_value(merge(m_opc1, m_a1));
    m_pOPCThread->get_value(merge(m_opc2, m_a2));
    m_pOPCThread->Tic();
  }
  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COpcParam::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

  if (m_pOPCThread) {
    m_pOPCThread->Tic();
    m_out1 = m_pOPCThread->get_value(merge(m_opc1, m_a1));
    m_out2 = m_pOPCThread->get_value(merge(m_opc2, m_a2));
  
    GetDlgItem(IDC_EDIT_OUT1)->SetWindowText(m_out1);
    GetDlgItem(IDC_EDIT_OUT2)->SetWindowText(m_out2);


//    UpdateData(FALSE);
  }
  
	CDialog::OnTimer(nIDEvent);
}

void COpcParam::OnClose() 
{
  if (m_pOPCThread) m_pOPCThread->Exit();

	CDialog::OnClose();
}

void COpcParam::OnChangeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    if (m_pOPCThread) {
      UpdateData(TRUE);

      m_pOPCThread->get_value(merge(m_opc1, m_a1));
      m_pOPCThread->get_value(merge(m_opc2, m_a2));
      m_pOPCThread->Tic();
//        m_pOPCThread->setLst(lst);
    }
	
}
