// CKorroziaZapros.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CKorroziaZapros.h"
#include "afxdialogex.h"

void setListQ(CComboBox *combo, CAdoFile *ado, const char *q, int sezon_korrozia)
{
    int cur_ii = 0;
    int ii = 0;

    
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s = ado->readStr(0);
            long y1 = ado->read_long("y1");

            if (y1 == sezon_korrozia) {
                cur_ii = ii;
            }

            combo->AddString(s);
            ado->MoveNext();
            ii++;
        }
        ado->closeTable();
    }

    combo->SetCurSel(cur_ii);
}


// CKorroziaZapros dialog

IMPLEMENT_DYNAMIC(CKorroziaZapros, CDialogEx)

CKorroziaZapros::CKorroziaZapros(CWnd* pParent, int sezon_korrozia)
	: CDialogEx(IDD_KORROZIA_ZAPROS, pParent)
	, m_year(_T(""))
	, m_check(FALSE)
	, m_date_plan(COleDateTime::GetCurrentTime())
{

    m_sezon_korrozia = sezon_korrozia;

}

CKorroziaZapros::~CKorroziaZapros()
{
}

void CKorroziaZapros::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrl_year);
	DDX_CBString(pDX, IDC_COMBO1, m_year);
	DDX_Check(pDX, IDC_CHECK1, m_check);
	//  DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date_plan);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date_plan);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date_plan_ctrl);
}


BEGIN_MESSAGE_MAP(CKorroziaZapros, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &CKorroziaZapros::OnClickedCheck1)
END_MESSAGE_MAP()


// CKorroziaZapros message handlers



BOOL CKorroziaZapros::OnInitDialog()
{
	CDialogEx::OnInitDialog();

//    setListQ(&m_ctrl_year, m_ado, m_q, m_sezon_korrozia);
    setListQ(&m_ctrl_year, m_ado, m_q, -1);

//    m_date_plan_ctrl.SetFormat(" ");


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CKorroziaZapros::OnClickedCheck1()
{
  UpdateData(TRUE);

  m_ctrl_year.EnableWindow(!m_check);
}

int regex_match(const char *p, const char *buf, vector<CString> & match);



void CKorroziaZapros::OnOK() 
{
    UpdateData(TRUE);

    vector<CString> match;
    int l = regex_match("^([0-9]+)\\-([0-9])+$", m_year, match);

    if ( l > 0)  {
        m_year1 = match[1];
    }

    if (m_date_plan.GetYear() != m_sezon_korrozia) {
        CString s;
        s.Format("Дата планирования должна быть в отопительном сезоне %d-%d", m_sezon_korrozia, m_sezon_korrozia+1);
        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
        return;
    }

    CDialogEx::OnOK();
}
