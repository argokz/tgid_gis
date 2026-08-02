#include "stdafx.h"
#include "gid6.h"
#include "PotrCx.h"
#include "geofile.h"
#include "CGidResist.h"
#include "MMenu.h"

#include "PropertyGrid\test\PropGridDlg.h"

#include "mmenu2.h"
#include "ado.h"
#include "Uf.h"

#include "Kti.h"

#include "InputDialogDate.h"

#include "Ultimate Grid\TableDialog.h"
#include "Dialog3.h"
#include "win.h"
#include <fstream>
#include "gidrView.h"

#include "CNagruzZdan.h"

#include "electro.h"

#include "TempGraph/NagrSum.h"
#include "CDefectElem.h"

#include "ado2.h"
#include "NewPassport/CWebDialog.h"
#include <duckx.hpp>

#include "CZdanieNagruzki.h"



bool readTableValuesInt(CAdoFile* ado, const char* q, int n, vector<long>& vv, int& not_null);

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);


static CString m_osmotr_or_remont = "";
static int m_osmotr_or_remont_id = -1;
static int m_obj_type_faktory_riskaID = -1;

void list_ms(CWnd *wnd, CString tn, int id);
void list_rs(CWnd* wnd, CString tn, int id);
void list_uch(CWnd* wnd, CString tn, int id);
void list_PT(CGidrView* view, CAdoFile* ado, CWnd* wnd, CString tn, int id);
//void list_US(CGidrView* view, CAdoFile* ado, CWnd* wnd, CString tn, int id);
void list_US(CGidrView *view, CAdoFile * ado, CWnd* wnd, CString tn, int id, CString tnl, CString title);

void AlsecoNagr(CPropertyGrid* wnd, int id);

bool AlsecoNagrMark(CPropertyGrid* wnd, int id, CGidrView* view, CString table, CString m_name);



// MFC application with a dialog that
// accepts two numbers (min/max) and a comment.
// The dialog allows choosing a value: minimum, maximum, or anything between.

#include <afxwin.h>
#include "resource.h"

class CMinMaxDialog : public CDialogEx {
public:
    CMinMaxDialog(const CString & title) : CDialogEx(IDD_MINMAX_DIALOG), m_min(0), m_max(100), m_value(0), m_title(title) {
    }

    int m_min;
    int m_max;
    int m_value;
    CString m_title;

protected:
    virtual BOOL OnInitDialog() override {
        CDialogEx::OnInitDialog();

        SetWindowText(m_title);

        SetDlgItemInt(IDC_EDIT_MIN, m_min);
        SetDlgItemInt(IDC_EDIT_MAX, m_max);
//        SetDlgItemText(IDC_EDIT_COMMENT, m_comment);
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
        return TRUE;
    }

    afx_msg void OnBnClickedButtonMin() {
        m_value = m_min;
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
    }

    afx_msg void OnBnClickedButtonMax() {
        m_value = m_max;
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
    }

    afx_msg void OnEnChangeEditValue() {
        BOOL ok = FALSE;
        int v = GetDlgItemInt(IDC_EDIT_VALUE, &ok);
        if (ok) m_value = v;
    }

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMinMaxDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_MIN, &CMinMaxDialog::OnBnClickedButtonMin)
    ON_BN_CLICKED(IDC_BUTTON_MAX, &CMinMaxDialog::OnBnClickedButtonMax)
    ON_EN_CHANGE(IDC_EDIT_VALUE, &CMinMaxDialog::OnEnChangeEditValue)
END_MESSAGE_MAP()

// Resource identifier for the dialog
//typedef enum { IDD_MAIN_DIALOG = 101 } RESOURCE_IDS;

/*
class CMfcSingleDialogApp : public CWinApp {
public:
    virtual BOOL InitInstance() override {
        CWinApp::InitInstance();

        // Передаем параметры извне
        int externalMin = 5;
        int externalMax = 50;
        CString externalComment = L"Число должно быть между 5 и 50";

        CMinMaxDialog dlg;
        dlg.m_min = externalMin;
        dlg.m_max = externalMax;
        dlg.m_comment = externalComment;

        dlg.DoModal();
        return FALSE;
    }
};
*/
//CMfcSingleDialogApp theApp;




void granizy_razdela_opr(CGidrView *view, CAdoFile * ado, CWnd* wnd, CString tn, int id)
{
#if 0
    CString q;

    q.Format
    (

"SELECT \n"
"        id,\n"
"        uzel AS 'Камера',\n"
//"        name1,\n"
//"        name2,\n"
//"        n1.externalNodeName AS nname1,\n"
//"        n2.externalNodeName AS nname2,\n"
"        diameterCondit AS 'Диаметр условный,мм'\n"
//"        obj.name,\n"
"FROM\n"
"(\n"
"    SELECT DISTINCT\n"
"        zd.id AS id,\n"
"        obj.id AS obj_id,\n"
"        IIF(zd.shape.STDistance(n1.shape) < zd.shape.STDistance(n2.shape), nn1.NodeName, nn2.NodeName) AS uzel,\n"
"        nn1.NodeName AS name1,\n"
"        nn2.NodeName AS name2,\n"
"        n1.externalNodeName AS nname1,\n"
"        n2.externalNodeName AS nname2,\n"
"        hps.diameterCondit,\n"
"        obj.name,\n"
"        l.id AS l_id,\n"
"        l.shape.STDistance(zd.shape) AS len,\n"
"        MIN(l.shape.STDistance(zd.shape)) OVER(PARTITION BY zd.id ) AS min_len\n"
"    FROM linesobj l\n"
"    JOIN opressovka_sektsioniruyuschaya_zadvizhka zd ON l.shape.STDistance(zd.shape) < 0.1\n"
"    JOIN opresDeployed dep ON dep.lineID=l.id\n"
"    JOIN opres obj ON obj.id=dep.directionID\n"
"    JOIN nodes n1 ON n1.id=l.nodeID1\n"
"    JOIN nodes n2 ON n2.id=l.nodeID2\n"
"    JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"    JOIN pipeSections pss ON pss.id=hps.pipeSectionID\n"
"    JOIN nodes nn1 ON nn1.id=pss.nodeID1\n"
"    JOIN nodes nn2 ON nn2.id=pss.nodeID2\n"

"    WHERE obj.id=%d AND l.removed=0\n"
") __ WHERE len=min_len\n", id);



    q.Format(

"SELECT \n"
"        id,\n"
"        uzel AS 'Камера',\n"
//"        name1,\n"
//"        name2,\n"
//"        n1.externalNodeName AS nname1,\n"
//"        n2.externalNodeName AS nname2,\n"
"        diameterCondit AS 'Диаметр условный,мм'\n"
//"        obj.name,\n"
"FROM\n"
"(\n"
"    SELECT DISTINCT\n"
"        zd.id AS id,\n"
"        zd.stateID,\"
"        obj.id AS obj_id,\n"
"        IIF(zd.shape.STDistance(n1.shape) < zd.shape.STDistance(n2.shape), nn1.NodeName, nn2.NodeName) AS uzel,\n"
"        nn1.NodeName AS name1,\n"
"        nn2.NodeName AS name2,\n"
"        n1.externalNodeName AS nname1,\n"
"        n2.externalNodeName AS nname2,\n"
"        hps.diameterCondit,\n"
"        obj.name,\n"
"        l.id AS l_id,\n"
"        l.shape.STDistance(zd.shape) AS len,\n"
"        MIN(l.shape.STDistance(zd.shape)) OVER(PARTITION BY zd.id ) AS min_len\n"
"    FROM linesobj l\n"
"    JOIN zapornaya_armatura zd ON l.shape.STDistance(zd.shape) < 0.1\n"
"    JOIN opresDeployed dep ON dep.lineID=l.id\n"
"    JOIN opres obj ON obj.id=dep.directionID\n"
"    JOIN nodes n1 ON n1.id=l.nodeID1\n"
"    JOIN nodes n2 ON n2.id=l.nodeID2\n"
"    JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"    JOIN pipeSections pss ON pss.id=hps.pipeSectionID\n"
"    JOIN nodes nn1 ON nn1.id=pss.nodeID1\n"
"    JOIN nodes nn2 ON nn2.id=pss.nodeID2\n"

"    WHERE obj.id=%d AND l.removed=0\n"
") __ WHERE len=min_len\n", id);

#endif

//   CString fn = "objects\\opres2\\oprez_zd2";

//   CString q = readQ(ado, fn, "", "");
    CString q;

    q.Format(
"SELECT DISTINCT\n"
"zd.id,\n"
"IIF (n.nodeName is NULL or n.nodeName = '' or n.nodeName = ' ',n.externalNodeName, n.nodeName) as 'Наименование узла размещения',\n"
"es.name AS 'Признак трубопровода',\n"
"st.name AS 'Состояние задвижки',\n"
"zd.nomer2 AS 'Номер задвижки',\n"
"zd.diametr AS 'Диаметр, м',\n"
"zd.primechanie AS 'Примечание'\n"

"FROM zapornaya_armatura zd\n"
"JOIN nodes n ON n.id=zd.nodeID_pts\n"

"JOIN linesobj l ON n.id IN (l.nodeID1, l.nodeID2)\n"
"JOIN opresDeployed dep ON dep.lineID=l.id \n"
"LEFT JOIN damperArmatureStates st ON st.id=zd.stateID\n"
"LEFT JOIN externalSigns es ON es.id=zd.priznak_truboprovoda\n"
"WHERE dep.directionID=%d AND zd.stateID=2\n", id);




//   CString ss;
//   ss.Format("%d", id);

//   q.Replace("$ID$", ss);
//   q.Replace("$fileID$", view->m_cxema->m_par);

//   CString qq;
//   qq.Format("SELECT * FROM (%s) ____ WHERE [Состояние] LIKE 'Зак%%'", q);
//   q = qq;


   int n_ret = 0;

//    CString tn2 = "opressovka_sektsioniruyuschaya_zadvizhka";
    CString tn2 = "zapornaya_armatura";

//    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
//        tn2, q, n_ret, virt_data_geo, "Список подключаемых потребителей к контуру", true);
//    dlg.DoModal();

    CWebDialog* dlg = new
    
    CWebDialog(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(),
        tn2, q, n_ret, virt_data_geo_edit, "Границы раздела", false);

    if (dlg != NULL) {
        BOOL ret = dlg->Create(IDD_TABLE_DLG, wnd);

        if (!ret) {
            delete dlg;
            return;
        }
//        dlg->setGrouping("col_1");
    }
}


int obj_type_faktory_riskaID()
{
    return m_obj_type_faktory_riskaID;
}

CString getOsmotrOrRemont()
{
    return m_osmotr_or_remont;
}


int getOsmotrOrRemontId()
{
    return m_osmotr_or_remont_id;
}



bool ind_save(CPropertyGrid* wnd);

// Факторы риска

int s_to_int(const char* s);


void pipeSections_faktory_riska(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    //  q.Format("Select l.id, o.name From osmotrDeployed d LEFT JOIN osmotr o ON o.id=d.directionID JOIN linesobj l ON l.id=d.lineID WHERE l.id=%d", id);
    q.Format(

        "SELECT DISTINCT\n"
        //"f.lineID,\n"
        "f.id,\n"
        "o.name AS 'Осмотр',\n"
        "o.data_osmotra AS 'Дата осмотра'\n"
        "FROM faktory_riska_truboprovoda f\n"
        "JOIN heatPipeSections hps ON hps.pipeSectionID=f.lineID\n"
        "JOIN osmotr o ON o.id=f.objID\n"
        "WHERE f.obj_type_faktory_riskaID=2\n"
        "AND \n"
        "hps.pipeSectionID=%d\n", id);

    int n_ret = 0;
    CString tn = "Осмотры";

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret,
        virt_data_unknown);

    int ret = dlg.DoModal();

    if (ret == IDOK) {
        int id = s_to_int(dlg.m_strRet);


        CGidrView* view = getView();
        if (!view) return;

        CString tn2 = "faktory_riska_truboprovoda";

        CPropertyGrid* prop = ado_info(wnd, ado, tn2, id, "Наружный осмотр", TP_READONLY, NULL, false);
    }
}


bool update_pipeSections_from_faktor_shurf(CGidrView* view, CAdoFile *ado, CString tn, int id)
{
    bool ret = false;
    CString fn = "";
    int obj_type_faktory_riskaID = 0;

    if (tn == "remont2") {
        fn = "objects\\defect\\update_pipeSectionsForRemont";
        obj_type_faktory_riskaID = 3;
    }
    else if (tn == "osmotr") {
        fn = "objects\\defect\\update_pipeSectionsForOsmotr";
        obj_type_faktory_riskaID = 2;
    }
    else if (tn == "shurfy") {
        fn = "objects\\defect\\update_pipeSectionsForShurfy";
        obj_type_faktory_riskaID = 1;
    }
    else {
        return false;
    }

    if (!isEditGeo(tn, false)) return true;

    CString q;
//    q.Format("SELECT id FROM faktory_riska_truboprovoda f WHERE f.objID = %d AND f.obj_type_faktory_riskaID = %d", id, obj_type_faktory_riskaID);
    q.Format("SELECT id FROM faktory_riska_truboprovoda f WHERE f.objID = %d AND f.obj_type_faktory_riskaID = %d", id, obj_type_faktory_riskaID);

    long idf = readTableValueInt(ado, q);

    bool ret2 = false;

    if (idf) {
        ret2 = (AfxMessageBox("Обновить информацию в Участке ПТС?", MB_YESNO) == IDYES);
    }

    if (ret2) {
        CString q = readQ(ado, fn, "", "");
        CString ss;
        ss.Format("%d", id);
        q.Replace("$faktorID$", ss);
        long affected;
        ret = ado->Execute(q, &affected);
    }
    return ret;
}


bool update_pipeSections_from_faktor(CGidrView* view, CAdoFile *ado, CString tn, int id)
{
    bool ret = false;
    CString fn = "";
    int obj_type_faktory_riskaID = 0;

    if (tn == "remont2") {
        fn = "objects\\defect\\update_pipeSectionsForRemont";
        obj_type_faktory_riskaID = 3;
    }
    else if (tn == "osmotr") {
        fn = "objects\\defect\\update_pipeSectionsForOsmotr";
        obj_type_faktory_riskaID = 2;
    }
    else if (tn == "shurfy") {
        fn = "objects\\defect\\update_pipeSectionsForShurfy";
        obj_type_faktory_riskaID = 1;
    }
    else {
        return false;
    }

    if (!isEditGeo(tn, false)) return true;

    CString q;
    q.Format("SELECT id FROM faktory_riska_truboprovoda f WHERE f.id = %d AND f.obj_type_faktory_riskaID = %d", id, obj_type_faktory_riskaID);

    long idf = readTableValueInt(ado, q);

    bool ret2 = false;

    if (idf) {
        ret2 = (AfxMessageBox("Обновить информацию в Участке ПТС?", MB_YESNO) == IDYES);
    }

    if (ret2) {
        CString q = readQ(ado, fn, "", "");
        CString ss;
        ss.Format("%d", id);
        q.Replace("$faktorID$", ss);
        long affected;
        ret = ado->Execute(q, &affected);
    }
    return ret;
}

void faktory_riska_truboprovoda_osmotr_ok2(int id)
{
    CGidrView* view = getView();
    if (!view) return;
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    bool setOsmotr(CAdoFile * ado, CString opres, CCxema * m_cxema, int directionID);
    int getOsmotrOrRemontId();
    setOsmotr(ado, getOsmotrOrRemont(), view->m_cxema, getOsmotrOrRemontId());

    if (getOsmotrOrRemont() == "remont2" || getOsmotrOrRemont() == "osmotr")
    {
        update_pipeSections_from_faktor(view, ado, getOsmotrOrRemont(), id);
    }

    view->Invalidate();
}

void faktory_riska_truboprovoda_osmotr_ok2(WPARAM wParam, LPARAM lParam)
{
    CPropertyParam* param = (CPropertyParam*)lParam;

    if (param) {
        faktory_riska_truboprovoda_osmotr_ok2(param->id);
    }
}

// История осмотров
void pipeSections_osmotr_history(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    //  q.Format("Select l.id, o.name From osmotrDeployed d LEFT JOIN osmotr o ON o.id=d.directionID JOIN linesobj l ON l.id=d.lineID WHERE l.id=%d", id);
    q.Format(

        "Select DISTINCT o.id,\n"
        "o.name AS 'Наименование',\n"
        "o.data_osmotra AS 'Дата осмотра'\n"
        "From osmotrDeployed d\n"
        "LEFT JOIN osmotr o ON o.id=d.directionID\n"
        "JOIN linesobj l ON l.id=d.lineID\n"
        "JOIN heatPipeSections hps ON hps.lineID=l.id\n"
        "WHERE hps.pipeSectionID=%d\n", id);

    int n_ret = 0;

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), "osmotr", q, n_ret,
        virt_data_unknown, "Осмотры");
    int ret = dlg.DoModal();

    if (ret == IDOK) {
        m_osmotr_or_remont = "osmotr";
        m_osmotr_or_remont_id = s_to_int(dlg.m_strRet);
        m_obj_type_faktory_riskaID = 2;
        faktory_riska_truboprovoda_osmotr_ok2(-1);

        CString q;
        q.Format("SELECT id FROM faktory_riska_truboprovoda fakt WHERE obj_type_faktory_riskaID = %d AND lineID = %d AND objID = %d",
            m_obj_type_faktory_riskaID, id, m_osmotr_or_remont_id);

        int idf = readTableValueInt(ado, q);

        if (idf == 0) {
            AfxMessageBox("Данный участок не был осмотрен");
            return;
        }


        CString tn2 = "faktory_riska_truboprovoda";
        CPropertyGrid* prop = ado_info(wnd, ado, tn2, idf, "Наружный осмотр", TP_READONLY, NULL, false);
    }
}

