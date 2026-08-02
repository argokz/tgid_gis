// VirtualDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6Doc.h"
#include "ado.h"

#include "wm_user.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "TgDlg.h"

#define WM_USER_TEMP_STAT WM_USER + 5001

double getScreenMas();

#include "ado2.h"


bool readTG(CAdoFile* ado);
bool getTG(long hSourceID, double tn, double& t1, double& t2, double& t3, double& tv);


/////////////////////////////////////////////////////////////////////////////
// CTgDlg dialog


CTgDlg::CTgDlg(CWnd* pParent, int type_of_net, CString conn, CString db, int ist, CString cap)
    : CDialog(CTgDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CTgDlg)
    //}}AFX_DATA_INIT

    m_type_of_net = type_of_net;
    m_connect = conn;
    m_db = db;

    m_tn = "deployedTempGraphs";
    m_q.Format("SELECT id,tn,Q_otn,t1,t2,t3,tv,t_bn,tg FROM %s WHERE hSourceID=%d ORDER BY tn DESC", m_tn, ist);
    //  m_q.Format("SELECT tn,Q_otn,t1,t2,t3,tv,t_bn,tg FROM %s WHERE hSourceID=%d ORDER BY tn DESC", m_tn, ist);

    m_cap = cap;
    m_ist = ist;

    //  m_id = id;
    m_text = "";
    m_out = "";

    m_check1 = TRUE;
    m_check2 = TRUE;
    m_check3 = TRUE;
    m_check4 = TRUE;
    m_check5 = TRUE;
    m_isTG = false;

    m_profile.Format("TableDlg\\%s", m_cap);
}

CTgDlg::~CTgDlg()
{
    //  delete m_data;
}


BOOL CTgDlg::PreTranslateMessage(MSG* pMsg)
{
    return CDialog::PreTranslateMessage(pMsg);
}


void CTgDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTgDlg)
    DDX_Control(pDX, IDC_GRAPH, m_tg);
    DDX_Text(pDX, IDC_OUT, m_out);
    DDX_Check(pDX, IDC_CHECK1, m_check1);
    DDX_Check(pDX, IDC_CHECK2, m_check2);
    DDX_Check(pDX, IDC_CHECK3, m_check3);
    DDX_Check(pDX, IDC_CHECK4, m_check4);
    DDX_Check(pDX, IDC_CHECK5, m_check5);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTgDlg, CDialog)
    //{{AFX_MSG_MAP(CTgDlg)
  //  ON_MESSAGE(WM_LDBL, OnDblClick)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_DATA, OnData)
    ON_BN_CLICKED(IDC_CALC, OnCalc)
    ON_BN_CLICKED(IDC_PRINT, OnPrint)
    ON_BN_CLICKED(IDC_PRINT_TABLE, OnPrintTable)
    ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    ON_MESSAGE(WM_TABLE_CHANGE, OnTableChange)
    ON_MESSAGE(WM_SET_OUT, OnSetOut)
    ON_BN_CLICKED(IDC_CHECK1, OnCheck)
    ON_BN_CLICKED(IDC_CHECK2, OnCheck)
    ON_BN_CLICKED(IDC_CHECK3, OnCheck)
    ON_BN_CLICKED(IDC_CHECK4, OnCheck)
    ON_BN_CLICKED(IDC_CHECK5, OnCheck)
    ON_BN_CLICKED(IDC_STATIONARY, OnStationary)
    ON_MESSAGE(WM_USER_TEMP_STAT, OnTgStatUpdated)


    //  ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
      //}}AFX_MSG_MAP
    ON_STN_CLICKED(IDC_OUT, &CTgDlg::OnStnClickedOut)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTgDlg message handlers


