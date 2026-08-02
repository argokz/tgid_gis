// DopPodklNagr.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "DopPodklNagr.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDopPodklNagr dialog


CDopPodklNagr::CDopPodklNagr(CWnd* pParent, double dp_min, double g, double p1, double p2, double poteri, double qq, double t1, double t2)
  : CDialog(CDopPodklNagr::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDopPodklNagr)
  m_dp_min = dp_min; /// Верхняя 6 м
  m_g = g;
  m_p1 = p1;
  m_p2 = p2;
  m_poteri = poteri;  // Нижняя 1.5
  m_qq = qq;
  m_t1 = t1;
  m_t2 = t2;
	m_g2 = 0.0;
	//}}AFX_DATA_INIT
}


void CDopPodklNagr::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDopPodklNagr)
  DDX_Text(pDX, IDC_DP_MIN, m_dp_min);
  DDX_Text(pDX, IDC_G, m_g);
  DDX_Text(pDX, IDC_P1, m_p1);
  DDX_Text(pDX, IDC_P2, m_p2);
  DDX_Text(pDX, IDC_POTERI, m_poteri);
  DDX_Text(pDX, IDC_Q, m_qq);
  DDX_Text(pDX, IDC_T1, m_t1);
  DDX_Text(pDX, IDC_T2, m_t2);
	DDX_Text(pDX, IDC_G2, m_g2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDopPodklNagr, CDialog)
  //{{AFX_MSG_MAP(CDopPodklNagr)
	ON_BN_CLICKED(ID_RAS, OnRas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDopPodklNagr message handlers

BOOL CDopPodklNagr::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CDopPodklNagr::OnRas() 
{
  UpdateData(TRUE);
  
  double P1 = m_p1;
  double P2 = m_p2;
  double G = m_g;

  double S = m_poteri/(G*G);

  double dPvh = m_dp_min; //6;
  double T1 = m_t1;
  double T2 = m_t2;

//  double P1i = P2+m_poteri+m_dp_min;
  double P1i = P2+m_dp_min;

  double Got = sqrt((P1i-P2)/S);

  m_g2 = Got;
  m_qq = Got*(T1-T2)/1000;

  UpdateData(FALSE);
}