void pipeSections_remont_history(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    //  q.Format("Select l.id, o.name From osmotrDeployed d LEFT JOIN osmotr o ON o.id=d.directionID JOIN linesobj l ON l.id=d.lineID WHERE l.id=%d", id);
    q.Format(

        "Select DISTINCT o.id,\n"
        "o.otchet_po_defektu AS 'Наименование',\n"
        "o.data_osmotra AS 'Дата осмотра'\n"
        "From remont2Deployed d\n"
        "LEFT JOIN remont2 o ON o.id=d.directionID\n"
        "JOIN linesobj l ON l.id=d.lineID\n"
        "JOIN heatPipeSections hps ON hps.lineID=l.id\n"
        "WHERE hps.pipeSectionID=%d\n", id);

    int n_ret = 0;
    CString tn = "remont2";

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), "remont2", q, n_ret,
        virt_data_unknown, "Ремонты");
    int ret = dlg.DoModal();

    if (ret == IDOK) {
        m_osmotr_or_remont = "remont2";
        m_osmotr_or_remont_id = s_to_int(dlg.m_strRet);
        m_obj_type_faktory_riskaID = 3;
        faktory_riska_truboprovoda_osmotr_ok2(-1);

        CString q;
        q.Format("SELECT id FROM faktory_riska_truboprovoda fakt WHERE obj_type_faktory_riskaID = %d AND lineID = %d AND objID = %d",
            m_obj_type_faktory_riskaID, id, m_osmotr_or_remont_id);

        int idf = readTableValueInt(ado, q);

        if (idf == 0) {
//            AfxMessageBox("Информация по ремонту для этого участка не была занесена");

            CGidrView* view = getView();
            if (view) {
                view->m_id_remont2 = m_osmotr_or_remont_id;
                view->LastOpres("remont2", _TR("Контуры ремонта"), view->m_id_remont2, view->m_id_remont2_old, false);
                return;
            }
        }

        CString tn2 = "faktory_riska_truboprovoda";
        CPropertyGrid* prop = ado_info(wnd, ado, tn2, idf, "Наружный осмотр", TP_READONLY, "faktor_remont", false);
    }
}



list<int> str_to_list(CString str)
{
    list<int> list_int;
    int nTokenPos = 0;
    CString strToken = str.Tokenize(_T(","), nTokenPos);

    while (!strToken.IsEmpty())
    {
        list_int.push_back(atoi(strToken));
        strToken = str.Tokenize(_T(","), nTokenPos);
    }
    return list_int;
}



//int ado_info_line2(CWnd* wnd, CAdoFile* ado, CLINE2* line, const char* tn1, int id1, const char* tn2, int id2, int idN1, int idN2, int fileID, const char* tn_out, int idgP, int idgO, const char* capt);
//int ado_info_line_big2(CWnd* wnd, CAdoFile* ado, const char* tn2, int id2, int idN1, int idN2, const char* capt);
//CPropertyGrid *ado_info(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, int typ_of_p, const char *tn_txt, bool autodelete);

#include "dialog_pr.h"
#include "wm_user.h"

LRESULT CPropertyGrid::OnShurfUtverdit(WPARAM wParam, LPARAM lParam)
{
    CString str_fio_utverzhdaemogo;
    CString str_dolzhnost_utverzhdaemogoID;
    CString str_sluzhba_utverzhdaemogoID;
    CString str_fio_viziruemogo_1;
    CString str_dolzhnost_viziruemogoID_1;
    CString str_data_utverzhdeniya_plana_shurfovok;
    CString str_naznachenie;

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "fio_utverzhdaemogo", str_fio_utverzhdaemogo))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "fio_utverzhdaemogo", str_fio_utverzhdaemogo);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "fio_viziruemogo_1", str_fio_viziruemogo_1))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "fio_viziruemogo_1", str_fio_viziruemogo_1);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);


    if (getPropValue0(WM_USER_SHURF_UTVERDIT, "naznachenie", str_naznachenie))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", str_naznachenie);



    //dolzhnost_viziruemogoID_1
    //dolzhnost_utverzhdaemogoID
    //sluzhba_utverzhdaemogoID

    CString q;
    q.Format(
        "SELECT\n"
        "    d.dolzhnost_utverzhdaemogoID,\n"
        "    d.sluzhba_utverzhdaemogoID,\n"
        "    d.dolzhnost_viziruemogoID_1\n"
        "from shurfy d\n"
        "join \n"
        "(select d.id, dolz_utv.id as id_d_utv, subd_utv.id as id_s_utv, dolz_v.id as id_d_v  from shurfy d \n"
        "LEFT JOIN dolzhnosti dolz_utv ON dolz_utv.znachenie='%s'\n"
        "LEFT JOIN dolzhnosti dolz_v ON dolz_v.znachenie='%s'\n"
        "LEFT JOIN subdivisions subd_utv ON subd_utv.name='%s') t\n"
        " on t.id = d.id\n",
        str_dolzhnost_utverzhdaemogoID,
        str_sluzhba_utverzhdaemogoID,
        str_dolzhnost_viziruemogoID_1
    );

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return 0L;

    vector<CString> v;

    if (readTableValues(ado, q, 3, v)) {
        SetNameValue("dolzhnost_utverzhdaemogoID", atoi(v[0]));
        SetNameValue("sluzhba_utverzhdaemogoID", atoi(v[1]));
        SetNameValue("dolzhnost_viziruemogoID_1", atoi(v[2]));
    }

    //    SetNameValue("sostoyanie_shurfaID", 1);

    SetNameValue("naznachenie", str_naznachenie);
    SetNameValue("fio_utverzhdaemogo", str_fio_utverzhdaemogo);
    SetNameValue("fio_viziruemogo_1", str_fio_viziruemogo_1);
    SetNameValue("data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);
    SetNameValue("utverdit", 1);

    return 0L;
}


void shurf_utverdit(CPropertyGrid* wnd)
{
    CPropGridDlg* pm_cEditDlg = dialog_pr(wnd, "shurf_utverdit", WM_USER_SHURF_UTVERDIT, _TR("Утверждение плана шурфовок"), -1);

    if (pm_cEditDlg) {
        CString str_fio_utverzhdaemogo = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_utverzhdaemogo", "");
        CString str_dolzhnost_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_utverzhdaemogoID", "");
        CString str_sluzhba_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "sluzhba_utverzhdaemogoID", "");
        CString str_fio_viziruemogo_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_viziruemogo_1", "");
        CString str_dolzhnost_viziruemogoID_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_viziruemogoID_1", "");
        CString str_data_utverzhdeniya_plana_shurfovok = AfxGetApp()->GetProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", "");

        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "fio_utverzhdaemogo", str_fio_utverzhdaemogo);
        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID);
        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID);
        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "fio_viziruemogo_1", str_fio_viziruemogo_1);
        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1);
        setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT, "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);
    }

#if 0

    //    if (!wnd->check_NotNull("fio_utverzhdaemogo")) return;
    //    if (!wnd->check_NotNull("dolzhnost_utverzhdaemogoID")) return;
    //    if (!wnd->check_NotNull("sluzhba_utverzhdaemogoID")) return;
    //    if (!wnd->check_NotNull("fio_viziruemogo_1")) return;
    //    if (!wnd->check_NotNull("dolzhnost_viziruemogoID_1")) return;
    //    if (!wnd->check_NotNull("")) return;

    CInputDialogDate dlg2(wnd, "", "Введите дату утверждения плана шурфовки");

    if (dlg2.DoModal() != IDOK) return;
    //    CString ds0 = dlg2.m_date1.Format("%Y%m%d"); 

    //    COleDateTime dt(dlg2.m_date1);
    COleDateTime dt(dlg2.m_date1.GetYear(), dlg2.m_date1.GetMonth(), dlg2.m_date1.GetDay(), 0, 0, 0);
    //        dlg2.m_date1.GetHour(), dlg2.m_date1.GetMinute(), dlg2.m_date1.GetSecond());

    wnd->SetNameValue("data_utverzhdeniya_plana_shurfovok", dt);

    wnd->SetNameValue("utverdit", 1);
#endif
}

CString date8_10(CString s)
{
    CString ss = "";
    vector<CString> match;
    int l = regex_match("([0-9]{4})([0-9]{2})([0-9]{2})", s, match);

    if (l > 0) {
        int y = atoi(match[1]);
        int m = atoi(match[2]);
        int d = atoi(match[3]);

        COleDateTime dt(y, m, d, 0, 0, 0);
        ss.Format("%02d.%02d.%04d", d, m, y);
    }
    return ss;
}


LRESULT CPropertyGrid::OnRemontUtverdit(WPARAM wParam, LPARAM lParam)
{
    if (AfxMessageBox("Вы уверены, что хотите утвердить данный контур ремонта?", MB_YESNO) != IDYES) {
        return 0L;
    }

    CString data_utverzhdeniya_plana;
    getPropValue0(WM_USER_REMONT_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);

    SetNameValue("data_utverzhdeniya_plana", date8_10(data_utverzhdeniya_plana));
    SetNameValue("stateID", 2);
    SetNameValue("utverdit", 1);

    GetOwner()->PostMessage(WM_COMMAND, IDOK, NULL);

    return 0L;
}

void remont_utverdit(CPropertyGrid* wnd)
{
    if (!wnd->check(true, false)) return;

    CPropGridDlg* pm_cEditDlg = dialog_pr(wnd, "remont_utverdit", WM_USER_REMONT_UTVERDIT, _TR("Утверждение плана ремонтов"), -1);

    if (pm_cEditDlg) {
        CTime theTime = CTime::GetCurrentTime();
        CString data_utverzhdeniya_plana = theTime.Format("%d.%m.%Y");

        setPropValueInit(pm_cEditDlg, WM_USER_REMONT_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);
    }
}


int getNumb(const char* txt);


// poterya_massy_srednyaya_pri_kislotnoy_obraboke__g

bool parse_math(CPropertyGrid* wnd, const char* ss, double& d);

long read_long_db(CAdoFile* ado, const char* q, const char* fn);
double read_double_db(CAdoFile* ado, const char* q, const char* fn);

#include "virtualnodelist.h"
#include "VirtualDlg.h"

CFPoint get_coord1(CString tn, int id)
{
    CFPoint pt(0, 0);
    CGidrView* view = getView();
    if (!view) return pt;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int i = kls->GetNom2(id);
        if (i > 0) {
            CCoordList cl;
            m_geofile->GetCoord(kls, i, cl);

            CCxema* cxema = view->m_cxema;
            CGraph2* graph = view->m_cxema->m_graph;
            pt = cl.first();
        }
    }
    return pt;
}


static CPropertyGrid* m_wnd = nullptr;

void faktory_riska_truboprovoda_osmotr(CPropertyGrid* wnd, CString tn, CString title, int id, int obj_type_faktory_riskaID)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    if (tn == "remont2") {
        if (!wnd->check_NotNull("data_nachala_remonta")) return;
    }

    int n_ret = 0;

    CString ss;
    CString fn = "objects\\osmotr\\osmotr_list_pts_uchastkov";

    if (obj_type_faktory_riskaID == 3) {
        fn = "objects\\osmotr\\osmotr_list_pts_uchastkov3";
    }


    CString q = readQ(ado, fn, "", "");

    CString osmotr = tn;

    ss.Format("%d", id);

    q.Replace("$m_opr_id$", ss);
    q.Replace("$opres$", osmotr);

    ss.Format("%d", obj_type_faktory_riskaID);

    q.Replace("$obj_type_faktory_riskaID$", ss);

    m_osmotr_or_remont = osmotr;
    m_osmotr_or_remont_id = id;
    m_obj_type_faktory_riskaID = obj_type_faktory_riskaID;
    m_wnd = wnd;

    CGidrView* view = getView();
    if (!view) return;

    //    CWebDialogExample dlg(view, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_line_big);


//    CWebDialog dlg(view, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_line_big, title, true);
    CWebDialog dlg(view, ado->m_type_of_net, ado->m_schema, ado->getConnect(), 
        "faktory_riska_truboprovoda", q, n_ret, virt_data_geo, title, true);

/*
    if (osmotr == "osmotr") {
        dlg.setTransIDMode(true);
    }
    else {
        dlg.setTransIDMode(false);
    }
*/

    dlg.setTransIDMode(true);

    
    dlg.setColorMode("col_6");
    dlg.DoModal();


    fn = "objects\\defect\\update_contur";
    CString qq = readQ(ado, fn, "", "");
    ss.Format("%d", getOsmotrOrRemontId());
    qq.Replace("$ID$", ss);
    ado->Execute(qq);

    q.Format(
        "select * FROM (\n"
        "select\n"
        "obj.id,\n"
        "sum(faktory_riska_truboprovoda.len_tube) as len_tube_fakt,\n"
        "sum(faktory_riska_truboprovoda.len_izol) as len_izol_fakt,\n"
        "sum(faktory_riska_truboprovoda.len_channel) as len_channel_fakt,\n"
        "sum(faktory_riska_truboprovoda.asfaltirovanie) as asfaltirovanie_fakt\n"
        "from remont2 obj\n"
        "left join remont2Deployed d on d.directionID = obj.id\n"
        "left JOIN heatPipeSections hpss ON hpss.lineID = d.lineID\n"
        "left join linesobj l on l.id = d.lineID\n"
        "JOIN pipeSections pss ON pss.id = hpss.pipeSectionID\n"
        "left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = pss.id and faktory_riska_truboprovoda.objID = obj.id and faktory_riska_truboprovoda.obj_type_faktory_riskaID = 3\n"
        "group by obj.id, obj.len_tube_plan,obj.len_izol_plan,obj.len_channel_plan,obj.asfaltirovanie_plan\n"
        ") _qq\n"
        "WHERE id = %d", getOsmotrOrRemontId());

    vector<CString> v;

    if (readTableValues(ado, q, 5, v)) {
        wnd->SetNameValue("len_tube_cur", atof(v[1]));
        wnd->SetNameValue("len_channel_cur", atof(v[2]));
        wnd->SetNameValue("len_izol_cur", atof(v[3]));
        wnd->SetNameValue("asfaltirovanie", atof(v[4]));
    }
}

CPropertyGrid* ado_info_multi_id(CWnd* wnd, CAdoFile* ado, const char* tn, int id, list<int>& list_id, const char* capt, TYP_OF_P typ_of_p, const char* tn_txt, bool autodelete);

int read_list_q(CAdoFile* ado, CString q, list<int>& list_id2)
{
    int n_f = 0;
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            long lineID = ado->read_long(1);

            list_id2.push_back(id);
            n_f += 1;
            nn = id;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return n_f;
}