BOOL CTgDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ////////////////////////////////////////////////////////////////////

    m_grid.AttachGrid(this, IDC_TABLE);

    // bind the datasource to data, provide it with query and connection information
    m_pDSList = new _tagDSList;
    m_pDSList->m_dsADO = new CADOImpDatasource;

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return 0;

    if (readTable("", "tg", m_head)) {
        m_grid.setHead(&m_head);
    }

    readTG(ado);

    m_pDSList->m_dsADO->OpenSQL(m_q, m_connect, m_type_of_net);
    m_pDSList->m_dsADO->m_head = m_head.rows > 0 ? &m_head : NULL;
    m_pDSList->m_dsADO->SetTableName(m_db, m_tn);

    m_grid.SetUniformRowHeight(TRUE);

    m_grid.AddDataSource(m_pDSList->m_dsADO);
    m_grid.SetDefDataSource(m_pDSList->m_dsADO->GetID());
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());
    m_grid.setName(m_cap, virt_data_edit);

    m_grid.setEdit(true);
    m_grid.setAdd(false);
    m_grid.setDel(false);

    /////////////////////////////////////////////////////////////////////////////

    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }

    readTG(ado);

    m_tg.init0(m_ist);

    double t1, t2, t3, tv;

    if (0 && !getTG(m_ist, 0, t1, t2, t3, tv)) {
        m_isTG = true;
        ado_info(this, ado, "heatSources", m_ist, _TR("Температурный график"), TP_ZERO, "TG", false);
    }

    CString q;
    q.Format("SELECT gr.name, gr.id FROM heatSources hs LEFT JOIN graphTypes gr ON hs.graphTypeID=gr.id WHERE hs.id=%d", m_ist);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            CString s = ado->readStr(0);
            long grId = ado->read_long(1);
            CWnd* check5 = GetDlgItem(IDC_CHECK5);
            if (check5) {               
                if (grId==1)
                    check5->EnableWindow(FALSE);
                else
                    check5->EnableWindow(TRUE);
            }
;           m_cap = CString("Температурный график (") + s + CString(")");

        }
        ado->closeTable();
    }



    SetWindowText(m_cap);

    resize();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}



void CTgDlg::resize()
{
    if (!m_grid.m_hWnd) return;
    if (!m_tg.m_hWnd) return;

    CRect sz, sz1, sz2;

    GetClientRect(sz);

    sz.top = 40*getScreenMas();

    sz1 = sz; sz1.right = min(sz.bottom, sz.right / 2);
    sz2 = sz; sz2.left = sz1.right;

    sz1.DeflateRect(3, 3);
    sz2.DeflateRect(3, 3);

    m_tg.MoveWindow(sz1);
    m_grid.MoveWindow(sz2);

    m_tg.Invalidate();
    m_grid.Invalidate();

    //  m_tg.MoveWindow( 0, 0, cy, cy );
    //  m_grid.MoveWindow( cy, 0, cx, cy );

    //  Size();
}


void CTgDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    resize();
}


LRESULT CTgDlg::OnDblClick(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CTgDlg::OnOK()
{
    UpdateData(TRUE);
    CDialog::OnOK();
}


void CTgDlg::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);


    CDialog::OnDestroy();
    //    if (!m_modal)
    //      delete this; // Shown as non-modal, we'll clean up ourselves
}

#include "TempGraph/tempgraph.h"


void CTgDlg::OnCalc()
{
    /*
        CAdoFile *ado = getAdo(getGidAdoName());
        if (!ado) return;

        m_isTG = true;

        ado_info(this, ado, "heatSources", m_ist, _TR("Температурный график"), "TG");
    */

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;

    CTempGraph *tg = new CTempGraph(ado, m_ist, true);
    readTG(ado);

    int nSheetNumber = m_grid.GetSheetNumber();
    _tagDSList* pTempItem = m_pDSList;

    for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
        pTempItem = pTempItem->next;

    if (pTempItem == NULL)
        return;

    m_grid.Refresh2();
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

    readTG(ado);

    m_tg.init0(m_ist);

    delete tg;

    Invalidate();
}

void CTgDlg::OnData()
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;

    m_isTG = false;

    ado_info(this, ado, "heatSources", m_ist, _TR("Температурный график"), TP_ZERO, "TG",  false);
}



LRESULT CTgDlg::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return 0;

    CString q;
    q.Format("SELECT * FROM heatSources WHERE id = %d", m_ist);

    long typ = 0;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            typ = ado->read_long("graphTypeID");
            CWnd* check5 = GetDlgItem(IDC_CHECK5);
            if (check5) {
               if (typ == 1)
                    check5->EnableWindow(FALSE);
                else
                    check5->EnableWindow(TRUE);
            }
        }
        ado->closeTable();
    }

    if (typ == 2 || typ == 4) {
        AfxMessageBox("Скорректированый температурный график используется при Qгвс/Qот < 0,3.\nПри скорректированом графике, удельный расход на ГВС при расчете гидравлического режима принимается равным нулю.");
    }
    if (typ == 3) {
        AfxMessageBox("Повышенный температурный график (регулирования по суммарной нагрузке отопления и ГВС) использутся при последовательной схеме включения подогревателей ГВС.\nПри повышенном графике, удельный расход на ГВС при расчете гидравлического режима принимается равным нулю.");
    }

    if (!m_isTG) return 0;

    m_isTG = false;


    CTempGraph tg(ado, m_ist, true);
    readTG(ado);

    int nSheetNumber = m_grid.GetSheetNumber();
    _tagDSList* pTempItem = m_pDSList;

    for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
        pTempItem = pTempItem->next;

    if (pTempItem == NULL)
        return 0;

    m_grid.Refresh2();
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

    readTG(ado);

    m_tg.init0(m_ist);
    Invalidate();

