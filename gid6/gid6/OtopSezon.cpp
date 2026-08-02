// RemontZapros.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OtopSezon.h"
#include "ado.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtopSezon dialog


COtopSezon::COtopSezon(CWnd* pParent, const char* capt, int year)
    : CDialog(COtopSezon::IDD, pParent)
{
    //{{AFX_DATA_INIT(COtopSezon)
//    m_date1 = 0;
//    m_date2 = 0;
    m_check = FALSE;
    m_sezon = 0;
    //}}AFX_DATA_INIT

    m_year1 = year;

    m_tab = "";

    m_capt = "";
    if (capt) {
        m_capt = capt;
    }
}

void COtopSezon::setTab(CAdoFile* ado, const char* tab)
{
    m_ado = ado;
    m_tab = tab;
}



void COtopSezon::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COtopSezon)
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date1);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_date2);
    DDX_Check(pDX, IDC_CHECK1, m_check);
    DDX_Control(pDX, IDC_COMBO1, m_combo1);
    DDX_LBIndex(pDX, IDC_COMBO1, m_sezon);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtopSezon, CDialog)
    //{{AFX_MSG_MAP(COtopSezon)
    ON_LBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
    ON_LBN_DBLCLK(IDC_COMBO1, OnDblclk)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtopSezon message handlers

BOOL COtopSezon::OnInitDialog()
{
    CDialog::OnInitDialog();

    CString q;
    q.Format("SELECT id, YEAR(date1) AS dt, CONCAT(YEAR(date1), '-', YEAR(date2)) AS name, date1, date2 FROM heating_seasons ORDER BY date1");

    int ii = 1;
    int cur_ii = -1;

    m_combo1.AddString("Все сезоны");

    if (m_ado->openTable0(q)) {
        while (!m_ado->isEOF()) {
            long id = m_ado->read_long("id");
            long dt = m_ado->read_long("dt");
            CString s = m_ado->readStr("name");
            COleDateTime date1 = m_ado->read_date("date1");
            COleDateTime date2 = m_ado->read_date("date2");

            m_map_date[s] = pair<COleDateTime, COleDateTime>(date1, date2);

            m_combo1.AddString(s);

            int y1 = date1.GetYear();

            if (y1 == m_year1) {
                m_date1 = date1;
                m_date2 = date2;
                cur_ii = ii;
            }
            ii ++;

            m_ado->MoveNext();
        }
        m_ado->closeTable();
    }

    UpdateData(FALSE);

    m_combo1.SetCurSel(cur_ii);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void COtopSezon::OnSelchangeCombo1()
{
    UpdateData(TRUE);

    CString s;

    if (m_sezon < 0) return;

//    m_combo1.GetLBText(m_sezon, s);

    m_combo1.GetText(m_sezon, s);

    auto it = m_map_date.find(s);
    if (it != m_map_date.end()) {
        m_date1 = it->second.first;
        m_date2 = it->second.second;
        m_year1 = m_date1.GetYear();
    }
    else {
        m_year1 = -1;
    }

    UpdateData(FALSE);
}


void COtopSezon::OnOK() 
{
    UpdateData(TRUE);
    
    CDialog::OnOK();
}

void COtopSezon::OnDblclk()
{
  UpdateData(TRUE);
  EndDialog(IDOK);
  OnOK();
}

