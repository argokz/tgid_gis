// KlTyp.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "win.h"
#include "KlTyp.h"
#include "Klassif.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKlTyp dialog

extern Prim *rs;


CKlTyp::CKlTyp(CWnd* pParent /*=NULL*/)
  : CDialog(CKlTyp::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKlTyp)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CKlTyp::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKlTyp)
  DDX_Control(pDX, 101, m_rastr);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKlTyp, CDialog)
  //{{AFX_MSG_MAP(CKlTyp)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKlTyp message handlers

BOOL CKlTyp::PreTranslateMessage(MSG* pMsg) 
{
  return CDialog::PreTranslateMessage(pMsg);
}


void CKlTyp::OnOK() 
{
  FILE *f;
  int i;

  f = fopenexe("kls/flibrt.ras", "wb");
  for (i = 0; i < 256; i++) {
    putc(rs[i].x, f);
    putc(rs[i].y, f);
    fwrite(rs[i].s,32,1,f);
  }
  fclose(f);
  
  CDialog::OnOK();
}
