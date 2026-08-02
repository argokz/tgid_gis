// Param1.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Param.h"
#include "Param1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CParam1 dialog


CParam1::CParam1(CWnd* pParent)
    : CDialog(CParam1::IDD, pParent)
{
    //{{AFX_DATA_INIT(CParam1)
    m_char_sety = FALSE;
    m_dross = FALSE;
    m_avtomat = FALSE;
    m_teplopoter = FALSE;
    m_teplovyd = FALSE;
    m_Tn = 0.0;
    m_ur = 0;
    m_veter = FALSE;
    m_sav_off = FALSE;
    m_sav_on = FALSE;
    m_mag_fn = "";
    m_save_po = FALSE;
    m_uf_new = FALSE;
    m_leto = FALSE;
    m_metod = 0;
    m_trtp = 0;
    m_nagr = 0;

    m_zn0 = 0;
    m_utechki = 0;

    m_uf_calc = FALSE;


    //}}AFX_DATA_INIT


    COleDateTime date = COleDateTime::GetCurrentTime();


    CString strRet = date.Format(_T("%d.%m.%Y %H:%M"));

    m_name.Format("Расчет планового режима %s", strRet);

    is_save_registr = true;
    is_read_registr = true;

}


CParam1::CParam1(CWnd* pParent, const GLOBAL *global, double Tn)
    : CDialog(CParam1::IDD, pParent)
{
    //{{AFX_DATA_INIT(CParam1)

    m_char_sety = FALSE;
    m_dross = FALSE;
    m_avtomat = FALSE;
    m_teplopoter = FALSE;
    m_teplovyd = FALSE;
    m_uf_calc = FALSE;

    if (global) {
        m_char_sety = global->is_char_sety;
        m_dross = global->is_dross_yes;
        m_avtomat = global->avtomat_yes;
        m_teplopoter = !global->no_teplopoter;
        m_teplovyd = !global->no_teplovyd;
        m_ur = global->is_tg ? 1 : 0;
        m_kv = global->is_kv ? 1 : 0;
        m_uf_calc = global->uf_calc ? 1 : 0;
    }

    m_Tn = Tn;
//    m_ur = 0;
    m_veter = FALSE;
    m_sav_off = FALSE;
    m_sav_on = FALSE;
//    m_mag_fn = "";
    m_save_po = FALSE;
    m_uf_new = FALSE;
    m_leto = FALSE;
    m_metod = 0;
    m_trtp = 0;
    m_nagr = 0;

    m_zn0 = 0;
    m_utechki = 0;


    //}}AFX_DATA_INIT


    COleDateTime date = COleDateTime::GetCurrentTime();

    CString strRet = date.Format(_T("%d.%m.%Y %H:%M"));

    m_name.Format("Расчет магистрального фрагмента", strRet);

    is_save_registr = false;
    is_read_registr = (global != nullptr);
}




void CParam1::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CParam1)
    DDX_Text(pDX, IDC_NAME, m_name);
    DDX_Check(pDX, IDC_CHAR_SETY, m_char_sety);
    DDX_Check(pDX, IDC_DROSS, m_dross);
    DDX_Check(pDX, IDC_AVTOMAT, m_avtomat);
    DDX_Check(pDX, IDC_TEPLOPOTER, m_teplopoter);
    DDX_Check(pDX, IDC_TEPLOVYD, m_teplovyd);
    DDX_Check(pDX, IDC_UF_CALC, m_uf_calc);


    DDX_Text(pDX, IDC_TN, m_Tn);
//    DDX_Check(pDX, IDC_UR, m_ur);
    DDX_Radio(pDX, IDC_UR, m_ur);

    DDX_Check(pDX, IDC_PLAN, m_plan);


    DDX_Check(pDX, IDC_VETER, m_veter);
    DDX_Text(pDX, IDC_MAG, m_mag_fn);
    DDX_Check(pDX, IDC_SAVE_PO, m_save_po);
    DDX_Check(pDX, IDC_UF_NEW, m_uf_new);
    DDX_Check(pDX, IDC_LETO, m_leto);
    DDX_Check(pDX, IDC_KV, m_kv);


    DDX_CBIndex(pDX, IDC_METOD, m_metod);
    DDX_CBIndex(pDX, IDC_TRTP, m_trtp);
    DDX_Radio(pDX, IDC_DOP_NAGR, m_nagr);

    DDX_Check(pDX, IDC_ZN0, m_zn0);
    DDX_Check(pDX, IDC_UTECHKI, m_utechki);

    DDX_Check(pDX, IDC_MAG_FRAGMENT, m_mag_fragment);

    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParam1, CDialog)
    //{{AFX_MSG_MAP(CParam1)
    ON_BN_CLICKED(IDC_MAG_BUT, OnMagBut)
    ON_BN_CLICKED(IDC_T_IST, OnTIst)
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_TEPLOPOTER, &CParam1::OnBnClickedTeplopoter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParam1 message handlers