//    OnCalc();

    return 0;
}


void CTgDlg::OnPrint()
{
    BOOL r = m_grid.IsSelected(0, 0, NULL);
    m_tg.Copy();
    //    m_grid.CopySelected();
}


void CTgDlg::OnPrintTable()
{
//    BOOL r = m_grid.IsSelected(0, 0, NULL);
//    m_tg.Copy();

    m_grid.ClearSelections();
    m_grid.SelectRange(1, 0, m_grid.m_GI->m_numberCols-2, m_grid.m_GI->m_numberRows-1);
    m_grid.CopySelected();
    m_grid.ClearSelections();
    m_grid.RedrawAll();
}



LRESULT CTgDlg::OnTableChange(WPARAM wParam, LPARAM)
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return 0;

    readTG(ado);
    m_tg.init0(m_ist);

    return 0;
}


void viewTG(CWnd* wnd, int id_ist)
{
    CAdoFile* ado = getAdo(getGidAdoName());

    if (!ado) return;

    CTgDlg dlg(wnd, ado->m_type_of_net, ado->getConnect(), ado->m_schema, id_ist, _TR("Температурный график"));
    dlg.DoModal();

}

void CTgDlg::OnStnClickedOut()
{
    // TODO: Add your control notification handler code here
}


LRESULT CTgDlg::OnSetOut(WPARAM wParam, LPARAM lParam)
{
    m_out = (LPCTSTR)lParam;

    UpdateData(FALSE);

    return 0;
}

void CTgDlg::OnCheck()
{
    UpdateData(TRUE);

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;

    readTG(ado);

    m_tg.SetLines(m_check1, m_check2, m_check3, m_check4, m_check5);

    m_tg.init0(m_ist);
    Invalidate();
}

#include "dialog_pr.h"

void CTgDlg::OnStationary()
{
    CPropGridDlg* pm_cEditDlg = dialog_pr(this, "tg_stat", WM_USER_TEMP_STAT, _TR("Температуры"), -1);

    if (pm_cEditDlg) {
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("TG1", "60");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("TG2", "60");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("TG3", "60");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("TGV", "60");

        //            CString path = AfxGetApp()->GetProfileString(szSection, ff[i], "");
        //            pm_cEditDlg->m_ctrlGrid.SetNameValueInit(ff[i], string(path));
    }
}

LRESULT CTgDlg::OnTgStatUpdated(WPARAM wParam, LPARAM)
{
    CString s_tg1, s_tg2, s_tg3, s_tgv;

    OnCalc();


    getPropValue(WM_USER_TEMP_STAT, "TG1", s_tg1);
    getPropValue(WM_USER_TEMP_STAT, "TG2", s_tg2);
    getPropValue(WM_USER_TEMP_STAT, "TG3", s_tg3);
    getPropValue(WM_USER_TEMP_STAT, "TGV", s_tgv);

    double tg1 = 60, tg2 = 60, tg3 = 60, tgv = 60;

    if (s_tg1 != "") tg1 = atof(s_tg1);
    if (s_tg2 != "") tg2 = atof(s_tg2);
    if (s_tg3 != "") tg3 = atof(s_tg3);
    if (s_tgv != "") tgv = atof(s_tgv);

    UpdateData(TRUE);

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return 0L;

    long affected;
    CString q;

    q.Format("UPDATE deployedTempGraphs SET t1=%g, t2=%g, t3=%g, tv=%g WHERE hSourceID=%d", tg1, tg2, tg3, tgv, m_ist);
    bool r = ado->Execute(q, &affected);

    m_grid.Refresh2();
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

    readTG(ado);

    m_tg.SetLines(m_check1, m_check2, m_check3, m_check4, m_check5);

    m_tg.init0(m_ist);
    Invalidate();

    //    int i, n = sizeof(ff) / sizeof(ff[0]);

    //    for (i = 0; i < n; i++)  {
    //        CString path = getPropValue(WM_USER_CATALOG_UPDATED, ff[i]);
    //        AfxGetApp()->WriteProfileString(szSection, ff[i], path);
    //    }

    return 0L;
    }
