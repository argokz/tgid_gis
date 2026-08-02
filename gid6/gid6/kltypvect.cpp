// KlTyp2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "KlTypVect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKlTypVect dialog


CKlTypVect::CKlTypVect(CWnd* pParent)
  : CDialog(CKlTypVect::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKlTypVect)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_is_white = FALSE;
}


void CKlTypVect::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKlTypVect)
  DDX_Control(pDX, 101, m_rastr);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKlTypVect, CDialog)
  //{{AFX_MSG_MAP(CKlTypVect)
  ON_BN_CLICKED(IDC_LINE, OnLine)
  ON_BN_CLICKED(IDC_RING, OnRing)
  ON_BN_CLICKED(IDC_ARC, OnArc)
  ON_BN_CLICKED(IDC_DEL, OnDel)
  ON_BN_CLICKED(IDC_CIRCLE, OnCircle)
  ON_BN_CLICKED(IDC_POLYGON, OnPolygon)
  ON_BN_CLICKED(IDC_COLOR, OnColor)
  ON_BN_CLICKED(IDC_CLEAR, OnClear)
  ON_BN_CLICKED(IDC_RESTORE, OnRestore)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKlTypVect message handlers

BOOL CKlTypVect::PreTranslateMessage(MSG* pMsg) 
{
  return CDialog::PreTranslateMessage(pMsg);
}


void CKlTypVect::OnLine()
{
  m_rastr.SetRegim(TIP_LINE);
} 

void CKlTypVect::OnRing()
{
  m_rastr.SetRegim(TIP_RING);
} 

void CKlTypVect::OnArc()
{
  m_rastr.SetRegim(TIP_ARC);
} 

void CKlTypVect:: OnCircle()
{
  m_rastr.SetRegim(TIP_CIRCLE);
}

void CKlTypVect:: OnPolygon()
{
  m_rastr.SetRegim(TIP_POLYGON);
}

void CKlTypVect:: OnDel()
{
  m_rastr.SetRegim(TIP_DEL);
}

void CKlTypVect:: OnRestore()
{
  m_rastr.restore();
}



void CKlTypVect:: OnColor()
{
    m_is_white = !m_is_white;
    m_rastr.setColor(m_is_white);
    if (m_is_white) {
        AfxMessageBox("÷вет белый", MB_OK|MB_ICONINFORMATION);
    }
    else {
        AfxMessageBox("÷вет черный", MB_OK|MB_ICONINFORMATION);
    }
}

void CKlTypVect:: OnClear()
{
  m_rastr.clear();
}



void CKlTypVect::OnOK() 
{
  UpdateData(TRUE);

  m_rastr.save();
  
  CDialog::OnOK();
}

