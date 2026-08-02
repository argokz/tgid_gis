// CDiams.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"

#include "ado.h"

#include "CDiams.h"
#include "afxdialogex.h"
#include "utp/utp_diam_rasch.h"


// CDiams dialog

IMPLEMENT_DYNAMIC(CDiams, CDialogEx)

CDiams::CDiams(CWnd* pParent, int standardID)
	: CDialogEx(IDD_DIAMS, pParent)
{
    m_nStd = 0;
    m_standardID = standardID;
    m_standard = "Россия";
}

CDiams::~CDiams()
{
}

void CDiams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIAMS, m_ctrlDiams);
	DDX_Control(pDX, IDC_STD, m_ctrlStd);
    DDX_CBIndex(pDX, IDC_STD, m_nStd);
}
                              
BEGIN_MESSAGE_MAP(CDiams, CDialogEx)
	ON_CBN_SELCHANGE(IDC_STD, &CDiams::OnSelchangeStd)
    ON_BN_CLICKED(IDC_CALC, &CDiams::OnClickedCalc)
    ON_NOTIFY(NM_DBLCLK, IDC_DIAMS, OnDblclkList1)
END_MESSAGE_MAP()


// CDiams message handlers

void CDiams::OnSelchangeStd()
{
    UpdateData(TRUE);

    CString stand;

    int n = m_ctrlStd.GetCurSel();

    if (n >= 0) {
        m_ctrlStd.GetLBText(n, stand);
        if (stand == "ГОСТ") stand = "Россия";

        show_diam(stand);
    }
}

void CDiams::show_std()
{
	CAdoFile* ado = getAdo(getGidAdoName());

	if (!ado) return;

	CString q;

    list_std.resize(256);

    q.Format("SELECT DISTINCT IIF(stand = N'Россия', N'ГОСТ', stand) AS stand, min(id) OVER(PARTITION BY stand) AS id FROM standardTubes ORDER BY id");

    q.Format("SELECT id, IIF(name = N'Россия', N'ГОСТ', name) AS name1, IIF(name = 'ГОСТ', 'Россия', name) AS name2 FROM standards");

    int ii = 0;

	if (ado->openTable0(q)) {
		while (!ado->isEOF()) {
			int id = ado->read_long(0);

			CString s1 = ado->readStr(1);
			CString s2 = ado->readStr(2);

            if (id == m_standardID) {
               m_nStd = ii;
               m_standard = s2;
            }

            list_std[ii++] = id;
			m_ctrlStd.AddString(s1);
			ado->MoveNext();
		}
		ado->closeTable();
	}
}

void CDiams::show_diam(const char *stand)
{
	CAdoFile* ado = getAdo(getGidAdoName());

	if (!ado) return;

	CString q;

    q.Format("SELECT * FROM standardTubes WHERE stand='%s'", stand);

    list_diam.clear();
    list_diam.resize(256);

    if (ado->openTable0(q)) {
        int ii = 0;

        m_ctrlDiams.DeleteAllItems();

        while (!ado->isEOF()) {
            DIAMS d;
            d.d1 = ado->read_double("diametr_usl");
            d.d2 = ado->read_double("diamVne");
            d.d3 = ado->read_double("diametr");
            d.tol = ado->read_double("tol");

            if (ii < 256) {
                list_diam[ii] = d;

                CString s1, s2, s3, s4;

                s1.Format("%g", d.d1);
                s2.Format("%g", d.d2);
                s3.Format("%g", d.d3);
                s4.Format("%g", d.tol);

                m_ctrlDiams.InsertItem(ii, s1);
                m_ctrlDiams.SetItem(ii, 2, LVIF_TEXT, s3, 0, 0, 0, 0);
                m_ctrlDiams.SetItem(ii, 1, LVIF_TEXT, s2, 0, 0, 0, 0);
                m_ctrlDiams.SetItem(ii, 3, LVIF_TEXT, s4, 0, 0, 0, 0);
            }

           ii ++;
           ado->MoveNext();
        }
        ado->closeTable();
    }
}


bool LoadFont2(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);

double getScreenMas();

BOOL CDiams::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_ctrlDiams.InsertColumn(0, _TR("Условный")   , LVCFMT_LEFT, 100*getScreenMas());
    m_ctrlDiams.InsertColumn(2, _TR("Внутренний") , LVCFMT_LEFT, 100*getScreenMas());
    m_ctrlDiams.InsertColumn(1, _TR("Внешний")    , LVCFMT_LEFT, 100*getScreenMas());
    m_ctrlDiams.InsertColumn(3, _TR("Толщина")    , LVCFMT_LEFT, 100*getScreenMas());

/*
    
    // in OnInitDialog()
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = 30;
    lf.lfWeight = 550;
    _tcscpy(lf.lfFaceName, _T("Tahoma"));
    m_font.CreateFontIndirect(&lf); // m_font is a CFont member variable
*/


    LOGFONT lf;
	COLORREF color;

	if (LoadFont2(_T("PropGridFont"), lf, color)) {
		m_font.CreateFontIndirect(&lf);
	}
	else {
		m_font.CreatePointFont(120, _T("Tahoma"));
	}



    m_ctrlDiams.SetFont(&m_font, TRUE);

    show_std();
    show_diam(m_standard);

    UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void CDiams::OnClickedCalc()
{
    CUTP_diam_rasch dlg(this);

    dlg.DoModal();
}

int getCurItem(CListCtrl& lst);


void CDiams::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)pNMHDR;
    CPoint point(pia->ptAction);

//    UINT uFlags;
    //  int nItem = m_List.HitTest(point, &uFlags);
    int nItem = pia->iItem;

    *pResult = 0;

    if (nItem >= 0) {
        m_diams = list_diam[nItem];
        int n = m_ctrlStd.GetCurSel();
        if (n >= 0) {
            m_diams.standID = list_std[n];
        }
        EndDialog(IDOK);
    }
}

void CDiams::OnOK()
{
    int nItem = getCurItem(m_ctrlDiams);
    if (nItem >= 0) {
        m_diams = list_diam[nItem];
        int n = m_ctrlStd.GetCurSel();
        if (n >= 0) {
            m_diams.standID = list_std[n];
        }
    }
	CDialogEx::OnOK();
}

