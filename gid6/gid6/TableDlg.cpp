// VirtualDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "TableDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CTableDlg dialog


CTableDlg::CTableDlg(CWnd* pParent, int type_of_net, CString conn, CString db, CString q, CString tn, CString cap)
  : CDialog(CTableDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CTableDlg)
  //}}AFX_DATA_INIT

    m_type_of_net = type_of_net;
  m_connect = conn;
  m_db = db;
  m_q = q;
  m_tn = tn;
  m_cap = cap;

//  m_id = id;
  m_text = "";

  m_profile.Format("TableDlg\\%s", m_cap);
}

CTableDlg::~CTableDlg()
{
//  delete m_data;
}


BOOL CTableDlg::PreTranslateMessage(MSG* pMsg) 
{
  return CDialog::PreTranslateMessage(pMsg);
}


void CTableDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTableDlg)
//  DDX_Control(pDX, IDC_EDIT1, m_edit);
//  DDX_Control(pDX, IDC_LIST1, m_grid);
//  DDX_Text(pDX, IDC_EDIT1, m_find);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableDlg, CDialog)
  //{{AFX_MSG_MAP(CTableDlg)
//  ON_MESSAGE(WM_LDBL, OnDblClick)
  ON_WM_DESTROY()
  ON_WM_SIZE()
//  ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableDlg message handlers




BOOL CTableDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  SetWindowText(m_cap);

  m_grid.AttachGrid(this, 101);

  // bind the datasource to data, provide it with query and connection information
  m_pDSList = new _tagDSList;
  m_pDSList->m_dsADO = new CADOImpDatasource;



  m_pDSList->m_dsADO->OpenSQL(m_q, m_connect, m_type_of_net);
  m_pDSList->m_dsADO->SetTableName(m_db, m_tn);

  m_grid.SetUniformRowHeight(TRUE);

  m_grid.AddDataSource( m_pDSList->m_dsADO );
  m_grid.SetDefDataSource( m_pDSList->m_dsADO->GetID());
  m_grid.SetGridUsingDataSource( m_pDSList->m_dsADO->GetID());  
  m_grid.setName(m_cap, virt_data_unknown);

  int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
  int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
  int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
  int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

  if (cx > 0 && cy > 0) {
    SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
  }

  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}



void CTableDlg::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);

  if (!m_grid.m_hWnd) return;

  m_grid.MoveWindow( 0, 0, cx, cy );

//  Size();
}



LRESULT CTableDlg::OnDblClick(WPARAM wParam, LPARAM lParam)
{

  return 0;
}

void CTableDlg::OnOK() 
{
  UpdateData(TRUE);
  CDialog::OnOK();
}


void CTableDlg::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi); 

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);


    CDialog::OnDestroy();
//    if (!m_modal)
//      delete this; // Shown as non-modal, we'll clean up ourselves
}