void faktory_riska_truboprovoda_osmotr_ok(CWnd* wnd, std::wstring wstr)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString result(wstr.c_str());

    list<int> list_id2;
    list<int> list_id = str_to_list(result);

    if (list_id.size() == 0) return;

    int n = *list_id.begin();

    CString q;
    CString tn2 = "faktory_riska_truboprovoda";

    int obj_type_faktory_riskaID = m_obj_type_faktory_riskaID;

    //    q.Format("SELECT id, lineID FROM %s WHERE lineID IN (%s) AND objID=%d obj_type_faktory_riskaID=2 ", tn2, 
    q.Format("SELECT id, lineID FROM %s\n"
        "WHERE lineID IN (%s) AND obj_type_faktory_riskaID=%d AND objID=%d",

        tn2, result, obj_type_faktory_riskaID, m_osmotr_or_remont_id);

    int nn = 0;

    int n_f = read_list_q(ado, q, list_id2);

    bool pervy_raz = false;

    if (list_id2.size() > 0) {
        CString q;
        nn = *list_id2.begin();
    }
    else {
        CString vv = "";

        for (auto& it : list_id) {
            int id = m_osmotr_or_remont_id;
            int lineID = it;
            CString ss;
            if (vv != "") vv += ",";
            ss.Format("(%d,%d,%d)", id, lineID, obj_type_faktory_riskaID);
            vv += ss;
        }

        pervy_raz = true;

        long affected;

        bool is_special = false;

        CString q2;

        if (obj_type_faktory_riskaID == 3 || obj_type_faktory_riskaID == 2) {
//            q2 = readQ(ado, "objects\\defect\\copy_last_faktors_remont", "", "");
//            q2 = readQ(ado, "objects\\defect\\faktory_riska_groups", "", "");
            q2 = readQ(ado, "objects\\defect\\last_faktory_riska_pipeSections", "", "");

            q2.Replace("$lineID$", result);
            CString ss;
            ss.Format("%d", m_osmotr_or_remont_id);
            q2.Replace("$objID$", ss);
            ss.Format("%d", obj_type_faktory_riskaID);
            q2.Replace("$obj_type_faktory_riskaID$", ss);

            long affected;
            if (ado->Execute(q2, &affected) && affected > 0) {
                is_special = true;
            }
        }
        if (!is_special) {
            q2.Format("INSERT INTO %s (objID, lineID, obj_type_faktory_riskaID) VALUES %s", tn2, vv);
            nn = ado->ExecuteInsert(q2, &affected);
        }
        n_f = read_list_q(ado, q, list_id2);
        if (list_id2.size() > 0) {
            nn = *list_id2.begin();
        }
    }

    CGidrView* view = getView();
    if (!view) return;

    CPropertyGrid* prop = nullptr;

    if (obj_type_faktory_riskaID == 2) {
        prop = ado_info(wnd, ado, tn2, nn, "Наружный осмотр", TP_FAKTORY_OSMOTR, NULL, pervy_raz);
    }
    else if (obj_type_faktory_riskaID == 3) {
        prop = ado_info(wnd, ado, tn2, nn, "Ремонт участка", TP_FAKTORY_OSMOTR, "faktor_remont", pervy_raz);
    }

    if (prop && list_id2.size() > 0) {
        ((CPropGridDlg*)prop->GetOwner())->m_property_param->m_list_id = list_id2;
    }
}

// Вывести факторы риска для контура getOsmotrOrRemontId()


CString getPar(const list<int>& v)
{
    CString par = "";
    for (auto& it : v) {
        CString s;
        s.Format("%d", it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}


void faktory_riska_truboprovoda_osmotr_cancel(WPARAM wParam, LPARAM lParam)
{
    CGidrView* view = getView();
    if (!view) return;
    CPropertyParam* param = (CPropertyParam*)lParam;
    if (!param) return;

    TYP_OF_P typ = (TYP_OF_P)wParam;

    if (param->table == "faktory_riska_truboprovoda" && typ == TP_FAKTORY_AUTODELETE) {
        CString q;
        int directionID = 0;
        q.Format("DELETE FROM %s WHERE id IN (%s)", param->table, getPar(param->m_list_id));
        long affected;
        view->m_cxema->m_ado->Execute(q, &affected);
        view->Invalidate();
        return;
    }
}



void faktory_riska_truboprovoda_shurf(CPropertyGrid* wnd, CString tn, int id, int obj_type_faktory_riskaID, CString param)
{
    CGidrView* view = getView();
    if (!view) return;

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    if (tn == "shurfy") {
        obj_type_faktory_riskaID = 1;
    }
    else if (tn == "osmotr") {
        obj_type_faktory_riskaID = 2;
    }
    else if (tn == "remont2") {
        obj_type_faktory_riskaID = 3;
    }


    vector<CString> match;
    CString faktor_name = "faktor_riska1";


    int l = regex_match("\\$.+?\\$(.+?)\\$", param, match);
    if (l >= 1) {
        faktor_name = match[1];
    }


    int lineID = 0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int i = kls->GetNom2(id);
        if (i <= 0) return;

        CCoordList cl;
        m_geofile->GetCoord(kls, i, cl);

        CCxema* cxema = view->m_cxema;
        CGraph2* graph = view->m_cxema->m_graph;
        CFPoint pt = cl.first();
        CLINE2* line = graph->findLine(0, pt, view->geom.masx * D5 / view->mas_otn);
        if (line) {
            CString capt = bline(line)->getTable() + " (" + where(line)->getFileName() + ")";

            int nomP = bline(line)->line.nomP;
            int idP2 = bline(line)->line.idP2;
            int nomO = bline(line)->line.nomO;
            int idO2 = bline(line)->line.idO2;
            int nom = nomP, id2 = idP2;

            lineID = nomP >= 0 ? nomP : nomO;
            lineID = bline(line)->line.pipeSectionID;
        }
    }

    CString tn2 = "faktory_riska_truboprovoda";

    CString q;
    q.Format("SELECT id FROM %s WHERE objID=%d", tn2, id);

    long n = readTableValueInt(ado, q);

//-------------------

    if (n == 0) {
        CString ss, q2 = readQ(ado, "objects\\defect\\last_faktory_riska_pipeSections", "", "");

        q2.Replace("$obj_type_faktory_riskaID$", "1");

        ss.Format("%d", id);
        q2.Replace("$objID$", ss);
        ss.Format("%d", lineID);
        q2.Replace("$lineID$", ss);

        long affected;

        n = ado->ExecuteInsert(q2, &affected);

        if (n == 0) {
            q.Format("INSERT INTO %s (objID, lineID, obj_type_faktory_riskaID) VALUES (%d,%d,%d)", tn2, id, lineID, obj_type_faktory_riskaID);
            n = ado->ExecuteInsert(q, &affected);
        }
    }

    if (n > 0) {
        ado_info(wnd, ado, tn2, n, "Наружный осмотр", TP_FAKTORY, faktor_name, false);
    }
    else {
        AfxMessageBox("Какая-то ошибка");
    }

    //            ado_info_line2(this, cxema->m_ado, line, "linesobj", nom, bline(line)->getTableMySQL(), id2, where(line)->id, other(line)->id, where(line)->node.fileID, bline(line)->getTableOutMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, capt);
    // }
}


void spisok_otkl_potr(CPropertyGrid* wnd, int id)
{
    CGidrView* view = getView();
    if (!view) return;

    CNodeListData* nd = new CNodeListData(view->m_cxema->m_graph, true, true);

    CVirtualDlg dlg(wnd, nd, -1, _TR("Список отключенных потребителей"), true);
    if (dlg.DoModal() == IDOK) {
        int id = dlg.m_id;
        CNode2* n = view->m_cxema->m_graph->find(id);
        if (n) {
            view->moveNode(n);
        }
    }
}


double getRealValueDouble(CPropertyGrid* wnd, const char* fn)
{
    double d = 0;
    wnd->GetNameValue(transl_l(fn), d);
    return d;
}

int getRealValueInt(CPropertyGrid* wnd, const char* fn)
{
    int d = 0;
    wnd->GetNameValue(transl_l(fn), d);
    return d;
}

CString getRealValueText(CPropertyGrid* wnd, const char* fn)
{
    CString s = "";
    wnd->GetNameValue(transl_l(fn), s);
    return s;
}

COleDateTime getRealValueDate(CPropertyGrid* wnd, const char* fn)
{
    COleDateTime d;
    wnd->GetNameValue(transl_l(fn), d);
    return d;
}


void ind_reset(CPropertyGrid* wnd);


void ind_result(CPropertyGrid* wnd)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    int id = wnd->GetID("poterya_massy_srednyaya_pri_kislotnoy_obraboke__g");

    CString tn = _TR("Индикатор_коррозии_по_годам");

    CString q;
    //    q.Format("SELECT * FROM %s WHERE id_i=%d", transl(tn), id);

    q.Format("SELECT ik.id AS id "
        //"--      ,id_i "
//        "      ,tekuschiy_nomer "
        //"--      ,nomer_indikatora_korrozii "
        //"--      ,truboprovod "

        "      ,sost.name AS sostoyanie"
        "      ,data_planirovaniya "
        "      ,data_ustanovki "
        "      ,data_izvlecheniya "
        "      ,kolichestvo_dney_ispytaniy "

        "      ,kolichestvo_plastin_v_sborke "
        "      ,sredniy_ves_plastiny_pri_ustanovke__g "
        "      ,radius_krugloy_plastiny__mm "
        "      ,radius_vtulki__mm "
        "      ,tolschina_plastiny__mm "

        "      ,si.name AS stateIndID"

        "      ,sredniy_ves_plastiny_posle_ispytaniy__g "
        "      ,poterya_massy_srednyaya_pri_kislotnoy_obraboke__g "
        "      ,srednyaya_skorost_korrozii__mm_god "
        //"--      ,otsenka_korrozionnogo_protsessa "
        "     ,ot.name AS otsenka_korrozionnogo_protsessa "

        "      ,nw.name AS agressivnost_setevoy_vody "
        "      ,vneshniy_vid_plastin "
        "  FROM indikator_korrozii_po_godam ik "
        " LEFT JOIN netWaterAggressivenesses nw ON nw.id=ik.agressivnost_setevoy_vody "

        " LEFT JOIN corrosionProcessMarks ot ON ot.id=ik.otsenka_korrozionnogo_protsessa "

        " LEFT JOIN stateInds si ON si.id=ik.stateIndID "
        " LEFT JOIN stateIndicator sost ON sost.id=ik.sostoyanie "


        "  WHERE id_i=%d "
        "  ORDER BY data_planirovaniya DESC, data_ustanovki DESC ", id);

    int n_ret = 0;

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_del);
    int ret = dlg.DoModal();
}



int regex_match(const char* p, const char* buf, vector<CString>& match);

int IsFile(const char* fN);

bool viewVirtualDlg(CWnd* wnd, const char* _bd, const char* q, long& id, const char* cap, CString& text, bool fnd = false, bool add = false, bool edit = false, bool del = false, CString tn = "", bool chk = false);


void setZero(CPropertyGrid* wnd, const char* fn)
{
    HITEM i = wnd->FindHItemName(fn);
    if (i) {
        double d;
        if (wnd->GetItemValue(i, d) && d != 0) {
            wnd->SetItemValue(i, 0);
        }
    }

}


bool getTableText(CWnd* wnd, const char* _db, const char* tn, int n_ret, CString& s, const char* title = nullptr)
{
    CString db = _db;

    db.Replace("$ps$", getPsAdoName());
    db.Replace("$main$", getGidAdoName());

    CAdoFile* ado = getAdo(db);
    if (ado) {
        CString q;
        q.Format("SELECT * FROM [%s]", transl(tn));
        CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title ? title : tn, q, n_ret, virt_data_unknown);
        int ret = dlg.DoModal();
        if (ret == IDOK) {
            s = dlg.m_strRet;
            return true;
        }
    }
    return false;
}

bool getTableTextQ(CWnd* wnd, const char* _db, const char* tn, const char* q, int n_ret, CString& s)
{
    CString db = _db;

    db.Replace("$ps$", getPsAdoName());
    db.Replace("$main$", getGidAdoName());

    CAdoFile* ado = getAdo(db);
    if (ado) {
        //        CString q;
        //        q.Format("SELECT * FROM [%s]", transl(tn));
        CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_unknown);
        int ret = dlg.DoModal();
        if (ret == IDOK) {
            s = dlg.m_strRet;
            return true;
        }
    }
    return false;
}


int getTableLookup(CWnd* wnd, const char* _db, const char* tn, CString& s)
{
    CString db = _db;

    db.Replace("$ps$", getPsAdoName());
    db.Replace("$main$", getGidAdoName());

    CAdoFile* ado = getAdo(db);

    CString q;
    q.Format("SELECT id, name FROM [%s] ORDER BY ord", transl(tn));
    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, 1, virt_data_unknown);
    int ret = dlg.DoModal();
    if (ret == IDOK) {
        s = dlg.m_strRet;
        return dlg.m_id;
    }
    return -1;
}

#include "CMSopr.h"

CString getRealPath(CString code)
{
    //    if (path == "PATH_CXEMA") return "Y:\\Схемы\\";

    CString path = AfxGetApp()->GetProfileString(szSection, code, "");

    return path;
}

CString TrimPath(CString val, CString path)
{
    if (path != "") {
        CString p = path; p.MakeLower();
        CString v = val; v.MakeLower();

        if (v.Find(p) == 0) {
            val = val.Mid(p.GetLength());
        }
    }
    return val;
}

bool viewFile(CWnd* wnd, CString ext, CString filter, CString path, CString strValue, CString& new_value)
{
    //    path = getRealPath(path);

    if (strValue != "") {
        strValue = path + strValue;
        if (IsFile(strValue)) {
            HINSTANCE hi = ShellExecute(wnd->m_hWnd, "open", strValue, NULL, NULL, SW_SHOWNORMAL);
        }
        else {
            CString ss;
            ss.Format(_TR("Нет файла %s"), strValue);
            AfxMessageBox(ss, MB_OK | MB_ICONINFORMATION);
        }
        return false;
    }

    CFileDialog fd(TRUE, ext, path, OFN_HIDEREADONLY, filter, wnd);

    if (fd.DoModal() == IDOK) {
        new_value = fd.GetPathName();
        new_value = TrimPath(new_value, path);
        return true;
    }
    return false;
}

bool funFileInfo0(CString fun, CString& ext, CString& filter, CString& path)
{
    vector<CString> match;
    int l = regex_match("File \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*$", fun, match);

    if (l >= 3) {
        CString aa = match[0];
        ext = match[1];
        filter = match[2];
        path = match[3];
        return true;
    }
    return false;
}

bool funFileInfo(CString fun, CString& ext, CString& filter, CString& path)
{
    bool r = funFileInfo0(fun, ext, filter, path);

    if (r) {
        path = getRealPath(path);
    }
    return r;
}

void CPropertyGrid::view_documents(CString fun, HITEM item)
{
    vector<CString> match;

    int l = regex_match("\\$view_documents\\$(.+)\\$(.+)\\$", fun, match);
    if (l >= 1) {
        CString tab1 = match[1];
        CString tab2 = match[2];

        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tn1 = pItem->m_table;
            int id = pItem->m_id_bd;

            CAdoFile* ado = NULL;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
            if (kls) {
                ado = kls->m_ado;
            }
            else {
                ado = getAdo(getPsAdoName());
            }

            CString q;
            int n_ret = 1;
            q.Format("SELECT d.id, d.objID, d.remontDocumentTypeID, p.name, date_doc, path FROM %s d"
                " LEFT JOIN %s p ON p.id=d.remontDocumentTypeID "
                " WHERE objID=%d"

                , tab1, tab2, id);


           if (tn1 == "remont2===") {

            q.Format("SELECT d.id, d.objID, d.remontDocumentTypeID, p.name, date_doc, path FROM %s d\n"
                " LEFT JOIN %s p ON p.id=d.remontDocumentTypeID\n"
                " WHERE objID=%d\n"

                "UNION \n"
                "select dd.id, dd.objID, dd.remontDocumentTypeID, p.name, date_doc, path \n"
                "from remont2 r2\n"
                "join remont r on r.otchet_po_defektu = r2.otchet_po_defektu\n"
                "join remontDocuments dd on dd.objID = r.id\n"
                "LEFT JOIN remontDocumentTypes p ON p.id=dd.remontDocumentTypeID \n"
                "where r2.id = %d\n"

                , tab1, tab2, id, id);
           
           
           
           }


#if 1
            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Документы"), q, n_ret, virt_data_edit_add_del);

            dlg.SetKey("objID", id);
#else 
            CWebDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Документы"), q, n_ret, virt_data_edit_add_del, "Документы", true);

            dlg.addDefaultKey("objID", id);
#endif      

            dlg.setHead(ado->m_schema, tab1);
            dlg.DoModal();
        }
    }
}





int get_act_columns(CString tab, CString col);


void CPropertyGrid::view_meropr(CString fun, HITEM item)
{
    vector<CString> match;

    int l = regex_match("\\$view_meropr\\$(.+)\\$(.+)\\$", fun, match);
    if (l >= 2) {
        CString tab1 = match[1];
        CString tab2 = match[2];

        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tab = pItem->m_table;
            int id = pItem->m_id_bd;

            //            int idCol = get_act_columns(tab, col);

            CAdoFile* ado = NULL;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tab);
            if (kls) {
                ado = kls->m_ado;
            }
            else {
                ado = getAdo(getPsAdoName());
            }

            int n_ret = 1;
            CString q;

            //                q.Format("SELECT d.id, d.objID, d.activity, d.activityID, p.name FROM %s d"
            q.Format("SELECT d.id, d.objID, d.activityID, p.name FROM %s d"
                "\nLEFT JOIN %s p ON p.id=d.activityID "
                "\nWHERE objID=%d"

                , tab1, tab2, id);

            CString title = pItem->m_name;


            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title, q, n_ret, virt_data_edit_add_del);

            //                dlg.SetIdCol(idCol);
            //                dlg.SetKey2("objID", id, "colID", idCol);
            dlg.SetKey("objID", id);

            CString s;

            s.Format("%s_%s", tab1, tab2);

            dlg.setHead("", s);
            dlg.DoModal();
        }
    }
}

#include "Filtr3.h"


