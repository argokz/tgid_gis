// CKorroziaZapros2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CKorroziaZapros2.h"
#include "afxdialogex.h"


// CKorroziaZapros2 dialog

IMPLEMENT_DYNAMIC(CKorroziaZapros2, CDialogEx)

CKorroziaZapros2::CKorroziaZapros2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KORROZIA_ZAPROS2, pParent)
	, m_year1(_T(""))
    , m_year2(_T(""))
{

}

CKorroziaZapros2::~CKorroziaZapros2()
{
}

void CKorroziaZapros2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrl_year1);
  DDX_Control(pDX, IDC_COMBO2, m_ctrl_year2);
	DDX_CBString(pDX, IDC_COMBO1, m_year1);
  DDX_CBString(pDX, IDC_COMBO2, m_year2);
}


BEGIN_MESSAGE_MAP(CKorroziaZapros2, CDialogEx)

END_MESSAGE_MAP()


// CKorroziaZapros2 message handlers

void setListQ(CComboBox *combo, CAdoFile *ado, const char *q, int sezon_korrozia);


BOOL CKorroziaZapros2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    COleDateTime date = COleDateTime::GetCurrentTime();

    setListQ(&m_ctrl_year1, m_ado, m_q, date.GetYear()-5);
    setListQ(&m_ctrl_year2, m_ado, m_q, date.GetYear()-1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void CKorroziaZapros2::OnChCombo()
{
  UpdateData(TRUE);

//  int y1 = atoi(m_year1)+1;
//  int y2 = atoi(m_year2)+1;


  UpdateData(FALSE);
}


void CKorroziaZapros2::OnOK()
{
    UpdateData(TRUE);

    m_y1 = atoi(m_year1)+1;
    m_y2 = atoi(m_year2)+1;

    if (m_y1 <= 0) {
        AfxMessageBox("√од начала обработки должен быть задан", MB_OK|MB_ICONINFORMATION);
        return;
    }
    if (m_y2 <= 0) {
        AfxMessageBox("√од завершени€ обработки должен быть задан", MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (m_y1 > m_y2) {
        AfxMessageBox("√од завершени€ обработки должен быть позже года начала обработки", MB_OK|MB_ICONINFORMATION);
        return;
    }


    UpdateData(FALSE);

    CDialogEx::OnOK();
}
