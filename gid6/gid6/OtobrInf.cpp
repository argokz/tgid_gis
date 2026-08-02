// OtobrInf.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OtobrInf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtobrInf dialog


COtobrInf::COtobrInf(CWnd* pParent, BOOL *bIsInfo)
  : CDialog(COtobrInf::IDD, pParent)
{
  m_bIsInfo = bIsInfo;

  //{{AFX_DATA_INIT(COtobrInf)
  m_diametr = m_bIsInfo[3];
  m_dlina = m_bIsInfo[2];
  m_napor = m_bIsInfo[0];
  m_objem = m_bIsInfo[4];
  m_rashod = m_bIsInfo[1];
  m_temp = m_bIsInfo[5];
  m_ras_pot = m_bIsInfo[6];
  //}}AFX_DATA_INIT

}



void COtobrInf::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COtobrInf)
  DDX_Check(pDX, ID_DIAMETR, m_diametr);
  DDX_Check(pDX, ID_DLINA, m_dlina);
  DDX_Check(pDX, ID_NAPOR, m_napor);
  DDX_Check(pDX, ID_OBJEM, m_objem);
  DDX_Check(pDX, ID_RASHOD, m_rashod);
  DDX_Check(pDX, ID_TEMP, m_temp);
  DDX_Check(pDX, ID_RAS_POT, m_ras_pot);
  DDX_Check(pDX, ID_RASPOLAG, m_raspolag);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtobrInf, CDialog)
  //{{AFX_MSG_MAP(COtobrInf)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtobrInf message handlers

void COtobrInf::OnOK() 
{
  UpdateData(TRUE);

  m_bIsInfo[0] = m_napor ;
  m_bIsInfo[1] = m_rashod;
  m_bIsInfo[2] = m_dlina ;
  m_bIsInfo[3] = m_diametr;
  m_bIsInfo[4] = m_objem ;
  m_bIsInfo[5] = m_temp ;
  m_bIsInfo[6] = m_ras_pot ;
  m_bIsInfo[7] = m_raspolag ;
  
  CDialog::OnOK();
}