void CPropertyGrid::view_filtr(CString fun, HITEM item)
{
    vector<CString> match;

    int l = regex_match("\\$view_filtr\\$(.+)\\$(.+)\\$", fun, match);
    if (l >= 2) {
        CString tab1 = match[1];
        CString tab2 = match[2];

        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tab = pItem->m_table;
            int id = pItem->m_id_bd;

            CAdoFile* ado = NULL;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tab);
            if (kls) {
                ado = kls->m_ado;
            }
            else {
                ado = getAdo(getPsAdoName());
            }

            CString q;

            q.Format("SELECT d1.id, d1.name, d2.id FROM %s d1 LEFT JOIN %s d2 ON d2.activityID=d1.id AND objID=%d", tab2, tab1, id);

            CString title = pItem->m_name;

            CFiltr3 dlg(this, title);

            if (ado->openTable0(q)) {
                while (!ado->isEOF()) {
                    int id = ado->read_long(0);
                    CString s = ado->readStr(1);
                    int on = ado->read_long(2);
                    dlg.Add(s, id, on);
                    ado->MoveNext();
                }
            }


            if (dlg.DoModal() != IDOK) return;

            CString q0 = "";

            for (auto it : dlg.map_item) {
                if (it.second.on) {
                    CString qq;
                    qq.Format("(%d,%d)", it.second.id, id);
                    if (q0 != "") q0 += ",";
                    q0 += qq;
                }
            }

            q.Format("DELETE FROM %s WHERE objID=%d", tab1, id);

            long affected;

            if (!ado->Execute(q, &affected)) {
                AfxMessageBox("Error1");
            }

            if (q0 != "") {
                q.Format("INSERT INTO %s (activityID, objID) VALUES ", tab1, id);
                q += q0;
                if (!ado->Execute(q, &affected)) {
                    AfxMessageBox("Error2");
                }
            }
        }
    }
}

void CPropertyGrid::view_UT(CItem* pItem)
{
    CGidrView* view = getView();
    if (!view) return;

    CString tn = pItem->m_table;
    int id = pItem->m_id_bd;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int i = kls->GetNom2(id);
        if (i <= 0) return;

        CCoordList cl;
        m_geofile->GetCoord(kls, i, cl);

        CCxema* cxema = view->m_cxema;
        CGraph2* graph = view->m_cxema->m_graph;
        CFPoint pt = cl.first();
        CLINE2* line = graph->findLine(0, pt, view->geom.masx * D5 / view->mas_otn);
        if (line) {
            CString capt = bline(line)->getTable() + " (" + where(line)->getFileName() + ")";

            int nomP = bline(line)->line.nomP;
            int idP2 = bline(line)->line.idP2;
            int nomO = bline(line)->line.nomO;
            int idO2 = bline(line)->line.idO2;
            int nom = nomP, id2 = idP2;

            ado_info_line2(this, cxema->m_ado, line, "linesobj", nom, bline(line)->getTableMySQL(), id2, where(line)->id, other(line)->id, where(line)->node.fileID, bline(line)->getTableOutMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, capt);
        }
    }
}


void CPropertyGrid::view_UTBIG(CItem* pItem)
{
    CGidrView* view = getView();
    if (!view) return;

    CString tn = pItem->m_table;
    int id = pItem->m_id_bd;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int i = kls->GetNom2(id);
        if (i <= 0) return;

        CCoordList cl;
        m_geofile->GetCoord(kls, i, cl);

        CCxema* cxema = view->m_cxema;
        CGraph2* graph = view->m_cxema->m_graph;
        CAdoFile* ado = cxema->m_ado;
        CFPoint pt = cl.first();
        CLINE2* line = graph->findLine(0, pt, view->geom.masx * D5 / view->mas_otn);
        if (line) {
            CString q;
            q.Format("SELECT p.id, p.nodeID1, p.nodeID2 FROM pipeSections p JOIN heatPipeSections hps ON hps.pipeSectionID=p.id WHERE hps.lineID=%d OR hps.lineID=%d", bline(line)->line.nomP, bline(line)->line.nomO);

            if (ado->openTable0(q)) {
                if (!ado->isEOF()) {
                    long id = ado->read_long("id");
                    long nodeID1 = ado->read_long("nodeID1");
                    long nodeID2 = ado->read_long("nodeID2");
                    ado_info_line_big2(this, ado, "pipeSections", id, nodeID1, nodeID2, _TR("Участок ПТС"));
                }
                ado->closeTable();
            }
        }
    }
}



void CPropertyGrid::view_meropr2(CString fun, HITEM item)
{
    vector<CString> match;

    int l = regex_match("\\$view_meropr2\\$(.+)\\$(.+)\\$(.+)\\$", fun, match);
    if (l >= 2) {
        CString tab1 = match[1];
        CString tab2 = match[2];
        CString tab3 = match[3];

        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tab = pItem->m_table;
            int id = pItem->m_id_bd;

            //            int idCol = get_act_columns(tab, col);

            CAdoFile* ado = NULL;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tab);
            if (kls) {
                ado = kls->m_ado;
            }
            else {
                ado = getAdo(getPsAdoName());
            }

            int n_ret = 1;
            CString q;

            //                q.Format("SELECT d.id, d.objID, d.activity, d.activityID, p.name FROM %s d"
            q.Format("SELECT d.id, d.objID, d.activityID, p.name AS name1, d.elementID, e.name AS name2 FROM %s d"
                "\nLEFT JOIN %s p ON p.id=d.activityID "
                "\nLEFT JOIN %s e ON e.id=d.elementID "
                "\nWHERE objID=%d"

                , tab1, tab2, tab3, id);

            CString title = pItem->m_name;


            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title, q, n_ret, virt_data_edit_add_del);

            //                dlg.SetIdCol(idCol);
            //                dlg.SetKey2("objID", id, "colID", idCol);
            dlg.SetKey("objID", id);

            CString s;

            s.Format("%s_%s", tab1, tab2);

            dlg.setHead("", s);
            dlg.DoModal();
        }
    }
}




CString getTabQ0(CAdoFile* ado, const char* tn);
bool getTabQ(CAdoFile* ado, const char* tn, CString& q, CString& par);


void CPropertyGrid::view_link(CString fun, HITEM item)
{
    vector<CString> match;

    int l = regex_match("\\$view_link\\$(.+)\\$(.+)\\$", fun, match);
    if (l >= 2) {
        CString tab1 = match[1];
        CString podp = match[2];

        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tab = pItem->m_table;
            int id = pItem->m_id_bd;

            CAdoFile* ado = getAdo(getPsAdoName());

            int n_ret = 1;

            CString q;

            q = getTabQ0(ado, tab1);

            CString qq;

            qq.Format(" WHERE objID=%d ", id);

            q += qq;

            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), podp, q, n_ret, virt_data_edit_add_del);

            dlg.SetKey("objID", id);
            dlg.setHead(ado->m_schema, tab1);
            dlg.DoModal();
        }
    }
}



void CPropertyGrid::defect_meropr(const char* defect, HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        CString tn1 = pItem->m_table;
        int id = pItem->m_id_bd;

        Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
        if (kls) {

            CAdoFile* ado = kls->m_ado;
            CString tn;

            tn.Format("%s2_meropriyatiya", defect);
            CString q;
            int n_ret = 1;
            q.Format("SELECT d.id, d.%sID, d.povrezhdeniya, p.znachenie AS povr, d.meropriyatie, m.znachenie AS znach, plan1 FROM %s2_meropriyatiya d"
                "\nLEFT JOIN opressovka_povrezhdeniya p ON p.id=d.povrezhdeniya "
                "\nLEFT JOIN opressovka_meropriyatiya_po_ustraneniyu_povrezhdeniy m ON m.id=d.meropriyatie "
                "\nWHERE %sID=%d"

                , defect, defect, defect, id);

            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Мероприятия"), q, n_ret, virt_data_edit_add_del);

            CString s;
            s.Format("%sID", defect);
            dlg.SetKey(s, id);
            dlg.setHead("", tn);

            dlg.DoModal();
        }
    }
}

void CPropertyGrid::defect_doc(const char* defect, HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        CString tn1 = pItem->m_table;
        int id = pItem->m_id_bd;

        Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
        if (kls) {

            CAdoFile* ado = kls->m_ado;
            CString tn;
            tn.Format("%s2_dokumenty", defect);
            CString q;
            int n_ret = 1;
            q.Format("SELECT d.id, d.%sID, d.docRemontTypeID, p.name, data, dokument FROM %s2_dokumenty d"
                " LEFT JOIN docRemontTypes p ON p.id=d.docRemontTypeID "
                " WHERE %sID=%d"

                , defect, defect, defect, id);

            CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Документы"), q, n_ret, virt_data_edit_add_del);
            CString s;
            s.Format("%sID", defect);
            dlg.SetKey(s, id);
            dlg.setHead("", tn);

            dlg.DoModal();
        }
    }
}



void CPropertyGrid::remont_uch(int typ_remont, HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        int id = pItem->m_id_bd;

        CString s;
        s.Format("%d %d", typ_remont, id);

        CAdoFile* ado = getAdo(getGidAdoName());

        CString title = "Нарушения ";
        CString tn = "";
        CString q = "";
        int n_ret = 1;

        /*
            remont_uch(ID_REMONT_DEFECT2, item);
            remont_uch(ID_REMONT_SHURF2, item);
            remont_uch(ID_REMONT_KORROZ, item);
        */

        switch (typ_remont) {
        case ID_REMONT_DEFECT2:
            title = "Нарушения ";
            tn = "defekt2";
            q.Format("SELECT "

                " defekt2.id,  "
                " _TAB2_.name AS Состояние, "
                " data_osmotra, "
                " vremya_osmotra, "
                " otchet_po_defektu "

                " FROM defekt2 "
                " LEFT JOIN statedefect _TAB2_ ON defekt2.stateID=_TAB2_.id "
                " JOIN linesobj l ON defekt2.lineID = l.id"
                " JOIN heatPipeSections hps ON hps.lineID = l.id"
                " JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID WHERE ps1.id=%d", id);
            break;
        case ID_REMONT_SHURF2:
            title = "Шурфы ";
            tn = "shurf2";
            q.Format("SELECT "

                " shurf2.id,  "
                " _TAB2_.name AS Состояние, "
                " data_osmotra, "
                " vremya_osmotra "
                //              " otchet_po_defektu "

                " FROM shurf2 "
                " LEFT JOIN statedefect _TAB2_ ON shurf2.stateID=_TAB2_.id "
                " JOIN linesobj l ON shurf2.lineID = l.id"
                " JOIN heatPipeSections hps ON hps.lineID = l.id"
                " JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID WHERE ps1.id=%d", id);
            break;
        case ID_REMONT_KORROZ:
            title = "Индикаторы коррозии ";
            tn = REM_INDICATOR;

            q.Format(" SELECT  "
                " IK.id,  "
                " _TAB2_.name AS 'Состояние', "
                " nomer_indikatora_korrozii, "
                " data_planirovaniya, "
                " data_ustanovki, "
                " data_izvlecheniya, "
                " vneshniy_vid_plastin, "
                " OZ.name AS 'Агрессивность сетевой воды', "
                " ots.name AS 'Оценка коррозионного процесса' "
                " FROM indikator_korrozii IK "
                " LEFT JOIN statedefect _TAB2_ ON IK.sostoyanie=_TAB2_.id "
                " LEFT JOIN netWaterAggressivenesses OZ ON OZ.ID=IK.otsenka_korrozionnogo_protsessa "
                " LEFT JOIN externalSigns es ON es.id=IK.truboprovod "
                " LEFT JOIN corrosionProcessMarks ots ON ots.id=IK.otsenka_korrozionnogo_protsessa "

                " JOIN linesobj l ON IK.lineID = l.id  "
                " JOIN heatPipeSections hps ON hps.lineID = l.id  "
                " JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID  "
                " WHERE ps1.id=%d ", id);

            break;

        case ID_REMONT_CONTROL_TU:
            title = "Контроль технического состояния ";
            tn = "kontrol_tehnicheskogo_sostoyaniya";
            q.Format(
                " SELECT  "
                "     KTS.id "
                "       ,data_kontrolya "
                "       ,otchet_po_izmereniyu "
                "       ,tip_pribora "
                "       ,data_gospoverki "
                "       ,preobrazovatel "
                "       ,master "
                "       ,master_udostoverenie "
                "       ,defektoskopist "
                "       ,defektoskopist_udostoverenie "

                "   FROM kontrol_tehnicheskogo_sostoyaniya KTS "
                "   JOIN linesobj l ON KTS.lineID = l.id   "
                "   JOIN heatPipeSections hps ON hps.lineID = l.id   "
                "   JOIN pipeSections ps1 ON ps1.id = hps.pipeSectionID   "
                " WHERE ps1.id=%d ", id);

            break;

        }

        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title, q, n_ret, virt_data_unknown);

        //        dlg.setHead(tn);
        dlg.DoModal();
    }
}

void get_map_vis(map <CString, map<CString, list<CString> > >& _map_vis);


#if 0

void CPropertyGrid::setCxema(const char* otop1, const char* otop2)
{
    map <CString, map<CString, list<CString> > > map_vis;
    get_map_vis(map_vis);

    set<CString> set_s;

    auto it1 = map_vis.find(otop1);
    if (it1 != map_vis.end()) {
        for (auto it2 : it1->second) {
            for (auto it3 : it2.second) {
                CItem* item = FindItemName(it3);
                if (item) {

                    if (it2.first == CString(otop2)) {
                        item->m_editable = true;
                        set_s.insert(it3);
                    }
                    else {
                        auto it5 = set_s.find(it3);
                        if (it5 == set_s.end()) {
                            item->m_editable = false;
                        }
                    }

                    item->m_is_hidden = !item->m_editable;
                }
            }
        }
    }
}

#endif

#if 0
void CPropertyGrid::setCxema(const char* otop1, const char* otop2)
{
    map <CString, map<CString, list<CString> > > map_vis;
    get_map_vis(map_vis);

    set<CString> set_s;

    auto it1 = map_vis.find(otop1);
    if (it1 != map_vis.end()) {
        for (auto it2 : it1->second) {
            for (auto it3 : it2.second) {
                CItem* item = FindItemName(it3);
                if (item) {
                    bool ed = item->m_editable;

                    if (it2.first == CString(otop2)) {
                        //                        item->m_editable = true;
                        ed = true;

                        set_s.insert(it3);
                    }
                    else {
                        auto it5 = set_s.find(it3);
                        if (it5 == set_s.end()) {
                            //                            item->m_editable = false;
                            ed = false;
                        }
                    }
                    //                    item->m_is_hidden = !item->m_editable;
                    item->m_is_hidden = !ed;
                }
            }
        }
    }
}
#endif

#if 1

void CPropertyGrid::setCxema(const char* otop1, const char* otop2)
{
    map <CString, map<CString, list<CString> > > map_vis;
    get_map_vis(map_vis);

    set<CString> set_s;

    auto it1 = map_vis.find(otop1);
    if (it1 != map_vis.end()) {
        for (auto it2 : it1->second) {
            for (auto it3 : it2.second) {
                CItem* item = FindItemName(it3);
                if (item) {
                    bool hid = item->m_is_hidden;

                    if (it2.first == CString(otop2)) {
                        //                        item->m_editable = true;
                        hid = false;

                        set_s.insert(it3);
                    }
                    else {
                        auto it5 = set_s.find(it3);
                        if (it5 == set_s.end()) {
                            //                            item->m_editable = false;
                            hid = true;
                        }
                    }
                    //                    item->m_is_hidden = !item->m_editable;
                    item->m_is_hidden = hid;
                }
            }
        }
    }
}

#endif

void CPropertyGrid::setCxema1(const char* otop1)
{
    int value;
    if (GetNameValue(otop1, value)) {
        CString ss;
        ss.Format("%d", value);
        setCxema(otop1, ss);
    }
}

void readCxema(const char* schemeNum, CString& otop, CString& gvs, CString& vent, CString& teh, CString& cond, CString& rez, CString& gvs_all, bool& isEl);


void CPropertyGrid::initCxema(const char* cxema)
{
    CString otop, gvs, vent, teh, cond, rez, gvs_all;
    bool isEl;
    readCxema(cxema, otop, gvs, vent, teh, cond, rez, gvs_all, isEl);


    CItem* item = FindItemName("calcThrustLosFlow");

    setCxema("otop", otop);
    setCxema("gvs", gvs);
    setCxema("gvs_all", gvs_all);
    setCxema("vent", vent);
    setCxema("rez", rez);

    Invalidate();
}

