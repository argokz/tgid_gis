// TranspDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "TranspDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranspDialog dialog


CTranspDialog::CTranspDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CTranspDialog::IDD, pParent)
{
  //{{AFX_DATA_INIT(CTranspDialog)
  m_slider = 0;
  //}}AFX_DATA_INIT
}


void CTranspDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTranspDialog)
  DDV_MinMaxSlider(pDX, m_slider, 0, 255);
  DDX_Slider(pDX, IDC_SLIDER1, m_slider);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranspDialog, CDialog)
  //{{AFX_MSG_MAP(CTranspDialog)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranspDialog message handlers