void CParam1::OnOK()
{
    UpdateData(TRUE);

    if (is_save_registr) {
        CString str;

        str.Format("%lg", m_Tn);
        AfxGetApp()->WriteProfileString(szSection, "Tn", str);

        AfxGetApp()->WriteProfileInt(szSection, "char_sety", m_char_sety);
        //  AfxGetApp()->WriteProfileInt(szSection, "GWS", m_nGWS);
        //  AfxGetApp()->WriteProfileInt(szSection, "GWS2", m_nGWS2);
        AfxGetApp()->WriteProfileInt(szSection, "dross", m_dross);
        AfxGetApp()->WriteProfileInt(szSection, "avtomat", m_avtomat);

        AfxGetApp()->WriteProfileInt(szSection, "teplopoter", m_teplopoter);
        AfxGetApp()->WriteProfileInt(szSection, "teplovyd", m_teplovyd);
        AfxGetApp()->WriteProfileInt(szSection, "uf_calc", m_uf_calc);

        
        AfxGetApp()->WriteProfileInt(szSection, "veter", m_veter);
        AfxGetApp()->WriteProfileInt(szSection, "ur", m_ur == 0 ? 1 : 0);
        AfxGetApp()->WriteProfileInt(szSection, "leto", m_leto);

        AfxGetApp()->WriteProfileInt(szSection, "mag_fragment", m_mag_fragment);

        AfxGetApp()->WriteProfileInt(szSection, "zn0", m_zn0);
        AfxGetApp()->WriteProfileInt(szSection, "utechki", m_utechki);


        //  AfxGetApp()->WriteProfileInt(szSection, "sav_off", m_sav_off);
        //  AfxGetApp()->WriteProfileInt(szSection, "sav_on", m_sav_on);

        AfxGetApp()->WriteProfileString(szSection, "mag_fn", m_mag_fn);
        //  set_mag_fn(m_mag_fn);

        AfxGetApp()->WriteProfileInt(szSection, "kv_ras", m_kv);
        AfxGetApp()->WriteProfileInt(szSection, "save_po", m_save_po);
        AfxGetApp()->WriteProfileInt(szSection, "uf_new", m_uf_new);
        AfxGetApp()->WriteProfileInt(szSection, "tp_metod", m_metod);
        AfxGetApp()->WriteProfileInt(szSection, "trtp", m_trtp);
    }

    CDialog::OnOK();
}

void CParam1::set_par()
{
    UpdateData(TRUE);
    GetDlgItem(IDC_METOD)->EnableWindow(m_ur == 1);
    GetDlgItem(IDC_TEPLOPOTER)->EnableWindow(m_ur == 1);
//    GetDlgItem(IDC_TEPLOVYD)->EnableWindow(m_ur == 1);
    GetDlgItem(IDC_UF_NEW)->EnableWindow(m_ur == 1);

    GetDlgItem(IDC_AVTOMAT)->EnableWindow(m_ur == 1);

    GetDlgItem(IDC_UF_CALC)->EnableWindow(m_ur == 1);

}