void diag_defect_new(CPropertyGrid* wnd, int diagID, CString tab)
{
    //    CString diag = "diag";
    CString diag = "shurfy";
    CString defect = "defect";
    CString tn_name = "Нарушение";


    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    Klassif* kls_diag = m_geofile->m_kl_list->findKlN(diag);
    if (!kls_diag) return;

    Klassif* kls_defect = m_geofile->m_kl_list->findKlN(defect);
    if (!kls_defect) return;

    int sredstvoDiagID;
    wnd->GetNameValue("sredstvoDiagID", sredstvoDiagID);

    if (!wnd->check(true, false)) return;


    int remontTypeID = 1;

    switch (sredstvoDiagID) {
    case 1: remontTypeID = 3; break;
    case 2: remontTypeID = 5; break;
    case 3: remontTypeID = 4; break;
    }
    remontTypeID = 3;
    //    if (tab == "shurfy") {
    //        wnd->GetNameValue("", d);
    //    }

    int ulicaID = -1;
    CString nomer_doma = "";
    COleDateTime data_nachala;

    wnd->GetNameValue("ulicaID", ulicaID);
    wnd->GetNameValue("nomer_doma", nomer_doma);
    wnd->GetNameValue("data_nachala", data_nachala);
    int nodeID_bizhajshej_kamery;
    double rasstoyanie_do_blizhajshej_kamery;
    wnd->GetNameValue("nodeID_bizhajshej_kamery", nodeID_bizhajshej_kamery);
    wnd->GetNameValue("rasstoyanie_do_blizhajshej_kamery", rasstoyanie_do_blizhajshej_kamery);

    CCoordList cl;

    int i = kls_diag->GetNom2(diagID);
    if (i <= 0) return;

    m_geofile->GetCoord(kls_diag, i, cl);

    map<CString, COleVariant> map_geo_value;
    map_geo_value["remontTypeID"] = (long)remontTypeID;
    map_geo_value["stateID"] = 1L;
    map_geo_value["ulicaID"] = (long)ulicaID;
    map_geo_value["nomer_doma"] = nomer_doma;
    map_geo_value["data_osmotra"] = data_nachala;

    map_geo_value["nodeID_bizhajshej_kamery"] = (long)nodeID_bizhajshej_kamery;
    map_geo_value["rasstoyanieDoPovrezhdeniyaNachKamery"] = rasstoyanie_do_blizhajshej_kamery;

    i = m_geofile->createObj2(defect, cl, map_geo_value);
    if (i <= 0) return;

    int sz_new = kls_defect->geo4.size();

    bool pod_obr = false;

    if (i + 1 == sz_new) {
        pod_obr = true;
    }

    CString q;
    long affected;
    long defectID = m_geofile->GetNom(kls_defect, i);

    q.Format("INSERT INTO defectsForShurfy (objID, defectID) VALUES (%d, %d) ", diagID, defectID);
    long id = ado->ExecuteInsert(q, &affected);

    kls_defect->geo4[i-1]->is_visible = false;

    if (id) {
        CGidrView* view = getView();
        if (view) {
            ado_info(view, ado, defect, defectID, tn_name, TP_GEO, NULL, true);
        }
    }
    if (pod_obr) {
        i += 1;

        kls_defect->geo4[i-1]->is_visible = false;

        long defectID = m_geofile->GetNom(kls_defect, i);

        q.Format("INSERT INTO defectsForShurfy (objID, defectID) VALUES (%d, %d) ", diagID, defectID);
        long id = ado->ExecuteInsert(q, &affected);

        if (id) {
            CGidrView* view = getView();
            if (view) {
                ado_info(view, ado, defect, defectID, tn_name, TP_GEO, NULL, true);
            }
        }
    }
//    kls_defect->geo4.erase(kls_defect->geo4.begin() + (i-1 - 1));
//    if (pod_obr) {
//        kls_defect->geo4.erase(kls_defect->geo4.begin() + (i-1+1 - 1));
//    }
}

void osmotr_defect_new(CPropertyGrid* wnd, int diagID)
{
    //    CString diag = "diag";
    //    CString diag = "shurfy";
    CString diag = "osmotr";
    CString defect = "defect";
    CString tn_name = "Нарушение";

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    //    Klassif* kls_diag = m_geofile->m_kl_list->findKlN(diag);
    //    if (!kls_diag) return;

    Klassif* kls_defect = m_geofile->m_kl_list->findKlN(defect);
    if (!kls_defect) return;

    int sredstvoDiagID;
    wnd->GetNameValue("sredstvoDiagID", sredstvoDiagID);

    if (!wnd->check(true, false)) return;

    int remontTypeID = 0;

    switch (sredstvoDiagID) {
    case 1: remontTypeID = 3; break;
    case 2: remontTypeID = 5; break;
    case 3: remontTypeID = 4; break;
    }

    CCoordList cl;
    cl.push_back(CFPoint(0, 0));

    //    int i = kls_diag->GetNom2(diagID);
    //    if (i <= 0) return;

    //    m_geofile->GetCoord(kls_diag, i, cl);

    map<CString, COleVariant> map_geo_value;
    map_geo_value["remontTypeID"] = (long)remontTypeID;
    map_geo_value["stateID"] = 1L;
    map_geo_value["show_map"] = 0L;

    int i = m_geofile->createObj2(defect, cl, map_geo_value);
    if (i <= 0) return;

    long defectID = m_geofile->GetNom(kls_defect, i);


    kls_defect->geo4.erase(kls_defect->geo4.begin() + (i - 1));

    CString q;

    //    q.Format("INSERT INTO defectsForShurfy (shurfyID, defectID) VALUES (%d, %d) ", diagID, defectID);
    q.Format("INSERT INTO defectsFor%s (objID, defectID) VALUES (%d, %d) ", diag, diagID, defectID);

    long affected;

    long id = ado->ExecuteInsert(q, &affected);

    if (id) {
        CGidrView* view = getView();
        if (view) {
            ado_info(view, ado, defect, defectID, tn_name, TP_GEO, NULL, true);
        }
    }
}

void zhurnal_fun(CPropertyGrid* wnd, int id, CString tn, CString title, CString fn)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    int n_ret = 0;

    //    CString title = "Журнал нарушений на участке";
    //    CString tn = "defect";

    CString ss;

    //    fn = "objects\\defect\\history_defects_by_defectsql";

    CString q = readQ(ado, fn, "", "");

    ss.Format("%d", id);

    q.Replace("$id$", ss);

    CWebDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo_edit, title, true);

    int ret = dlg.DoModal();
}

// Журнал нарушений на участке

void zhurnal_defect(CPropertyGrid* wnd, int id)
{
    CString fn = "objects\\defect\\history_defects_by_defect";
    zhurnal_fun(wnd, id, "defect", "Журнал нарушений на участке", fn);
}

void zhurnal_shurfy(CPropertyGrid* wnd, int id)
{
    CString fn = "objects\\defect\\history_shurfs_by_shurf_id";
    zhurnal_fun(wnd, id, "shurfy", "Журнал шурфов на участке", fn);
}

void zhurnal_remont(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;

    int n_ret = 0;

    CString title = "Журнал ремонтов на участке";
    CString tn = "defect";

    CString fn, ss;

    fn = "objects\\defect\\defects_journal_remontov";

    q = readQ(ado, fn, "", "");

    ss.Format("%d", id);

    q.Replace("$id$", ss);

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo, title);

    int ret = dlg.DoModal();
}

int WordDefectZhurnal(CString doc_name, CString fn, int id);

void zhurnal_defect_word(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    int n_ret = 0;

    CString title = "Журнал нарушений на участке";
    CString tn = "defect";

    CString ss;

    CString tmpName;
    tmpName.Format("%s\\%s", getenv("TMP"), _TR("Журнал нарушений на участке.docx"));

    //    CString fn = "objects\\defect\\history_defects_by_defect";
//    CString fn = "objects\\defect\\defects_journal";
    CString fn = "objects\\defect\\defects_journal_ut";
//    CString fn = "objects\\defect\\history_defects_by_defect";
    CString q = readQ(ado, fn, "", "");


    CGidrView* view = getView();
    if (!view) return;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int i = kls->GetNom2(id);
        if (i <= 0) return;

        CCoordList cl;
        m_geofile->GetCoord(kls, i, cl);

        CCxema* cxema = view->m_cxema;
        CGraph2* graph = view->m_cxema->m_graph;
        CAdoFile* ado = cxema->m_ado;
        CFPoint pt = cl.first();
        CLINE2* line = graph->findLine(0, pt, view->geom.masx * D5 / view->mas_otn);
        if (line) {
            ss.Format(" AND pipeSectionID = %d", bline(line)->line.pipeSectionID);
            q.Replace("$and_pipesectionid$", ss);
        }
    }

    q.Replace("$state_cond$", "");
    q.Replace("$and_condition2$", "");

    void sezon_condition(CString & q, int m_sezon_korrozia, COleDateTime m_sezon_date1, COleDateTime m_sezon_date2);

    COleDateTime d1, d2;
    sezon_condition(q, -1, d1, d2);

    WordDefectZhurnal(tmpName, q, id);

    //    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo, title);
    //    int ret = dlg.DoModal();
}

void akt_rasledovania(CPropertyGrid* wnd, int id)
{
    void AktRasledovania(CPropertyGrid * wnd, int id);

    if (!wnd->check_NotNull("nomer_akta,data_sostavleniya_akta")) return;

    AktRasledovania(wnd, id);
}

void akt_shurfovki(CPropertyGrid* wnd, int id)
{
    void AktShurfovki(CPropertyGrid * wnd, int id);

    if (!wnd->check_NotNull("nomer_akta,data_utverzhdenija_akta")) return;
    
    AktShurfovki(wnd, id);
}

void karta_povrezhdaemosti(CPropertyGrid* wnd, int id) {
    void KartaPovrezhdaemosti(CPropertyGrid * wnd, int id);
    KartaPovrezhdaemosti(wnd, id);
}

void add_node(CTableDialog* wnd, void* pp1, void* pp2, void* pp3, void* pp4)
{
    AfxMessageBox("Укажите узел на схеме", MB_OK | MB_ICONINFORMATION);

    CGidrView* pView = getView();
    if (pView) {
        pView->regim = R_NODE_ADD;
        pView->m_table_node = wnd;
        pView->m_prop_grid = (CPropertyGrid*)pp1;
        pView->m_prop_id = (int)pp2;
    }
}

void add_node2(CTableDialog* wnd, void* pp1, void* pp2, void* pp3, void* pp4)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CNode2* n = (CNode2*)pp1;
    if (n) {
        //        AfxMessageBox(n->getName(), MB_OK | MB_ICONINFORMATION);
        int id = (int)pp2;
        CString q;
        q.Format("INSERT INTO spisok_kamer_drenazh (objID, nodeID) VALUES (%d,%d)", id, n->id);
        //        long affected;
        //        long id_new = ado->ExecuteInsert(q, &affected);
        wnd->Execute(q);
        //        wnd->SendMessage(WM_COMMAND, ID_REFRESH_DATA, 0);
        //        wnd->UpdateWindow();
    }
}

#include "CDiams.h"

void kamery_list_for_defect(CPropertyGrid* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;


    CString q;


    q.Format(
        "SELECT \n"
        "sp.id,\n"
        "ec.name AS 'Код РС',\n"
        "n.externalNodeName AS 'Наименование',\n"
        "fr.name AS 'Фрагмент'\n"

        "FROM spisok_kamer_drenazh sp\n"
        "LEFT JOIN nodes n ON n.id = sp.nodeID\n"
        "LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
        "LEFT JOIN fragments fr ON n.fileID=fr.id\n"
        "WHERE sp.objID=%d", id);

    CString tn = "";

    CTableDialog* dlg = new CTableDialog(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, 1, virt_data_del);

    dlg->addProc(IDC_ADD, add_node, (void*)wnd, (void*)id, (void*)nullptr, (void*)nullptr);
    dlg->addProc(12345, add_node2, (void*)0, (void*)nullptr, (void*)nullptr, (void*)nullptr);
    dlg->noModal();

    BOOL ret = dlg->Create(IDD_TABLE_DEFECT, wnd);
    dlg->ShowWindow(SW_SHOW);


    //    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, 1, virt_data_unknown);
    //    dlg.addProc(IDC_ADD, add_node, (void*)nullptr, (void*)nullptr, (void*)nullptr, (void*)nullptr);

    //    int ret = dlg.DoModal();
    //    if (ret == IDOK) {
    //        s = dlg.m_strRet;
    //        return dlg.m_id;
    //    }
}

bool read3double(CString s, double &e1, double &e2, double &e3)
{
    e1 = 0, e2 = 0, e3 = 0;

    vector<CString> match;
    int l = regex_match("([0-9]+\\.?[0-9]*)\\|([0-9]+\\.?[0-9]*)\\|([0-9]+\\.?[0-9]*)", s, match);

    if (l > 0) {
        e1 = atof(match[1]);
        e2 = atof(match[2]);
        e3 = atof(match[3]);
        return true;
    }
    return false;
}


