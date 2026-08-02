// Inf2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Inf2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInf2 dialog

//#define N 14


CInf2::CInf2(CWnd* pParent, int typ, int nInf, BOOL *bIsInfo)
  : CDialog(CInf2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CInf2)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_bIsInfo = bIsInfo;
  m_typ = typ;

  m_nInf = nInf;

}


void CInf2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInf2)
  DDX_Control(pDX, IDOK, m_idok);
  DDX_Control(pDX, IDCANCEL, m_idcancel);
  DDX_Control(pDX, IDC_LIST, m_list);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInf2, CDialog)
  //{{AFX_MSG_MAP(CInf2)
//  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInf2 message handlers

BOOL CInf2::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_list.SetFont(GetFont());

  if (m_typ == 1) {
    m_list.AddString(_TR("Напоры, H, м.вод.ст."));  // H=                                    // 0
    m_list.AddString(_TR("Расходы, G, т/ч"));                                          // 1
    m_list.AddString(_TR("Длины, L, м"));                                            // 2
    m_list.AddString(_TR("Диаметры внутренние, Двн, мм"));                            // 3
    m_list.AddString(_TR("Диаметры условные, Ду, мм"));                               // 4
    m_list.AddString(_TR("Объемы, V, м3"));                                           // 5
    m_list.AddString(_TR("Температуры, t, °C")); // T=                                // 6
    m_list.AddString(_TR("Узловой расход, G, т/ч"));  // Gз Gп Gо                         // 7
    m_list.AddString(_TR("Скорость, w, м/c"));                                         // 8
    m_list.AddString(_TR("Удельные потери, R, мм.вод.ст./м"));                                  // 9
    m_list.AddString(_TR("Нагрузка потребителя, Гкал/ч"));    // Qот Qв Qгвс Qсум                  // 10
    m_list.AddString(_TR("Располагаемый напор, dH, м"));  // dH                      // 11
    m_list.AddString(_TR("Геодезическая отметка, h, м"));  // h                      // 12
    m_list.AddString(_TR("Расчетная нагрузка на участках, Q, Гкал/ч"));                   // 13

    m_list.AddString(_TR("Полные напоры, H, м.вод.ст."));  // H=                          //14         


//    m_list.AddString(_TR("Время замерзания"));                                  // 13
//    m_list.AddString(_TR("Номер магистрали"));                                  // 14
//    m_list.AddString("OPC");
  }
  else if (m_typ == 2) {
    m_list.AddString(_TR("Диаметры"));
    m_list.AddString(_TR("Расходы"));
    m_list.AddString(_TR("Длины"));
    m_list.AddString(_TR("Расстояние"));
    m_list.AddString(_TR("Удельные потери"));
  }
  else if (m_typ == 3) {
    m_list.AddString(_TR("Длины"));
    m_list.AddString(_TR("Диаметры"));
/*
    m_list.AddString(_TR("Объемы"));
    m_list.AddString(_TR("Геодезическая отметка"));
    m_list.AddString(_TR("Напоры"));
    m_list.AddString(_TR("Расходы"));
*/
  }

  for ( int i = 0; i < m_list.GetCount(); i++) {
    m_list.SetCheck(i, m_bIsInfo[i]);
  }

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CInf2::OnOK() 
{
  UpdateData(TRUE);

//  int n = sizeof(bIsInfo)/sizeof(bIsInfo[0]);

  int i;

  for ( i = 0; i < m_nInf; i++) {
    m_bIsInfo[i] = 0;
  }

  for ( i = 0; i < m_list.GetCount(); i++) {
    m_bIsInfo[i] = m_list.GetCheck(i);
  }
  
  CDialog::OnOK();
}

/*
HBRUSH CInf2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
  pDC->SetBkMode(TRANSPARENT);
  return m_Brush;
}
*/