BOOL CParam1::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString str;



    if (!is_save_registr) {

    
    GetDlgItem(IDC_NAME)->EnableWindow(FALSE);
//    GetDlgItem(IDC_CHAR_SETY)->EnableWindow(FALSE);
    GetDlgItem(IDC_DROSS)->EnableWindow(FALSE);
    GetDlgItem(IDC_AVTOMAT)->EnableWindow(FALSE);
    GetDlgItem(IDC_TEPLOPOTER)->EnableWindow(FALSE);
    GetDlgItem(IDC_TEPLOVYD)->EnableWindow(FALSE);
    GetDlgItem(IDC_UF_CALC)->EnableWindow(FALSE);


    GetDlgItem(IDC_TN)->EnableWindow(FALSE);
//        GetDlgItem(IDC_UR)->EnableWindow(FALSE);
    GetDlgItem(IDC_UR)->EnableWindow(FALSE);
    GetDlgItem(IDC_UR2)->EnableWindow(FALSE);
    GetDlgItem(IDC_PLAN)->EnableWindow(FALSE);

    GetDlgItem(IDC_VETER)->EnableWindow(FALSE);
    GetDlgItem(IDC_MAG)->EnableWindow(FALSE);
    GetDlgItem(IDC_SAVE_PO)->EnableWindow(FALSE);
    GetDlgItem(IDC_UF_NEW)->EnableWindow(FALSE);
    GetDlgItem(IDC_LETO)->EnableWindow(FALSE);
    GetDlgItem(IDC_KV)->EnableWindow(FALSE);

    GetDlgItem(IDC_METOD)->EnableWindow(FALSE);
    GetDlgItem(IDC_TRTP)->EnableWindow(FALSE);
    GetDlgItem(IDC_DOP_NAGR)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);

    GetDlgItem(IDC_ZN0)->EnableWindow(FALSE);
    GetDlgItem(IDC_UTECHKI)->EnableWindow(FALSE);

    GetDlgItem(IDC_MAG_FRAGMENT)->EnableWindow(FALSE);
    
    m_dross = FALSE;
    m_zn0 = FALSE;

    }



    if (is_save_registr) {

        str = AfxGetApp()->GetProfileString(szSection, "Tn", "-32");
        m_Tn = atof(str);

        m_char_sety = AfxGetApp()->GetProfileInt(szSection, "char_sety", m_char_sety);

        //  m_nGWS = AfxGetApp()->GetProfileInt(szSection, "GWS", 0);
        //  m_nGWS2 = AfxGetApp()->GetProfileInt(szSection, "GWS2", 0);
        m_dross = AfxGetApp()->GetProfileInt(szSection, "dross", 1);
        m_dross = FALSE;


        //  m_avtomat = AfxGetApp()->GetProfileInt(szSection, "avtomat", 0);
        // Не запоминать расчет автоматизированных потребителей
        m_avtomat = 0;

        m_teplopoter = AfxGetApp()->GetProfileInt(szSection, "teplopoter", 1);
        m_teplovyd = AfxGetApp()->GetProfileInt(szSection, "teplovyd", 1);
        m_uf_calc = AfxGetApp()->GetProfileInt(szSection, "uf_calc", 0);
        m_veter = AfxGetApp()->GetProfileInt(szSection, "veter", 0);


        m_zn0 = AfxGetApp()->GetProfileInt(szSection, "zn0", 0);
        m_zn0 = 0;

        m_utechki = AfxGetApp()->GetProfileInt(szSection, "utechki", 0);

        m_leto = AfxGetApp()->GetProfileInt(szSection, "leto", 0);

        m_mag_fragment = AfxGetApp()->GetProfileInt(szSection, "mag_fragment", 0);

        //  m_sav_off = AfxGetApp()->GetProfileInt(szSection, "sav_off", 0);
        //  m_sav_on = AfxGetApp()->GetProfileInt(szSection, "sav_on", 0);

        m_mag_fn = AfxGetApp()->GetProfileString(szSection, "mag_fn", "");
        //  set_mag_fn(m_mag_fn);

        m_kv = AfxGetApp()->GetProfileInt(szSection, "kv_ras", TRUE);

        m_save_po = AfxGetApp()->GetProfileInt(szSection, "save_po", FALSE);
        m_save_po = FALSE;   // Не сохранять в обоб потре


        m_ur = AfxGetApp()->GetProfileInt(szSection, "ur", 0);
        m_ur = m_ur ? 0 : 1;

        m_uf_new = AfxGetApp()->GetProfileInt(szSection, "uf_new", 1);
        m_uf_new = 0;

        m_metod = AfxGetApp()->GetProfileInt(szSection, "tp_metod", 0);
        m_trtp = AfxGetApp()->GetProfileInt(szSection, "trtp", 0);

        int isDop = AfxGetApp()->GetProfileInt("Settings", _T("IsDop"), 0);

        //  GetDlgItem(IDC_DOP_NAGR )->EnableWindow(isDop);



        //  GetDlgItem(IDC_SAVE_PO)->ShowWindow(m_mag_fn.IsEmpty() ? SW_HIDE : SW_SHOW);
        //  GetDlgItem(IDC_SAVE_PO)->EnableWindow(!m_mag_fn.IsEmpty());

        m_sav_off = 0;
        m_sav_on = 0;

        m_plan = 0;

        m_kol_iter = 20;
    }

    UpdateData(FALSE);

    if (is_save_registr) {
        set_par();
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CParam1::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);

    
    if (nID == IDC_UR || nID == IDC_UR2) {
        UpdateData(TRUE);
        set_par();

        if (m_ur == 0) {
            m_avtomat = FALSE;
        }
        UpdateData(FALSE);
    }
    else if (nID == IDC_TEPLOPOTER) {
        UpdateData(TRUE);
        if (!m_teplopoter) {
            m_avtomat = FALSE;
        }
        UpdateData(FALSE);
    }
    else if (nID == IDC_AVTOMAT) {
        UpdateData(TRUE);
        if (m_avtomat) {
            m_teplopoter = TRUE;
            m_ur = 1;
            UpdateData(FALSE);
            set_par();
        }
        UpdateData(FALSE);
    }

    return CDialog::OnCommand(wParam, lParam);
}

void CParam1::OnMagBut()
{
    CFileDialog fd(TRUE, _T("MDB"), NULL, OFN_HIDEREADONLY,
        _TR("Схема (*.mdb)|*.mdb|"), this);

    if (fd.DoModal() == IDOK) {
        m_mag_fn = fd.GetPathName();
        //    GetDlgItem(IDC_SAVE_PO)->EnableWindow(!m_mag_fn.IsEmpty());
        UpdateData(FALSE);
    }
}


//#include "TIst.h"

void CParam1::OnTIst()
{
    AfxMessageBox("!!", MB_OK | MB_ICONINFORMATION);
    /*
      CTIst tist(this, m_dbf);

      if (tist.DoModal() == IDOK) {
        map_t_ist = tist.map_t_ist;
      }
    */
}


void CParam1::OnBnClickedTeplopoter()
{
    // TODO: Add your control notification handler code here
}