int CPropertyGrid::prop_fun(CString fun, HITEM item)
{
    CString s;

    CGidrView* pView = getView();
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!pView || !ado) {
        AfxMessageBox("Непонятная ошика, такого никогда не было");
        return 0;
    }

    CString field_name = "";

    CItem* pItem = FindItem(item);
    if (pItem) {
        field_name = pItem->m_name;
    }

    if (fun == "diameter_std") {
        int standardID = 1;
        GetNameValue("standardID", standardID);

        DIAMS* d = diametr(this, standardID);

        if (d) {
            SetNameValue("standardID", d->standID);

            SetNameValue("diameterInternal", d->d3);
            SetNameValue("diameterCondit", d->d1);
            SetNameValue("diameterExternal", d->d2);
            SetNameValue("wallThickness", d->tol);
        }
        return 1;
    }
    else if (fun == "diameter_std2") {
        int standardID = 1;
        //        GetNameValue("standardID", standardID);

        DIAMS* d = diametr(this, standardID);

        if (d) {
            SetNameValue(field_name, d->d1);
            //            SetNameValue(string(field_name), d->d3);
            //            SetNameValue("diameterCondit", d->d1);
            //            SetNameValue("diameterExternal", d->d2);
            //            SetNameValue("wallThickness", d->tol);
        }
        return 1;
    }
    else if (fun == "schemeNum") {

        CString otop, gvs, vent, teh, cond, rez, gvs_all;
        bool isEl;

        CString strValue;
        GetItemValue(item, strValue);
        CString cxema = strValue;

        readCxema(cxema, otop, gvs, vent, teh, cond, rez, gvs_all, isEl);

        m_otopl = 0; m_gv = 0; m_ventil = 0; m_rez = 10;

        if (otop == "n") GetNameValue("calcHLindep", m_otopl);
        if (otop == "z") GetNameValue("calcHLdep", m_otopl);

        if (gvs == "pr") GetNameValue("avgHLGVScloseParall", m_gv);
        if (gvs == "sm") GetNameValue("avgHLGVScloseMix", m_gv);
        if (gvs == "ps") GetNameValue("avgHLGVScloseConseq", m_gv);
        if (gvs == "pw") GetNameValue("avgHLGVSclosePreON", m_gv);
        if (gvs == "op") GetNameValue("avgHLGVSopenFlow", m_gv);
        if (gvs == "oo") GetNameValue("avgHLGVSopenRet", m_gv);

        if (rez == "1") GetNameValue("circHLosOpen", m_rez);
        if (vent == "1") GetNameValue("calcHLventil", m_ventil);

        double m_dog_otopl = 0, m_dog_gv = 0, m_dog_ventil = 0, m_dog_rez = 10;

        if (otop == "n") GetNameValue("contcalcHLindep", m_dog_otopl);
        if (otop == "z") GetNameValue("contCalcHLdep", m_dog_otopl);

        if (gvs == "pr") GetNameValue("contAvgHLGVScloseParall", m_dog_gv);
        if (gvs == "sm") GetNameValue("contAvgHLGVScloseMix", m_dog_gv);
        if (gvs == "ps") GetNameValue("contAvgHLGVScloseConseq", m_dog_gv);
        if (gvs == "pw") GetNameValue("contAvgHLGVSclosePreON", m_dog_gv);
        if (gvs == "op") GetNameValue("contAvgHLGVSopenFlow", m_dog_gv);
        if (gvs == "oo") GetNameValue("contAvgHLGVSopenRet", m_dog_gv);

        //        if (rez == "1") GetNameValue("circHLosOpen", m_dog_rez);
        //        if (vent == "1") GetNameValue("calcHLventil", m_dog_ventil);


        CPotrCx dlg(this, strValue);

        if (dlg.DoModal() == IDOK) {

            bool isEl;
            readCxema(dlg.m_nomer, otop, gvs, vent, teh, cond, rez, gvs_all, isEl);

            SetItemValue(item, dlg.m_nomer);

            SetNameValue("calcHLindep", (otop == "n") ? m_otopl : 0);
            SetNameValue("calcHLdep", (otop == "z") ? m_otopl : 0);

            SetNameValue("avgHLGVScloseParall", (gvs == "pr") ? m_gv : 0);
            SetNameValue("avgHLGVScloseMix", (gvs == "sm") ? m_gv : 0);
            SetNameValue("avgHLGVScloseConseq", (gvs == "ps") ? m_gv : 0);
            SetNameValue("avgHLGVSclosePreON", (gvs == "pw") ? m_gv : 0);
            SetNameValue("avgHLGVSopenFlow", (gvs == "op") ? m_gv : 0);
            SetNameValue("avgHLGVSopenRet", (gvs == "oo") ? m_gv : 0);

            SetNameValue("circHLosOpen", (rez == "1") ? m_rez : 0);
            SetNameValue("calcHLventil", (vent == "1") ? m_ventil : 0);


            SetNameValue("contcalcHLindep", (otop == "n") ? m_dog_otopl : 0);
            SetNameValue("contCalcHLdep", (otop == "z") ? m_dog_otopl : 0);

            SetNameValue("contAvgHLGVScloseParall", (gvs == "pr") ? m_dog_gv : 0);
            SetNameValue("contAvgHLGVScloseMix", (gvs == "sm") ? m_dog_gv : 0);
            SetNameValue("contAvgHLGVScloseConseq", (gvs == "ps") ? m_dog_gv : 0);
            SetNameValue("contAvgHLGVSclosePreON", (gvs == "pw") ? m_dog_gv : 0);
            SetNameValue("contAvgHLGVSopenFlow", (gvs == "op") ? m_dog_gv : 0);
            SetNameValue("contAvgHLGVSopenRet", (gvs == "oo") ? m_dog_gv : 0);

            //            SetNameValue("circHLosOpen", (rez == "1") ? m_dog_rez : 0);
            //            SetNameValue("calcHLventil", (vent == "1") ? m_dog_ventil : 0);


            ///            is_potr_real_save = true;

            initCxema(dlg.m_nomer);

            return 1;

            //            return 2;
        }
    }
    else if (fun == "sredn_max") {
        CItem* pItem = FindItem(item);
        if (pItem) {
            CString fn_max = pItem->m_name;
            fn_max.Replace("sredn", "maks");

            CString ss = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");
            double coef24 = atof(ss);
            double d;
            GetNameValue(fn_max, d);
            SetItemValue(item, d / coef24);
        }
    }

    else if (fun == "max_sredn") {
        CItem* pItem = FindItem(item);
        if (pItem) {
            CString fn_max = pItem->m_name;
            fn_max.Replace("maks", "sredn");

            CString ss = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");
            double coef24 = atof(ss);
            double d;
            GetNameValue(fn_max, d);
            SetItemValue(item, d * coef24);
        }
    }
    else if (fun == "heatTestsCoeff") {

        //      CKti dlg(this, const char *kod, double dlina, double diametr, double diametr_usl, double tol, int year, const char *typ_pr, int kolwork,
        //      CTime date_isp_tep, double tn_isp_tep, double tg_isp_tep, double G1_isp_tep, double G2_isp_tep, double dt1_isp_tep, double dt2_isp_tep, double t1_isp_tep, double t2_isp_tep 
        int tubingTypeID, signNumWork;
        double pipeSectLength, diameterInternal, diameterCondit, wallThickness;
        double temperOutAir, temperGround, expendDWflow, expendDWret, temperCoolFlow, temperCoolRet, temperDWflow, temperDWret;

        int kod0;

        int id2 = 0;
        CItem* pItem = FindItem(item);
        if (pItem) {
            id2 = pItem->m_id_bd;
        }

        GetNameValue("externalCodeID", kod0);

        GetNameValue("pipeSectLength", pipeSectLength);
        GetNameValue("diameterInternal", diameterInternal);
        GetNameValue("diameterCondit", diameterCondit);
        GetNameValue("wallThickness", wallThickness);

        GetNameValue("tubingTypeID", tubingTypeID);
        GetNameValue("signNumWork", signNumWork);

        GetNameValue("temperOutAir", temperOutAir);
        GetNameValue("temperGround", temperGround);
        GetNameValue("expendDWflow", expendDWflow);
        GetNameValue("expendDWret", expendDWret);
        GetNameValue("temperCoolFlow", temperCoolFlow);
        GetNameValue("temperCoolRet", temperCoolRet);
        GetNameValue("temperDWflow", temperDWflow);
        GetNameValue("temperDWret", temperDWret);
        string val;

        int year = 1990;

        COleDateTime dt;

        if (GetNameValue("lastTransDate", dt)) {
            year = dt.GetYear();
        }

        CTime date_isp_tep;

        CKti dlg(this, id2, kod0, pipeSectLength, diameterInternal, diameterCondit, wallThickness, year, tubingTypeID, signNumWork,
            date_isp_tep, temperOutAir, temperGround, expendDWflow, expendDWret, temperCoolFlow, temperCoolRet, temperDWret, temperDWret);


        if (dlg.DoModal() == IDOK) {
            double kti;
            if (sscanf(dlg.m_kti, "%lg", &kti) == 1) {
                SetItemValue(item, kti);
            }
        }
    }
    else if (fun == "color") {
        int color;
        GetItemValue(item, color);

        CColorDialog dlgColor(color);
        //      dlgColor.m_cc.lpCustColors = color;

        if (dlgColor.DoModal() == IDOK)
        {
            SetItemValue(item, (long)dlgColor.GetColor());
        }
    }
    else if (fun == "uf") {
        double t1 = 0, t2 = 0, t3 = 0;

        GetNameValue("t1_r", t1);
        GetNameValue("t2_r", t2);
        GetNameValue("t3_r", t3);

        CUf dlg(this, t1, t2, t3);
        if (dlg.DoModal() == IDOK && dlg.m_u > 0) {
            SetNameValue("uf", dlg.m_u);
        }
        return 1;
    }

    else if (fun == "uf150-70") {
        CUf dlg(this, 150, 70, 95);
        if (dlg.DoModal() == IDOK && dlg.m_u > 0) {
            SetItemValue(item, dlg.m_u);
        }
        return 1;
    }


    else if (fun.Find("SELECT") == 0) {
        CString q, txt;

        CItem* pItem = FindItem(item);
        if (pItem) {
            long id = -1;
            bool ret = viewVirtualDlg(this, pItem->m_schema, fun, id, pItem->m_label, txt);

        }
    }
    else if (fun == _TR("Местные сопротивления")) {
        CItem* pItem = FindItem(item);
        if (pItem) {
            int id = pItem->m_id_bd;
            CMSopr dlg(this, id);
            if (dlg.DoModal() == IDOK) {
                SetItemValue(item, dlg.m_sopr);
            }
        }
    }
    else if (fun == _TR("Доля местных потерь")) {
        CItem* pItem = FindItem(item);
        if (pItem) {
            int id = pItem->m_id_bd;
            //            CMSopr dlg(this, id);
            //            if (dlg.DoModal() == IDOK) {
            //                SetItemValue(item, dlg.m_sopr);
            //            }
        }
    }

    else if (fun == "climat1") {
        CString tn = "13_klimaticheskie_dannye";

        CAdoFile* ado = getAdo("sprav");
        if (ado) {
            CString q;
            q.Format("SELECT * FROM [%s]", tn);
            CTableDialog dlg(this, ado->m_type_of_net, "sprav", ado->getConnect(), "Климатические данные", q, 0, virt_data_unknown);
            int ret = dlg.DoModal();
            if (ret == IDOK) {
                SetNameValue("nasel_point", dlg.getValue("sity"));

                SetNameValue("t_or", dlg.getValue("otopl_pr"));
                SetNameValue("t_vr", dlg.getValue("vetn_pr"));

                SetNameValue("tn_god", dlg.getValue("t_sr"));
            }
        }
    }

    else if (fun.Find("Math ") == 0) {
        vector<CString> match;

        int l = regex_match("Math\\s+([^ ]+)$", fun, match);
        if (l >= 1) {
            CString ss = match[1];
            double d = 0;
            if (parse_math(this, ss, d)) {
                ss.Format(_TR("Установить значение поля %g"), d);
                if (AfxMessageBox(ss, MB_YESNO) == IDYES) {
                    SetItemValue(item, d);
                }
            }
        }
        else {
            l = regex_match("Math\\s+([^ ]+) (.+)$", fun, match);
            if (l >= 1) {
                CString ss = match[1];
                CString ss2 = match[2];

                CString c24 = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");
                ss2.Replace("$coef24$", c24);

                double d = 0;
                if (parse_math(this, ss, d)) {
                    ss.Format(_TR("Установить значение поля %g\n%s"), d, ss2);
                    if (AfxMessageBox(ss, MB_YESNO) == IDYES) {
                        SetItemValue(item, d);
                    }
                }
            }
        }
    }

    else if (fun.Find("Table ") == 0) {

        vector<CString> match;
        CString ss;

        int l = regex_match("Table \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)\\s*,\\s*\"(.+)\"\\s*$", fun, match);
        if (l >= 4) {
            CString db = match[1];
            CString tn = match[2];
            CString t3 = match[3];
            CString t4 = match[4];
            int n_ret = atoi(t3);

            if (getTableText(this, db, tn, n_ret, s, t4)) {
                SetItemValue(item, s);
            }
        }
        else {
            int l = regex_match("Table \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)$", fun, match);
            if (l >= 3) {
                CString db = match[1];
                CString tn = match[2];
                CString t3 = match[3];
                int n_ret = atoi(t3);

                if (getTableText(this, db, tn, n_ret, s)) {
                    SetItemValue(item, s);
                }
            }
        }
    }

    else if (fun.Find("TableQ ") == 0) {
        vector<CString> match;
        CString ss;

        int l = regex_match("TableQ \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)$", fun, match);
        if (l >= 4) {
            CString db = match[1];
            CString tn = match[2];
            CString q = match[3];
            CString t4 = match[4];
            int n_ret = atoi(t4);

            int magID;

            GetNameValue("fragment_resultID", magID);
            CString ss = "";
            CString kod_m = "";
            ss.Format("%d", magID);

            q.Replace("$magID$", ss);

            GetNameValue("kod_m", kod_m);
            q.Replace("$kod_m$", kod_m);


            if (getTableTextQ(this, db, tn, q, n_ret, s)) {
                SetItemValue(item, s);
            }
        }
    }


    else if (fun.Find("MenuQ ") == 0) {
        vector<CString> match;
        CString ss;

        int l = regex_match("MenuQ \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9]+)$", fun, match);
        if (l >= 4) {
            CString db = match[1];
            CString tn = match[2];
            CString q = match[3];
            CString t4 = match[4];
            int n_ret = atoi(t4);

            int magID;

            GetNameValue("fragment_resultID", magID);
            CString ss = "";
            CString kod_m = "";
            ss.Format("%d", magID);

            q.Replace("$magID$", ss);

            GetNameValue("kod_m", kod_m);
            q.Replace("$kod_m$", kod_m);


            CMMenu menu(this, IDD_MENU2, tn);

            if (ado->openTable0(q)) {
                while (!ado->isEOF()) {
                    CString txt = ado->readStr(0);
                    menu.lst.Add(txt);
                    ado->MoveNext();
                }
            }

           if (menu.DoModal() == IDOK) {
               SetItemValue(item, menu.m_strText);
           }



//            if (getTableTextQ(this, db, tn, q, n_ret, s)) {
//                SetItemValue(item, s);
//            }
        }
    }


    else if (fun.Find("minmax ") == 0) {
        vector<CString> match;
        CString ss;

        int l = regex_match("minmax\\s+([0-9\\.]+),([0-9\.]+)$", fun, match);
        if (l >= 2) {
            int n1 = atoi(match[1]);
            int n2 = atoi(match[2]);

            double value;

            GetItemValue(item, value);


            CString str = "Выберите значение";

            CItem* pItem = FindItem(item);
            if (pItem) {
                str.Format("Выберите значение %s", pItem->m_label);

                CMinMaxDialog dlg(str);

                dlg.m_min = n1;
                dlg.m_max = n2;
                dlg.m_value = value;
    //            dlg.m_comment = "Привет";

                if (dlg.DoModal()) {
                    if (pItem->m_type == IT_DOUBLE) {
                        SetItemValue(item, (double)dlg.m_value);
                    }
                    else {
                        SetItemValue(item, dlg.m_value);
                    }
                }   
            }

        }
    }


    else if (fun.Find("File ") == 0) {
        CString ext, filter, path;

        if (funFileInfo(fun, ext, filter, path)) {
            CString strValue;
            GetItemValue(item, strValue);

            CString new_value;
            if (viewFile(this, ext, filter, path, strValue, new_value)) {
                SetItemValue(item, new_value);
            }
        }
    }
    else if (fun == "average3") {
        CDialog3 dlg(this);
        if (dlg.DoModal() == IDOK) {
            SetItemValue(item, dlg.m_sum);
        }
    }

    else if (fun == "NoEdit_average31" || fun == "NoEdit_average32") {
        CString slst, s1;
        CString slst1 = "spisok_plastin_pri_ustanovke";
        CString slst2 = "spisok_plastin_posle_ispytaniy";
        double m1, m2, m3;
        double e1, e2, e3;

        if (fun == "NoEdit_average31") {
            slst = slst1;
        }
        else {
            slst = slst2;
            GetNameValue(slst1, s1);
            read3double(s1, m1, m2, m3);
        }

        GetNameValue(slst, s1);

        read3double(s1, e1, e2, e3);

        if (fun != "NoEdit_average31" && e1 == 0 && e2 == 0 && e2 == 0) {
            e1 = m1;
            e2 = m2;
            e3 = m3;
        }

        CDialog3 dlg(this, IDD_DIALOG_3_IND, e1, e2, e3);

        if (fun != "NoEdit_average31") {
            dlg.setMax(m1, m2, m3);
        }

        if (dlg.DoModal() == IDOK) {
            SetItemValue(item, dlg.m_sum);

            CString s;
            s.Format("%g|%g|%g",  dlg.m_e1, dlg.m_e2, dlg.m_e3);
            SetNameValue(slst, s);
        }
    }
    
    else if (fun == "$ind_save") {
        ind_save(this);
    }
    else if (fun == "$ind_result") {
        ind_result(this);
    }
    else if (fun == "$ind_reset") {
        ind_reset(this);
    }
    else if (fun == "hydroRes") {
        double len;
        GetNameValue("pipeSectLength", len);
        CGidResist dlg(this, len);
        if (dlg.DoModal() == IDOK) {
            SetNameValue("hydroRes", dlg.m_edit3);
        }
    }
    else if (fun == "org") {
        CString tn = _TR("Организации проектные");
        CMMenu2 menu(this, IDD_MENU10, "");
        menu.AddColumn(_TR("Наименование"));
        menu.AddColumn(_TR("Телефон организации"));
        menu.AddColumn(_TR("Телефон руководителя"));
        menu.AddColumn(_TR("Улица"));
        menu.AddColumn(_TR("Номер дома"));

        //        CDbFile dbf;
        //        init_menu(&dbf, &menu);

        //        menu.AddProc(IDC_ADD, add, (void*)&dbf, (void*)(const char*)tn);
        //        menu.AddProc(IDC_DEL, del, (void*)&dbf, (void*)(const char*)tn);
        //        menu.AddProc(IDC_ED, edit, (void*)&dbf, (void*)(const char*)tn);

        if (menu.DoModal() == IDOK) {
            CString v = menu.m_strText;
        }
    }
    else if (fun == "Dir") {
        bool getDir(CWnd * wnd, CString & dir);
        CString strValue;
        GetItemValue(item, strValue);

        CString path = strValue;

        if (getDir(this, path)) {
            SetItemValue(item, path);
        }
    }
    else if (fun.Find("FileView") == 0) {
        vector<CString> match;

        int l = regex_match("FileView\\s+\"(.+?)\\s*$", fun, match);
        if (l >= 1) {
            CString aa = match[0];
            CString path = match[1];
        }

        CString strValue;
        GetItemValue(item, strValue);

        HINSTANCE hi = ShellExecute(this->m_hWnd, "open", strValue, NULL, NULL, SW_SHOWNORMAL);
    }
    else if (fun.Find("GoogleDoc") == 0) {
        AfxMessageBox("GoogleDoc", MB_OK | MB_ICONINFORMATION);
    }
    else if (fun.Find("$defekt2_meropr") == 0) {
        defect_meropr("defekt", item);
    }
    else if (fun.Find("$defekt2_doc") == 0) {
        defect_doc("defekt", item);
    }
    else if (fun.Find("$shurf2_meropr") == 0) {
        defect_meropr("shurf", item);
    }
    else if (fun.Find("$shurf2_doc") == 0) {
        defect_doc("shurf", item);
    }
    else if (fun.Find("$view_documents$") == 0) {
        view_documents(fun, item);
    }
    else if (fun.Find("$view_meropr$") == 0) {
        view_meropr(fun, item);
    }
    else if (fun.Find("$view_meropr2$") == 0) {
        view_meropr2(fun, item);
    }
    else if (fun.Find("$view_link$") == 0) {
        view_link(fun, item);
    }
    else if (fun.Find("$view_filtr$") == 0) {
        view_filtr(fun, item);
    }
    else if (fun.Find("$view_UT$") == 0) {
        view_UT(pItem);
    }
    else if (fun.Find("$view_UTBIG$") == 0) {
        view_UTBIG(pItem);
    }


    else if (fun.Find("$ms_view") == 0) {
        CString q, txt;
        CItem* pItem = FindItem(item);
        if (pItem) {
            long id = pItem->m_id_bd;

            q.Format("SELECT DISTINCT ms.id, ms.opisanie_uchastka_ms AS 'Наименование участка МС'"
                " FROM nodes n"
                " JOIN externalCodes ec ON ec.id=n.externalCodeID"
                " JOIN linesobj l ON n.id=l.nodeID1 OR n.id=l.nodeID2"
                " JOIN heatPipeSections hps ON hps.lineID = l.id"
                " JOIN uchastok_ms ms ON ms.id=hps.magistralSite "
                " WHERE n.id=%d", id);


            viewVirtualDlg(this, pItem->m_schema, q, id, pItem->m_label, txt);

        }
    }
    else if (fun.Find("$table_view_") == 0) {
        CString q, txt;
        /*codeName - код объекта птс*/
        CString codeName = fun.Mid(12, fun.GetLength() - 1);
        CItem* pItem = FindItem(item);
        if (pItem) {
            long id = pItem->m_id_bd;

            //q.Format("SELECT tn3.id, CONCAT('Запорная арматура № ', tn3.id,' ( ',ISNULL(el3.name,'признак трубопровада не указан'),' ), ','диаметр: ',ISNULL(diametr, 'не указан'))  name FROM zapornaya_armatura tn3 LEFT JOIN heatPipeSections hps3 ON hps3.lineID = tn3.lineID LEFT JOIN pipeSections ps3 ON ps3.id = hps3.pipeSectionID LEFT JOIN externalSignLine el3 ON el3.id = tn3.priznak_truboprovoda WHERE ps3.id = %d", id);
            CString fStr;
            fStr.Format("%ssql\\field_list\\%s.sql", argpath(), codeName);
            ifstream f(fStr);
            if (f.good())
            {

                fStr.Format("field_list\\%s", codeName);
                if (codeName.Find("shurf") == 0) {
                    codeName = "shurfy";
                }
                else if (codeName.Find("cut_out") == 0 || codeName.Find("shurf") == 0 || codeName.Find("certification") == 0) {
                    codeName = "diag";
                }
                else
                    if (codeName.Find("tk_diagnostika") == 0 || codeName.Find("pavilion_diagnostika") == 0)
                        codeName = "diag";
                    else
                        if (codeName.Find("heat_point_real_consumers") == 0)
                            codeName = "realConsumers";
                        else
                            if (codeName.Find("heat_point_list_contract") == 0)
                                codeName = "realConsumers";
                            else
                                if (codeName.Find("heat_point_buildings") == 0)
                                    codeName = "buildings";
                Klassif* kls = m_geofile->m_kl_list->findKlN(codeName);
                q = readQ(getAdo(getGidAdoName()), fStr, "", id);
                q.Replace("$FragmentIds$", pView->m_cxema->m_par);

                viewVirtualDlg(this, pItem->m_schema, q, id, pItem->m_label, txt, false, false, false, false, codeName);
            }
        }
    }

    else if (fun.Find("$remont_defect_list") == 0) {
        long id = pItem->m_id_bd;
        CString tn = "defect";
        CString osmotr = pItem->m_table;

        CString fn = "objects\\osmotr\\remont_defect_list";

        CString ss, q = readQ(ado, fn, "", "");
        ss.Format("%d", id);
        q.Replace("$id$", ss);

        int n_ret = 0;

        CWebDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo_edit, "Дефекты ремонта", true);
        if (dlg.DoModal() == IDOK) {
        }
    }
    else if (fun.Find("$opres_defect_list") == 0) {
        long id = pItem->m_id_bd;
        CString tn = "defect";
        CString osmotr = pItem->m_table;

        CString fn = "objects\\osmotr\\opres_defect_list";

        CString ss, q = readQ(ado, fn, "", "");
        ss.Format("%d", id);
        q.Replace("$id$", ss);

        int n_ret = 0;

        CWebDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo_edit, "Дефекты опрессовки", true);
        if (dlg.DoModal() == IDOK) {
        }
    }

    else if (fun.Find("$osmotr_defect_list") == 0) {
        long id = pItem->m_id_bd;
        CString tn = "defect";
        CString osmotr = pItem->m_table;

        CString fn = "objects\\osmotr\\osmotr_defect_list";

        CString ss, q = readQ(ado, fn, "", "");
        ss.Format("%d", id);
        q.Replace("$id$", ss);

        int n_ret = 0;

        CWebDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo_edit, "Дефекты осмотра", true);
        //        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_edit_add_del, "Нарушения", IDD_TABLE_DEFECT1);
        if (dlg.DoModal() == IDOK) {
            Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
            if (kls) {
                //                    pView->GeoInfo(tn, dlg.m_id, false);
                //                    ado_info(pView, kls->m_ado, tn, dlg.m_id, kls->RusName(), TP_GEO, NULL, false);
            }
        }
    }
    else if (fun.Find("$diag_defect_list") == 0) {
        long id = pItem->m_id_bd;
        CString q, txt;

        CString tn = "defect";
        CString osmotr = pItem->m_table;

        q.Format(
            "select def.id,\n"
            "ISNULL(def.defectDescription, '-') AS 'Описание дефекта', "
            "ISNULL(FORMAT(def.data_osmotra, 'dd-MM-yyyy'), '-') as 'Дата осмотра'\n"
            "from defect def\n"
            //            "join defectsForShurfy dfd ON dfd.defectID = def.id\n"
            //            "AND dfd.shurfyID = %d", id);
            "join defectsFor%s dfd ON dfd.defectID = def.id\n"
            "AND dfd.objID = %d", osmotr, id);

        //         viewVirtualDlg(this, pItem->m_schema, q, id, pItem->m_label, txt, false, false, false, true, tn);


        q = readQ(ado, "objects\\defect\\shurf_defect", "", "");

        CString ss;
        ss.Format("%d", id);

        q.Replace("$ID$", ss);


        int n_ret = 0;

        CWebDialog dlg(pView,  ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo_edit, "Дефекты", true);

//        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_edit_add_del, "Нарушения", IDD_TABLE_DEFECT1);

        if (dlg.DoModal() == IDOK) {
             Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
             if (kls) {
//                 ado_info(pView, kls->m_ado, tn, dlg.m_id, kls->RusName(), TP_GEO, NULL, false);
             }
        }
    }
    else if (fun.Find("$diag_defect_new") == 0) {
        long id = pItem->m_id_bd;
        CString tn = pItem->m_table;

        diag_defect_new(this, id, tn);
    }
    else if (fun.Find("$osmotr_defect_new_pick") == 0) {
        long id = pItem->m_id_bd;
        pView->setOpresNull();
        pView->m_id_osmotr2 = id;

        Klassif* kls = m_geofile->m_kl_list->findKlN("defect");
        if (kls) pView->addKls(kls);
    }
    else if (fun.Find("$opres_defect_new_pick") == 0) {
        long id = pItem->m_id_bd;
        pView->setOpresNull();
        pView->m_id_opr = id;

        Klassif* kls = m_geofile->m_kl_list->findKlN("defect");
        if (kls) pView->addKls(kls);
    }
    else if (fun.Find("$osmotr_defect_new") == 0) {
        long id = pItem->m_id_bd;

        osmotr_defect_new(this, id);
    }

    else if (fun.Find("$rs_view") == 0) {
        CString q, txt;
        CItem* pItem = FindItem(item);
        if (pItem) {
            long id = pItem->m_id_bd;

            q.Format("SELECT DISTINCT"
                " rs.id, rs.naimenovanie_uchastka_rs AS 'Наименование участка РС'"
                " FROM nodes n"
                " JOIN externalCodes ec ON ec.id=n.externalCodeID"
                " JOIN linesobj l ON n.id=l.nodeID1 OR n.id=l.nodeID2"
                " JOIN heatPipeSections hps ON hps.lineID = l.id"
                " JOIN uchastok_rs rs ON rs.id=hps.distSite "
                " WHERE n.id=%d", id);

            viewVirtualDlg(this, pItem->m_schema, q, id, pItem->m_label, txt);
        }
    }
    else if (fun.Find("$defect_elem") == 0) {
        long id = pItem->m_id_bd;

        CDefectElem dlg(id, this);
        dlg.DoModal();
    }





    // Ремонты в участке
  /*

    $remont_defekt2 Дефекты
    $remont_shurf2 Шурфы
    $remont_korroz Индикаторы коррозии

              case ID_OPRES_ZADV:        fn = _TR("Опрессовка секционирующая задвижка"); break;
              case ID_REMONT_DEFECT:     fn = STR_REMONT_DEFECT;   break;
              case ID_REMONT_DEFECT2:    fn = STR_REMONT_DEFECT2;   break;
              case ID_REMONT_DEFECT_OPR: fn = _TR("Опрессовка дефект");   break;
              case ID_REMONT_SHURF:      fn = STR_REMONT_SHURF;    break;
              case ID_REMONT_SHURF2:      fn = STR_REMONT_SHURF2;    break;
              case ID_REMONT_CONTROL_TU:  fn = STR_REMONT_CONTROL_TU;    break;
              case ID_REMONT_OSMOTR:     fn = STR_REMONT_OSMOTR;   break;
              case ID_REMONT_KORROZ:     fn = STR_REMONT_KORROZ;   break;
              case ID_REMONT_ISPYT:      fn = STR_REMONT_ISPYT;    break;
              case ID_REMONT_KAPITAL:    fn = STR_REMONT_KAPITAL;  break;
  */

    else if (fun.Find("$remont_defekt2") == 0) {
        remont_uch(ID_REMONT_DEFECT2, item);
    }
    else if (fun.Find("$remont_shurf2") == 0) {
        remont_uch(ID_REMONT_SHURF2, item);
    }
    else if (fun.Find("$remont_korroz") == 0) {
        remont_uch(ID_REMONT_KORROZ, item);
    }
    else if (fun.Find("$control_tu") == 0) {
        remont_uch(ID_REMONT_CONTROL_TU, item);
    }


    else if (fun.Find("povr") == 0) {
        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tn1 = pItem->m_table;
            int id = pItem->m_id_bd;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
            if (kls) {
                CAdoFile* ado = kls->m_ado;
                CString tn = "defekt2_meropriyatiya";
                CString q;
                int n_ret = 1;
                q.Format("SELECT d.id, d.defektID, d.povrezhdeniya, p.znachenie AS povr, d.meropriyatie, m.znachenie AS znach, plan FROM defekt2_meropriyatiya d"
                    " LEFT JOIN opressovka_povrezhdeniya p ON p.id=d.povrezhdeniya "
                    " LEFT JOIN opressovka_meropriyatiya_po_ustraneniyu_povrezhdeniy m ON m.id=d.meropriyatie "
                    " WHERE defektID=%d"

                    , id);

                CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Мероприятия"), q, n_ret, virt_data_edit_add_del);
                dlg.SetKey("defektID", id);
                dlg.setHead("", tn);

                dlg.DoModal();
            }
        }
    }
    else if (fun == "ms_rs") {
        CItem* pItem = FindItem(item);

        viewLookup(pItem);

        if (pItem->m_index != -1) {
            if (pItem->m_name == "magistral") {
                SetNameNull("distSite");
            }
            else if (pItem->m_name == "magistralSite") {

                CString q;

                int id_ms;
                if (GetNameValue("magistralSite", id_ms) && id_ms > 0) {
                    q.Format("SELECT magistral FROM uchastok_ms WHERE id=%d", id_ms);
                    long id = read_long_db(ado, q, "magistral");
                    if (id > 0) {
                        SetNameValue("magistral", id);
                    }
                    else {
                        SetNameNull("magistral");
                    }
                }

                SetNameNull("distSite");
            }
            else if (pItem->m_name == "distSite") {
                SetNameNull("magistral");
                SetNameNull("magistralSite");
            }
        }

    }
    else if (fun == "standardDampLink") {
        CItem* pItem = FindItem(item);

        if (pItem->m_index != -1) {
            int id;
            if (GetItemValue(item, id)) {
                CString q;
                q.Format("SELECT d FROM standardDampers WHERE id=%d", id);
                double d = read_double_db(ado, q, "d");
                SetNameValue("diameterCondit", d);
            }
        }
    }
    else if (fun == "nodeName") {  // Название ПС по обычному
        CString name;
        CString kod_id;
        GetNameValue("externalCodeID", kod_id);
        GetNameValue("externalNodeName", name);

        int numb = getNumb(kod_id);

        CString ss;
        ss.Format("%s(%d)", name, numb);
        SetNameValue("nodeName", ss);

        //        CString ss = m_graph->getKod(kod_id);
        //        AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);

    }
    else if (fun == "length") {  // Длина
        CItem* pItem = FindItem(item);
        if (pItem) {
            CString tn1 = pItem->m_table;
            int id = pItem->m_id_bd;


            Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
            if (kls) {
                CAdoFile* ado = kls->m_ado;
                CString q;
                q.Format("SELECT shape.STLength() AS len FROM [%s] WHERE id=%d", tn1, id);
                if (ado->openTable0(q)) {
                    if (!ado->isEOF()) {
                        double len = ado->read_double(0);
                        SetItemValue(item, len);
                    }
                }
            }
        }
    }
    else if (fun == "nagruz_ot" || fun == "nagruz_vt") {  // Нагрузка потребителя или вентиляции

        if (fun == "nagruz_ot") {
            Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
            if (kls) {
                return prop_fun("alsecoNagr", item);
            }
        }
        
        CNagruzZdan dlg(this, fun == "nagruz_ot");


        int buildingTypeID = 1;

        GetNameValue("calcTempHR", dlg.m_tvn);
        GetNameValue("buildingTypeID", buildingTypeID);

        //        GetNameValue("calcTempVS", dlg.m_tvn);

        //        GetStringFromBaseWhere(ss1,BaseType,SYSTEMT_NAME, "t_or", "");
        //        GetStringFromBaseWhere(ss2,BaseType,SYSTEMT_NAME, "t_vr", "");

        long lParam = (fun == "nagruz_ot") ? 5 : 6;

        double t_or = 0, t_vr = 0;

        if (ado && ado->openTable0("SELECT * FROM heatSystem")) {
            if (!ado->isEOF()) {
                t_or = ado->read_double("t_or");
                t_vr = ado->read_double("t_vr");
            }
            ado->closeTable();
        }

        //        GetNameValue("calcTempHR", t_or);
        //        GetNameValue("calcTempVS", t_vr);


        /*
                GetStringFromBaseLong(s1,BaseType,REPOTREB_BUILDING_NAME,"name_building",REPOTREB_BUILDING_KOD,kodval);
                GetStringFromBaseLong(s2,BaseType,REPOTREB_BUILDING_NAME,"name_zd",REPOTREB_BUILDING_KOD,kodval);
                GetStringFromBaseLong(s3,BaseType,REPOTREB_BUILDING_NAME,"year",REPOTREB_BUILDING_KOD,kodval);

                GetStringFromBaseLong(s5,BaseType,REPOTREB_BUILDING_NAME,"q_ot",REPOTREB_BUILDING_KOD,kodval);
                GetStringFromBaseLong(s6,BaseType,REPOTREB_BUILDING_NAME,"q_vt",REPOTREB_BUILDING_KOD,kodval);
                */

                //        int m, y, d;
                //        if (sscanf(s3, "%d.%d.%d", &m, &d, &y) == 3) {
                //          dlg.m_god = y;
                //        }

        GetNameValue("buildingVolume", dlg.m_v);

        if (lParam == 5) {
            //          dlg.m_udel = atof(s5);
            dlg.m_tn = t_or;
        }
        else {
            //          dlg.m_udel = atof(s6);
            dlg.m_tn = t_vr;
        }
        //        dlg.m_tvn = atof(ss4);
        dlg.m_nagr = 0.0;


        if (buildingTypeID == 1) {
            dlg.m_nazn = "Жилое";
        }

        if (dlg.DoModal() == IDOK) {
            SetItemValue(item, dlg.m_nagr);
        }
    }
    else if (fun == "NagrSum") {  // Нагрузка температурного графика
        int id = pItem->m_id_bd;

        CNagrSum dlg(this, id);
        if (dlg.DoModal() == IDOK) {
            SetItemValue(item, dlg.m_edit_sum);
            //            SetNameValue("q_r", dlg.m_edit1);
            //            SetNameValue("q_gv", dlg.m_edit3 + dlg.m_edit4 + dlg.m_edit5);
        }
    }

    else if (fun == "viewNsDb") {  // Список насосов
        viewNsDb(pItem->m_id_bd);
    }
    else if (fun == "nodes") {  // Список всех узлов
        int nodes_list(CWnd * wnd, CString & txt);
        CString txt;
        int id = nodes_list(this, txt);

        if (id > 0 || id == -1) {
            pItem->m_strValue = txt;
            pItem->m_index = id;
            pItem->m_undefined = pItem->m_index == -1;
            pItem->m_modified = true;
            itemChanged(pItem);
            Invalidate();
        }
    }
    else if (fun == "nodes2") {  // Список всех узлов
//        q.Format("SELECT * FROM [%s]", transl(tn));
        CString readQ(CAdoFile * ado, const char* tn, const char* tn2, const char* fileID);


        CString q = readQ(ado, "sql2\\find_node2", "", pView->m_cxema->m_par);

        CString tn = "1";
        CString title = "2";
        int n_ret = 1;


        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title ? title : tn, q, n_ret, virt_data_unknown, "Узлы", IDD_TABLE_DEFECT3);
        int ret = dlg.DoModal();
        if (ret == IDOK) {
            CString s = dlg.getValue("id");
            s.Replace(" ", "");
            s.Replace("\xA0", "");

            int id = atoi(s);
            CString txt = dlg.getValue("Наименование");

            pItem->m_strValue = txt;
            pItem->m_index = id;
            pItem->m_undefined = pItem->m_index == -1;
            pItem->m_modified = true;
            itemChanged(pItem);
            Invalidate();
            return true;
        }
    }
    else if (fun == "node_pick") {  // Выбрать узел
        AfxMessageBox("Укажите узел на схеме", MB_OK | MB_ICONINFORMATION);
        pView->regim = R_NODE_PICK;
        pView->m_prop_grid = this;
        pView->m_prop_field = pItem->m_name;
    }
    else if (fun.Find("node_pick$") == 0) {  // Выбрать узел с расстоянием
        AfxMessageBox("Укажите узел на схеме", MB_OK | MB_ICONINFORMATION);

        vector<CString> match;

        pView->m_field_for_dist = "";

        int l = regex_match("node_pick\\$(.+)$", fun, match);
        if (l >= 1) {
            pView->m_field_for_dist = match[1];
            int id = pItem->m_id_bd;
            pView->m_pt_object = get_coord1(pItem->m_table, id);
        }

        pView->regim = R_NODE_PICK;
        pView->m_prop_grid = this;
        pView->m_prop_field = pItem->m_name;
    }
    else if (fun == "node_pick1" || fun == "node_pick2") {  // Выбрать узел
        AfxMessageBox("Укажите узел на схеме", MB_OK | MB_ICONINFORMATION);
        pView->regim = fun == "node_pick1" ? R_NODE_PICK1 : R_NODE_PICK2;
        pView->m_prop_grid = this;
        pView->m_prop_field = pItem->m_name;
    }
    else if (fun.Find("$zhurnal_defect_word") == 0) {  // Журнал нарушений на участке
        int id = pItem->m_id_bd;
        zhurnal_defect_word(this, id);
    }
    else if (fun.Find("$zhurnal_defect") == 0) {  // Журнал нарушений на участке
        int id = pItem->m_id_bd;
        zhurnal_defect(this, id);
    }
    else if (fun.Find("$zhurnal_shurfy") == 0) {  // Журнал нарушений на участке
        int id = pItem->m_id_bd;
        zhurnal_shurfy(this, id);
    }
    else if (fun.Find("$zhurnal_remont") == 0) {  // Журнал ремонтов на участке
        int id = pItem->m_id_bd;
        zhurnal_remont(this, id);
    }
    else if (fun == "$akt_rasledovania") {
        int id = pItem->m_id_bd;
        akt_rasledovania(this, id);
        void InfoAreaDoc(CWnd * wnd);
        //InfoAreaDoc(this);
    }
    else if (fun == "$karta_povrezhaemosti") {
        int id = pItem->m_id_bd;
        karta_povrezhdaemosti(this, id);
    }
    else if (fun == "$kamery_list_for_defect") {
        int id = pItem->m_id_bd;
        kamery_list_for_defect(this, id);
    }
    else if (fun == "$spisok_otkl_potr") {  // Список отключенных потребителей
        int id = pItem->m_id_bd;
        spisok_otkl_potr(this, id);
    }
    else if (fun == "utverdit") {
        CItem* pItem = FindItem(item);

        viewLookup(pItem);
        setCxema1("utverdit");
        Invalidate();
    }
    else if (fun.Find("$faktory_riska_truboprovoda$") == 0) {  // Факторы риска для шурфа

        CItem* pItem = FindItem(item);
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        faktory_riska_truboprovoda_shurf(this, tn, id, 1, fun);
    }
    else if (fun == "$faktory_riska_truboprovoda_osmotr$") {  // Факторы риска
        CItem* pItem = FindItem(item);
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        faktory_riska_truboprovoda_osmotr(this, tn, "Осмотр трубопроводов контура", id, 2);
    }
    else if (fun == "$faktory_riska_truboprovoda_remont$") {  // Факторы риска
        CItem* pItem = FindItem(item);
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        faktory_riska_truboprovoda_osmotr(this, tn, "Ремонт", id, 3);
    }
    else if (fun == "$akt_na_osmotr_tepl$") {
        int id = pItem->m_id_bd;
        akt_shurfovki(this, id);
        //void OpresTableAkt(CPropertyGrid* wnd, int id);
        //OpresTableAkt(this, 8);


        //void PlanOpresProbation(CWnd* wnd);
        //void GraphOpresProbation(CWnd* wnd);
        //PlanOpresProbation(this);
        //GraphOpresProbation(this);
    }
    else if (fun == "$shurf_utverdit") {
        shurf_utverdit(this);
    }
    else if (fun == "$pipeSections_osmotr_history") {
        int id = pItem->m_id_bd;
        pipeSections_osmotr_history(this, id);
    }
    else if (fun == "$pipeSections_remont_history") {
        int id = pItem->m_id_bd;
        pipeSections_remont_history(this, id);
    }
    else if (fun == "$pipeSections_faktory_riska") {
        int id = pItem->m_id_bd;
        pipeSections_faktory_riska(this, id);
    }
    else if (fun == "$remont_utverdit") {
        int id = pItem->m_id_bd;
        remont_utverdit(this);
    }
    else if (fun == "$list_ms") { // Описание магистральных сетей контура
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_ms(this, tn, id);
    }
    else if (fun == "$list_rs") { // Описание распределительных сетей контура 
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_rs(this, tn, id);
    }
    else if (fun == "$list_uch") { // Описание участков ПТС
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_uch(this, tn, id);
    }
    else if (fun == "$list_PT") { // Список подключаемых потребителей к контуру
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_PT(pView, ado, this, tn, id);
    }
    else if (fun == "$granizy_razdela_opr") { // Границы раздела
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        granizy_razdela_opr(pView, ado, this, tn, id);
    }
    else if (fun == "$list_US_1") { // Список узлов с манометрами
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_US(pView, ado, this, tn, id, "list_opres_node1", "Выберите места установки манометров");
    }
    else if (fun == "$list_US_2") { // Список узлов с 
        CString tn = pItem->m_table;
        int id = pItem->m_id_bd;
        list_US(pView, ado, this, tn, id, "list_opres_node2", "Выберите места установки расходомеров");
    }
    else if (fun == "$OpresTableAkt") { // Акт на проведение гидравлических испытаний
        int id = pItem->m_id_bd;
        void OpresTableAkt(CPropertyGrid* wnd, int id);

        OpresTableAkt(this, id);
    }
    else if (fun == "alsecoNagr") {
        int id = pItem->m_id_bd;
        CGidrView* view = getView();

        if (!AlsecoNagrMark(this, id, view, pItem->m_table, pItem->m_name)) {
            AfxMessageBox("Выберите здание с нагрузками");

            pView->regim = R_ALSECO_PICK;
            pView->m_prop_grid = this;
            pView->m_prop_table = pItem->m_table;
            pView->m_prop_field = pItem->m_name;
        }
    }
    else if (fun == "alseco_nagr") { //Нагрузки Alseco
        int id = pItem->m_id_bd;
        AlsecoNagr(this, id);
    }

    else {
        AfxMessageBox(fun, MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}

bool check_unique(CString table, CString fn, int id, CString val)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return true;

    CString s, q;
    s.Format("%s %s %d\n%s", table, fn, id, val);

    q.Format("SELECT id FROM %s WHERE id <> %d AND %s='%s'", table, id, fn, val);

    return isEmptyQ(ado, q);
}

