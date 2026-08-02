// PrintIni.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "PrintIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintIni dialog


CPrintIni::CPrintIni(CWnd* pParent, CFRect *rectPrint, CFRect *rectBorder, double *mas)
  : CDialog(CPrintIni::IDD, pParent)
{
  //{{AFX_DATA_INIT(CPrintIni)
  m_x1 = -0.01*rectPrint->bottom;
  m_x2 = -0.01*rectPrint->top;
  m_y1 = 0.01*rectPrint->left;
  m_y2 = 0.01*rectPrint->right;
  //}}AFX_DATA_INIT

  m_strMas.Format(_T("%g"), *mas);
  m_rectPrint = rectPrint;
  m_rectBorder = rectBorder;
  m_mas = mas;
}


void CPrintIni::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPrintIni)
  DDX_CBString(pDX, IDC_MAS, m_strMas);
  DDX_Text(pDX, IDC_X1, m_x1);
  DDX_Text(pDX, IDC_X2, m_x2);
  DDX_Text(pDX, IDC_Y1, m_y1);
  DDX_Text(pDX, IDC_Y2, m_y2);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintIni, CDialog)
  //{{AFX_MSG_MAP(CPrintIni)
  ON_COMMAND(ID_ALL, OnAll)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintIni message handlers

void CPrintIni::OnOK()
{
  // TODO: Add extra validation here

  UpdateData(TRUE);

  m_rectPrint->bottom = -m_x1*100;
  m_rectPrint->top    = -m_x2*100;
  m_rectPrint->left    = m_y1*100;
  m_rectPrint->right   = m_y2*100;

  _stscanf(m_strMas, _T("%lg"), m_mas);

  CDialog::OnOK();
}


void CPrintIni::OnAll()
{
  m_rectPrint->bottom = m_rectBorder->bottom;
  m_rectPrint->top    = m_rectBorder->top;
  m_rectPrint->left   = m_rectBorder->left;
  m_rectPrint->right  = m_rectBorder->right;


  m_x1 = -m_rectPrint->bottom/100;
  m_x2 = -m_rectPrint->top/100;
  m_y1 = m_rectPrint->left/100;
  m_y2 = m_rectPrint->right/100;


  UpdateData(FALSE);
}
