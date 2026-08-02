// PrintFrDial.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "PrintFrDial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintFrDial dialog


CPrintFrDial::CPrintFrDial(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintFrDial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintFrDial)
	m_mas = _T("1000");
	m_paper = 0;
	m_orient = 0;
	//}}AFX_DATA_INIT
}


void CPrintFrDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintFrDial)
	DDX_CBString(pDX, IDC_MAS, m_mas);
	DDX_CBIndex(pDX, IDC_PAPER, m_paper);
	DDX_Radio(pDX, IDC_PORTRET, m_orient);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintFrDial, CDialog)
	//{{AFX_MSG_MAP(CPrintFrDial)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintFrDial message handlers