/*
  tubingTypeID Тип прокладки

  firstPICdateHP Дата первичного ввода в эксплуатацию
  DiamUslov Диаметр условный трубопровод, мм
  pipeLength Протяженность, м
  organizationID Балансовая принадлежность участка

  
  objectTypeID Назначение трубопровода
  Sreda Рабочая среда
  press Рабочее давление, м.вод.ст
  temperature Рабочая температура, Град.C
  SpokSluzhbiRash Расчетный срок службы, лет
  ressurs Расчетный ресурс,час
  

  
  lastTransDate Дата последней перекладки
  lastIsolDate Дата последней замены изоляционной конструкции
  standart Стандарт на изготовление трубопровода
  inventNumber Инвентарный номер
  tubeTypeID Материал трубы
  pusk Расчетное число пусков, раз
*/


bool getTG(long hSourceID, double tn, double& t1, double& t2, double& t3, double& tv);


#if 0

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

void ShowCustomThreeButtonDialog()
{
    TASKDIALOGCONFIG tdc = { sizeof(tdc) };
    tdc.hwndParent = AfxGetMainWnd()->GetSafeHwnd();
    tdc.hInstance = AfxGetInstanceHandle();
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.pszWindowTitle = L"Подтверждение действия";
    tdc.pszMainIcon = TD_WARNING_ICON;
    tdc.pszMainInstruction = L"Что вы хотите сделать с файлом?";
    tdc.pszContent = L"Файл уже существует. Выберите действие:";

    TASKDIALOG_BUTTON buttons[] = {
        { 100, L"Да (перезаписать)" },
        { 101, L"Нет (пропустить)" },
        { 102, L"Переименовать автоматически" }
    };

    tdc.cButtons = _countof(buttons);
    tdc.pButtons = buttons;
    tdc.nDefaultButton = 100;

    int nClicked = 0;
    TaskDialogIndirect(&tdc, &nClicked, nullptr, nullptr);

    switch (nClicked)
    {
    case 100: AfxMessageBox("Выбрано: Да"); break;
    case 101: AfxMessageBox("Выбрано: Нет"); break;
    case 102: AfxMessageBox("Выбрано: Переименовать"); break;
    default:  AfxMessageBox("Диалог закрыт"); break;
    }
}

#endif

#include "CustomYesNoDlg.h"

int ShowCustomThreeButtonDialog(CWnd *wnd, const CString &str)
{
    CCustomYesNoDlg dlg(wnd, str, "");

    dlg.DoModal();

    return dlg.m_result;
}


bool CPropertyGrid::SetNameValueAsk(CString name, double value, bool &ask, int & num)
{
    double value_old;

    GetNameValue(name, value_old);

    if (ask && value != value_old) {
        CItem* item = FindItemName(name);
        if (item) {
            CString str;
//            str.Format("Значение %s поменялось\n, было %g стало %g\n изменить?", item->m_label, value_old, value);
            str.Format("Установить значение %s в %g ?", item->m_label, value);

            int ret = ShowCustomThreeButtonDialog(this, str);

            if (ret == CCustomYesNoDlg::RES_NO) {
//            if (AfxMessageBox(str, MB_YESNO) != IDYES) {
                return false;
            }
            if (ret == CCustomYesNoDlg::RES_SOMETHING) {
                ask = false;
            }

            num += 1;
        }

    }

    return SetNameValue(name, value);
}

bool CPropertyGrid::SetNameValueAsk(CString name, int value, bool &ask, int & num)
{
    int value_old;

    GetNameValue(name, value_old);

    if (ask && value != value_old) {
        CItem* item = FindItemName(name);
        if (item) {
            CString str;
//            str.Format("Значение %s поменялось\n, было %g стало %g\n изменить?", item->m_label, value_old, value);
            str.Format("Установить значение %s в %d ?", item->m_label, value);

//            if (AfxMessageBox(str, MB_YESNO) != IDYES) {

            int ret = ShowCustomThreeButtonDialog(this, str);

            if (ret == CCustomYesNoDlg::RES_NO) {
                return false;
            }
            if (ret == CCustomYesNoDlg::RES_SOMETHING) {
                ask = false;
            }
            num += 1;

        }
    }

    return SetNameValue(name, value);
}



bool CPropertyGrid::SetNameValueAsk(CString name, COleDateTime & value, bool &ask, int & num)
{
    COleDateTime value_old;

    GetNameValue(name, value_old);

    if (ask && value != value_old) {
        CItem* item = FindItemName(name);
        if (item) {

            if (value.GetStatus() != 0 || value == 0) {
                return true;
            }

//            if (value)

            CString str;
//            str.Format("Значение %s поменялось\n, было %g стало %g\n изменить?", item->m_label, value_old, value);
            CString s = value.Format(_T("%d-%m-%Y"));

            str.Format("Установить значение %s в %s ?", item->m_label, s);


//            if (AfxMessageBox(str, MB_YESNO) != IDYES) {
            int ret = ShowCustomThreeButtonDialog(this, str);

            if (ret == CCustomYesNoDlg::RES_NO) {
                return false;
            }
            if (ret == CCustomYesNoDlg::RES_SOMETHING) {
                ask = false;
            }
            num += 1;
        }

    }

    return SetNameValue(name, value);
}


CString getVirtualDlgText(const char* bd, const char* q, long id);


bool CPropertyGrid::SetNameValueLAsk(CString name, int value, bool &ask, int & num)
{
    int value_old;

    GetNameValue(name, value_old);

    if (ask && value != value_old) {
        CItem* item = FindItemName(name);
        if (item) {
            CString str;
//            str.Format("Значение %s поменялось\n, было %d стало %d\n изменить?", item->m_label, value_old, value);
            CString strV = getVirtualDlgText(item->m_schema, item->m_strLookup, value);


            str.Format("Установить значение %s в %s ?", item->m_label, strV);

//            if (AfxMessageBox(str, MB_YESNO) != IDYES) {
            int ret = ShowCustomThreeButtonDialog(this, str);

            if (ret == CCustomYesNoDlg::RES_NO) {
                return false;
            }
            if (ret == CCustomYesNoDlg::RES_SOMETHING) {
                ask = false;
            }
            num += 1;
        }
    }

    return SetNameValue(name, value);
}

void CPropertyGrid::copyPts()
{
    double diam;

    CGidrView* pView = getView();
    if (!pView) return;

    CAdoFile* ado = pView->m_cxema->m_ado;

    GetNameValue("DiamUslov", diam);

    int mag = diam >= 400 ? 1 : 2;

    CItem* item = FindItemName("name");

    double t150 = 150;

    if (item) {
        int id = item->m_id_bd;
        CLINE2 *l = pView->m_cxema->m_graph->find_line_big(id);

        if (l) {
            if (bline(l)->line.ms > 0) {
                mag = 1;
            }
            if (bline(l)->line.rs > 0) {
                mag = 2;
            }


            CString q;

            CNode2 *n = where(l);
            const RasCx* cx = pView->m_cxema->getPC(n->node.id_kod);


            if (cx) {
                q.Format("SELECT t1_r FROM %s WHERE id=%d", "heatSources", cx->heatSourceID);
                vector<CString> vv;

                bool ret = readTableValues(ado, q, 1, vv);

                if (ret) {
                    t150 = atof(vv[0]);
                }
            }
        }
    }


    bool ask = true;
    int num = 0;

    SetNameValueLAsk("objectTypeID", mag, ask, num); //  1/2 магистраль/распредсеть
    SetNameValueLAsk("Sreda", 1, ask, num);       // 1 - Вода
    SetNameValueAsk("press", 160.0, ask, num);
    SetNameValueAsk("temperature", t150, ask, num);
    SetNameValueAsk("SpokSluzhbiRash", 30, ask, num);
    SetNameValueAsk("ressurs", 3000, ask, num);

    if (num == 0) {
        AfxMessageBox("Все данные уже заполнены");
    }

}



void CPropertyGrid::copyHps()
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return;

    CString q;
    CItem* item = FindItemName("name");

    if (item) {
        int id = item->m_id_bd;

        q.Format("SELECT max(hps.firstPICdateHP) as firstPICdateHP, max(hps.diameterCondit) as diameterCondit, sum(hps.pipeSectLength) as pipeSectLength, max(l.organizationID) as organizationID FROM heatpipesections hps JOIN linesobj l on l.id=hps.lineID WHERE pipesectionid=%d", id);
//        AfxMessageBox(q);

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                COleDateTime firstPICdateHP = ado->read_date("firstPICdateHP");
                long organizationID = ado->read_long("organizationID");

                //COleDateTime firstPICdateHP = ado->read_date("firstPICdateHP");

                double diameterCondit = ado->read_double("diameterCondit");
                double pipeSectLength = ado->read_double("pipeSectLength");

                ado->MoveNext();

                bool ask = true;
                int num = 0;

                SetNameValueAsk("firstPICdateHP", firstPICdateHP, ask, num);
                SetNameValueAsk("DiamUslov", (int)diameterCondit, ask, num);
                SetNameValueAsk("pipeLength", pipeSectLength, ask, num);
                SetNameValueLAsk("organizationID", organizationID, ask, num);

                if (num == 0) {
                    AfxMessageBox("Все данные уже заполнены");
                }

            }
            ado->closeTable();
        }
    }
}
