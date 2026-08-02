// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "Pjezo.h"
#include "win.h"
#include "mmenu.h"
#include "FileList.h"
#include "TimeDial.h"
#include "NewPassport/CWebViewTable.h"
#include "win.h"

#include "Filtr3.h"

#include "dialog_pr.h"
#include <fstream>

#include "geodez.h"

#include "electro.h"

#include "wm_user.h"

#include "OpcParam.h"


void startWeb(CGidrView* view);

int getOutID(CAdoFile* ado, int fileID);


void set_Central_Meridian(double _Central_Meridian);
void set_False_Easting(double _False_Easting);

BOOL isEditMain();

bool is_water_line(const char* tn);
bool is_water_node(const char* tn);


#include "LineStyle.h"
#include "KlMenu.h"


#include "virtualnodelist.h"
#include "VirtualDlg.h"

#include "PtsAlmaM2.h"
#include "ps_alma.h"

#include "TaskProgressDlg.h"

//#include "MenuProverka.h"


//#include "virtlistDoc.h"
#include "virtlistView.h"
#include "adolist.h"

//#include "GridView.h"

//#include "DialogNew2.h"

#include "maptabs.h"

#include <process.h>    /* _beginthread, _endthread */

#include "maps.h"

#include "graph2.h"

#include "tables.h"
#include "menuitem.h"

#include "ado.h"
CString findTableRusNameGeo(CString n);
void create_b5(CAdoFile* ado, CNode2* nP, CCxema* cxema);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void Status(int n, LPCTSTR txt);
CString get_Select(const char* tn, const CNode* parent = NULL);
CString GetTitle(CString path);
CString GetExt(const char* path);
int excel(const char* path);
void bmp2bbb(const char* pcxN, const char* bbbN);
void gif2bbb(const char* pcxN, const char* bbbN);
void jpg2bbb(const char* pcxN, const char* bbbN);
void tif2bbb(const char* pcxN, const char* bbbN);

void find_line_p(const CLINE2* line, CFPoint p, CFPoint& point1, CFPoint& point2);
void dxf_min_max(cdxf* dxf, double& xmin, double& xmax, double& ymin, double& ymax);

CString getPSTable(int n)
{
    switch (n) {
    case PS_PAV: return _TR("Павильон");
    case PS_TRP: return _TR("ТРП");
    case PS_TK: return _TR("Тепловая камера");
    case PS_POD: return _TR("Подземный узел");
    case PS_NAD: return _TR("Надземный узел");
    case PS_VV: return _TR("Ввод здание");
    case PS_NEUST: return _TR("Неустановленный узел");
    case PS_NS: return _TR("Насосная станция");
    case PS_IS: return _TR("Источник тепла");

    // Добавили 2023-12-02

    case PS_VP: return _TR("Вертикальный подъем/опуск трубопровода");
    case PS_VC: return _TR("Границы вертикальных компенсаторов");
    case PS_OP: return _TR("Камера опуска/подъёма");
    case PS_PD: return _TR("Переход диаметра");
    case PS_TP: return _TR("Тепловой пункт");
    case PS_UP: return _TR("Угол поворота трубопровода");
    case PS_UK: return _TR("Узел канала");

    }


    return "";
}


CString getPSTableEng(int n)
{
    switch (n) {
    case PS_PAV: return _TR("pavilions"); // Павильон
    case PS_TRP: return _TR("trps"); // ТРП
    case PS_TK: return _TR("heatChambers"); // Тепловая камера
    case PS_POD: return _TR("undergroundNodes"); // Подземный узел
    case PS_NAD: return _TR("overgroundNodes"); // Надземный узел
    case PS_VV: return _TR("buildingEntries"); // Ввод здание
    case PS_NEUST: return _TR("uninstalledNodes"); // Неустановленный узел
    case PS_NS: return _TR("pumpStations"); // Насосная станция
    case PS_IS: return _TR("heatSources"); // Источник тепла

    // Добавили 2023-12-02

    case PS_VP: return _TR("");
    case PS_VC: return _TR("");
    case PS_OP: return _TR("");
    case PS_PD: return _TR("");
    case PS_TP: return _TR("");
    case PS_UP: return _TR("");
    case PS_UK: return _TR("");

    }
    return "";
}

BOOL isDeletable(const char* fn1, int n, bool isMessage) {
    CString q;

    CString fn = transl(fn1);
    fn.MakeLower();

    CAdoFile* ado = getAdo(getPsAdoName());

    if (ado) {
        if (fn == transl("здания ту")) {
            q.Format("SELECT * FROM %s WHERE %s = %d", transl("Технические условия"), transl("Здание"), n);
            if (getCountTable2(ado, q) > 0) {
                AfxMessageBox(_TR("Нельзя удалить здание с заданными техническими условиями"), MB_OK | MB_ICONINFORMATION);
                return FALSE;
            }
        }
        else if (fn == transl("здания потребителей")) {
            q.Format("SELECT * FROM %s WHERE %s = %d", transl("Жилье"), transl("Здание"), n);
            if (getCountTable2(ado, q) > 0) {
                AfxMessageBox(_TR("Нельзя удалить здание с заданными Нагрузками"), MB_OK | MB_ICONINFORMATION);
                return FALSE;
            }
            q.Format("SELECT * FROM %s WHERE %s = %d", transl("Организации"), transl("Здание"), n);
            if (getCountTable2(ado, q) > 0) {
                AfxMessageBox(_TR("Нельзя удалить здание с заданными Нагрузками"), MB_OK | MB_ICONINFORMATION);
                return FALSE;
            }
        }
    }

    return TRUE;
}

extern int is_geobaza;
extern int demoversion;
extern bool update_now;

bool setMarkNode1Value(CAdoFile* ado, CGraph2* graph, const char* fn, const char* val, bool all);

extern CSemaphore c_s_opc;

BOOL m_bIsMas;
double m_MasPic = 1500;
double m_MasPodp = 5000;

TypMenu m_idMenu = TYP_MENU_REZHIM;
int m_bIsRezhim = 0;

extern CGeoFile* m_geofile;

CString m_pathMap = "";

CString getPathMap()
{
    return m_pathMap;
}


static int m_isMapYes = 0;


int getMapYes()
{
    return m_isMapYes;
}

void setMapYes(int mapYes)
{
    m_isMapYes = mapYes;
}



void SetBar(CView* pView);

CPtsAlmaM2* getCPtsAlmaM2();

void setAddr()
{
    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (a) a->SetKls(m_geofile->m_kl_list);

    CAddressLeft* a2 = (CAddressLeft*)getDockablePane(IDD_ADDRESS4);
    if (a2) a2->SetKls(m_geofile->m_kl_list);

    CLeft4Dialog* b = (CLeft4Dialog*)getDockablePane(IDD_LEFT4);
    if (b) b->SetKls(m_geofile->m_kl_list);

    CPtsAlmaM2* msR = (CPtsAlmaM2*)getDockablePane(IDD_PTS_RIGHT_MS);
    if (msR) msR->Init0();
    CPtsAlmaM2* rsR = (CPtsAlmaM2*)getDockablePane(IDD_PTS_RIGHT_RS);
    if (rsR) rsR->Init0();

    //  CPtsSprav2 *ssR = (CPtsSprav2 *) getDockablePane(IDD_SPRAV);
    //  if (ssR) ssR->Init0();

    //    if (pm_cSheetPtsAlma->m_address) {
    //     pm_cSheetPtsAlma->m_address->SetKls(m_geofile->m_kl_list);
}


void check_alma() {}

/////////////////////////////////////////////////////////////////////////////
// CGidrView

IMPLEMENT_DYNCREATE(CGidrView, CScroll)

BEGIN_MESSAGE_MAP(CGidrView, CScroll)
    //{{AFX_MSG_MAP(CGidrView)
    ON_COMMAND_RANGE(ID_NOMAP, ID_OPENSTREETMAP, OnMap)
    ON_COMMAND_RANGE(ID_SPRAV_1, ID_SPRAV_28, OnSprav1)

    ON_COMMAND(ID_MAP_SEARCH, OnMapSearch)

    ON_MESSAGE(WM_USER_MAP_SEARCH, OnMapSearch2)

    ON_MESSAGE(WM_USER_ALSECO1_UPDATED, OnAlseco1Updated)
    ON_MESSAGE(WM_USER_ALSECO2_UPDATED, OnAlseco2Updated)


    ON_COMMAND(ID_POPUP_DOMA_ALSECO, OnDomaAlseco)

    ON_COMMAND(ID_NACHALNIK, OnNachalnik)
    ON_COMMAND(ID_SET_DATE_OSVID, OnSetOsvid)
    ON_COMMAND(ID_LIST_OSVID, OnListOsvid)

    ON_COMMAND(ID_SET_SHAIBA, OnSetShaiba)

    ON_COMMAND(ID_MAP_CITY, OnMapCity)
    //  ON_COMMAND(ID_ADD_CITY, OnAddCity)
    ON_UPDATE_COMMAND_UI_RANGE(ID_NOMAP, ID_OPENSTREETMAP, OnUpdateMap)
    ON_COMMAND_RANGE(ID_NODE, ID_DEL, OnRegim)
    ON_UPDATE_COMMAND_UI_RANGE(ID_NODE, ID_DEL, OnUpdateRegim)
    ON_WM_DESTROY()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_INF, OnInf)
    ON_UPDATE_COMMAND_UI(ID_INF, OnUpdateInf)
    ON_COMMAND(ID_REREAD_MAP, OnRereadMap)
    ON_COMMAND(ID_FONT, OnFont)
    ON_COMMAND(ID_MASALLALL, OnMasAllAll)
    ON_COMMAND(ID_EDIT_FIND_NODE, OnEditFind)

    ON_COMMAND(ID_TEST, OnTest)

    ON_COMMAND(ID_POPUP_CANAL, OnPopupCanal)


    ON_COMMAND(ID_EDIT_FIND_PS, OnEditFindPs)
    ON_COMMAND(ID_VIEW_DB, OnViewDb)
    ON_COMMAND(ID_VIEW_DB_PTS, OnViewDbPts)
    ON_COMMAND(ID_PTS_TABLE, OnViewDbPts)
    //    ON_COMMAND(ID_PTS_TABLE,OnPtsTable         )
    ON_COMMAND(ID_RAS_LIST, OnRasList)

    ON_COMMAND(ID_NEW, OnFileNew)

    ON_COMMAND(ID_CLEAR_OUT, OnClearOut)
    ON_COMMAND(ID_REBUILD_INDEXES, OnRebuildIndexes)

    ON_COMMAND(ID_DO_IT, OnDoIt)
    ON_COMMAND(ID_DO_IT_DR, OnDoItDr)

    ON_COMMAND(ID_DO_IT_LIST, OnDoItList)
    ON_COMMAND(ID_DO_IT_LIST_DR, OnDoItDrList)

    ON_COMMAND(ID_NO_PTS, OnNoPts)

    ON_COMMAND(ID_ANALIZ_RABOTY, OnDoItAnal)
    ON_COMMAND(ID_ANALIZ_REZHIMA, OnAnalizRezhima)

    ON_COMMAND(ID_END_PYTHON, OnEndPython)
    ON_COMMAND(ID_END_PASSPORT, OnEndPassport)

    ON_COMMAND(ID_END_IMPORT, OnEndImport)
    ON_COMMAND(ID_END_EXPORT, OnEndExport)
    ON_COMMAND(ID_END_UNITE , OnEndUnite)

    ON_COMMAND(ID_ZAP_NEW_DOC, OnZapNewDoc)

    ON_COMMAND(ID_END_DOIT, OnEndDoIt)
    ON_COMMAND(ID_END_DOIT2, OnEndDoIt2)

    ON_COMMAND(ID_POT_NAGR_0, OnPotNagr0)

    ON_COMMAND(ID_DEL_MARK, OnDelMark)

    ON_COMMAND(ID_SETUP_PROXY, OnSetupProxy)

    ON_COMMAND(ID_FIND_PODL, OnFindPodl)
    ON_COMMAND(ID_PODL_DIR, OnPodlDir)

    ON_COMMAND(ID_TABLE_DAN, OnTableDan)
    ON_COMMAND(ID_DANIIL, OnDaniil)

    ON_UPDATE_COMMAND_UI(ID_NODE_NEOTR, OnUpdateNodeNeotr)
    ON_COMMAND(ID_GEOBAZA, OnGeobaza)
    ON_UPDATE_COMMAND_UI(ID_GEOBAZA, OnUpdateGeobaza)

    ON_COMMAND(ID_COLOR_MAG_RS_GID, OnColorMagRsGid)
    ON_UPDATE_COMMAND_UI(ID_COLOR_MAG_RS_GID, OnUpdateColorMagRsGid)


    ON_COMMAND(ID_COLOR_ONLY_PTS, OnColorOnlyPTSColor)
    ON_UPDATE_COMMAND_UI(ID_COLOR_ONLY_PTS, OnUpdateOnlyPTSColor)

    ON_COMMAND(ID_REMONT_ISPR, OnRemontIspr)

    ON_COMMAND(ID_MAG_FRAGMENT, OnMagFragment)

    ON_COMMAND(ID_IZNOS, OnWear)

    ON_COMMAND(ID_UCH_LIST, OnRemontUchList)

    ON_UPDATE_COMMAND_UI(ID_FIND_GEO, OnUpdateGeo)

    //  ON_COMMAND(ID_FIND_GEO_ADDR, OnTuFind)

    ON_COMMAND(ID_FIND_GEO_ADDR, OnFindGeoAddr)
    ON_COMMAND(ID_GEO_ZDAN, OnGeoZdan)


    ON_COMMAND(ID_FIND_GEO_ADDR_CONFIG, OnAddrConfig)


    ON_COMMAND(ID_POPUP_GEO_ALSECO1, OnGeoAlseco1)
    ON_COMMAND(ID_POPUP_GEO_ALSECO2, OnGeoAlseco2)


    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY_TABLE, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY_ADD_1, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY_ADD_2, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY_ON_OFF, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMA_XLSX, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMA_NENAID_1, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMA_NENAID_2, OnUpdateAlseco)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMA_NENAID_3, OnUpdateAlseco)

    ON_COMMAND(ID_NAGR_ALMATY_ADD_1, OnNagrAlmatyAdd1)
    ON_COMMAND(ID_NAGR_ALMATY_ADD_2, OnNagrAlmatyAdd2)
    ON_COMMAND(ID_NAGR_ALMATY_TABLE, OnNagrAlmatyTable)
    ON_COMMAND(ID_NAGR_ALMATY, OnNagrAlmaty)
    ON_COMMAND(ID_NAGR_ALMATY_ON_OFF, OnNagrAlmatyOnOff)
    ON_UPDATE_COMMAND_UI(ID_NAGR_ALMATY_ON_OFF, OnUpdateNagrAlmatyOnOff)

    ON_COMMAND(ID_NAGR_ALMA_XLSX, OnNagrAlmaXlsx)

    ON_COMMAND(ID_NAGR_ALMA_NENAID_1, OnNagrAlmaNenaid1)
    ON_COMMAND(ID_NAGR_ALMA_NENAID_2, OnNagrAlmaNenaid2)
    ON_COMMAND(ID_NAGR_ALMA_NENAID_3, OnNagrAlmaNenaid3)

    ON_COMMAND(ID_SQL_QUERY, OnSqlQuery)


    ON_COMMAND(ID_ZDANIE_NAGRUZKI, OnNagrAlmaty2)

    ON_COMMAND(ID_OSMOTR_ADD, OnOsmotrAdd)
    ON_COMMAND(ID_INIT_SEZON, OnInitSezon)

    ON_WM_SETFOCUS()

    ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(ID_OUT, OnOut)
    ON_COMMAND(ID_ESC, OnEsc)
    ON_COMMAND(ID_PJEZO, OnPjezo)
    ON_UPDATE_COMMAND_UI(ID_PJEZO, OnUpdatePjezo)
    //    ON_COMMAND(ID_PROTOCOL, OnProtocol)
    //    ON_UPDATE_COMMAND_UI(ID_PROTOCOL, OnUpdateProtocol)
    ON_COMMAND(ID_OPENGL, OnOpengl)
    ON_COMMAND(ID_ZONA, OnZona)
    ON_COMMAND(ID_FIND_GEO, OnFindGeo)
    ON_COMMAND(ID_FON, OnFon)
    ON_UPDATE_COMMAND_UI(ID_KAM, OnUpdateKam)
    ON_UPDATE_COMMAND_UI(ID_KAM2, OnUpdateKam2)
    ON_COMMAND(ID_GEODEZ, OnGeoGeodez)
    ON_UPDATE_COMMAND_UI(ID_GEODEZ, OnUpdateGeoGeodez)
    ON_COMMAND(ID_UKRUP, OnUkrup)
    ON_UPDATE_COMMAND_UI(ID_UKRUP, OnUpdateUkrup)
    ON_COMMAND(ID_GIDINF, OnGidinf)
    ON_UPDATE_COMMAND_UI(ID_GIDINF, OnUpdateGidinf)
    ON_COMMAND(ID_NOVISUAL, OnNovisual)
    //    ON_COMMAND(ID_ISOCHNIK, OnIstochnik)


    ON_COMMAND(ID_ALL_PLAN_REMONT2, OnAllPlanRemont2)
    ON_COMMAND(ID_ALL_PLAN_SHURFY,  OnAllPlanShurfy)


ON_COMMAND(ID_NODE_ZD, OnNodeZd)

ON_COMMAND(ID_POPUP_OPEN_CLOSE_STATE_ID, OnPopupOpenCloseStateId)


ON_COMMAND(ID_CLOSE_ZD, OnCloseOpresZd)



//  ON_COMMAND(ID_SAVE_OUT, OnSaveOut)
ON_COMMAND(ID_ZAM, OnZam)
ON_COMMAND(ID_ZAP1, OnZap1)
ON_COMMAND(ID_ZAP2, OnZap2)
ON_COMMAND(ID_ZAP3, OnZap3)
ON_COMMAND(ID_ZAP4, OnZap4)
ON_COMMAND(ID_ZAP5, OnZap5)
ON_COMMAND(ID_ZAP6, OnZap6)
ON_COMMAND(ID_ZAP6_1, OnZap6)
ON_COMMAND(ID_ZAP7, OnZap7)
ON_COMMAND(ID_POTR_OTKL, OnPotrOtkl)

ON_COMMAND(ID_POTR_UR, OnPotrUR)
ON_COMMAND(ID_POTR_TR, OnPotrTR)


ON_COMMAND(ID_UT_ZAKR, OnUtZakr)
ON_COMMAND(ID_UT_ZAKR_ALL, OnUtZakrAll)

ON_COMMAND(ID_ZAP7_1, OnZap7_1)
ON_COMMAND(ID_ANAL, OnAnal)
ON_COMMAND(ID_TRIO, OnTrio)

ON_COMMAND(ID_REMONT_ALL_ALL, OnRemontAllAll)
ON_COMMAND(ID_REMONT_ALL, OnRemontAll)
ON_COMMAND(ID_REMONT_1, OnRemont1)
ON_COMMAND(ID_REMONT_2, OnRemont2)
ON_COMMAND(ID_REMONT_3, OnRemont3)

ON_COMMAND(ID_SHURF_ALL, OnShurfAll)
ON_COMMAND(ID_SHURF_1, OnShurf1)
ON_COMMAND(ID_SHURF_2, OnShurf2)
ON_COMMAND(ID_SHURF_3, OnShurf3)

ON_COMMAND(ID_KAPITAL_ALL, OnKapitalAll)
ON_COMMAND(ID_KAPITAL_1, OnKapital1)
ON_COMMAND(ID_KAPITAL_2, OnKapital2)
ON_COMMAND(ID_KAPITAL_3, OnKapital3)

ON_COMMAND(ID_OPRESSOVKA_ALL, OnOpressovkaAll)
ON_COMMAND(ID_OPRESSOVKA_1, OnOpressovka1)
ON_COMMAND(ID_OPRESSOVKA_2, OnOpressovka2)
ON_COMMAND(ID_OPRESSOVKA_3, OnOpressovka3)

ON_COMMAND(ID_KORROZ_ALL, OnKorrozAll)
ON_COMMAND(ID_KORROZ_1, OnKorroz1)
ON_COMMAND(ID_KORROZ_2, OnKorroz2)
ON_COMMAND(ID_KORROZ_3, OnKorroz3)

ON_COMMAND(ID_CTRL_ASTR, OnCtrlAstr)
ON_COMMAND(ID_SET_MS_NEW, OnSetMSNew)
ON_COMMAND(ID_SET_RS_NEW, OnSetRSNew)




ON_COMMAND(ID_SET_KORROZIA, OnSetKorrozia)
ON_UPDATE_COMMAND_UI(ID_SET_KORROZIA, OnUpdateSetKorrozia)


ON_COMMAND(ID_Q_GVS, OnQGvs)

ON_COMMAND(ID_POPUP_VV, OnPopupPasVV)
ON_COMMAND(ID_CHECK_PO, OnCheckPO)

ON_COMMAND(ID_KM, OnKm)
ON_COMMAND(ID_METR, OnMetr)
ON_UPDATE_COMMAND_UI(ID_KM, OnUpdateKm)
ON_UPDATE_COMMAND_UI(ID_METR, OnUpdateMetr)

//  ON_COMMAND(ID_OWNER, OnOwner)

//  ON_COMMAND(ID_REMONT, OnRemont)

ON_WM_RBUTTONDOWN()

ON_COMMAND_RANGE(ID_NEXT_UZEL1, ID_NEXT_UZEL2, OnNextUzel)

ON_COMMAND(ID_FIND_TU_DOG, OnFindTuDog)

ON_COMMAND(ID_COORD, OnCoord)
ON_MESSAGE(WM_USER_COORD_UPDATED, OnCoordUpdated)

ON_COMMAND(ID_POPUP_NAGR, OnPopupNagr)
ON_COMMAND(ID_POPUP_NAGR2, OnPopupNagr2)
ON_COMMAND(ID_POPUP_ACTIV, OnPopupActiv)
ON_COMMAND(ID_POPUP_INFO, OnPopupInfo)
ON_COMMAND(ID_POPUP_INFO_NODE, OnPopupInfoNode)
ON_COMMAND(ID_POPUP_INFO_LINE, OnPopupInfoLine)

ON_COMMAND(ID_POPUP_INFO_LINE_BIG, OnPopupInfoLineBig)
ON_COMMAND(ID_DEFECT_DOC_TABLE, OnDefectDocTable)

ON_COMMAND(ID_POPUP_DEFECT_1, OnPopupDefect1)
ON_COMMAND(ID_POPUP_DEFECT_2, OnPopupDefect2)

ON_COMMAND(ID_SET_PS_MAP, OnSetPsMap)

ON_COMMAND(ID_TEXT_OPR, OnTextOpr)

ON_COMMAND(ID_POPUP_UNITE2, OnPopupUnite2)
ON_COMMAND(ID_POPUP_UNITE, OnPopupUnite)
ON_COMMAND(ID_POPUP_OUT, OnPopupOut)
ON_COMMAND(ID_POPUP_OUT_DR, OnPopupOutDR)
ON_COMMAND(ID_POPUP_DEL, OnPopupDel)
ON_COMMAND(ID_POPUP_DEL_ZN, OnPopupDelZn)
ON_COMMAND(ID_POPUP_DEL_UP, OnPopupDelUp)

ON_COMMAND(ID_POPUP_DEL_VP, OnPopupDelVp)

ON_COMMAND(ID_POPUP_DEL_NODE, OnPopupDelNode)
ON_COMMAND(ID_POPUP_DEL_LINE, OnPopupDelLine)

ON_COMMAND(ID_POPUP_WINCC_INFO, OnPopupWinccInfo)
ON_COMMAND(ID_POPUP_NEW_UZEL, OnPopupNewUzel)
ON_COMMAND(ID_POPUP_RENAME, OnPopupRename)
ON_COMMAND(ID_POPUP_ZN, OnPopupZn)
ON_COMMAND(ID_POPUP_ORP, OnPopupORP)
ON_COMMAND(ID_POPUP_OTKR, OnPopupOtkr)
ON_COMMAND(ID_POPUP_OTKR_PT, OnPopupOtkrPr)
ON_COMMAND(ID_POPUP_UP, OnPopupUp)
ON_COMMAND(ID_POPUP_KAM, OnPopupKam)
ON_COMMAND(ID_POPUP_VYD, OnPopupVyd)
ON_COMMAND(ID_POPUP_PRIBOR, OnPopupPribor)
ON_COMMAND(ID_POPUP_PRIBOR_DEL, OnPopupPriborDel)
ON_UPDATE_COMMAND_UI(ID_POPUP_NEW_UZEL, OnUpdatePopupNewUzel)
ON_COMMAND(ID_POPUP_MOVE, OnPopupMove)
ON_COMMAND(ID_POPUP_LENGTH, OnPopupLength)
ON_COMMAND(ID_POPUP_COPY_AKT, OnPopupCopyAkt)

ON_COMMAND(ID_POPUP_GEO_MARK, OnPopupGeoMark)



ON_COMMAND(ID_POPUP_GEO_INFO, OnPopupGeoInfo)
ON_COMMAND(ID_POPUP_GEO_DEL, OnPopupGeoDel)
ON_COMMAND(ID_POPUP_GEO_ISM, OnPopupGeoIsm)
ON_COMMAND(ID_POPUP_GEO_ISM_PODP, OnPopupGeoIsmPodp)
ON_COMMAND(ID_POPUP_GEO_NADPIS, OnPopupGeoNadpis)
ON_COMMAND(ID_POPUP_GEO_TU, OnPopupGeoTU)
ON_COMMAND(ID_POPUP_GEO_TU_LINE, OnPopupGeoTULine)


ON_COMMAND(ID_POPUP_GEO_ZD2, OnPopupGeoZD2)

ON_COMMAND(ID_POPUP_GEO_TU2, OnPopupGeoTU2)
ON_COMMAND(ID_POPUP_GEO_NAGR, OnPopupGeoNagr)
ON_COMMAND(ID_POPUP_GEO_HIDE, OnPopupGeoHide)
ON_COMMAND(ID_POPUP_GEO_SHOW, OnPopupGeoShow)
ON_COMMAND(ID_POPUP_GEO_NAPR, OnPopupGeoNapr)

ON_COMMAND(ID_DOP_PODKL_NAGR, OnDopPodklNagr)

ON_COMMAND(ID_PASP_NEW, OnPasp2)  // Новый паспорт
ON_COMMAND(ID_PASP, OnMsPassport) // Старый паспорт

ON_COMMAND(ID_ODNOLIN, OnOdnolin)
ON_COMMAND(ID_RAZNOLIN, OnRaznolin)

//  ON_COMMAND(ID_PASP_MS , OnPaspMS)
//  ON_COMMAND(ID_PASP_RS , OnPaspRS)
//  ON_COMMAND(ID_PASP_KAM , OnPaspKam)
//  ON_UPDATE_COMMAND_UI(ID_PASP_MS, OnUpdatePaspMS)
//  ON_UPDATE_COMMAND_UI(ID_PASP_RS, OnUpdatePaspRS)

ON_COMMAND(ID_TU_FIND, OnTuFind)
ON_COMMAND(ID_TU_TRUB, OnTuTrub)

ON_COMMAND(ID_EDITOR1, OnEditor1)
ON_COMMAND(ID_EDITOR2, OnEditor2)
//ON_COMMAND(ID_EDITOR3, OnEditor3)
ON_COMMAND(ID_EDITOR4, OnEditor4)

ON_UPDATE_COMMAND_UI(ID_EDITOR1, OnUpdateEditor1)
ON_UPDATE_COMMAND_UI(ID_EDITOR2, OnUpdateEditor2)
ON_UPDATE_COMMAND_UI(ID_EDITOR3, OnUpdateEditor3)
ON_UPDATE_COMMAND_UI(ID_EDITOR4, OnUpdateEditor4)

ON_COMMAND(ID_POPUP_CLEAR, OnPopupClear)
ON_COMMAND(ID_POPUP_PODP, OnPopupPodp)

ON_COMMAND(ID_POPUP_PODP_ON, OnPopupPodpOn)
ON_COMMAND(ID_POPUP_PODP_OFF, OnPopupPodpOff)

ON_COMMAND(ID_POPUP_VNUTR, OnPopupVnutr)
ON_COMMAND(ID_POPUP_OPC_PARAM, OnPopupOpcParam)
ON_COMMAND(ID_POPUP_OPC_WRITE, OnPopupOpcWrite)
ON_COMMAND(ID_TEPLOPOTERI, OnTeplopoteri)

ON_COMMAND(ID_INDEXES, OnIndexes)


//ON_UPDATE_COMMAND_UI(ID_IZNOS, OnUpdateIznos)
//ON_UPDATE_COMMAND_UI(ID_REMONT_PLAN, OnUpdateIznos)

ON_UPDATE_COMMAND_UI(ID_TEPLOPOTERI, OnUpdateTeplopoteri)
ON_UPDATE_COMMAND_UI(ID_NAGR_ZD, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_NAGR_ORG, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_TABLE, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_YEAR, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_SOST, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_EXCEL, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_SVOD, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_ZHURNAL, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_FIND_TU_IST, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_FIND, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_PRIS_NAGR_EDIT, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_Q_GVS, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_NAGR_ZD_NEIZ, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_NAGR_ORG_NEIZ, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_TABLE_NEIZ, OnUpdateTU)
ON_UPDATE_COMMAND_UI(ID_TU_IST, OnUpdateTU)

ON_COMMAND(ID_GEO_ONOFF, OnGeoOnOff)
ON_COMMAND(ID_REMONT_ONOFF,   OnRemontOnOff)
ON_COMMAND(ID_PASSPORT_ONOFF, OnPassportOnOff)
ON_COMMAND(ID_ELECTRO_ONOFF,  OnElectroOnOff)
ON_COMMAND(ID_KORROZIA_ONOFF, OnKorroziaOnOff)

ON_COMMAND(ID_REMONT_POVR_DEF_OTOBR2, OnRemontOnOff)      
ON_COMMAND(ID_PASSPORT_ONOFF2       , OnPassportOnOff)    
ON_COMMAND(ID_ELECTRO_ONOFF2        , OnElectroOnOff)     
ON_COMMAND(ID_KORROZIA_ONOFF2       , OnKorroziaOnOff)    


//  ON_UPDATE_COMMAND_UI(ID_KORROZIA_ONOFF, OnUpdateKorroziaOnOff)

ON_COMMAND(ID_KORROZIA_ZHURNAL1, OnKorroziaZhurnal1)
ON_COMMAND(ID_KORROZIA_ZHURNAL2, OnKorroziaZhurnal2)
ON_COMMAND(ID_KORROZIA_ZHURNAL3, OnKorroziaZhurnal3)
ON_COMMAND(ID_KORROZIA_ZHURNAL4, OnKorroziaZhurnal4)

ON_COMMAND(ID_POPUP_KORROZIA_PLAN, OnPopupKorroziaPlan)



ON_COMMAND(ID_KORROZIA_CURRENT, OnZhurnalKorroziaCurrent)
ON_COMMAND(ID_KORROZIA_ALL, OnZhurnalKorroziaAll)
ON_COMMAND(ID_KORROZIA_GOD, OnZhurnalKorroziaGod)

ON_COMMAND(ID_KORROZIA_SEZON, OnSezon)


//    ON_UPDATE_COMMAND_UI(ID_KORROZIA_CURRENT, OnUpdateKorroziaCurrent)
//    ON_UPDATE_COMMAND_UI(ID_KORROZIA_ALL, OnUpdateKorroziaAll)

ON_UPDATE_COMMAND_UI(ID_KORROZIA_ALL, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_CURRENT, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_GOD, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_ZHURNAL1, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_ZHURNAL2, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_ZHURNAL3, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_KORROZIA_ZHURNAL4, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_ZHURNAL_DEFECT, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_ZHURNAL_DIAG, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_ZHURNAL_ELECTRO, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_ZHURNAL_KORROZ, OnUpdateZhurnal)
ON_UPDATE_COMMAND_UI(ID_ZHURNAL_REMONT, OnUpdateZhurnal)


ON_COMMAND(ID_DEFECT_ANALIZ, OnDefectAnaliz)


ON_COMMAND(ID_KORROZIA_UPDATE, OnKorroziaUpdate)
ON_MESSAGE(WM_KORROZIA_UPDATED, OnKorroziaUpdate2)


ON_COMMAND(ID_REMONTS_ADD_DEFECT, OnRemontsAddDefect)
ON_COMMAND(ID_REMONTS_ADD_REMONT, OnRemontsAddRemont)
ON_COMMAND(ID_REMONTS_ADD_DIAG, OnRemontsAddDiag)

ON_COMMAND(ID_REMONTS_ADD, OnRemontsAdd)
ON_COMMAND(ID_REMONTS_INFO, OnRemontsInfo)
ON_COMMAND(ID_REMONTS_DEL, OnRemontsDel)
ON_COMMAND(ID_REMONTS_TABLE, OnRemontsTable)
ON_COMMAND(ID_REMONTS_ZHURNAL, OnRemontsZhurnal)

//    ON_COMMAND(ID_REMONTS_ZHURNAL   ,OnRemontsZhurnal   )
ON_COMMAND(ID_PTS_ADD, OnPtsAdd)
ON_COMMAND(ID_PTS_INFO, OnPtsInfo)
ON_COMMAND(ID_PTS_DEL, OnPtsDel)

ON_COMMAND(ID_PTS_ZHURNAL, OnPtsZhurnal)



ON_COMMAND(ID_KORROZ_ADD, OnKorrozAdd)
ON_COMMAND(ID_KORROZ_INFO, OnKorrozInfo)
ON_COMMAND(ID_KORROZ_DEL, OnKorrozDel)
ON_COMMAND(ID_KORROZ_TABLE, OnKorrozTable)


//ON_COMMAND(ID_KORROZ_TABLE_GOD, OnKorrozTableGod)

//    ON_COMMAND(ID_KORROZ_ZHURNAL    ,OnKorrozZhurnal    )
ON_COMMAND(ID_ELECTRO_ADD, OnElectroAdd)
ON_COMMAND(ID_ELECTRO_INFO, OnElectroInfo)
ON_COMMAND(ID_ELECTRO_DEL, OnElectroDel)
ON_COMMAND(ID_ELECTRO_TABLE, OnElectroTable)
ON_COMMAND(ID_ELECTRO_ZHURNAL, OnElectroZhurnal)

ON_COMMAND(ID_ZHURNAL_DEFECT, OnZhurnalDefect)
ON_COMMAND(ID_ZHURNAL_DIAG, OnZhurnalDiag)
ON_COMMAND(ID_ZHURNAL_REMONT, OnZhurnalRemont)
ON_COMMAND(ID_ZHURNAL_ELECTRO, OnZhurnalElectro)
ON_COMMAND(ID_ZHURNAL_KORROZ, OnZhurnalKorroz)
ON_COMMAND(ID_ZHURNAL_EXCEL, OnZhurnalExcel)
//ON_COMMAND(ID_ASYNC_CHECK, OnAsyncCheck)

ON_COMMAND(ID_ZHURNAL_DEFECT_OPRES, OnZhurnalDefectOpres)


ON_COMMAND(ID_NEW_OPRES, OnNewOpres)

ON_COMMAND(ID_REMONTS_ADD_OPRES, OnRemontsAddOpres)



ON_COMMAND(ID_TEPLOPOTERI_FAKT, OnTeplopoteriFakt)


ON_COMMAND(ID_TEPLOPOTERI_F, OnTeplopoteriF)
ON_COMMAND(ID_TEPLOPOTERI_EXCEL, OnTeplopoteriExcel)
ON_COMMAND(ID_BMP, OnBmp)
ON_COMMAND(ID_BMP_FIND, OnBmpFind)
ON_UPDATE_COMMAND_UI(ID_BMP_FIND, OnUpdateBmpFind)

ON_COMMAND(ID_SET_LENGTH, OnSetLength)
ON_COMMAND(ID_EXPORT_DXF, OnExportDxf)

ON_WM_TIMER()

ON_MESSAGE(WM_USER_FIND_NODE, OnFindNode)
ON_MESSAGE(WM_USER_FIND_LINE, OnFindLine)
ON_MESSAGE(WM_USER_FIND_LINE_BIG, OnFindLineBig)
ON_MESSAGE(WM_USER_FIND_LINE_BIG_OSMOTR, OnFindLineBigOsmotr)
ON_MESSAGE(WM_USER_FIND_LINE_BIG2, OnFindLineBig2)
ON_MESSAGE(WM_USER_FIND_GEO, OnFindGeo)
ON_MESSAGE(WM_USER_FIND_GEO_TU, OnFindGeoTu)
ON_MESSAGE(WM_USER_FIND_GEO_EDIT, OnFindGeoEdit)
ON_MESSAGE(WM_USER_FIND_GEO_BIG_LINE, OnFindGeoBigLine)

ON_MESSAGE(WM_USER_REFRESH, OnRefresh)

ON_MESSAGE(WM_USER_FIND_SEL_GEO_OBJECT, OnFindSelGeoObject)

ON_MESSAGE(WM_USER_FIND_ZH, OnFindZh)
ON_MESSAGE(WM_USER_FIND_ZD, OnFindZd)

ON_COMMAND(ID_COLORS, OnColors)
ON_COMMAND(ID_ZAPROS, OnZapros)
ON_COMMAND(ID_ZIMA, OnZima)
ON_UPDATE_COMMAND_UI(ID_ZIMA, OnUpdateZima)
ON_COMMAND(ID_FONT2, OnFont2)
ON_COMMAND(ID_FONT_MAG, OnFontMag)
ON_COMMAND(ID_FONT_PANEL, OnFontPanel)

ON_COMMAND(ID_REP, OnRep)
ON_COMMAND(ID_REPR, OnRepR)
ON_COMMAND(ID_REPW, OnRepW)
ON_UPDATE_COMMAND_UI(ID_SET_LENGTH, OnUpdateSetLength)
ON_COMMAND(ID_OUT2, OnOut2)
ON_COMMAND(ID_ADD_DOP, OnAddDopFile)
ON_COMMAND(ID_TABLE_EXP, OnEmptyTemplate)
ON_COMMAND(ID_MAIN_CXEMA, OnMainCxema)
ON_COMMAND(ID_GEO_FIND_NEXT, OnGeoFindNext)
ON_UPDATE_COMMAND_UI(ID_GEO_FIND_NEXT, OnUpdateGeoFindNext)
//    ON_COMMAND(ID_CH_NAME, OnChName)


ON_COMMAND(ID_SET_TUBING_TYPE, OnSetTubingType)
ON_COMMAND(ID_SET_LOSES_SHARE, OnSetLosesShare)

ON_COMMAND(ID_GEODZ, OnGeodz)

ON_COMMAND(ID_GEODZ1, OnGeodz1)
ON_COMMAND(ID_GEODZ2, OnGeodz2)


//ON_COMMAND(ID_GEODZ_INTERNET, OnGeodzInternet)
ON_COMMAND(ID_SET_DIAMS, OnSetDiams)
ON_COMMAND(ID_SET_OTV, OnSetOtv)
ON_COMMAND(ID_SET_ORG, OnSetOrg)

ON_COMMAND(ID_SET_OPEN_REZ, OnSetOpenRez)
ON_COMMAND(ID_SET_OPEN_KOEF, OnSetOpenKoef)
ON_COMMAND(ID_SET_OPEN_REZ_T, OnSetOpenRezT)
ON_COMMAND(ID_SET_OPEN_GVS_T, OnSetOpenGvsT)


ON_COMMAND(ID_SET_UR, OnSetUr)
ON_COMMAND(ID_SET_TR, OnSetTr)
ON_COMMAND(ID_SET_TP, OnSetTp)
ON_COMMAND(ID_SET_KV_UT, OnSetKvUt)
ON_COMMAND(ID_SET_KV_PT, OnSetKvPt)
ON_COMMAND(ID_SET_UF, OnSetUf)


ON_COMMAND(ID_SET_COORD_NULL, OnSetCoordNull)


ON_COMMAND(ID_SET_AVTO_ON, OnSetAvtoOn)
ON_COMMAND(ID_SET_AVTO_OFF, OnSetAvtoOff)

ON_UPDATE_COMMAND_UI(ID_ZHURNAL_EXCEL, OnUpdateZhurnalExcel)



ON_UPDATE_COMMAND_UI(ID_SET_MS_NEW, OnUpdateSetMSNew)
ON_UPDATE_COMMAND_UI(ID_SET_RS_NEW, OnUpdateSetRSNew)

//  ID_SET_OPRESSOVKA
ON_UPDATE_COMMAND_UI(ID_SET_OPRESSOVKA, OnUpdateSetOpressovka)

//  ON_COMMAND(ID_SAVE_OPRESSOVKA, OnSaveOpressovka)
//  ON_UPDATE_COMMAND_UI(ID_SAVE_OPRESSOVKA, OnUpdateSaveOpressovka)

ON_COMMAND(ID_CALCS, OnCalcs)

ON_COMMAND(ID_MS_PASSPORT, OnMsPassport)
ON_UPDATE_COMMAND_UI(ID_MS_PASSPORT, OnUpdatePasp)
ON_UPDATE_COMMAND_UI(ID_PASP, OnUpdatePasp)

//  ON_UPDATE_COMMAND_UI(ID_MS_NODE_1, OnUpdateSaveMS)
//  ON_UPDATE_COMMAND_UI(ID_MS_NODE_2, OnUpdateSaveMS)

//ON_COMMAND(ID_OPRESSOVKA_PASSPORT, OnOpressovkaPassport)
//  ON_UPDATE_COMMAND_UI(ID_OPRESSOVKA_PASSPORT, OnUpdateSaveOpressovka)

ON_COMMAND(ID_OPR_NEOPR, OnOprNeopr)

ON_COMMAND(ID_MAG_NODE, OnMagNode)

ON_COMMAND(ID_SAVE_MS, OnSaveMS)
ON_COMMAND(ID_SAVE_RS, OnSaveRS)
ON_COMMAND(ID_SAVE_MAG, OnSaveMag)

ON_UPDATE_COMMAND_UI(ID_SAVE_MS, OnUpdateSaveMS)
ON_UPDATE_COMMAND_UI(ID_SAVE_RS, OnUpdateSaveRS)
ON_UPDATE_COMMAND_UI(ID_SAVE_MAG, OnUpdateSaveMag)

//  ON_COMMAND(ID_SAVE_MAG, OnSaveUtPats)
//  ON_UPDATE_COMMAND_UI(ID_SAVE_UT_PTS, OnUpdateSaveUtPts)


    //  ON_COMMAND(ID_SET_KVART, OnSetKvart)
    ON_COMMAND(ID_SET_SHER, OnSetSher)

    ON_COMMAND(ID_SET_DATE1, OnSetDate1)
    ON_COMMAND(ID_SET_DATE2, OnSetDate2)

    ON_COMMAND(ID_SET_DATE3, OnSetDate3)

    ON_COMMAND(ID_POVREZHD, OnPovrezhd)

    ON_COMMAND(ID_SET_PIPE_REMONT_TYPE, OnSetRemontType)


    ON_COMMAND(ID_SET_KOL_CHAS, OnSetKolChas)
    ON_COMMAND(ID_SET_KTI, OnSetKti)

    ON_COMMAND(ID_SET_UDOB_OT, OnSetUdobOt)
    ON_COMMAND(ID_SET_UDOB_VENT, OnSetUdobVent)

    ON_COMMAND(ID_SET_PODP_ON, OnSetPodpOn)
    ON_COMMAND(ID_SET_PODP_OFF, OnSetPodpOff)

    ON_COMMAND(ID_SET_KOD_RS, OnSetKodRs)
    //  ON_COMMAND(ID_SET_KOD_OPR , OnSetKodOpr)

    ON_UPDATE_COMMAND_UI(ID_SET_NACH_MAG, OnUpdateVyd)

    ON_COMMAND(ID_HTTP2, OnHttp2)

    ON_UPDATE_COMMAND_UI(ID_SET_DATE1, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_DATE2, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_DATE3, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_PIPE_REMONT_TYPE, OnUpdateVyd)





    ON_UPDATE_COMMAND_UI(ID_SET_TUBING_TYPE, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_LOSES_SHARE, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_GEODZ_INTERNET, OnUpdateGeodzInternet)

    ON_UPDATE_COMMAND_UI(ID_GEODZ1, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_GEODZ2, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_SET_SHAIBA, OnUpdateVyd)
    

    ON_UPDATE_COMMAND_UI(ID_GEODZ, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_DIAMS, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_OTV, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_ORG, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_OPEN_KOEF, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_OPEN_REZ, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_OPEN_REZ_T, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_OPEN_GVS_T, OnUpdateVyd)


    ON_UPDATE_COMMAND_UI(ID_SET_UR, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_TR, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_SET_KV_UT, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KV_PT, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_TP, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_UF, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_SET_DATE, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KOL_CHAS, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KTI, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_SHER, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KV, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KVART, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_UDOB_OT, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_UDOB_VENT, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_PODP_ON, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_PODP_OFF, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_MS, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_RS, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_SET_AVTO_ON, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_AVTO_OFF, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_SET_KOD_RS, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_SET_KOD_OPR, OnUpdateVyd)

    ON_UPDATE_COMMAND_UI(ID_FRAGMENT_POLY, OnUpdateVyd)
    ON_UPDATE_COMMAND_UI(ID_FRAGMENT_POLY_GEO, OnUpdateVyd)

    ON_COMMAND(ID_REZHIM, OnRezhim)
    ON_UPDATE_COMMAND_UI(ID_REZHIM, OnUpdateRezhim)

    ON_COMMAND(ID_PODP, OnPodp)
    ON_COMMAND(ID_PODP_NIZ, OnPodpNiz)
    ON_COMMAND(ID_PODP_VERH, OnPodpVerh)
    ON_COMMAND(ID_PODP_PO, OnPodpPO)
    ON_UPDATE_COMMAND_UI(ID_PODP, OnUpdatePodp)
    ON_UPDATE_COMMAND_UI(ID_PODP_NIZ, OnUpdatePodpNiz)
    ON_UPDATE_COMMAND_UI(ID_PODP_VERH, OnUpdatePodpVerh)
    ON_UPDATE_COMMAND_UI(ID_PODP_PO, OnUpdatePodpPO)
    ON_COMMAND(ID_PODP_RES, OnPodpRes)
    ON_UPDATE_COMMAND_UI(ID_PODP_RES, OnUpdatePodpRes)
    ON_COMMAND(ID_PRIBOR, OnPribor)
    ON_UPDATE_COMMAND_UI(ID_PRIBOR, OnUpdatePribor)
    ON_COMMAND(ID_FLAG, OnFlag)
    ON_UPDATE_COMMAND_UI(ID_FLAG, OnUpdateFlag)

    ON_COMMAND(ID_PICTURE, OnPicture)
    ON_UPDATE_COMMAND_UI(ID_PICTURE, OnUpdatePicture)
    ON_COMMAND(ID_NEW_UCH2, OnNewUch2)
    ON_UPDATE_COMMAND_UI(ID_NEW_UCH2, OnUpdateNewUch2)
    ON_COMMAND(ID_EXPORT_SHAPE, OnExportShape)
    ON_COMMAND(ID_PR_PO, OnPrPo)
    ON_COMMAND(ID_TEMP, OnTemp)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_COMMAND(ID_EDIT_UNDO_DEL, OnEditUndoDel)
    ON_COMMAND(ID_RASPR_MAG, OnRasprMag)
    ON_COMMAND(ID_RASPR_MAG_NEW, OnRasprMagNew)
    ON_COMMAND(ID_RASPR_MAG2, OnRasprMag2)

    ON_COMMAND(ID_PROT, OnProt)
    ON_COMMAND(ID_PROT_NEW, OnProtNew)
    ON_COMMAND(ID_PROT_MAG, OnProtMag)
    ON_COMMAND(ID_PROT_PASS, OnProtPassport)
    ON_COMMAND(ID_PROT_RS, OnProtRs)
    ON_COMMAND(ID_PROT_KOT, OnProtKot)
    ON_COMMAND(ID_PROT_ALL, OnProtAll)

    ON_COMMAND(ID_POPUP_MEROPR, OnPopupMeropr)

    ON_COMMAND(ID_POPUP_DEL_TEXT, OnDelText)
    ON_COMMAND(ID_POPUP_EDIT_TEXT, OnEditText)
    ON_COMMAND(ID_POPUP_FONT_TEXT, OnFontText)

    ON_UPDATE_COMMAND_UI(ID_IS_MAS, OnUpdateIsMas)
    ON_COMMAND(ID_IS_MAS, OnIsMas)
    ON_COMMAND(ID_MAS_PIC, OnMasPic)
    ON_COMMAND(ID_MAS_PODPIS, OnMasPodp)
    ON_UPDATE_COMMAND_UI(ID_NAPR_GID, OnUpdateNaprGid)
    ON_COMMAND(ID_NAPR_GID, OnNaprGid)
    ON_COMMAND(ID_ZAP_ORP, OnZapOrp)
    ON_COMMAND(ID_ZAP_OTR, OnZapOtr)

    ON_COMMAND(ID_PT_TEMP_MIN, OnPtTempMin)
    ON_COMMAND(ID_ZAVOZD, OnZapZavozd)

    ON_COMMAND(ID_ZAP_VN_CX, OnZapVnCx)
    ON_COMMAND(ID_ZAP_NULL_GEO, OnZapNullGeo)
    ON_COMMAND(ID_ZAP_NEZAK, OnZapNezak)
    ON_COMMAND(ID_ZAP_IZM, OnZapIzm)
    ON_COMMAND(ID_IST_SETY, OnIstSety)
    ON_COMMAND(ID_BALANS, OnBalans)
    ON_COMMAND(ID_TIME_PR, OnTimePr)
    ON_COMMAND(ID_POTERI, OnPoteri)
    ON_COMMAND(ID_WINCC, OnWincc)
    ON_COMMAND(ID_OTKLONENIA, OnOtklonenia)
    ON_COMMAND(ID_SAVE_PJEZO, OnSavePjezo)
    ON_COMMAND(ID_LIST_PJEZO, OnListPjezo)

    ON_COMMAND(ID_REMONT_POVR_DEF_PANEL, OnRemontPovrDefPanel)

    ON_COMMAND(ID_REMONT_POVR_SHURF_ADD, OnRemontPovrShurfAdd)
    ON_COMMAND(ID_REMONT_POVR_SHURF_ADD_NEPLAN, OnRemontPovrShurfAddNeplan)
    ON_COMMAND(ID_REMONT_POVR_SHURF_DEL, OnRemontPovrShurfDel)
    ON_COMMAND(ID_REMONT_POVR_SHURF_MOVE, OnRemontPovrShurfMove)
    ON_COMMAND(ID_REMONT_POVR_SHURF_INFO, OnRemontPovrShurfInfo)


    ON_COMMAND(ID_OPRES_ZADV, OnOpresZadvAdd)

    ON_COMMAND(ID_REMONT_POVR_DEF_ADD, OnRemontPovrDefAdd)
    ON_COMMAND(ID_REMONT_POVR_DEF_DEL, OnRemontPovrDefDel)
    ON_COMMAND(ID_REMONT_POVR_DEF_MOVE, OnRemontPovrDefMove)
    ON_COMMAND(ID_REMONT_POVR_DEF_INFO, OnRemontPovrDefInfo)
    ON_COMMAND(ID_REMONT_POVR_KARTA, OnRemontPovrKarta)
    ON_COMMAND(ID_REMONT_POVR_KARTA2, OnRemontPovrKarta2)
    ON_COMMAND(ID_REMONT_POVR_OTOP, OnRemontPovrOtop)
    ON_COMMAND(ID_REMONT_POVR_OTKL_POVR, OnRemontPovrOtklPovr)
    ON_COMMAND(ID_REMONT_POVR_DEF_OTOBR, OnRemontOnOff)
    ON_COMMAND(ID_REMONT_POVR_DEF_OTOBR, OnRemontOnOff)
    ON_COMMAND(ID_REMONT_SHURF_OTOBR, OnRemontOnOff)

    ON_COMMAND(ID_SHURF_PROCESS, OnShurfProcess)
    ON_COMMAND(ID_SHURF_VYPOLNENO, OnShurfVypolneno)

    ON_COMMAND(ID_REMONT_WORD1, OnRemontWord1)
    ON_COMMAND(ID_REMONT_WORD2, OnRemontWord2)
    ON_COMMAND(ID_REMONT_WORD3, OnRemontWord3)
    ON_COMMAND(ID_REMONT_WORD4, OnRemontWord4)


    ON_COMMAND(ID_REMONT_UTVERDIT, OnRemontPlanUtverdit)

    ON_COMMAND(ID_REMONT_SHURF_PLAN, OnRemontShurfPlan)
    ON_COMMAND(ID_REMONT_SHURF_PLAN_UTVERDIT, OnRemontShurfPlanUtverdit)
    ON_COMMAND(ID_REMONT_SHURF_PLAN_EXCEL, OnRemontShurfPlanExcel)
    ON_COMMAND(ID_REMONT_SHURF_PLAN_EXCEL_MONTH, OnRemontShurfPlanExcelMonth)
    ON_COMMAND(ID_REMONT_SHURF_PREDPIS, OnRemontShurfPredpis)
    ON_COMMAND(ID_REMONT_SHURF_NARUSH, OnRemontShurfNarush)






    ON_COMMAND(ID_SHURF_TABLE, OnShurfTable)

    ON_COMMAND(ID_DEFECT_WORD1, OnDefectWord1)
    ON_COMMAND(ID_DEFECT_WORD2, OnDefectWord2)

    ON_COMMAND(ID_REMONT_SHURF_ZHURNAL, OnRemontShurfZhurnal)

    //  ON_COMMAND(ID_REMONT_POVR_NACH, OnRemontPovrNach)
    //  ON_COMMAND(ID_REMONT_POVR_RS, OnRemontPovrRs)
    //  ON_COMMAND(ID_REMONT_POVR_MS, OnRemontPovrMs)
    //  ON_COMMAND(ID_REMONT_POVR_ALL, OnRemontPovrAll)

    ON_COMMAND(ID_REMONT_POVR_DEF_FIND, OnRemontPovrDefFind)
    ON_COMMAND(ID_REMONT_POVR_DEF_TABLE, OnRemontPovrDefTable)

/*
    ON_COMMAND(ID_REMONT_POVR_DEF_ZHUR, OnRemontPovrDefZhur)
    ON_COMMAND(ID_REMONT_POVR_DEF_ZHUR_1, OnRemontPovrDefZhur1)
    ON_COMMAND(ID_REMONT_POVR_DEF_ZHUR_2, OnRemontPovrDefZhur2)
    ON_COMMAND(ID_REMONT_POVR_DEF_ZHUR_3, OnRemontPovrDefZhur3)
*/

    ON_COMMAND(ID_DEFECT_ZHURNAL_EXPLUATACIA, OnDefectZhurnalExpluatacia)
    ON_COMMAND(ID_DEFECT_ZHURNAL_SHURF, OnDefectZhurnalShurf)
    ON_COMMAND(ID_DEFECT_ZHURNAL_OSMOTR, OnDefectZhurnalOsmotr)
    ON_COMMAND(ID_DEFECT_ZHURNAL_OPRESS, OnDefectZhurnalOpress)
    ON_COMMAND(ID_DEFECT_ZHURNAL_NARUSHENIE, OnDefectZhurnalNarushenie)
    ON_COMMAND(ID_DEFECT_ZHURNAL_REMONT, OnDefectZhurnalRemont)
    ON_COMMAND(ID_DEFECT_ZHURNAL_POSLE_REMONT, OnDefectZhurnalPosleremont)
    ON_COMMAND(ID_DEFECT_ZHURNAL_ALL, OnDefectZhurnalAll)

    ON_COMMAND(ID_SET_MIN_PEREPAD, OnSetMinPerepad)

    ON_COMMAND(ID_DEFECT_BEZ_UCH, OnDefectBezUch)
    ON_COMMAND(ID_SHURF_BEZ_UCH, OnShurfBezUch)

    ON_COMMAND(ID_MAKE_UT_PTS, OnMakeUtPTS)

//    ON_COMMAND(ID_REMONT_POVR_DEF_ZHUR_REMONT, OnRemontPovrDefZhurRemont)

    ON_COMMAND(ID_REMONT_POVR_1, OnRemontPovr_1)

    ON_COMMAND(ID_REMONT_POVR_POVR, OnRemontPovrPovr)
    ON_COMMAND(ID_REMONT_POVR_REMONT, OnRemontPovrRemont)
    ON_COMMAND(ID_REMONT_POVR_POSLE_REMONT, OnRemontPovrPosleRemont)
    ON_COMMAND(ID_REMONT_POVR_REMONT_ALL, OnRemontPovrRemontAll)

    ON_COMMAND(ID_REMONT_POVR_2, OnRemontPovr_2)

    ON_COMMAND(ID_REMONT_POVR_REMONT_TRUB, OnRemontPovrRemontTrub)
    ON_COMMAND(ID_REMONT_POVR_RAZM_ZAPL, OnRemontPovrRazmZapl)
    ON_COMMAND(ID_REMONT_POVR_DLINA_ZAM_TRUB, OnRemontPovrDlinaZamTrub)
    ON_COMMAND(ID_REMONT_POVR_REM_IZOL, OnRemontPovrRemIzol)
    ON_COMMAND(ID_REMONT_POVR_REM_KANAL, OnRemontPovrRemKanal)
    ON_COMMAND(ID_REMONT_POVR_REM_ALL, OnRemontPovrRemAll)

    ON_COMMAND(ID_REMONT_POVR_3, OnRemontPovr_3)

    ON_COMMAND(ID_REMONT_POVR_DEF_EKSPL, OnRemontPovrDefEkspl)
    ON_COMMAND(ID_REMONT_POVR_DEF_OPR, OnRemontPovrDefOpr)
    ON_COMMAND(ID_REMONT_POVR_DEF_OSV, OnRemontPovrDefOsv)
    ON_COMMAND(ID_REMONT_POVR_DEF_SHURF_PLAN, OnRemontPovrDefShurfPlan)
    ON_COMMAND(ID_REMONT_POVR_ALL, OnRemontPovrAll)

    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_TIP, OnRemontPovrDefAnalizTip)
    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_PRICHINA, OnRemontPovrDefAnalizPrichina)
    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_ELEMENT, OnRemontPovrDefAnalizElement)
    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_TIP_PROKL, OnRemontPovrDefAnalizTipProkl)
    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_ITD, OnRemontPovrDefAnalizItd)
    ON_COMMAND(ID_REMONT_POVR_DEF_ANALIZ_ALL, OnRemontPovrDefAnalizAll)


    ON_COMMAND(ID_REMONT_OSMOTR_PANEL, OnRemontOsmotrPanel)
    ON_COMMAND(ID_REMONT_OSMOTR_TEPLO_SET, OnRemontPovrTeploSet)
    ON_COMMAND(ID_REMONT_OSMOTR_SEZON, OnRemontOsmotrSezon)
    ON_COMMAND(ID_REMONT_OSMOTR_TABLE, OnRemontOsmotrTable)
    ON_COMMAND(ID_REMONT_OSMOTR_ZHURNAL_OSMOTR, OnRemontOsmotrZhurnalOsmotr)
    ON_COMMAND(ID_REMONT_OSMOTR_ZHURNAL_POVR, OnRemontOsmotrZhurnalPovr)
    ON_COMMAND(ID_REMONT_OSMOTR_REGISTR, OnRemontOsmotrRegistr)
    ON_COMMAND(ID_REMONT_OSMOTR_AKT, OnRemontOsmotrAkt)
    ON_COMMAND(ID_REMONT_OSMOTR_DOCUMENT, OnRemontOsmotrDocument)
    ON_COMMAND(ID_REMONT_OSMOTR_ADD, OnRemontOsmotrAdd)
    ON_COMMAND(ID_REMONT_OSMOTR_DEF_ADD, OnRemontOsmotrDefAdd)
    ON_COMMAND(ID_REMONT_OSMOTR_SAVE_OSMOTR, OnRemontOsmotrSaveOsmotr)
    ON_COMMAND(ID_REMONT_OSMOTR_DEF_DEL, OnRemontOsmotrDefDel)
    ON_COMMAND(ID_REMONT_OSMOTR_DEF_MOVE, OnRemontOsmotrDefMove)
    ON_COMMAND(ID_REMONT_OSMOTR_DEF_INFO, OnRemontOsmotrDefInfo)






    ON_COMMAND(ID_EXCEL2, OnExcel2)
    ON_COMMAND(ID_EXCEL3, OnExcel3)
    ON_COMMAND(ID_EXCEL4, OnExcel4)
    ON_COMMAND(ID_ISPRAVL, OnIspravl)
    ON_COMMAND(ID_MAP_PATH, OnMapPath)
    ON_COMMAND(ID_MAP_YES, OnMapYes)
    ON_UPDATE_COMMAND_UI(ID_MAP_YES, OnUpdateMapYes)

    ON_COMMAND(ID_ALMA, OnAlma)
    ON_UPDATE_COMMAND_UI(ID_ALMA, OnUpdateAlma)

    ON_COMMAND(ID_SAVE_PNG, OnSavePNG)

    ON_UPDATE_COMMAND_UI(ID_SAVE_PJEZO, OnUpdateSavePjezo)
    ON_COMMAND(ID_UT_NAPR2, OnUtNapr2)

    ON_COMMAND(ID_C3_DEL, OnC3Del)
    ON_COMMAND(ID_C3_ADD, OnC3Add)
    ON_COMMAND(ID_ISPR_LAST, OnIsprLast)

    ON_COMMAND(ID_POVOROT2, OnPovorot2)
    ON_UPDATE_COMMAND_UI(ID_POVOROT2, OnUpdatePovorot2)

    ON_COMMAND(ID_COLOR_LINE, OnColorLine)
    ON_COMMAND(ID_COLOR_LINE1, OnColorLine1)


    ON_COMMAND(ID_RESET_GID, OnResetGid)
    ON_COMMAND(ID_RESET_GEO, OnResetGeo)

    //  ON_COMMAND(ID_USERS, OnUsers)

    ON_COMMAND(ID_AKT_TABLE, OnAktTable)
    ON_COMMAND(ID_AKT_CREATE, OnAktCreate)
    ON_COMMAND(ID_AKT_ONOFF, OnAktOnOff)
    ON_COMMAND(ID_AKT_INFO, OnAktInfo)
    ON_UPDATE_COMMAND_UI(ID_AKT_ONOFF, OnUpdateAktOnOff)
    ON_COMMAND(ID_AKT_DEL, OnAktDel)

    ON_UPDATE_COMMAND_UI(ID_AKT_CREATE, OnUpdateAktCreate)

    ON_COMMAND(ID_PASSPORT_TP, OnPassportTpCreate)
    ON_UPDATE_COMMAND_UI(ID_PASSPORT_TP, OnUpdatePassportTpCreate)

    ON_COMMAND(ID_VYD_RS, OnVydRs)
    ON_COMMAND(ID_VYD_MS, OnVydMs)

    ON_COMMAND(ID_VYD_RS2, OnVydRs2)
    ON_COMMAND(ID_VYD_MS2, OnVydMs2)


    ON_COMMAND(ID_VYD_MAG, OnVydMag)
    ON_COMMAND(ID_VYD_OPR_UCH, OnVydUchOpr)


    ON_COMMAND(ID_PT_VYD_KV, OnPtVydKv)
    ON_COMMAND(ID_PT_VYD_UR, OnPtVydUr)
    ON_COMMAND(ID_PT_VYD_TR, OnPtVydTr)



    //  ON_COMMAND(ID_VYD_KOD0, OnVydKod0)

    ON_COMMAND(ID_INVERT, OnInvert)

    ON_UPDATE_COMMAND_UI(ID_GEO_INFO, OnUpdateGeo)
    ON_UPDATE_COMMAND_UI(ID_KLASSIF, OnUpdateGeo)
    ON_UPDATE_COMMAND_UI(ID_FIND_PODL, OnUpdateGeo)
    //  ON_UPDATE_COMMAND_UI(ID_PODL_DIR, OnUpdateGeo)
    ON_UPDATE_COMMAND_UI(ID_ZD, OnUpdateGeo)
    ON_UPDATE_COMMAND_UI(ID_FILTR, OnUpdateGeo)
    ON_WM_CLOSE()

    ON_COMMAND(ID_SET_ADDR, OnSetAddr)

    ON_COMMAND(ID_NAGR_ZD, OnNagrZd)
    ON_COMMAND(ID_NAGR_ORG, OnNagrOrg)
    ON_COMMAND(ID_NAGR_ZD_NEIZ, OnNagrZdNeiz)
    ON_COMMAND(ID_NAGR_ORG_NEIZ, OnNagrOrgNeiz)

    //    ON_COMMAND(ID_TU_IST, OnTuIst)

    ON_COMMAND(ID_TU_EXCEL, OnTuExcel)
    ON_COMMAND(ID_TU_SVOD, OnTuSvod)
    ON_COMMAND(ID_TU_ZHURNAL, OnTuZhurnal)

    ON_COMMAND(ID_TU_TABLE, OnTuTable)
    ON_COMMAND(ID_TU_TABLE_NEIZ, OnTuTableNeiz)
    ON_COMMAND(ID_TU_YEAR, OnTuYear)
    ON_COMMAND(ID_TU_IST, OnTuIst)
    ON_COMMAND(ID_TU_RAY, OnTuRay)
    ON_COMMAND(ID_TU_KAM, OnTuKam)
    ON_COMMAND(ID_TU_SOST, OnTuSost)
    ON_COMMAND(ID_TU_ISTO, OnTuIstO)

    ON_COMMAND(ID_TU_ZAV, OnTuZav)

    ON_COMMAND(ID_PRIS_NAGR, OnPrisNagr)
    ON_COMMAND(ID_PRIS_NAGR_EDIT, OnPrisNagrEdit)
    ON_COMMAND(ID_PRIS_NAGR_DEL, OnPrisNagrDel)

    ON_COMMAND(ID_FIND_TU_IST, OnFindTuIst)
    ON_COMMAND(ID_TU_LINE, OnTuLine)

    //    ON_COMMAND(ID_VIEW_DB_ALMA, OnViewDbAlma)
    ON_COMMAND(ID_VIEW_DB_ALMA, OnViewDbPts)

    ON_COMMAND(ID_GOOGLE_ELEVATION_CONFIG, OnGoogleElevationConfig)

    ON_UPDATE_COMMAND_UI(ID_ELEVATION2, OnUpdateElevation2)

//    ON_COMMAND(ID_OPENSTREETMAP_INFO, OnOSMInfo)

    ON_COMMAND(ID_POPUP_PAS, OnPopupPas)

    ON_UPDATE_COMMAND_UI(ID_MAP_SEARCH, OnUpdateGoogleMap)
//    ON_UPDATE_COMMAND_UI(ID_GOOGLE_ELEVATION, OnUpdateGoogleMap)
//    ON_UPDATE_COMMAND_UI(ID_OPENSTREETMAP_INFO, OnUpdateGoogleMap)
    ON_UPDATE_COMMAND_UI(ID_MAP_CITY, OnUpdateGoogleMap)
    ON_UPDATE_COMMAND_UI(ID_ADD_CITY, OnUpdateGoogleMap)

    ON_COMMAND(ID_KORROZ_DOC1, OnKorrozDoc1)
    ON_COMMAND(ID_KORROZ_DOC2, OnKorrozDoc2)
    ON_COMMAND(ID_KORROZ_DOC3, OnKorrozDoc3)
    ON_COMMAND(ID_KORROZ_DOC4, OnKorrozDoc4)
    ON_COMMAND(ID_KORROZ_DOC5, OnKorrozDoc5)



    ON_COMMAND(ID_REMONT_OTCHET, OnRemontOtchet)
    ON_COMMAND(ID_KORROZ_OTCHET, OnKorrozOtchet)

    ON_COMMAND(ID_POPUP_OTKL_POTR, OnPopupOtklPotr)

    ON_COMMAND(ID_POPUP_ADD_KAPITAL, OnPopupAddKapital)

    ON_COMMAND(ID_ELECTRO_IST, OnElectroIst)
    ON_COMMAND(ID_ELECTRO_PRIEM, OnElectroPriem)
    ON_COMMAND(ID_ELECTRO_LINE, OnElectroLine)
    ON_COMMAND(ID_ELECTRO_MUFTA, OnElectroMufta)
    ON_COMMAND(ID_ELECTRO_OPORA, OnElectroOpora)
    ON_COMMAND(ID_ELECTRO_GIL, OnElectroGil)
    ON_COMMAND(ID_ELECTRO_KANAL, OnElectroKanal)
    ON_COMMAND(ID_ELECTRO_REPORT, OnElectroRemont)

    ON_COMMAND(ID_SET_LINEID, OnSetLineID)

    ON_UPDATE_COMMAND_UI(ID_ELECTRO_IST, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_PRIEM, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_LINE, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_MUFTA, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_OPORA, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_GIL, OnUpdateElectro)
    ON_UPDATE_COMMAND_UI(ID_ELECTRO_KANAL, OnUpdateElectro)

    ON_COMMAND(ID_POPUP_INFO_NORM_TEMP, OnPopupNormTemp)
    ON_COMMAND(ID_POPUP_TG, OnPopupTg)
    ON_COMMAND(ID_POPUP_TG2, OnPopupTg2)

    //    ON_COMMAND(ID_POPUP_PR, OnPopupPr)

    ON_COMMAND(ID_TUBING_1, OnTubing1)
    ON_COMMAND(ID_TUBING_2, OnTubing2)
    ON_COMMAND(ID_TUBING_3, OnTubing3)
    ON_COMMAND(ID_TUBING_4, OnTubing4)
    ON_COMMAND(ID_TUBING_5, OnTubing5)
    ON_COMMAND(ID_FIND_KTI, OnFindKti)

    ON_COMMAND(ID_SET_IST, OnSetIst)
    ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    ON_MESSAGE(WM_USER_ADO_NOUPDATED, OnAdoNoUpdated)

    ON_MESSAGE(WM_USER_SHURF_UTVERDIT_ALL, OnShurfUtverditALL)


    ON_COMMAND(ID_SQL_SAVE, OnSqlSave)
    ON_COMMAND(ID_SQL_RESTORE, OnSqlRestore)

    ON_COMMAND(ID_SQL_COPY_FRAGMENT, OnSqlCopyFragment)
    ON_COMMAND(ID_SQL_DEL_FRAGMENT, OnSqlDelFragment)

    ON_COMMAND(ID_SQL_BACKUP, OnSqlBackup)
    ON_COMMAND(ID_SQL_BACKUP_RESTORE, OnSqlBackupRestore)


    ON_COMMAND(ID_EXPORT_PTS, OnExportPTS)
    ON_COMMAND(ID_IMPORT_PTS, OnImportPTS)


    ON_COMMAND(ID_ADD_FRAGMENT, OnAddFragment)

    ON_COMMAND(ID_EXPORT_FRAGMENT, OnExportFragment)
    ON_COMMAND(ID_EXPORT_FRAGMENT_VYD, OnExportFragmentVyd)
    ON_COMMAND(ID_EXPORT_FRAGMENTS_MULTY, OnExportFragmentsMulty)

    ON_COMMAND(ID_IMPORT_FRAGMENT_MDB, OnImportFragmentMDB)
    ON_COMMAND(ID_IMPORT_FRAGMENT_MAG, OnImportFragmentMag)

    ON_COMMAND(ID_EXPORT_CXE, OnExportCxe)
    ON_COMMAND(ID_IMPORT_CXE, OnImportCxe)

    ON_COMMAND(ID_IMPORT_FRAGMENT, OnImportFragment)

    ON_COMMAND(ID_MAS_MARK_ALL, OnMasMarkAll)
    ON_UPDATE_COMMAND_UI(ID_MAS_MARK_ALL, OnUpdateMasMarkAll)

    ON_COMMAND(ID_ASTR, OnAstr)
    ON_UPDATE_COMMAND_UI(ID_ASTR, OnUpdateAstr)

    ON_COMMAND(ID_1000, On1000)
    ON_COMMAND(ID_1001, On1001)
    ON_COMMAND(ID_1002, On1002)


    ON_COMMAND(ID_CREATE_SORT_NODE, OnCreateSortNode)



    ON_UPDATE_COMMAND_UI(ID_LAST_OPRES, OnUpdateOpr)
    ON_UPDATE_COMMAND_UI(ID_ZHURNAL_DEFECT_OPRES, OnUpdateSaveCurentOpres)

    ON_UPDATE_COMMAND_UI(ID_SAVE_OPRES_NEW, OnUpdateSaveOpres)
    ON_UPDATE_COMMAND_UI(ID_SAVE_OPRES, OnUpdateSaveCurentOpres)
    ON_UPDATE_COMMAND_UI(ID_NAPR_TABLE_OPRES, OnUpdateSaveCurentOpres)
    ON_UPDATE_COMMAND_UI(ID_SAVE_OPRES, OnUpdateSaveOpres)
    ON_UPDATE_COMMAND_UI(ID_REMONTS_ADD_OPRES, OnUpdateSaveCurentOpres)

    // ---------------------------------------

    ON_COMMAND(ID_LIST_OPRES, OnListOpres)
    ON_COMMAND(ID_LAST_OPRES, OnLastOpres)
    ON_COMMAND(ID_NAPR_TABLE_OPRES, OnNaprTableOpres)
    ON_COMMAND(ID_SAVE_OPRES, OnSaveOpres)
    ON_COMMAND(ID_SAVE_OPRES_NEW, OnSaveOpresNew)

    // ---------------------------------------

    ON_COMMAND(ID_LIST_REMONT2, OnListRemont2)
    ON_COMMAND(ID_LAST_REMONT2, OnLastRemont2)
    ON_COMMAND(ID_NAPR_TABLE_REMONT2, OnNaprTableRemont2)
    ON_COMMAND(ID_SAVE_REMONT2, OnSaveRemont2)
    ON_COMMAND(ID_SAVE_REMONT2_NEW, OnSaveRemont2New)

    // ---------------------------------------

    ON_COMMAND(ID_LIST_OSVIDET2, OnListOsvidet2)
    ON_COMMAND(ID_LAST_OSVIDET2, OnLastOsvidet2)
    ON_COMMAND(ID_NAPR_TABLE_OSVIDET2, OnNaprTableOsvidet2)
    ON_COMMAND(ID_SAVE_OSVIDET2, OnSaveOsvidet2)
    ON_COMMAND(ID_SAVE_OSVIDET2_NEW, OnSaveOsvidet2New)

    // ---------------------------------------

    ON_COMMAND(ID_LIST_OSMOTR2, OnListOsmotr2)
    ON_COMMAND(ID_LAST_OSMOTR2, OnLastOsmotr2)
    ON_COMMAND(ID_NAPR_TABLE_OSMOTR2, OnNaprTableOsmotr2)
    ON_COMMAND(ID_SAVE_OSMOTR2, OnSaveOsmotr2)
    ON_COMMAND(ID_SAVE_OSMOTR2_NEW, OnSaveOsmotr2New)

    // ---------------------------------------


    ON_COMMAND(ID_OSMOTR_ZAPROS1, OnOsmotrZapros1)
    ON_COMMAND(ID_OSMOTR_ZAPROS2, OnOsmotrZapros2)
    ON_COMMAND(ID_OSMOTR_ZAPROS3, OnOsmotrZapros3)
    ON_COMMAND(ID_OSMOTR_ZAPROS4, OnOsmotrZapros4)
    ON_COMMAND(ID_OSMOTR_ZAPROS5, OnOsmotrZapros5)


    // Ремонты

    ON_UPDATE_COMMAND_UI(ID_REMONT_ADD_PLAN, OnUpdateVydRemont)
    ON_UPDATE_COMMAND_UI(ID_REMONT_ADD_CURRENT, OnUpdateVydRemont)

    ON_UPDATE_COMMAND_UI(ID_OPRES_ADD_PLAN, OnUpdateVydRemont)

    ON_COMMAND(ID_REMONT_VYBOR_NACH, OnOpresTeploSet)
    ON_COMMAND(ID_OPRES_VYBOR_NACH, OnOpresTeploSet)

    ON_COMMAND(ID_OPRES_ADD_PLAN, OnOpresAddPlan)

    ON_COMMAND(ID_REMONT_ADD_PLAN, OnRemontAddPlan)
    ON_COMMAND(ID_REMONT_ADD_CURRENT, OnRemontAddCurrent)
    ON_COMMAND(ID_LIST_REMONT2, OnListRemont2)
    ON_COMMAND(ID_LAST_REMONT2, OnLastRemont2)
    ON_COMMAND(ID_REMONT_TRUB, OnRemontTrub)
    ON_COMMAND(ID_REMONT_PLAN, OnRemontPlan)
    ON_COMMAND(ID_REMONT_CURRENT, OnRemontCurrent)
    ON_COMMAND(ID_REMONT_PROCESS, OnRemontProcess)
    ON_COMMAND(ID_REMONT_VYPOLNENO, OnRemontVypolneno)
    ON_COMMAND(ID_REMONT_ALL2, OnRemontAll2)
    ON_COMMAND(ID_REMONT_UTVERDIT, OnRemontUtverdit)
    ON_COMMAND(ID_OPRESSOVKA_PASSPORT, OnOpressovkaPassport)
    ON_COMMAND(ID_REMONT_ANALIZ2, OnRemontAnaliz2)
    ON_COMMAND(ID_REMONT_ANALIZ, OnRemontAnaliz)


    ON_COMMAND(ID_OSMOTR_SAVE, OnSaveOsmotr2)
    ON_COMMAND(ID_OPRES_SAVE , OnSaveOpres )
    ON_COMMAND(ID_REMONT_SAVE, OnSaveRemont2)

    ON_UPDATE_COMMAND_UI(ID_OSMOTR_SAVE, OnUpdateOsmotrSave)
    ON_UPDATE_COMMAND_UI(ID_OPRES_SAVE , OnUpdateOpresSave)
    ON_UPDATE_COMMAND_UI(ID_REMONT_SAVE, OnUpdateRemontSave)


    ON_COMMAND(ID_OPRES_PLAN        ,OnOpresPlan      )
    ON_COMMAND(ID_OPRES_PROCESS     ,OnOpresProcess   )
    ON_COMMAND(ID_OPRES_VYPOLNENO   ,OnOpresVypolneno )
    ON_COMMAND(ID_OPRES_ALL2        ,OnOpresAll2      )
    ON_COMMAND(ID_OPRES_WORD1       ,OnOpresWord1     )
    ON_COMMAND(ID_OPRES_WORD2       ,OnOpresWord2     )
    ON_COMMAND(ID_OPRES_ANALIZ2     ,OnOpresAnaliz2   )
    ON_COMMAND(ID_OPRES_ANALIZ      ,OnOpresAnaliz    )



    //    BUTTON      ID_REMONT_POVR_DEF_OTOBR
    //    BUTTON      ID_REMONT_POVR_OTOP
    //    BUTTON      ID_REMONT_OSMOTR_TEPLO_SET
    //    BUTTON      ID_MARK_REV

    ON_COMMAND(ID_REMONT_VYBOR_NACH, OnRemontVyborNach)


    ON_COMMAND(ID_OSMOTR_AKT, OnOsmotrAkt)
    ON_COMMAND(ID_OSMOTR_ANALIZ, OnOsmotrAnaliz)
    ON_COMMAND(ID_OSMOTR_ANALIZ2, OnOsmotrAnaliz2)

    ON_COMMAND(ID_LAST_FAKTORY, OnLastFaktory)

    //----------------------------------------------------
    // Опрессовки

    ON_COMMAND(ID_OPRES_GRANITSA_RAZDELA, OnOpresZdAll)



    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CScroll::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CScroll::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScroll::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGidrView construction/destruction





CString getParams(CAdoFile* ado, const char* tn)
{
    if (!ado) return "*";

    int i, n = ado->NColsMySQL(tn);

    CString par = "";

    for (i = 0; i < n && i < 1000; i++) {
        MySQLColumns* col = ado->findMySQLColumnsOrd(tn, i + 1);
        if (col) {
            //            if (col->strDATA_TYPE != "geometry" && col->strDATA_TYPE != "ntext") {
            if (col->strDATA_TYPE != "geometry") {
                if (par != "") par += ",";
                par += "[";
                par += col->strCOLUMN_NAME;
                par += "]";
            }
        }
    }

    return par;
}

CString getParamsObj(CAdoFile* ado, const char* tn, const char* obj)
{
    if (!ado) return "*";

    int i, n = ado->NColsMySQL(tn);

    CString par = "";

    for (i = 0; i < n && i < 1000; i++) {
        MySQLColumns* col = ado->findMySQLColumnsOrd(tn, i + 1);
        if (col) {
            //            if (col->strDATA_TYPE != "geometry" && col->strDATA_TYPE != "ntext") {
            if (col->strDATA_TYPE != "geometry" && col->strCOLUMN_NAME != "id") {
                if (par != "") par += ",";

                par += "[";
                par += obj;
                par += "].";

                par += "[";
                par += col->strCOLUMN_NAME;
                par += "]";
            }
        }
    }

    return par;
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID)
{
    CString fn;

    fn.Format("%ssql\\%s.sql", argpath(), tn);

    CString q = readFile(fn), ss;

    if (q != "") {
        if (fileID >= 0) {
            ss.Format("%d", fileID);
            q.Replace("$fileID$", ss);
        }
        else if (fileID == -1) {
            q.Replace("=$fileID$", "<> -1");
        }
    }
    else {
        if (ado) {
            CString par = getParams(ado, tn);
            q.Format("SELECT %s FROM %s", par, tn);
        }
        else {
            q.Format("SELECT * FROM %s", tn);
        }
    }

    return q;
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID)
{
    CString fn;

    fn.Format("%ssql\\%s.sql", argpath(), tn);

    CString q = readFile(fn), ss;

    if (q != "") {
        //      ss.Format("%s", fileID);
        //      q.Replace("$fileID$", ss);

        ss.Format(" IN (%s)", fileID);
        q.Replace("=$fileID$", ss);
        q.Replace("in ($fileID$)", ss);
        q.Replace("IN ($fileID$)", ss);
    }
    else {
        if (ado) {
            CString par = getParams(ado, tn);
            q.Format("SELECT %s FROM %s", par, tn);
        }
        else {
            q.Format("SELECT * FROM %s", tn);
        }
    }

    return q;
}


CString readQ_select(CAdoFile* ado, CGraph2* graph, const char* tn, const char* tn2)
{
    CString fn, vyd, s;

    fn.Format("%ssql\\%s.sql", argpath(), tn);

    CString q = readFile(fn), ss;

    if (q != "") {
        if (q.Find("n1.fileID=$fileID$") > 0) {
            create_vyd_line_table(ado, graph, vyd);
            s.Format("l.id IN (SELECT ID from %s)", vyd);
            q.Replace("n1.fileID=$fileID$", s);
        }
        if (q.Find("n.fileID=$fileID$") > 0) {
            create_vyd_node_table(ado, graph, vyd);
            s.Format("n.id IN (SELECT ID from %s)", vyd);
            q.Replace("n.fileID=$fileID$", s);
        }
    }

    return q;
}

/*
void setView(CGidrView* pView0)
{
    CGidrView* pView;

    CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

    if (pFrame) {
        CMDIChildWnd* curChild, * curChild1;
        curChild1 = curChild = pFrame->MDIGetActive();

        do {
            pView = (CGidrView*)(curChild->GetActiveView());

            if (pView == pView0) {
                break;
            }

            pFrame->MDINext();
            curChild = pFrame->MDIGetActive();
        } while (curChild != curChild1);
    }
}
*/


void setView(CView* pView0)
{
    CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

    log1("setView");

    if (pFrame) {
        CMDIChildWnd* curChild, * curChild1;
        curChild1 = curChild = pFrame->MDIGetActive();

        do {
            CView* pView = (CView*)(curChild->GetActiveView());

            if (pView == pView0) {
                if (pView->IsKindOf(RUNTIME_CLASS(CGidrView))) {
                    setGidView((CGidrView*)pView);
                }
                break;
            }
            pFrame->MDINext();
            curChild = pFrame->MDIGetActive();
        } while (curChild != curChild1);
    }
}

CGidrView* getGidView();

CGidrView* getView()
{
    log1("getView");
    
    CGidrView* gv = getGidView();
    if (gv) return gv;

    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    CGidrView* pView;

    CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

    if (pFrame) {
        CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
        pView = (CGidrView*)pChild->GetActiveView();
        if (pChild && pView) {

            Cgid6Doc* pDoc = (Cgid6Doc*)pChild->GetActiveView()->GetDocument();

            if (pDoc) {
                POSITION pos = pDoc->GetFirstViewPosition();
                while (pos != NULL)
                {
                    pView = (CGidrView*)pDoc->GetNextView(pos);
                    if (pView->IsKindOf(RUNTIME_CLASS(CGidrView))) {
                        log1("Нашел");
                        return pView;
                    }
                }
            }
        }
    }

    log1("Не нашел");
    
    return NULL;
}

CString getParAll()
{
    CGidrView * view = getView();

    if (view) {
        return view->m_cxema->getParAll();
    }
    return "";
}

CGidFile* CGidrView::curFile()
{
    if (!m_cxema->m_cur_file) {

        int nn = 0;

        CGidFile* gf = NULL;

        map<long, CGidFile*>::iterator it = m_cxema->m_file_map.begin();
        for (; it != m_cxema->m_file_map.end(); it++) {
            gf = it->second;
            nn++;
        }

        if (nn == 1 && gf) {
            setActiv(gf->id);
            return gf;
        }
    }
    return m_cxema->m_cur_file;
}



void CGidrView::init()
{
    m_isMarkOnly = 0;

    m_isMS_RS_KOT_Yes = FALSE;  // Показывать цвета по типам РС
    m_isOnlyPTSColor = FALSE; // Выделять цветами только участки ПТС и чужие. остальные черным

    m_idMenu = TYP_MENU_REZHIM;
    m_napr1 = NULL;
    //  m_parent_kam = NULL;
    m_kls_for_del = NULL;
    m_kls = NULL;
    m_klsP = NULL;

    m_id_opr_old = -1;
    m_id_remont2_old = -1;
    m_id_osvidet2_old = -1;
    m_id_osmotr2_old = -1;


    m_id_opr = -1;
    m_id_remont2 = -1;
    m_id_osvidet2 = -1;
    m_id_osmotr2 = -1;

    m_bIsOpen = FALSE;

    m_bIsFlag = FALSE;
    m_zima = true;
    m_bIsNaprGid = TRUE;
    m_CurNode = NULL;
    m_CurLine = NULL;
    m_bIsPodp = TRUE;
    m_bIsPodpNiz = FALSE;
    m_bIsPodpVerh = TRUE;
    m_bIsRezhim = TRUE;
    m_bIsPTS = FALSE;
    m_bIsPribor = TRUE;
    m_bIsPodpRes = FALSE;
    m_isGeobaza = TRUE;
    m_bIsMas = FALSE;
    m_bIsGeoGeodez = TRUE;
    m_bOnlyGid = FALSE;
    m_bIsPicture = FALSE;
    m_bIsOpenPicture = FALSE;
    m_isKm = TRUE;
    m_bIsAlma = FALSE;
    m_strPicture = "";
    m_pathMap = "";
    m_dxf = NULL;

    m_bk_color = 0xFFFFFF;

    m_cxema = NULL;
    m_menuType = IDR_GIDRTYPE;

    m_tmpPicN[0] = 0;
    m_nRegim = ID_INFO;
    m_nTimer = 0;
    m_nTimer2 = 0;
    m_nTimer3 = 0;
    //  m_pOPCThread = 0;
    scroll_type = 1;
    m_pMapsThread = NULL;
    m_map = ID_NOMAP;
    m_isInternet = FALSE;
    m_reread = FALSE;
    m_isUkrup = FALSE;

    m_Magistral1 = "";
    m_MagistralID1 = -1;
    m_Magistral2 = "";
    m_MagistralID2 = -1;

    m_MagistralTyp = 1;

    m_mag_n = 0;

    m_MagLine = 0;
    m_MagLineNode = NULL;

    m_mag = 0;
    m_ms = 0;
    m_rs = 0;

    m_mag_old = 0;
    m_ms_old = 0;
    m_rs_old = 0;

    cur_geo_tn = "";
    cur_geo_n = -1;

    m_id_RS = -1;
    m_id_MS = -1;
    m_kod = "";
    m_is_rs = -1;

    m_ms_kod0 = 0;
    m_rs_kod0 = 0;
    m_magistral_kod0 = 0;

    m_pts_ut_kod = 0;

    m_kod_vyd = -1;
    //  m_kod0_vyd = "";
    //  m_kod0_ms_rs = 0;

    m_magistral_kod = 0;
    m_ms_kod = 0;
    m_rs_kod = 0;

    m_pts_ut_kod = 0;

    m_node_first = NULL;
    m_node_last = NULL;

    cur_node = NULL;
    qqq_list = new CPodlList;
    m_n_kapremont = 0;

    m_isOpressovaka = false;
    m_isNeOpr = false;

    m_opressovka = 0;
    m_opressovka_uchastok = 0;

    m_isPodpAll = FALSE;

    m_field_for_dist = "";

    m_QG = -1;

}

CGidrView::CGidrView()
{
    log1("CGidrView::CGidrView");

    m_is_CTeploset = false;
    m_is_reset_visible = true;
    m_node_pick1 = nullptr;
    m_node_pick2 = nullptr;

    m_prop_grid = nullptr;
    m_nachalnikID = -1;
    m_n1 = nullptr;
    m_n2 = nullptr;

    m_n1_neotr = nullptr;
    m_n2_neotr = nullptr;

    m_is_geo = false;
    m_ado_update_typ = -1;
    m_ado_update_typ = -1;
    m_parent_id = 0;
    m_text_del = nullptr;
    m_wait = 0;

    m_niz_code = -1;

    m_zhurnal = -1;

    m_fileID2 = 0;

    COleDateTime date = COleDateTime::GetCurrentTime();

    m_sezon_korrozia = date.GetYear();
    m_sezon_date1 = COleDateTime();
    m_sezon_date2 = COleDateTime();

    m_linePTS_ID = -1;

    init();
    //  CClientDC screenDC(NULL);
    //  geom.dmas = screenDC.GetDeviceCaps(LOGPIXELSX)/2.54;

    m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_GIDRTYPE));

}

HTREEITEM FindChild(CTreeCtrl* tree, HTREEITEM hItem, const char* txt);


CGidrView::~CGidrView()
{
    CString ss;
    ss.Format("CGidrView::~CGidrView %d", this);
    log1(ss);

//    ::WaitForSingleObject(m_pMapsThread->m_hThread, INFINITE);
//    m_pMapsThread = nullptr;
//    log1("CGidrView::~CGidrView");


}


BOOL CGidrView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CScroll::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGidrView drawing

/*
void CGidrView::OnDraw(CDC* pDC)
{
Cgid6Doc* pDoc = GetDocument();
ASSERT_VALID(pDoc);
// TODO: add draw code for native data here
}
*/

/////////////////////////////////////////////////////////////////////////////
// CGidrView printing

BOOL CGidrView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    //  return DoPreparePrinting(pInfo);
    return CScroll::OnPreparePrinting(pInfo);

}

void CGidrView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CGidrView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGidrView diagnostics

#ifdef _DEBUG
void CGidrView::AssertValid() const
{
    CScroll::AssertValid();
}

void CGidrView::Dump(CDumpContext& dc) const
{
    CScroll::Dump(dc);
}

Cgid6Doc* CGidrView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgid6Doc)));
    return (Cgid6Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGidrView message handlers

bool CGidrView::isPot()
{
    if (IsMain()) return false;
    if (!m_cxema) return false;

    CNode2* n = m_cxema->m_graph->find(m_parent_id);
    if (n) {
#if 0
        if (n->node.typ == TIP_PR || n->node.typ == TIP_PO) {
            AfxMessageBox(_TR("Не разрешено редактирование структуры потребителей"), MB_OK | MB_ICONINFORMATION);
            return true;
        }
#endif
    }
    return false;
}


bool CGidrView::del(const CFPoint& point)
{
    CNode2* node;
    CLINE2* line;
    CDC* dc = NULL;
    CString str;

    if (!isEditGid()) return false;

    //    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    if (pDoc && /* pDoc->i */ isNoDelete()) {
        AfxMessageBox(_TR("Удаление запрещено!"), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    if (isPot()) return false;

    m_CurNode = NULL;
    m_CurLine = NULL;

    node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);

    if (node) {
        return delNode(node, true);
    }

    line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx * D5 / mas_otn);
    if (line) {
        return delLine(line, true);
    }

    set<CText>::const_iterator p = m_cxema->set_text.begin();

    while (p != m_cxema->set_text.end()) {
        double h = hypot(point.x - p->coord.x, point.y - p->coord.y);
        if (h < geom.masx * D5 && p->internalNodeID == m_parent_id) {
            CString str;
            str.Format(_TR("Удалить текст \"%s\"?"), p->text);
            if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) == IDYES) {
                if (m_cxema->delText(&(*p))) {
                    GetDocument()->SetModifiedFlag(true);
                    InvalidateG();
                    return true;
                }
            }
            break;
        }
        p++;
    }
    return false;
}

bool CGidrView::del_geo(const char* fn, int n)
{
    if (!isDeletable(fn, n)) return false;

    if (!isEditGeo(fn)) return false;

    if (n) {
        CString str;
        str.Format(_TR("Удалить %s?"), findTableRusNameGeo(fn));
        if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) == IDYES) {
            delGeoObj(fn, n);
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
            return true;
        }
    }
    return false;
}

bool CGidrView::del_geo(const char* fn, const CFPoint& pt)
{
    int n = m_geofile->findXY3(pt, D5 * geom.masx, 3, fn);
    if (n) {
        return del_geo(fn, n);
    }
    return false;
}




//#include "GidrDlg.h"
//#include "CxemaDlg.h"
//#include "DataEdit.h"


/*
void AddTable(CFPoint point, const char *fn, double md)
{
Klassif *kls = m_geofile->m_kl_list->findKlN(fn);
if (kls && kls->ins && kls->m_db) {
int n = m_geofile->findXY3(point, md, 3, fn);
if (n) {
AddTable(kls->m_db->m_strFileName, fn, n);
}
}
}
*/


bool CGidrView::info_TGID(const CFPoint& point)
{
    bool ret;

    if (m_idMenu == TYP_MENU_PTS) {
        m_idMenu = TYP_MENU_READONLY;
        ret = info(point);
        m_idMenu = TYP_MENU_PTS;
    }
    else {
        ret = info(point);
    }
    return ret;
}



bool CGidrView::infoNode(CNode2* node)
{
    return true;
}


bool CGidrView::info(const CFPoint& point)
{
    bool ret = false;
    int n_geo = 0;


    char fn_geo[256];


    bool is_geo_point_find = false;

    if (m_isGeobaza && m_parent_id == 0) {
        n_geo = m_geofile->findXY(point, D5 * geom.masx, 3, fn_geo);
        if (n_geo && isEditGeo(fn_geo)) {
            Klassif* kls = m_geofile->m_kl_list->findKlN(fn_geo);
            if (kls && isPoint(kls->loc)) {
                is_geo_point_find = true;
            }
        }
    }


    CNode2* node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);
    CLINE2* line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx * D5 / mas_otn);

    if (node || line) {
        ret = info(m_cxema, node, line);
        return ret;
    }


    if (n_geo) {
        GeoInfo(fn_geo, n_geo, false);
    }

    return ret;
}


// Если щелкаешь два раза


bool CGidrView::info2(const CFPoint& point)
{
    CNode2* node = NULL;
    CLINE2* line = NULL;
    CString str, q;

    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();


    //  ShowCXEMA(NULL, NULL);

    node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);
    line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx * D5 / mas_otn);

    //  setIsEdit(isEditGid());

    return info(m_cxema, node, line);
}

bool CGidrView::info_gid_node(const CNode2* node)
{
    BeginDrawNode(node);

    CDC* dc = SaveImage();

    if (node->node.nomgP == -1 && node->node.nomgO == -1) {
        AfxMessageBox(_TR("Отсоединенный фрагмент"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        info_gid(m_cxema, node, NULL);



        //        CString s;
        //        s.Format("%s\n%d %d\n%s", node->getTableMySQL(), node->node.nomgP, node->node.nomgO, node->getName());
        //        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                /**
                if ((node->node.typ == TIP_PR || node->node.typ == TIP_PO) && node->node.nomPT >= 0) {
                if (node->file->m_dbf->openTable("PT_OUT")) {
                COutDialog od(this, node->file->m_dbf, _TR("Потребитель"), "PT_OUT", node->nomPT, -1);
                od.DoModal();
                node->file->m_dbf->closeTable();
                }
                }
                else if ((node->typ == TIP_HS) && node->nomPT >= 0) {
                if (node->file->m_dbf->openTable("NST_OUT")) {
                COutDialog od(this, node->file->m_dbf, _TR("Насосная станция"), "NST_OUT", node->nomPT, -1);
                od.DoModal();
                node->file->m_dbf->closeTable();
                }
                }
                else {
                if (node->file->m_dbf->openTable("US_OUT")) {
                COutDialog od(this, node->file->m_dbf,
                //          nodef[node->typ].out,
                _TR("Узел"),
                "US_OUT",
                node->nomgP, node->nomgO);
                od.DoModal();
                node->file->m_dbf->closeTable();
                }
                }
                */
    }
    CloseImage(dc);

    EndDrawNode();
    return false;
}



bool CGidrView::info_gid_node_DR(const CNode2* node)
{
    BeginDrawNode(node);

    CDC* dc = SaveImage();

    if (node->node.nomgP == -1 && node->node.nomgO == -1) {
        AfxMessageBox(_TR("Отсоединенный фрагмент"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        info_gid_DR(m_cxema, node);
    }
    CloseImage(dc);

    EndDrawNode();
    return false;
}




int ado_info_line_out(CWnd* wnd, CAdoFile* ado, const char* tn1, int id1, const char* tn2, int id2, int idN1, int idN2, int fileID, const char* tn_out, int idgP, int idgO, const char* capt);

bool CGidrView::info_gid_line(const CLINE2* line)
{
    CLine2* l = bline(line);

    CDC* dc1 = SaveImage();

    BeginDrawLine(line);
    CDC* dc = SaveImage();
    CString s;

    if (l->line.nomgP == -1 && l->line.nomgO == -1) {
        //        AfxMessageBox(_TR("Отсоединенный фрагмент"), MB_OK|MB_ICONINFORMATION);
        s.Format(_TR("Отсоединенный фрагмент %s\n%d %d\n%s"), bline(line)->getTableMySQL(), bline(line)->line.nomP, bline(line)->line.nomO, bline(line)->getName());
        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
    }
    else {
        //        s.Format("%s\n%d %d\n%s", bline(line)->getTableMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, bline(line)->getName());
        //        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
        //        ado_info_line_out(this, cxema->m_ado, bline(line)->getTableOutMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, bline(line)->getTable());

        info_gid(m_cxema, NULL, line);
    }
    //    else if (where(line)->file->m_dbf->openTable(linef[l->typ].out)) {
    //        COutDialog od(this, where(line)->file->m_dbf, linef[l->typ].opis, linef[l->typ].out, l->nomgP, l->nomgO);
    //        od.DoModal();
    //    }
    //    where(line)->file->m_dbf->closeTable();

    CloseImage(dc);
    EndDrawLine();
    CloseImage(dc1);

    return true;
}


bool CGidrView::info_gid(const CFPoint& point)
{
    CNode2* node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);
    if (node) {
        return info_gid_node(node);
    }

    CLINE2* line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx * D5 / mas_otn);
    if (line) {
        return info_gid_line(line);
    }
    return false;
}

void CGidrView::ism(const CLINE2* l)
{
    if (!isEditPS()) return;

    CLine2* bl = bline(l);

    m_NP = bl->line.m_NP;

    if (IsBegin(l)) {
        m_NP.push_front(where(l)->node.coord);
        m_NP.push_back(other(l)->node.coord);
    }
    else {
        m_NP.push_front(other(l)->node.coord);
        m_NP.push_back(where(l)->node.coord);
    }

    m_CurLine = bl;

    m_locNP = LOC2;

    m_tecF = -1;


    regim = R_BEGIN_MOVE_LINE;
    MySetCursor(0, IDC_CROSS);
    CDC* m_dc;
    if ((m_dc = BeginPaint()) != NULL) {
        DrawLine(m_dc);
        EndPaint(m_dc);
    }
}

long getLastID(CAdoFile* ado, CString tn)
{
    long id = 0;
    CString q;
    q.Format("SELECT MAX(id) as id FROM %s", tn);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            id = ado->read_long(0);
        }
        ado->closeTable();
    }
    return id;
}

// Копировать одну строку linesobj

int copyRowLine1(CAdoFile* ado, CString tn, int id, int externalSignLineID, CString coord)
{
    CString s;
    int i, n = ado->NColsMySQL_S(ado->m_schema, tn);

    CString params1 = "";
    CString params2 = "";

    for (i = 0; i < n; i++) {
        MySQLColumns* col = ado->findMySQLColumnsOrdS(ado->m_schema, tn, i + 1);
        if (col) {

            if (col->strCOLUMN_NAME != "id") {
                if (params1 != "") params1 += ",";
                if (params2 != "") params2 += ",";

                params1 += col->strCOLUMN_NAME;

                if (col->strCOLUMN_NAME == "externalSignLineID") {
                    s.Format("%d", externalSignLineID);
                    params2 += s;
                }
                else if (col->strCOLUMN_NAME == "coords") {
                    s.Format("'%s'", coord);
                    params2 += s;
                }
                else {
                    params2 += col->strCOLUMN_NAME;
                }
            }
        }
    }

    CString q;

    long affected;

    q.Format("insert into %s (%s) select %s from %s where id = %d", tn, params1, params2, tn, id);
    int id2 = ado->ExecuteInsert(q, &affected);
    id2 = getLastID(ado, tn);

    return id2;
}


// Копировать одну строку Участка


int copyRowLine2(CAdoFile* ado, CString tn, int id, int lineID)
{
    int i, n = ado->NColsMySQL_S(ado->m_schema, tn);

    CString params1 = "";
    CString params2 = "";

    for (i = 0; i < n; i++) {
        MySQLColumns* col = ado->findMySQLColumnsOrdS(ado->m_schema, tn, i + 1);
        if (col) {

            if (col->strCOLUMN_NAME != "id") {
                if (params1 != "") params1 += ",";
                if (params2 != "") params2 += ",";
                params1 += col->strCOLUMN_NAME;

                if (col->strCOLUMN_NAME == "lineID") {
                    CString s;
                    s.Format("%d", lineID);
                    params2 += s;
                }
                else {
                    params2 += col->strCOLUMN_NAME;
                }
            }
        }
    }

    CString q;

    long affected;

    q.Format("insert into %s (%s) select %s from %s where id = %d", tn, params1, params2, tn, id);
    int id2 = ado->ExecuteInsert(q, &affected);
    id2 = getLastID(ado, tn);

    return id2;
}

int isElPoint(const char* tn1);

bool CGidrView::updateObj()
{
    if (m_nRegim == ID_FRAGMENT_POLY) {
        regim = R_FRAGM;
        return true;
    }
    else if (m_nRegim == ID_ISM_GEO) {
        if (m_kls && isEditGeo(m_kls->nazv)) {

            if (m_kls && isElPoint(m_kls->nazv)) {
                CFPoint pt = m_NP.first();

                int n = m_geofile->findXY3(pt, D5 * geom.masx, 2, EL_LINE);
                if (!n) {
                    regim = R_INIT;
                    m_NP.clear();
                    AfxMessageBox("Объект можно установить только на линию электропередач", MB_OK | MB_ICONINFORMATION);
                    InvalidateG();
                    return true;
                }
            }

            if (is_water_line(m_kls->nazv)) {  // Участок водопровода
                updateWaterLine(m_kls);
            }

            updateObj(m_kls->nazv, m_num, m_NP);
            regim = R_INIT;
            m_nRegim = ID_NODE_MOVE;
            m_NP.clear();
        }
        else {
            AfxMessageBox(_TR("Ошибка"), MB_OK | MB_ICONINFORMATION);
        }
    }
    else if (m_nRegim == ID_ISM_GEO_PODP) {
        /*
        if (m_kls && isEditGeo(m_kls->nazv)) {
        if (m_geofile->InitS(m_kls->nazv) && m_geofile->GoToID(m_num)) {
        CCoordList cl;
        m_geofile->GetCoord(cl);

        CFPoint p0 = cl.centroid();
        CFPoint p1 = m_NP.first();
        CFPoint p2 = m_NP.second();

        CFPoint pt = p1+p2;
        pt = CFPoint(pt.x/2, pt.y/2);
        pt = pt-p0;

        double x3, y3, x4, y4;

        void rect_points(double x0, double y0, double x1, double y1, double m_otn, double &x3, double &y3, double &x4, double &y4);

        rect_points(p1.x, p1.y, p2.x, p2.y, m_otn, x3, y3, x4, y4);
        double uu = atan2(y4-p1.y, x4-p1.x);

        uu = uu*180./M_PI;

        double h1 = hypot(m_xxx, m_yyy);
        double h2 = hypot(p1.x-p2.x, p1.y-p2.y);

        double coef = m_coef*h2/h1;

        if (m_geofile->GetLoc() == 1) {
        pt.y -= m_yyy;
        }
        m_geofile->writeXYText(m_kls->nazv, m_num, pt.x, pt.y, uu, coef, 0);

        regim = R_INIT;
        m_nRegim = ID_NODE_MOVE;
        m_NP.clear();
        }
        }
        else {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        }
        */
    }
    else {
        if (!isEditPS()) return false;
        m_NP.pop_front();
        m_NP.pop_back();

        CLine2* cl = bline(m_CurLine);

        if (!cl) {
            if (m_kls && isElPoint(m_kls->nazv)) {
                AfxMessageBox("Объект можно установить только на линию электропередач", MB_OK | MB_ICONINFORMATION);
            }

        }
        else {
            if (cl->line.nomP == -1 || cl->line.nomO == -1 || m_nRegim == ID_NODE_MOVE) {
                setCoord(m_CurLine, m_NP);
            }
            else if (cl->line.nomP != -1 && cl->line.nomO != -1) {
                int nomP = cl->line.nomP;
                int nomO = cl->line.nomO;
                int idP2 = cl->line.idP2;
                int idO2 = cl->line.idO2;

                CString q;
                CAdoFile* ado = m_cxema->m_ado;
                long affected;

                q.Format("UPDATE linesobj SET externalSignLineID=2 WHERE id=%d", nomP);

                if (ado->Execute(q, &affected)) {
                    CString coord = m_NP.saveStr();
                    if (nomP == nomO) {
                        nomO = copyRowLine1(ado, "linesobj", nomP, 3, coord);
                        idO2 = copyRowLine2(ado, cl->getTableMySQL(), cl->line.idP2, nomO);
                    }
                    else {
                        q.Format("UPDATE linesobj SET externalSignLineID=3, SET coords='%s' WHERE id=%d", coord, nomO);
                        bool ret = ado->Execute(q, &affected);

                        reset_shape_line(ado, nomO);
                    }
                    CLINE2* line2 = m_cxema->m_graph->insert_line(where(cl), other(cl), m_NP);

                    if (line2) {
                        CLine2* l2 = bline(line2);
                        l2->line = cl->line;

                        l2->line.nomP = -1;
                        l2->line.nomO = nomO;

                        l2->line.idP2 = -1;
                        l2->line.idO2 = idO2;
                        l2->line.m_NP = m_NP;

                        cl->line.nomO = -1;
                        cl->line.idO2 = -1;
                    }
                    Invalidate();
                }
            }
        }
        m_CurLine = NULL;
    }

    m_NP.clear();

    GetDocument()->SetModifiedFlag(true);
    InvalidateG();
    return true;
}

int compare(const void* arg1, const void* arg2);


bool CGidrView::setMark(const TCHAR* table)
{
    /*
    char kod[256], uzel[256], pr[2];
    char kod1[256], uzel1[256], pr1[2];
    char kod2[256], uzel2[256], pr2[2];
    CNode *n1, *n2, *n;
    CString q;

    int is_line = 1;
    int ret;

    q.Format("Select [pr_videlen], [kod1], [uzel1], [pr1], [kod2], [uzel2], [pr2]  from [%s] ", table);

    ret = m_cxema->m_dbf->openTable(q);

    if (!ret) {
    q.Format("Select [pr_videlen], [kod], [uzel], [pr]  from [%s] ", table);
    ret = m_cxema->m_dbf->openTable(q);
    is_line = 0;
    }

    if (!ret) {
    q.Format("Select [pr_videlen], [kod], [uzel]  from [%s] ", table);
    ret = m_cxema->m_dbf->openTable(q);
    is_line = 0;
    }

    bool isVyd = false;

    if (ret) {
    if (!m_cxema->m_dbf->rs->IsEOF()) {
    m_cxema->m_dbf->rs->MoveFirst();

    while ( !m_cxema->m_dbf->rs->IsEOF() ) {
    long b = 0;

    if (is_line) {
    m_cxema->m_dbf->readLine(1, kod1, uzel1, pr1);
    m_cxema->m_dbf->readLine(2, kod2, uzel2, pr2);

    n1 = m_cxema->m_graph->find2(CNode(CNodeName(kod1, uzel1, pr1[0])));
    n2 = m_cxema->m_graph->find2(CNode(CNodeName(kod2, uzel2, pr2[0])));

    if (n1 && n2) {
    CLINE2 *l = m_cxema->m_graph->Link(n1, n2);
    if ( l && bline(l)->isPjezo) b = true;
    }
    }
    else {
    m_cxema->m_dbf->readLine(0, kod, uzel, pr);
    n = m_cxema->m_graph->find(CNodeName(kod, uzel, pr[0]));
    if (!n) n = m_cxema->m_graph->find(CNodeName(kod, uzel, ' '));
    b = n && isMarked(n);
    }

    m_cxema->m_dbf->Edit();
    COleVariant val(b, VT_BOOL);

    if (b) {
    isVyd = true;
    }

    m_cxema->m_dbf->rs->SetFieldValue(0, &val);
    m_cxema->m_dbf->Update();

    m_cxema->m_dbf->rs->MoveNext();
    }
    }
    m_cxema->m_dbf->closeTable();
    }

    return isVyd;
    */

    return 0;
}

/*
CGidDoc *getGitDoc(CDAOMultiDocTemplate *pDAODocTemplate, const char *table)
{
CDocument *pDoc;
POSITION pos = pDAODocTemplate->GetFirstDocPosition();

while (pos != NULL)
{
pDoc = pDAODocTemplate->GetNextDoc(pos);
if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CGidDoc))) {
if (pDoc->GetTitle().CompareNoCase(table)) {
return (CGidDoc *) pDoc;
}
}
}

return NULL;
}


void closeTable(const char *table)
{
Cgid6App* pApp = (Cgid6App*) AfxGetApp();
CGidDoc * pDocument = getGitDoc(pApp->pDAODocTemplate, table);

if (pDocument) {
POSITION pos = pDocument->GetFirstViewPosition();
while (pos != NULL)
{
CGidView *pView = (CGidView*) pDocument->GetNextView(pos);
if (pView && pView->IsKindOf(RUNTIME_CLASS(CGidView))) {
pView->GetParent()->GetParent()->DestroyWindow();
break;
}
}
}
}

*/


bool CGidrView::move_to(const CFPoint& point)
{
    CNode2* node;
    CString q;

    node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);
    if (node) {
        int i = node->node.typ;
        if (i >= 0) {
            viewTable2(m_cxema->m_ado->m_schema, getNodeCode(i), getNodeTableMySQL(i), getNodeTable(i), virt_data_node);
        }
    }
    //    AfxMessageBox(_TR("Просмотр таблицы"), MB_OK|MB_ICONINFORMATION);
    return false;
}

//-----------------------------------------------
// Добавить неотрисованные узлы
//-----------------------------------------------



/*
CString getNamePT(CDbFile *dbf, CNode *n, CDbFile *dbPR, CDbFile *dbPO)
{
CString s = "";

if (n->typ == TIP_PR || n->typ == TIP_PO) {
CDbFile *dbf = (n->typ == TIP_PR) ? dbPR : dbPO;

if (n->nomP > 0) if (dbf->GotoKey(n->nomP)) {
s = dbf->read("name_building");
}
}

return s;
}
*/


/*
bool CGidrView::neotr_node(CNode2* node, CFPoint point)
{
    if (!isEditGid()) return false;


    if (node) {
        CMMenu menu(this, IDD_MENU2, "Список конвертированных узлов");

        for (CLINE2 *l = node->lines; l; l = next(l)) {
            CNode2 *n = other(l);
            if (n->node.coord.x == 0 && n->node.coord.y == 0) {
                CString s;
                s.Format("%s %s", n->getName(), bline(l)->getTable());

                menu.Add(n->getName(), n);
            }
        }
        if (menu.lst.GetSize() == 0) {
            AfxMessageBox("Все узлы отрисованы", MB_OK|MB_ICONINFORMATION);
            return true;
        }
        if (menu.DoModal() == IDOK) {
            CNode2* n = (CNode2*)menu.getV();
            if (n) {
                m_n1 = node;
                m_n2 = n;

                m_n1_neotr = node;
                m_n2_neotr = n;

                m_nRegim = ID_LINE;
                regim = R_BEGIN_DRAW_LINE;
                m_pt1 = m_pt2 = CFPoint(m_n1->node.coord.x, m_n1->node.coord.y);
                m_NP.push_back(m_pt1);

                return true;
            }
        }
        return false;
    }


    CNodeListData* nd = new CNodeListData(m_cxema->m_graph, false);

    CVirtualDlg dlg(this, nd, -1, _TR("Список конвертированных узлов"), true);
    if (dlg.DoModal() == IDOK) {
        int id = dlg.m_id;
        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            if (n->node.coord.x == 0 && n->node.coord.y == 0) {
                setXY(n, point);
                Invalidate();
            }
        }
    }
    return false;
}
*/


#include "Ultimate Grid\TableDialog.h"


int s_to_int(const char* s)
{
    CString ss = "";

    for (int i = 0; s[i]; i++) {
        if ('0' <= s[i] && s[i] <= '9') {
            ss += s[i];
        }
    }
    return atoi(ss);
}


bool isNeotr(const CNode2* node);


bool CGidrView::neotr_node(CNode2* node, CFPoint point)
{

    if (!isEditGid()) return false;

    CString q;
    CString tn = "Список неотрисованных узлов";
    CAdoFile* ado = m_cxema->m_ado;


    int n_ret = 0;


    if (node) {
        if (!isNeotr(node)) {
            AfxMessageBox("Все узлы отрисованы", MB_OK | MB_ICONINFORMATION);
            return false;
        }

        CString tn = "Список неотрисованных узлов связанных с выбранным";

        q.Format(
            "SELECT DISTINCT \n"
            "n.id,"
            "ec.name AS 'Код РС',"
            "n.externalNodeName AS 'Имя узла',\n"
            "rc.name AS 'Описание',\n"
            "hps.pipeSectLength AS 'Длина',"
            "fr.name AS 'Фрагмент'\n"


            "FROM nodes n\n"
            "LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
            "JOIN fragments fr ON fr.id=n.fileID\n"
            "LEFT JOIN realConsumers rc ON rc.nodeID =n.id\n"
            "JOIN linesobj l ON(l.nodeID1 = n.id OR l.nodeID2 = n.id) AND(l.nodeID1 = %d OR l.nodeID2 = %d)\n"
            "LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
            "WHERE n.fileID in (%s) AND (n.x=0 OR n.x IS NULL) AND (n.y=0 OR n.y IS NULL) AND n.internalNodeID IS NULL\n", node->id, node->id, m_cxema->m_par

        );
        CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_unknown);
        if (dlg.DoModal() == IDOK) {
            int id = s_to_int(dlg.m_strRet);
            CNode2* n = m_cxema->m_graph->find(id);
            if (n) {
                m_n1 = node;
                m_n2 = n;

                m_n1_neotr = node;
                m_n2_neotr = n;

                m_nRegim = ID_LINE;
                regim = R_BEGIN_DRAW_LINE;
                m_pt1 = m_pt2 = CFPoint(m_n1->node.coord.x, m_n1->node.coord.y);
                m_NP.push_back(m_pt1);

                return true;
            }
        }
        return true;
    }



    q.Format(
        "SELECT \n"
        "n.id,"
        "ec.name AS 'Код РС',"
        "n.externalNodeName AS 'Имя узла',\n"
        "rc.name AS 'Описание',\n"
        "fr.name AS 'Фрагмент'\n"


        "FROM nodes n\n"
        "LEFT JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
        "JOIN fragments fr ON fr.id=n.fileID\n"
        "LEFT JOIN realConsumers rc ON rc.nodeID =n.id\n"
        "WHERE n.fileID in (%s) AND n.x=0 AND n.y=0 AND n.internalNodeID IS NULL\n", m_cxema->m_par

    );

    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_unknown);
    if (dlg.DoModal() == IDOK) {
        int id = s_to_int(dlg.m_strRet);
        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            setXY(n, point);
            Invalidate();
        }
    }


    return false;
}



bool CGidrView::findUP(const CFPoint& pt)
{
    CNode2* node;
    CLINE2* line;

    node = m_cxema->m_graph->find(m_parent_id, pt, geom.masx * D5 / mas_otn);
    line = m_cxema->m_graph->findLine(m_parent_id, pt, geom.masx * D5 / mas_otn);

    if (regim == R_INIT) {
        switch (m_nRegim) {
        case ID_MOVE_TO:  move_to(pt);  break;
        }
    }
    return true;
}

void CGidrView::redrawOnlyGid()
{
    m_bOnlyGid = TRUE;
    InvalidateG(FALSE);
    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
    m_bOnlyGid = FALSE;
}


void CGidrView::sw_p(CLINE2* line)
{
    CLine2* l = bline(line);

    l->line.isPjezo = !l->line.isPjezo;

    if (l->line.isPjezo) {
        where(line)->node.isPjezo = true;
        other(line)->node.isPjezo = true;
    }
    else {
        if (incP(where(line)) == 1) where(line)->node.isPjezo = false;
        if (incP(other(line)) == 1) other(line)->node.isPjezo = false;
    }
    m_mark_changed = true;
}


bool CGidrView::sfind(const CFPoint& pt)
{
    CNode2* node;
    CLINE2* line;

    findNodeLine(pt, &node, &line, D5);

    if (line) {
        CLine2* l = bline(line);
        /*
        if (m_isPjezo2) {
        if (l->isPjezo) {
        l->isPjezo2 = !l->isPjezo2;
        }
        }
        else {
        */
        sw_p(line);
        //    }
        InvalidateG();
    }

    return true;
}

void getProject(CCoordList& cl, CFPoint point, CCoordList& m_NP, int loc)
{
    CFPoint p1 = cl.first();
    CFPoint p2 = cl.last();
    double m5 = 500;
    double h = hypot(p1.x - p2.x, p1.y - p2.y);
    double mas_otn = 1;

    point = cl.GetProjection(point, p1, p2);
    m_NP.push_back(point);

    //  if (!isPoint(loc)) {
    if (loc == LOC6) {
        point = CFPoint(point.x + (p2.x - p1.x) * m5 / h / mas_otn, point.y + (p2.y - p1.y) * m5 / mas_otn / h);
        m_NP.push_back(point);
    }
}


void getProject(CLINE2* line, CFPoint point, CCoordList& m_NP, int loc)
{
    CFPoint p1 = where(line)->node.coord;
    CFPoint p2 = other(line)->node.coord;
    double mas_otn = 1;


    CLine2* l = bline(line);
    CCoordList cl;
    cl.push_back(p1);
    CCoordList::const_iterator it = l->line.m_NP.begin();
    for (; it != l->line.m_NP.end(); ++it) {
        cl.push_back(*it);
    }
    cl.push_back(p2);

    point = cl.GetProjection(point, p1, p2);
    double m5 = 100;
    double h = hypot(p1.x - p2.x, p1.y - p2.y);

    m_NP.clear();

    m_NP.push_back(point);

    if (loc == LOC6) {
        point = CFPoint(point.x + (p2.x - p1.x) * m5 / h / mas_otn, point.y + (p2.y - p1.y) * m5 / mas_otn / h);
        m_NP.push_back(point);
    }
}

int isElPoint(const char* tn1);
int isElEnd(const char* tn1);


bool CGidrView::updateElEnd(Klassif* m_kls, CFPoint point, int loc)
{
    CString sl = EL_LINE;
    Klassif* kls = m_geofile->m_kl_list->findKlN(sl);

    if (!kls) return false;

    CString q;

    CString fname = "naimenovanie_istochnika";

    if (m_kls->nazv == EL_PRIEM) fname = "naimenovanie_priemnika";

    q.Format(

        "SELECT \n"
        "l.id\n"
        "FROM liniya_elektroperedach l\n"
        "JOIN [%s] obj ON (obj.shape.STDistance(l.shape.STStartPoint()) < 1 OR obj.shape.STDistance(l.shape.STPointN (l.shape.STNumPoints())) < 1) OR obj.id=l.%s\n"
        "WHERE obj.id=%d\n", m_kls->nazv, fname, m_num);

    set<int> set_l;

    CAdoFile* ado = kls->m_ado;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            set_l.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    CFPoint p_old = m_NP.first();

    for (auto it : set_l) {

        int n = it;

        CCoordList cl;
        int id = kls->GetNom2(n);
        if (id >= 1) {
            m_geofile->GetCoord(kls, id, cl);

            CFPoint p1 = cl.first();
            CFPoint p2 = cl.last();

            if (Length(p1, p_old) < 1) {
                auto it = cl.begin();
                if (it != cl.end()) {
                    *it = point;
                }
            }
            if (Length(p2, p_old) < 1) {
                auto it = cl.rbegin();
                if (it != cl.rend()) {
                    *it = point;
                }
            }
            updateObj(sl, n, cl);
            Invalidate();
        }


    }

    return true;
}




bool CGidrView::updateEl(Klassif* m_kls, CFPoint point, int loc)
{
    CString sl = EL_LINE;

    if (isElEnd(m_kls->nazv)) {
        updateElEnd(m_kls, point, loc);
    }
    else if (isElPoint(m_kls->nazv)) {
        Klassif* kls = m_geofile->m_kl_list->findKlN(sl);

        if (kls) {
            int n = m_geofile->findXY3(point, D5 * geom.masx, 1, sl);

            if (n) {
                int id = kls->GetNom2(n);
                if (id >= 1) {
                    m_NP.clear();
                    getProject(kls->geo4[id - 1]->cl, point, m_NP, loc);

                    regim = R_INIT;
                    updateObj();
                    GetDocument()->SetModifiedFlag(true);
                    InvalidateG();
                    m_nRegim = ID_INFO;
                    return true;
                }
            }
        }
    }
    else if (is_water_node(m_kls->nazv)) {  // Узел водопровода
        char fn[1024];
        void get_water_line(set<CString>& _set_water_line);
        set<CString> set_water_line;
        map<CString, set<int> > set_out;
        get_water_line(set_water_line);

        CFPoint p_old = m_NP.first();

        bool mv = false;

        bool b = m_geofile->findXY_end_list_exact(p_old, set_water_line, set_out);
        if (b) {
            for (auto &it : set_out) {
                CString tn = it.first;
                CString q;

                Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
                if (kls) {
                    set<int> set1;
                    set<int> set2;

                    for (auto &i : it.second) {
                        CCoordList cl;
                        kls->geo4[i - 1]->cl;
                        CFPoint p1 = kls->geo4[i - 1]->cl.first();
                        CFPoint p2 = kls->geo4[i - 1]->cl.last();
                        if (Length(p1, p_old) < 1e-5) {
                            kls->geo4[i - 1]->cl.pop_front();
                            kls->geo4[i - 1]->cl.push_front(point);
                            set1.insert(kls->geo4[i - 1]->nom);
                        }
                        if (Length(p2, p_old) < 1e-5) {
                            kls->geo4[i - 1]->cl.pop_back();
                            kls->geo4[i - 1]->cl.push_back(point);
                            set2.insert(kls->geo4[i - 1]->nom);
                        }
                    }
                    CString set_to_text(set<int>&set_i);

                    if (!set1.empty()) {
                        q.Format("UPDATE %s SET shape=dbo.insert_point_to_line(shape, geometry::STPointFromText('POINT(%.12f %.12f)', 0), 1) WHERE id IN (%s)", tn, point.x / 100, -point.y / 100, set_to_text(set1));
                        kls->m_ado->Execute(q);
                        mv = true;
                    }

                    if (!set2.empty()) {
                        q.Format("UPDATE %s SET shape=dbo.insert_point_to_line(shape, geometry::STPointFromText('POINT(%.12f %.12f)', 0), -1) WHERE id IN (%s)", tn, point.x / 100, -point.y / 100, set_to_text(set2));
                        kls->m_ado->Execute(q);
                        mv = true;
                    }
                }
            }

            InvalidateG();

            m_nRegim = ID_INFO;
            return false;
        }
    }


    regim = R_INIT;
    m_nRegim = ID_INFO;

    return false;
}

// Создать внутреннюю схему

long addLineZD(CAdoFile* ado, int typ, CNode2* n1, CNode2* n2, int externalSignLineID, CCoordList& cl, long& idv, double diam);


bool CGidrView::initUS(const CNode2* parent)
{
    double R = 2000, X0 = 40000, Y0 = -40000;

    CLINE2* l;
    CNode2* o;
    int n_n = 0;

    CAdoFile* ado = m_cxema->m_ado;
    CGraph2* m_graph = m_cxema->m_graph;

    for (l = parent->lines; l; l = next(l)) {
        //        n_n += other(l)->node_name.p == ' ' ? 2 : 1;
        n_n += 1;
    }

    double fi = 0, dR = n_n <= 2 ? 0 : R / 8;

    CNode2* nP = NULL, * nO = NULL;

    //    CLINE2 *line;
    if (parent->node.externalSignID == 1 || parent->node.externalSignID == 2 || parent->node.externalSignID == 4) {
        //        AfxMessageBox("np!", MB_OK|MB_ICONINFORMATION);
        nP = addUS1(parent->node.fileID, parent->id, 2, CFPoint(X0 + dR, Y0 - R / 4));
    }
    if (parent->node.externalSignID == 1 || parent->node.externalSignID == 3 || parent->node.externalSignID == 5) {
        //        AfxMessageBox("no!", MB_OK|MB_ICONINFORMATION);
        nO = addUS1(parent->node.fileID, parent->id, 3, CFPoint(X0, Y0 + R / 4));
        //        nO = addNode(TIP_US, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, COBR), CFPoint(X0, Y0+R/4));
    }


    for (l = parent->lines; l; l = next(l)) {
        CNode2* n1P = NULL, * n1O = NULL;
        o = other(l);
        double cosfi = cos(fi);
        double sinfi = sin(fi);
        sinfi = (o->node.coord.x - parent->node.coord.x) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);
        cosfi = (o->node.coord.y - parent->node.coord.y) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);

        double diamP = bline(l)->line.pod.diam_usl;
        double diamO = bline(l)->line.obr.diam_usl;


        CFPoint ptP = CFPoint(X0 + R * sinfi + dR, Y0 + R * cosfi - R / 4);
        CFPoint ptO = CFPoint(X0 + R * sinfi, Y0 + R * cosfi + R / 4);



        //        if (bline(l)->line.nomP > 1 && bline(l)->line.nomO > 1) {
        if (bline(l)->line.nomP >= 1) {
            n1P = addUS2(o->node.fileID, parent->id, o->id, 2, ptP);
        }
        if (bline(l)->line.nomO >= 1) {
            n1O = addUS2(o->node.fileID, parent->id, o->id, 3, ptO);
        }
        /*
                }
                else {
                    CNode2 *nn =  addUS2(o->node.fileID, parent->id, o->id, bline(l)->line.nomP > 1 ? 2 : 3, CFPoint(X0 + R * sinfi, Y0 + R * cosfi));

                    if (bline(l)->line.nomP > 1) n1P = nn;
                    else n1O = nn;
                }
        */

        CCoordList cl;
        long idv;

        if (n1P && nP) {
            long id = addLineZD(ado, TIP_ZD, n1P, nP, 2, cl, idv, diamP);

            CLINE2* line = m_graph->insert_line(n1P, nP);
            if (line) {
                bline(line)->line.nomP = id;
                bline(line)->line.idP2 = idv;
                bline(line)->line.typ = TIP_ZD;
            }
        }
        if (n1O && nO) {
            long id = addLineZD(ado, TIP_ZD, n1O, nO, 3, cl, idv, diamO);

            CLINE2* line = m_graph->insert_line(n1O, nO);
            if (line) {
                bline(line)->line.nomO = id;
                bline(line)->line.idO2 = idv;
                bline(line)->line.typ = TIP_ZD;
            }
        }

        fi += M_PI * 2 / n_n;
    }

#if 0

    for (l = parent->lines; l; l = next(l)) {
        CLine2* ll = bline(l);

        double diamP = ll->line.pod.diam;
        double diamO = ll->line.obr.diam;

        o = other(l);
        if (ll->line.nomP != -1 && ll->line.nomO != -1) {
            CNode2* n1;
            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, CPOD));

            if (n1 && (line = m_graph->insert_line(nP, n1)) != NULL) {
                addLine(line, TIP_ZD, diamP);
            }

            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, COBR));

            if (n1 && (line = m_graph->insert_line(nO, n1)) != NULL) {
                addLine(line, TIP_ZD, diamO);
            }
        }
        else {
            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, ll->line.nomP != -1 ? CPOD : COBR));
            if (n1
                && (ll->line.nomP != -1 && (line = m_graph->insert_line(nP, n1)) != NULL)
                || (ll->line.nomO != -1 && (line = m_graph->insert_line(nO, n1)) != NULL)
                ) {
                addLine(line, TIP_ZD, ll->line.nomP != -1 ? diamP : diamO);
            }
        }
    }
#endif

    return true;
}


bool CGidrView::initCxema(CNode2* parent)
{
    // if (m_graph->set_node.size() > 0) return false;

    double R = 2000, X0 = 40000, Y0 = -40000;

    if (parent) {
        if (parent->node.typ == TIP_PR) {
            create_b5(m_cxema->m_ado, parent, m_cxema);
        }
        else if (parent->node.typ == TIP_PO) {
            AfxMessageBox("Нельзя создавать внутреннюю схему в обобщенном потребителе", MB_OK | MB_ICONINFORMATION);
            return false;
        }
        else if (parent->node.nZN != -1) {
            AfxMessageBox("Нельзя создавать внутреннюю схему в узле с заданным напором", MB_OK | MB_ICONINFORMATION);
            return false;
        }
        else if (parent->node.typ == TIP_C3) {
            //      initC3(gid_file, parent);
        }
        else if (parent->node.typ == TIP_HS || parent->node.typ == TIP_US || parent->node.typ == TIP_C3 || parent->node.typ == TIP_IS) {
            initUS(parent);
        }
        /**
                else if (parent->node.typ == TIP_IS) {
        //            initUS(parent);
        //            addNode(TIP_US2, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, CPOD), CFPoint(X0, Y0));
        //            addNode(TIP_US2, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, COBR), CFPoint(X0, Y0+R));

                      CNode2 *nP = addUS2(parent->node.fileID, parent->id, parent->id, 2, CFPoint(X0, Y0));
                      CNode2* nO = addUS2(parent->node.fileID, parent->id, parent->id, 3, CFPoint(X0, Y0 + R));
                }
        */
        else {
            //     addNode(TIP_US2, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, CPOD), CFPoint(X0, Y0));
            //     addNode(TIP_US2, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, COBR), CFPoint(X0, Y0+R));
        }
    }
    return true;
}


void CGidrView::OnIst(UINT nID)
{
    char tabist[][80] = {
        "GO_Ожидаемый  месячный график работы источника",
        "T1_Фактический часовой режим работы источника",
        "T2_Фактический суточный режим работы источника",
        "T3_Фактический месячный режим работы источника",
        "T4_Начало конец отопительного сезона источника",
        "TG_Развернутый температурный график",
        "TK_График температур качественного регулирования",
        "TN_График нормативных температур",
        "V1_Фактические часовые температуры воздуха и грунта",
        "V2_Фактические суточные температуры воздуха и грунта",
        "V3_Фактические месячные температуры воздуха и грунта",
        "VM_Многолетние температуры воздуха и грунта",
    };

    //    RunTable(argpath() + _TR("ГрафИсточ.mdb"), tabist[nID - ID_IST_1], NULL, "Help_ГрафИсточ.mdb", -1, 1, FALSE, FALSE, FALSE, FALSE);
}

/*
bool vyd_kapremont(int id)
{
bool isCh = false;
Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KAPITAL));
if (kls && id > 0) {
allGeoVisible(STR_REMONT_KAPITAL, false);
int nnn = kls->geo4.size();
for (int i = 0; i < nnn; i++) {
int nn = kls->geo4[i]->id_remont;
if (nn == id) {
if (!kls->geo4[i]->is_visible) {
isCh = true;
kls->geo4[i]->is_visible = true;
}
}
else {
if (kls->geo4[i]->is_visible) {
isCh = true;
kls->geo4[i]->is_visible = false;
}
}
}
}
return isCh;
}
*/

bool vyd_kapremont(const char* tn, int id);


void CGidrView::OnDelMark()
{
    if (m_cxema->m_graph->isMark()) {
        if (AfxMessageBox(_TR("Удалить выделенные объекты"), MB_YESNO) == IDYES) {
            if (AfxMessageBox(_TR("Внимание! Удаляются все выделенные объекты. Вы уверены?"), MB_YESNO) == IDYES) {
                delMark();
                InvalidateG();
            }
        }
        return;
    }
}


void CGidrView::OnRegim(UINT nID)
{
    if (nID == ID_DEL) {
/*
        if (m_cxema->m_graph->isMark()) {
            if (AfxMessageBox(_TR("Удалить выделенные объекты"), MB_YESNO) == IDYES) {
                if (AfxMessageBox(_TR("Внимание! Удаляются все выделенные объекты. Вы уверены?"), MB_YESNO) == IDYES) {
                    delMark();
                    InvalidateG();
                }
            }
            return;
        }
*/
    }

    if (nID == ID_NAPR_2) {
    }


    if (nID == ID_KAPREMONT_INFO) {
#if NOMYSQL
        if (m_geofile && m_geofile->m_old_remont > 0) {
            //      allGeoVisible(STR_REMONT_KAPITAL, true);
            allGeoVisible(m_geofile->m_old_TN, true);
            m_geofile->m_old_remont = 0;
            refreshRemont();
            Invalidate();
            return;
        }
#endif
    }

    if (nID == ID_REMONT_KAPITAL) {
        //    allGeoVisible(STR_REMONT_KAPITAL, false);
        m_geofile->m_old_remont = 0;
        Invalidate();
    }
    if (nID == ID_REMONT_KAPITAL_DOB) {
        if (vyd_kapremont(m_geofile->m_old_TN, m_geofile->m_old_remont)) {
            Invalidate();
        }
    }
    /*
    if (nID == ID_REMONT_OPRESSOVKA_DOB) {
    if (vyd_kapremont(STR_REMONT_OPRESSOVKA, m_geofile->m_old_remont)) {
    Invalidate();
    }
    }
    */

    if (nID == ID_NAPR || nID == ID_NAPR_OPRES || nID == ID_NAPR_2 || nID == ID_NAPR_REMONT2) {
        m_napr1 = NULL;
    }

    m_nRegim = nID;
    m_locNP = 2;

    if (nID == ID_MOVEW) {
        OnMoveW();
        regim = R_BEGIN_MOVE_VIEW;
        SetCur(m_nRegim);
        return;
    }
    else if ((nID == ID_KAM || nID == ID_KAM2) && !IsMain()) {
        m_parent_id = 0;

        geom.masx = old_masx;
        geom.masy = old_masy;
        geom.m_bx = old_m_bx;
        geom.m_by = old_m_by;

        EndDrawNode();

//        CFRect rect = m_cxema->m_graph->getBorder(0);
        SetBorder(old_rect);

        Invalidate();
    }

    if (nID == ID_MARK || nID == ID_MARK_DEL || nID == ID_MARK_PS || nID == ID_MARK_GEO) {
        regim = R_BEGIN_LEN_OKR1;
        m_mark_regim = nID;
        SetCur(m_nRegim);
    }
    else if (nID == ID_FRAGMENT_POLY) {
        regim = R_BEGIN_LEN_OKR_VYD_1;
        m_locNP = 3;
        SetCur(m_nRegim);
    }
    else if (nID == ID_FRAGMENT_POLY_GEO || nID == ID_FRAGMENT_POLY_GEO || nID == ID_FRAGMENT_POLY_KORROZIA_AR || nID == ID_FRAGMENT_POLY_KORROZIA_PLAN) { // Начинаем выделять индикаторы коррозии
        m_regim_geo = nID;
        m_regim_geo_tn = REM_INDICATOR;

        regim = R_BEGIN_LEN_OKR_VYD_GEO_1;
        m_locNP = 3;
        SetCur(m_nRegim);
    }
    else if (nID == ID_MOVE_VYD) {
        regim = R_BEGIN_MOVE_FRAGMENT_1;
        m_locNP = 3;
        SetCur(m_nRegim);
    }
    else if (nID == ID_POVOROT) {
        if (0 && geom.m_alpha != 0) {
            /*
            regim = R_INIT;
            m_nRegim = ID_INFO;
            m_alpha = 0;
            set_coord_gid(0, 0, 0);
            InvalidateG();
            */
        }
        else {
            regim = R_BEGIN_POVOROT_1;
            SetCur(m_nRegim);
        }
    }
    else {
        regim = R_INIT;
        SetCur(m_nRegim);
    }
}

//-----------------------------------------------

void CGidrView::OnUpdateRegim(CCmdUI* pCmdUI)
{
    //  if (pCmdUI->m_nID == ID_POVOROT) {
    //      pCmdUI->Enable(m_alpha == 0);
    //      return;
    //  }

    if (isNoDelete()) {
        if (    
                pCmdUI->m_nID == ID_NODE 
            ||
                pCmdUI->m_nID == ID_LINE 
            ||
                pCmdUI->m_nID == ID_RENAME 
            ||
                pCmdUI->m_nID == ID_DEL
            ||
                pCmdUI->m_nID == ID_NODE_MOVE1
            ) {
            pCmdUI->Enable(FALSE);
            return;
        }
    }

    if (pCmdUI->m_nID == ID_MS_NODE_1 || pCmdUI->m_nID == ID_MS_NODE_2) {
        pCmdUI->Enable(m_ms_kod > 0 || m_rs_kod > 0);
        if (pCmdUI->m_nID == ID_MS_NODE_1) {
            pCmdUI->SetCheck(m_nRegim == ID_MS_NODE_1);
        }
        if (pCmdUI->m_nID == ID_MS_NODE_2) {
            pCmdUI->SetCheck(m_nRegim == ID_MS_NODE_2);
        }
        return;
    }

    if (pCmdUI->m_nID == ID_OPR_NODE_1 || pCmdUI->m_nID == ID_OPR_NODE_2) {
        //      pCmdUI->Enable(m_opressovka_uchastok > 0);
        pCmdUI->Enable(m_cxema->m_graph->isMark());

        if (pCmdUI->m_nID == ID_OPR_NODE_1) {
            pCmdUI->SetCheck(m_nRegim == ID_OPR_NODE_1);
        }
        if (pCmdUI->m_nID == ID_OPR_NODE_2) {
            pCmdUI->SetCheck(m_nRegim == ID_OPR_NODE_2);
        }
        return;
    }


    if (pCmdUI->m_nID == ID_REMONT_KAPITAL_DOB) {
        pCmdUI->Enable(m_geofile && m_geofile->m_old_remont > 0);
        return;
    }

//    if (pCmdUI->m_nID == ID_GOOGLE_ELEVATION || pCmdUI->m_nID == ID_OPENSTREETMAP_INFO || pCmdUI->m_nID == ID_MAP_CITY || pCmdUI->m_nID == ID_ADD_CITY) {
    if (pCmdUI->m_nID == ID_OPENSTREETMAP_INFO || pCmdUI->m_nID == ID_MAP_CITY || pCmdUI->m_nID == ID_ADD_CITY) {
        pCmdUI->Enable(m_map != ID_NOMAP && m_map != ID_YANDEX_MAP && m_map != ID_YANDEX_PMAP && m_map != ID_YANDEX_SAT);
        return;
    }

    if (pCmdUI->m_nID == ID_MAG_UZEL) {
        pCmdUI->Enable(m_MagistralID1 >= 0 || m_MagistralID2 >= 0);
    }

    if (m_nRegim == ID_KAM && !IsMain()) {
        pCmdUI->SetCheck(FALSE);
    }
    else
        pCmdUI->SetCheck(m_nRegim == pCmdUI->m_nID);


    if (isEditGid()) {
        switch (pCmdUI->m_nID) {
        case ID_NODE_NEOTR: OnUpdateNodeNeotr(pCmdUI); break;
            //      case ID_TEXT : pCmdUI->Enable(m_parent_kam != NULL); break;
        case ID_TEXT: pCmdUI->Enable(TRUE); break;

        case ID_GEO_INFO:
        case ID_MOVE_VYD:
            pCmdUI->Enable(regim == R_FRAGM || regim == R_BEGIN_MOVE_LINE);
            break;
        default: pCmdUI->Enable(TRUE); break;
        }
    }
    else {

        switch (pCmdUI->m_nID) {
        case ID_NODE:
        case ID_LINE:
        case ID_NODE_MOVE:
        case ID_NODE_MOVE_NEXT:
        case ID_NODE_MOVE1:
        case ID_NODE_MOVE1_NEXT:
            //    case ID_INFO :
            //    case ID_KAM :
            //    case ID_MOVE_TO :
        case ID_SWAP:
        case ID_RENAME:
        case ID_MOVE_LINE:
        case ID_ISM:
        case ID_OPEN_CLOSE:
        case ID_ZN:
            //    case ID_MOVEW :
            //    case ID_NAPR :
        case ID_NEW_GEO:
        case ID_ISM_HOUSE:
        case ID_ISM_GEO:
        case ID_ISM_GEO_PODP:
            //    case ID_INFO_GID :
            //    case ID_GEO_INFO :
            //    case ID_KAM2 :
            //    case ID_ZOOM :
        case ID_DEL:
        case ID_TEXT:
            //    case ID_VYD :
            pCmdUI->Enable(FALSE);
            break;
        case ID_NODE_NEOTR:
            OnUpdateNodeNeotr(pCmdUI);
            break;
        default:
            pCmdUI->Enable(TRUE);
            break;
        }
    }
}

//-----------------------------------------------
//
//-----------------------------------------------


void setPages(int n);
bool openAdo(const char* server, int port, const char* database, const char* user, const char* password);


#if 0

void CGidrView::openGidODBC()
{
    if (IsMain()) {
        m_mmasx = geom.masx, m_mmasy = geom.masy, m_mbx = geom.m_bx, m_mby = geom.m_by;
    }

    CString str = "";

    m_parent_id = 0;

    m_CurNode = NULL;
    m_CurLine = NULL;

    Cgid6Doc* pDoc = GetDocument();

    setPages(1);

    m_cxema->initGidTree();

    CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
    SetWindowText2(GetDocument()->GetTitle() + str);


    if (rect.left == LONG_MAX) {
        SetBorder(CFRect(0. + 40000., 0. - 40000., 400. + 40000., 400. - 40000.));
        geom.m_bx = geom.m_by = 32000;
        geom.masx = geom.masy = 1;
        MasAll();
    }
    else {
        //    rect.InflateRect(100, 100);
        rect.InflateRect(100, 100);
        SetBorder(rect);
        MasAll();
    }
}

#endif


extern int m_isMySQL;


void CGidrView::openBmp(const char* fn)
{
    qqq_list->clear();

    if (m_tmpPicN[0]) DeleteFile(m_tmpPicN);

    GetTempFileName(getenv("TMP"), "tmp", 0, m_tmpPicN);

    CString ext = GetExt(fn);

    if (m_dxf) {
        delete m_dxf;
        m_dxf = NULL;
    }
    if (!ext.CompareNoCase(_T("bmp"))) bmp2bbb(fn, m_tmpPicN);
    else if (!ext.CompareNoCase(_T("gif"))) gif2bbb(fn, m_tmpPicN);
    else if (!ext.CompareNoCase(_T("jpg")) || !ext.CompareNoCase(_T("jpeg"))) jpg2bbb(fn, m_tmpPicN);
    else if (!ext.CompareNoCase(_T("tif")) || !ext.CompareNoCase(_T("tiff"))) tif2bbb(fn, m_tmpPicN);
    else if (!ext.CompareNoCase(_T("dxf"))) {
        FILE* f;
        void dxf_in(FILE * f, cdxf * dxf);
        void set_dxf_dx_dy(double dx, double dy);

        CString title = GetTitle(fn);

        if (title == "ТС Город Семей 2020") {
            set_dxf_dx_dy(35221.33442059075, 5126.937927326165);
        }

        if ((f = fopen(fn, "r")) != NULL) {

            m_dxf = new cdxf;
            dxf_in(f, m_dxf);
            fclose(f);

            if (0) {
                double xmin, xmax, ymin, ymax;
                dxf_min_max(m_dxf, xmin, xmax, ymin, ymax);
                CFRect rect_pic(xmin, ymin, xmax, ymax);
                //        rect_pic.UnionRect(&rect_pic, &m_rectBorder);
                //        SetBorder(rect_pic);
            }
        }
        m_bIsOpenPicture = TRUE;
        return;
    }

    //    strcpy(m_tmpPicN, "D:\\TGID_DB\\ТГИД\\primer_map\\1\\tmp127C.tmp");

    qqq_list->openFile(m_tmpPicN);

    m_bIsOpenPicture = TRUE;
}

//-----------------------------------------------

HWND m_hWndMain = 0;

void CGidrView::OnInitialUpdate()
{
    log1("<< CGidrView::OnInitialUpdate");

    m_hWndMain = m_hWnd;

    CString strBuffer;

    CScroll::OnInitialUpdate();

    Cgid6Doc* pDoc = GetDocument();

    pDoc->m_ask_close = true;

    CClientDC screenDC(NULL);
    geom.dmas = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

    //  openGidODBC();

    m_mmasx = geom.masx, m_mmasy = geom.masy, m_mbx = geom.m_bx, m_mby = geom.m_by;

    m_parent_id = 0;

    m_CurNode = NULL;
    m_CurLine = NULL;

    ps_alma.init();

    setPages(1);
    m_cxema = pDoc->m_cxema;

    m_cxema->initGidTree();

    CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
    //    SetWindowText2(GetDocument()->GetTitle());

    if (rect.left == LONG_MAX) {
        SetBorder(CFRect(0. + 40000., 0. - 40000., 400. + 40000., 400. - 40000.));
        geom.m_bx = geom.m_by = 32000;
        geom.masx = geom.masy = 1;
        MasAll();
    }
    else {
        //    rect.InflateRect(100, 100);
        rect.InflateRect(100, 100);
        SetBorder(rect);
        MasAll();
    }

    if (demoversion && m_cxema->m_graph->map_node.size() > N65) {
        AfxMessageBox(_TR("Это демонстрационная версия"), MB_OK | MB_ICONINFORMATION);
        //    GetParent()->
        //      DestroyWindow();
        //    m_cxema->close();
        m_cxema->m_graph->clear();
        return;
    }

    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_APPDATA, 1);

    m_isMapYes = AfxGetApp()->GetProfileInt(szSection, "isMapYes", 0);
    m_pathMap = AfxGetApp()->GetProfileString(szSection, "path_map", path1);

    if (m_isMapYes) {
        m_map = AfxGetApp()->GetProfileInt(szSection, "GoogleMap", m_map);
        is_google_map = GOOGLE_ALWAYS || (m_map != ID_NOMAP);
        if (m_cxema) m_cxema->is_google_map = is_google_map;
    }


    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)pApp->m_pMainWnd;

    m_isGeobaza = TRUE;


    m_isGeobaza = AfxGetApp()->GetProfileInt(szSection, _T("IsGeobaza"), m_isGeobaza);
    m_bIsPicture = AfxGetApp()->GetProfileInt(szSection, _T("IsPicture"), m_bIsPicture);


    //    int fileID = AfxGetApp()->GetProfileInt(szSection, _T("fileID"), 0);
    //    setActiv(fileID);

    //    m_cxema->m_cur_file = cur_gid;
    //    m_cxema->m_cur_file = NULL;



    try {
        CString pn = pDoc->m_bd_gid;
        if (pn == "") pn = "MySQL";

        if (pn != "") {
            if (m_parent_id == 0) {
                strBuffer.Format("%lg", geom.masx * geom.dmas);
                strBuffer = AfxGetApp()->GetProfileString(pn, "mas", strBuffer);
                sscanf(strBuffer, "%lg", &geom.masx);

                if (geom.masx <= 0) geom.masx = 500;

                geom.masx = geom.masy = geom.masx / geom.dmas;
                strBuffer.Format("%lg", geom.m_bx);
                strBuffer = AfxGetApp()->GetProfileString(pn, "m_bx", strBuffer);
                sscanf(strBuffer, "%lg", &geom.m_bx);

                strBuffer.Format("%lg", geom.m_by);
                strBuffer = AfxGetApp()->GetProfileString(pn, "m_by", strBuffer);
                sscanf(strBuffer, "%lg", &geom.m_by);
                m_strPicture = AfxGetApp()->GetProfileString(pn, "picture", "");

                if (m_bIsPicture && m_strPicture != "") {
                    openBmp(m_strPicture);
                }

                /*
                CString tn = AfxGetApp()->GetProfileString(pn, "hide_tn", "");
                int num = AfxGetApp()->GetProfileInt(pn, "hide_num", 0);

                if (m_geofile->InitS(tn) && m_geofile->GoToID(num)) {
                m_geofile->GetCoord(m_clExpl);
                }
                */
            }
            m_password = AfxGetApp()->GetProfileString(pn, "Password", "");
            AfxGetApp()->WriteProfileString(pn, "Password", m_password);
        }
    }
    catch (...) {
    }

    m_cxema->m_bIsGidrInf = FALSE;

    for (int i = 0; i < sizeof(m_cxema->m_bIsInfo) / sizeof(m_cxema->m_bIsInfo[0]); i++) {
        CString sss;
        sss.Format("IsInfo%d", i);
        m_cxema->m_bIsInfo[i] = AfxGetApp()->GetProfileInt(szSection, sss, 0);
    }

    m_colors.load(isPassport());

    m_bk_color = AfxGetApp()->GetProfileInt(szSection, _T("m_bk_color"), m_bk_color);

    strBuffer = AfxGetApp()->GetProfileString(szSection, _T("BMP"), _T("BMP"));
    qqq_list->openDir(strBuffer);

    m_idMenu = (TypMenu) AfxGetApp()->GetProfileInt(szSection, _T("idMenu"), (int) m_idMenu);

    m_cxema->m_bIsGidrInf = AfxGetApp()->GetProfileInt(szSection, _T("IsGidrInf"), m_cxema->m_bIsGidrInf);
    m_bIsGeoGeodez = AfxGetApp()->GetProfileInt(szSection, _T("IsGeoGeodez"), m_bIsGeoGeodez);
    m_bIsPodp = AfxGetApp()->GetProfileInt(szSection, _T("IsPodp"), m_bIsPodp);
    m_bIsPodpNiz = AfxGetApp()->GetProfileInt(szSection, _T("IsPodpNiz"), m_bIsPodpNiz);
    m_bIsPodpVerh = AfxGetApp()->GetProfileInt(szSection, _T("IsPodpVerh"), m_bIsPodpVerh);

    m_bIsFlag = AfxGetApp()->GetProfileInt(szSection, _T("isFlag"), m_bIsFlag);

    m_bIsRezhim = AfxGetApp()->GetProfileInt(szSection, _T("IsRezhim"), m_bIsRezhim);

    m_sezon_korrozia = -1;

    int m_sezon_korrozia2 = AfxGetApp()->GetProfileInt(szSection, _T("sezon_korrozia"), m_sezon_korrozia);
    if (m_sezon_korrozia2 > 0) {
        setKorroziaSezon(m_sezon_korrozia2, true);
    }

    if (m_idMenu == TYP_MENU_PTS) {
        m_bIsRezhim = TRUE;
    }

    m_bIsPTS = AfxGetApp()->GetProfileInt(szSection, _T("IsPodpPO"), m_bIsPTS);
    m_bIsPodpRes = AfxGetApp()->GetProfileInt(szSection, _T("IsPodpRes"), m_bIsPodpRes);
    m_bIsPribor = AfxGetApp()->GetProfileInt(szSection, _T("IsPribor"), m_bIsPribor);
    m_bIsMas = AfxGetApp()->GetProfileInt(szSection, _T("IsMas"), m_bIsMas);
    m_bIsNaprGid = AfxGetApp()->GetProfileInt(szSection, _T("IsNaprGid"), m_bIsNaprGid);

    m_bIsAlma = AfxGetApp()->GetProfileInt(szSection, _T("IsAlma"), m_bIsNaprGid);


    CString str = AfxGetApp()->GetProfileString(szSection, "ring", "1");
    m_ring = atof(str);

    m_color_uzel = AfxGetApp()->GetProfileInt(szSection, _T("color_uzel"), WHITE);


    str = AfxGetApp()->GetProfileString(szSection, "COORD_Z3", "0");
    double Central_Meridian = atof(str);
    //    set_Central_Meridian(Central_Meridian);

    str = AfxGetApp()->GetProfileString(szSection, "False_Easting", "0");
    double False_Easting = atof(str);
    if (False_Easting == 0) False_Easting = Central_Meridian * 10000;
    //    set_False_Easting(False_Easting);

    str = AfxGetApp()->GetProfileString(szSection, "False_Northing", "0");
    double False_Northing = atof(str);

    str = AfxGetApp()->GetProfileString(szSection, "Latitude_Of_Origin", "0");
    double Latitude_Of_Origin = atof(str);

    str = AfxGetApp()->GetProfileString(szSection, "Scale_Factor", "1");
    double Scale_Factor = atof(str);
    if (Scale_Factor <= 0) Scale_Factor = 1;

    str = AfxGetApp()->GetProfileString(szSection, "Alpha", "0");
    double Alpha = atof(str);


    //  set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Alpha);

    strBuffer.Format("%lg", m_MasPic);
    strBuffer = AfxGetApp()->GetProfileString(szSection, "MasPic", strBuffer);
    sscanf(strBuffer, "%lg", &m_MasPic);

    strBuffer.Format("%lg", m_MasPodp);
    strBuffer = AfxGetApp()->GetProfileString(szSection, "MasPodp", strBuffer);
    sscanf(strBuffer, "%lg", &m_MasPodp);

    m_nTimer = SetTimer(1, 250, 0);

    m_nTimeOut = AfxGetApp()->GetProfileInt(szSection, "timeout", 1);
    m_nTimer2 = SetTimer(2, m_nTimeOut * 1000, 0);

    //  m_nTimer3 = SetTimer(3, 5000, 0);

    setNavigator(true);

    //void thread_code(void *threadno);

    //  int thread_id;
    //  thread_id = _beginthread(thread_code,4096,(void *)this);

    //  m_pOPCThread = new COPCThread(this);


    m_pMapsThread = new CMapsThread(this);
    m_pMapsThread->CreateThread();

//    if (m_idMenu == TYP_MENU_READONLY && isEditMain()) {
//        m_idMenu = TYP_MENU_REZHIM;
//    }
//    if (m_idMenu == TYP_MENU_REZHIM && !isEditGid()) {
//        m_idMenu = TYP_MENU_READONLY;
//    }

    setMenu(m_idMenu);

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

//    if (m_idMenu == 3) {
        //    mf->SetControlBar2(-1, -1, -1, 5);
//    }

//    if (!/* pDoc->i */ isEditGid()) {
//        m_idMenu = TYP_MENU_READONLY;
//        m_menuType = IDR_GIDRTYPE_READONLY;
//    }


#if NOMYSQL

    pm_cSheetBottom->Init();
#endif

    m_colors.save1();

    if (m_idMenu == 4) {
        m_colors.set_defPS();
    }
    ::SetBar(this);

    SetWindowText2();

    m_bIsOpen = TRUE;


    // Тут я закрыл запуск нижней таблицы (журнал)

    /*
        int cod = mf->getToolbarID();

        if (cod > 0) {
            RunTableNizCod(cod);
        }
    */

    // Запуск веб для Даниилового редактора

    //  startWeb(this);



#if 0
    void log1(const char* text);

    //    CMenu *menu = CMenu::FromHandle(IDR_GIDRTYPE));
    //    CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();


    CMFCMenuBar* m = &mf->m_wndMenuBar;

    if (m) {
        CMenu* menu = CMenu::FromHandle(m->GetHMenu());
        if (menu) {
            int count = menu->GetMenuItemCount();

            for (int i = 0; i < count; i++)
            {
                CString str;
                menu->GetMenuString(i, str, MF_BYPOSITION);
                //                AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

                CMenu* m0 = menu->GetSubMenu(i);
                if (m0) {
                    int count = m0->GetMenuItemCount();

                    for (int i = 0; i < count; i++)
                    {
                        CString str1;
                        m0->GetMenuString(i, str1, MF_BYPOSITION);
                        //                        AfxMessageBox(str + " - " + str1, MB_OK|MB_ICONINFORMATION);
                    }
                }
            }
            //            int c0 = m0->GetMenuItemCount();
        }


        CMFCToolBarButton* m1 = m->GetMenuItem(0);
        if (m1) {
            log1("11");
        }
    }


    for (int i = 0; i < 256; i++) {
        CMFCToolBarButton* it = m->GetMenuItem(i);
        if (it) {
            AfxMessageBox(it->m_strText, MB_OK | MB_ICONINFORMATION);
        }
    }

#endif

    log1("Закончили CGidrView");
}

//-----------------------------------------------


CString name_node(CNode2* node, int is_ps)
{
    if (!node) return "";

    if (is_ps) {
        return node->node.namePS;
    }
    CString s;

    s.Format("%s %s %c", node->node.node_name.kod, node->node.name, node->node.node_name.p);
    return s;
}


CString name(CNode2* node, BOOL m_bIsPodpVerh)
{
    if (!node) return "";

    if (m_idMenu == 4 && m_bIsPodpVerh && node->typPS() > 0) {
        return node->node.namePS;
    }
    CString s;

    s.Format("%s %s %c", node->node.node_name.kod, node->node.name, node->node.node_name.p);
    return s;
}

CString name_typ(CNode2* node, BOOL m_bIsPodpVerh)
{
    if (!node) return "";

    if (m_idMenu == 4 && m_bIsPodpVerh && node->typPS() > 0) {
        return getPSTable(node->typPS());
    }
    return nodef[node->node.typ].table1;
}



void CGidrView::OnMouseMove(UINT nFlags, CPoint point)
{
    CNode2* node = NULL;
    CLINE2* line = NULL;
    CString str;
    char fn_geo[256];
    int n_geo = 0;
    int n_geo0 = 0;

    //  CFPoint pt = ScreenToCoordOb(point);
    CFPoint pt = ScreenToCoord(point);

    //  if (regim != R_BEGIN_RECT && regim != R_BEGIN_DRAW_LINE) {
    if (1 || regim == R_INIT) {
        /*
        if (m_nRegim == ID_INFO2) {
        if (m_isGeobaza) {
        n_geo = m_geofile->findXY2(pt, D5*masx, 3, fn_geo);
        if (n_geo/ * && isEditGeo(fn_geo)* /) {
        Status(1, fn_geo);
        }
        else {
        Status(1, "");
        }
        }
        goto QUIT;
        }
        */


        set<CText>::const_iterator p = m_cxema->set_text.begin();

        while (p != m_cxema->set_text.end()) {

            double h = hypot(pt.x - p->coord.x, pt.y - p->coord.y);
            if (h < geom.masx * D5 && p->internalNodeID == m_parent_id) {
                Status(1, p->text);
                goto QUIT;
            }
            p++;
        }

        bool is_geo_point_find = false;

        if (m_isGeobaza && m_parent_id == 0) {
            n_geo = m_geofile->findXY(pt, D5 * geom.masx, 3, fn_geo);
            if (n_geo/* && isEditGeo(fn_geo)*/) {
                Klassif* kls = m_geofile->m_kl_list->findKlN(fn_geo);
                if (kls && isPoint(kls->loc)) {
                    is_geo_point_find = true;
                }
            }
        }

        findNodeLine2(pt, &node, &line, D5);

        //    node = m_cxema->m_graph->find(pt, geom.masx*D5);

        if (node) {
            str.Format(_T("%s %s"), node->getTable(), node->getName());
            //      str.Format(_T("%s %s"), name_typ(node, !m_bIsPodpNiz), name(node, !m_bIsPodpNiz));
            Status(1, str);
            goto QUIT;
        }

        CString ss;
        ss.Format("m_graph=%d mas_otn = %g", m_cxema->m_graph, mas_otn);

        if (!is_geo_point_find) {
            //      line = m_cxema->m_graph->findLine(pt, geom.masx*D5/mas_otn);

            if (line) {
                //        str.Format(_T("%s %s - %s"), bline(line)->getTable(), name(where(line), !m_bIsPodpNiz), name(other(line), !m_bIsPodpNiz));
                str.Format(_T("%s %s"), bline(line)->getTable(), bline(line)->getName());

                Status(1, str);

                goto QUIT;
            }
        }


        if (m_isGeobaza && m_parent_id == 0) {
            //      n = m_geofile->findXY(pt, D5*masx, 3, fn);
            if (n_geo/* && isEditGeo(fn_geo)*/) {
                CString getRusName(const char* tn);

                Status(1, getRusName(fn_geo));

                goto QUIT;
            }
        }

        Status(1, "");
    }

    Status(3, m_MagistralTyp == 1 ? m_Magistral1 : m_Magistral2);

QUIT:

    CScroll::OnMouseMove(nFlags, point);
}
//-----------------------------------------------
// Отображаемая информация
//-----------------------------------------------

#include "OtobrInf.h"
#include "Inf2.h"


void CGidrView::OnInf()
{
    CInf2 oi(this, (m_idMenu == 4) ? 3 : 1, sizeof(m_cxema->m_bIsInfo) / sizeof(m_cxema->m_bIsInfo[0]), m_cxema->m_bIsInfo);

    if (oi.DoModal() == IDOK) {
        m_cxema->m_bIsGidrInf = TRUE;

        for (int i = 0; i < sizeof(m_cxema->m_bIsInfo) / sizeof(m_cxema->m_bIsInfo[0]); i++) {
            CString sss;
            sss.Format("IsInfo%d", i);
            AfxGetApp()->WriteProfileInt(szSection, sss, m_cxema->m_bIsInfo[i]);
        }

        InvalidateG();
    }
}

//-----------------------------------------------

void CGidrView::OnUpdateInf(CCmdUI* pCmdUI)
{
    int n = 0;
    for (int i = 0; i < sizeof(m_cxema->m_bIsInfo) / sizeof(m_cxema->m_bIsInfo[0]); i++)
        if (m_cxema->m_bIsInfo[i]) n++;

    pCmdUI->SetCheck(n == 0);
}

//-----------------------------------------------

void CGidrView::OnFont()
{
    LOGFONT lf;
    COLORREF color;
    CFont font;

    font.CreatePointFont(100, _T("Arial"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont(_T("Font Gidr"), lf, color);

    CFontDialog fd(&lf);

    if (fd.DoModal() == IDOK) {
        fd.GetCurrentFont(&lf);

        SaveFont(_T("Font Gidr"), lf, fd.GetColor());
        InvalidateG();
    }
}


void CGidrView::OnFont2()
{
    LOGFONT lf;
    COLORREF color;
    CFont font;

    font.CreatePointFont(100, _T("Arial"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont(_T("Font Gidr 2"), lf, color);

    CFontDialog fd(&lf);

    if (fd.DoModal() == IDOK) {
        fd.GetCurrentFont(&lf);

        SaveFont(_T("Font Gidr 2"), lf, fd.GetColor());
        InvalidateG();
    }
}


void CGidrView::OnFontMag()
{
    LOGFONT lf;
    COLORREF color;
    CFont font;

    font.CreatePointFont(80, _T("Arial"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont(_T("Font Gidr Mag"), lf, color);

    CFontDialog fd(&lf);

    if (fd.DoModal() == IDOK) {
        fd.GetCurrentFont(&lf);

        SaveFont(_T("Font Gidr Mag"), lf, fd.GetColor());

        memcpy(&m_lf_mag, &lf, sizeof(LOGFONT));


        InvalidateG();
    }
}

//-----------------------------------------------


int nodes_list(CWnd* wnd, CString& txt)
{
    txt = "";
    CGidrView* gid = getView();
    if (!gid) return -1;

    CNodeListData* nd = new CNodeListData(gid->m_cxema->m_graph, true);

    CVirtualDlg dlg(wnd, nd, -1, _TR("Укажите узел для поиска"), true);

    int ret = dlg.DoModal();

    if (ret == IDOK ) {
        int id = dlg.m_id;
        CNode2* n = gid->m_cxema->m_graph->find(id);
        if (n) {
            txt = n->getNameFull();
            return n->id;
        }
    }
    if (ret == 1004 ) {
        return -1;
    }

    return -1;
}


void CGidrView::OnEditFind()
{
    //    CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();

    bool b = isToolbar(ID_TU_ON_OFF);

    if (b) {
        Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
        if (kls) {
            OnTuFind();
            return;
        }
    }

    CNodeListData* nd = new CNodeListData(m_cxema->m_graph, true);

#if 0
    CVirtualDlg* m_pDialog = new CVirtualDlg(this, nd, _TR("Укажите узел для поиска"), false);

    if (m_pDialog != NULL) {
        BOOL ret = m_pDialog->Create(IDD_VIRTUAL, this);

        if (!ret) {
            AfxMessageBox(_T("Error creating Dialog"), MB_OK | MB_ICONINFORMATION);
        }
        m_pDialog->ShowWindow(SW_SHOW);
    }
#else 

    CVirtualDlg dlg(this, nd, -1, _TR("Укажите узел для поиска"), true);
    if (dlg.DoModal() == IDOK) {
        int id = dlg.m_id;
        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            moveNode(n);
        }
    }

#endif

}

void CGidrView::OnEditFindPs()
{
    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

    CMMenu menu(this, IDD_MENU2, _TR("Укажите узел для поиска"));
    CString str;

    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            if (n->node.namePS != "") {
                CString str = n->node.namePS;

                if (n->typPS() > 0 && n->typPS() != PS_NEUST) {
                    str += " " + getPSTable(n->typPS());
                }
                menu.Add(str, n);
            }
        }
        p++;
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет узлов"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (menu.DoModal() == IDOK) {
            CNode2* n = (CNode2*)menu.getV();

            if (IsMain() && m_isGeobaza && m_cxema->isGeobaza() && m_geofile->isOpen() && m_parent_id == 0) {
                moveXYmas(n->node.coord);
            }
            else {
                moveXY(n->node.coord);
            }
        }
    }
}



//-----------------------------------------------


struct menuitemMySQL {
    char* name;
    char* kode;
    char* query;
    char* nameMySQL;
    int node_line;
};


menuitemMySQL menudocUnvis[] = {
    //    ST_VP, "VP", NULL, "WDOdevices",1,      //  Водоразборный прибор 

    //  ST_ZE, "ZE", NULL, "",   //  Здание 
        ST_KV, "KV", NULL, "varCoefficients", 0, //  Коэффициенты вариации 
    //  ST_MT, "MT", NULL, "",   //  Манометр 
    //  ST_V1, "V1", NULL, "",   //  Местные сопротивления участка трубопровода 


    //   _TR("Объекты с измеряемыми параметрами"),_TR("Объекты с измеряемыми параметрами"), NULL, "",   //  Объекты с измеряемыми параметрами 
    //   ST_OR, "OR", NULL, "",   //  Организация 
    //   ST_PRIBOR, "PRIBOR", NULL, "",
    //   ST_RN, "RN", NULL, "",   //  Район эксплуатации 

       ST_PC, "PC", NULL, "externalCodes", 0,      //  Расчетная схема 
    //   ST_CT, "CT", NULL, "fragments", 0,        //  Система теплоснабжения 
    // ST_TE, "TE", NULL, "",   //  Термометр 
       ST_UR, "UR", NULL, "specExpends", 0,      //  Удельные расходы 
       ST_UP, "UP", NULL, "refillNodes", 1,   //  Узел подпитки 
       ST_ZN, "ZN", NULL, "setPressNodes", 1,    //  Узел с заданным напором 
    //   ST_ZP, "ZP", NULL, "",   //  ЦТП 
       NULL, "", NULL, "",
};


CAdoFile* getAdo(const char* name);

void RunTableNodes(CWnd* wnd, int i, CGraph2* graph);
void RunTableLines(CWnd* wnd, int i, CGraph2* graph);
void RunTableNodesOut(CWnd* wnd, const char* tn, CGraph2* graph);
void RunTableLinesOut(CWnd* wnd, const char* tn, CGraph2* graph);



void CGidrView::OnViewDb()
{
    int i, k = 0;
    CMMenu menu(this, IDD_MENU3, _TR("Выберите таблицу объекта с исходной информацией"));

    int ret = 0;
    bool all = false;

    do {
        k = 0;
        if (m_cxema->m_graph->isMark()) {
            for (i = 0; i < nodefsize; i++) {
                if (all || m_cxema->m_graph->inGraphNodeVyd(nodef[i].typ, m_parent_id)) {
                    menu.Add(nodef[i].table, (void*)k);
                }
                k++;
            }
            for (i = 0; i < linefsize; i++) {
                if (all || m_cxema->m_graph->inGraphLineVyd(linef[i].typ, m_parent_id)) {
                    menu.Add(linef[i].table, (void*)k);
                }
                k++;
            }
        }
        else {
            if (!curFile()) {
                AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
                return;
            }
            for (i = 0; i < nodefsize; i++) {
                if (all || m_cxema->m_graph->inGraphNode(nodef[i].typ, m_parent_id, m_cxema->m_cur_file->id)) {
                    menu.Add(nodef[i].table, (void*)k);
                }
                k++;
            }
            for (i = 0; i < linefsize; i++) {
                if (all || m_cxema->m_graph->inGraphLine(linef[i].typ, m_parent_id, m_cxema->m_cur_file->id)) {
                    menu.Add(linef[i].table, (void*)k);
                }
                k++;
            }
            int tsize = sizeof(menudocUnvis) / sizeof(menudocUnvis[0]);

            for (i = 0; i < tsize; i++) {
                menu.Add(menudocUnvis[i].name, (void*)k); k++;
            }
        }

        ret = menu.DoModal();

        if (ret == IDC_DBALL) {
            all = !all;
        }
    } while (ret == IDC_DBALL);

    if (ret == IDOK) {
        int i = (int)menu.getV();

        if (i >= 0 && i < nodefsize) {
            viewTable2(m_cxema->m_ado->m_schema, nodef[i].code, nodef[i].tableMySQL, nodef[i].table, virt_data_node, all);
        }
        if (i >= nodefsize && i < nodefsize + linefsize) {
            viewTable2(m_cxema->m_ado->m_schema, linef[i - nodefsize].code, linef[i - nodefsize].tableMySQL, linef[i - nodefsize].table, virt_data_line, all);
        }
        if (i >= nodefsize + linefsize) {
            int ii = i - (nodefsize + linefsize);

            CString table = menudocUnvis[ii].name;
            if (table[0]) {
                VIRT_DATA typ = virt_data_unknown;

                if (menudocUnvis[ii].node_line == 1) {
                    typ = virt_data_node;
                }
                viewTable2(m_cxema->m_ado->m_schema, menudocUnvis[ii].kode, menudocUnvis[ii].nameMySQL, table, typ);
            }
        }
    }
}

#include "excel.h"




void CGidrView::OnFindPodl()
{
    CFPoint pt = qqq_list->find();

    if (!(pt == CFPoint(0, 0))) moveXY(pt);
}


Klassif* getZdan(CString& f_street, CString& f_dom)
{
    Klassif* klsZ = NULL;

    f_street = "";
    f_dom = "";

    if (m_geofile->m_kl_list) {
        /*
        if (!klsZ) {
        klsZ = m_geofile->m_kl_list->findKlN(transl("Насосные станции"));
        }

        if (!klsZ) {
        klsZ = m_geofile->m_kl_list->findKlN(transl("Источники тепла"));
        }
        */
        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания"));
            f_street = "NAME_STREE";
            f_dom = "NUMBER_1";
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Zdaniya"));
            f_street = "NAME_STREE";
            f_dom = "NUMBER_1";
        }


        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
            f_street = transl("Улица");
            f_dom = transl("Дом");
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания Карта"));
            f_street = "STREET_NAM";
            f_dom = "NUMBER_1";
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Zdaniya"));
            f_street = transl("Улица");
            f_dom = transl("Дом");
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Строения"));
            f_street = transl("Улица");
            f_dom = transl("Дом");
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("СТРОЕНИЯ_ЖИЛЫЕ_ОГНЕСТОЙКИЕ"));
            f_street = transl("Улица");
            f_dom = transl("Дом");
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("building_polygon"));
            f_street = "A_STRT";
            f_dom = "A_HSNMBR";
        }


    }
    return klsZ;
}



Klassif* getZdan()
{
    Klassif* klsZ = NULL;

    if (m_geofile->m_kl_list) {
        /*
        if (!klsZ) {
        klsZ = m_geofile->m_kl_list->findKlN(transl("Насосные станции"));
        }

        if (!klsZ) {
        klsZ = m_geofile->m_kl_list->findKlN(transl("Источники тепла"));
        }
        */
        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Здания Карта"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Zdaniya"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("Строения"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("СТРОЕНИЯ_ЖИЛЫЕ_ОГНЕСТОЙКИЕ"));
        }

        if (!klsZ) {
            klsZ = m_geofile->m_kl_list->findKlN(transl("boundary_polygon"));
        }

    }
    return klsZ;
}

void setPagesRemontElectro()
{
#if NOMYSQL
    if (!m_geofile->m_kl_list) return;

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

    Klassif* klsO = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    Klassif* klsR = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    Klassif* klsK = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KORROZ));
    Klassif* klsKap = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    Klassif* klsC = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_CONTROL_TU));
    Klassif* klsE = m_geofile->m_kl_list->findKlN(EL_LINE);
    Klassif* klsZ = getZdan();

    pm_cSheetBottom->showPage(pm_cSheetBottom->m_remonts, klsR != NULL && mf->isToolbar(IDR_TOOLBAR_REMONT));
    pm_cSheetBottom->showPage(pm_cSheetBottom->m_kapremonts, klsR != NULL && mf->isToolbar(IDR_TOOLBAR_KAPREMONT));
    pm_cSheetBottom->showPage(pm_cSheetBottom->m_table, 0);
    pm_cSheetBottom->showPage(pm_cSheetBottom->m_korroz, klsK != NULL && mf->isToolbar(IDR_TOOLBAR_KORROZIA)); // Коррозия
    pm_cSheetBottom->showPage(pm_cSheetBottom->m_electro, klsE != NULL && mf->isToolbar(IDR_TOOLBAR_ELECTRO));
    pm_cSheetBottom->showPage(pm_cSheetBottom->m_control_tu, klsC != NULL && mf->isToolbar(IDR_TOOLBAR_CONTROL_TU));

    pm_cSheetPtsAlma->showPage(pm_cSheetPtsAlma->m_almaM2, 1);
    pm_cSheetPtsAlma->showPage(pm_cSheetPtsAlma->m_almaR2, 1);
    pm_cSheetPtsAlma->showPage(pm_cSheetPtsAlma->m_address, 1 || klsZ != NULL);
    pm_cSheetPtsAlma->showPage(pm_cSheetPtsAlma->m_sprav, 1);
    pm_cSheetPtsAlma->showPage(pm_cSheetPtsAlma->m_opress, klsO != NULL && mf->isToolbar(IDR_TOOLBAR_OPRESSOVKA));
#endif
}

void setPages(int n)
{
    setAddr();


#ifdef COOLDIAL
    //  setPagesRemontElectro();
    //  pm_cSheetBottom->reset();
    //  pm_cSheetPtsAlma->reset();

    Klassif* klsZ = getZdan();
    if (m_geofile->m_kl_list) {
        Klassif* klsZ = getZdan();

        pm_cSheet->showPage(2, (n > 0 && klsZ ? 1 : 0));
        pm_cSheet->showPage(3, (n > 0 && m_geofile->m_kl_list->findKlN(transl("Жилые_комплексы"))) ? 1 : 0);

        if (pm_cSheet->m_left1) {
            pm_cSheet->m_left1->SetKls(m_geofile->m_kl_list);
            m_geofile->initGeoTree(pm_cSheet->m_left1->m_ctlTree);
        }
        setAddr();
    }
    else {
        pm_cSheet->showPage(2, 0);
        pm_cSheet->showPage(3, 0);
        pm_cSheet->showPage(4, 0);
    }
#endif
}



//-----------------------------------------------
// Выбор геобазы
//-----------------------------------------------


void CGidrView::OnGeoFile()
{
}



//BOOL GetDirectory(CString &strDir);
bool SelDirDial(CWnd* pParentWnd, CString& cs);

//-----------------------------------------------
// Каталоги с картинками
//-----------------------------------------------

void CGidrView::OnPodlDir()
{
    CString strDir = AfxGetApp()->GetProfileString(szSection, _T("BMP"), _T("BMP"));

    SelDirDial(this, strDir);
    AfxGetApp()->WriteProfileString(szSection, _T("BMP"), strDir);
    qqq_list->clear();
    qqq_list->openDir(strDir);
    m_bIsPicture = TRUE;

    InvalidateG();
}

//-----------------------------------------------
// Неотрисованные узлы
//-----------------------------------------------

void CGidrView::OnUpdateNodeNeotr(CCmdUI* pCmdUI)
{
    if (!isEditGid()) {
        pCmdUI->Enable(FALSE);
        return;
    }

    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();
    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x == 0 && n->node.coord.y == 0 && n->node.internalNodeID == 0) {
            pCmdUI->Enable(TRUE);
            //      AfxMessageBox(n->getName(), MB_OK|MB_ICONINFORMATION);
            return;
        }
        p++;
    }

    pCmdUI->Enable(FALSE);
}

void CGidrView::OnGeobaza()
{
    m_isGeobaza = !m_isGeobaza;
    setNavigator(true);
    InvalidateG();
}

void CGidrView::OnUpdateGeobaza(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isGeobaza);
    pCmdUI->Enable(IsMain() && m_geofile->isOpen());
}


void CGidrView::OnColorMagRsGid()
{
    m_isMS_RS_KOT_Yes = !m_isMS_RS_KOT_Yes;
    InvalidateG();
}

void CGidrView::OnUpdateColorMagRsGid(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isMS_RS_KOT_Yes);
}


void CGidrView::OnColorOnlyPTSColor()
{
    m_isOnlyPTSColor = !m_isOnlyPTSColor;
    InvalidateG();
}

void CGidrView::OnUpdateOnlyPTSColor(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isOnlyPTSColor);
}



void CGidrView::OnUpdateGeo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsMain() && m_isGeobaza && m_cxema->isGeobaza() && m_geofile->isOpen());
}


void CGidrView::OnUpdateGeoFindNext(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsMain() && m_isGeobaza && m_cxema->isGeobaza() && m_geofile->isOpen() && m_geofile->isNext());
}

void CGidrView::vydGeo(const char* tn, CCoordList& m_NP)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);

    CString q;

    q.Format("SELECT obj.id FROM %s obj WHERE obj.shape.STWithin (%s) = 1", tn, str);

    set<int> set_id;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int i = 0, n = kls->geo4.size();

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->mark = true;
        }
    }
    Invalidate();
}

#include "InputDialogDate.h"


void CGidrView::korroziaPlan(const char* tn, CCoordList& m_NP)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    if (AfxMessageBox("Сделать выделенные Индикаторы коррозии плановыми", MB_YESNO) != IDYES) return;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);

    CInputDialogDate dlg(this, "", "Введите дату планирования");

    if (dlg.DoModal() != IDOK) return;

    int sost = 1;
    int sost2 = 4;

    CString q;

    q.Format(

        "UPDATE obj\n"
        "set obj.sostoyanie = %d,\n"
        "obj.data_planirovaniya = '%s'\n"
        "from indikator_korrozii obj\n"
        "where obj.sostoyanie = %d AND obj.shape.STWithin (%s) = 1\n", sost, dlg.m_date1.Format("%Y%m%d"), sost2, str);

    if (!ado->Execute(q)) {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return;
    }

    q.Format("SELECT obj.id FROM %s obj WHERE obj.sostoyanie = %d AND obj.shape.STWithin (%s) = 1", tn, sost, str);

    set<int> set_id;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int i = 0, n = kls->geo4.size();

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            //            kls->geo4[i]->mark = true;
            kls->geo4[i]->cl.color = getGeoColor(sost);
        }
    }
    Invalidate();
}

/*

void CGidrView::korroziaAr(const char* tn, CCoordList& m_NP)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);

    int sost = 4;
    int sost2 = 1;

    if (AfxMessageBox("Сделать выделенные Индикаторы коррозии архивными", MB_YESNO) != IDYES) return;


    CString q;


    q.Format(

        "UPDATE obj\n"
        "set obj.sostoyanie = %d,\n"
        "data_planirovaniya = NULL,\n"
        "data_ustanovki = NULL,\n"
        "data_izvlecheniya = NULL\n"
        "from indikator_korrozii obj\n"
        "where obj.sostoyanie = %d AND obj.shape.STWithin (%s) = 1\n", sost, sost2, str);

    if (!ado->Execute(q)) {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return;
    }

    q.Format("SELECT obj.id FROM %s obj WHERE obj.sostoyanie = %d AND obj.shape.STWithin (%s) = 1", tn, sost, str);

    set<int> set_id;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int i = 0, n = kls->geo4.size();

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            //            kls->geo4[i]->mark = true;
            kls->geo4[i]->cl.color = getGeoColor(sost);
        }
    }
    Invalidate();
}
*/

bool CGidrView::korrozia_del(const char* tn, int id)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return false;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);
    CString s;

    s.Format("Удалить выбранный индикатор коррозии из отопительного сезона %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);

    if (AfxMessageBox(s, MB_YESNO) != IDYES) return false;

    CString q;

    q.Format(
        "DELETE FROM indikator_korrozii_po_godam \n"
        "WHERE (YEAR(data_planirovaniya) = %d OR YEAR(data_ustanovki) = %d)\n"
        "AND id_i = %d \n", m_sezon_korrozia, m_sezon_korrozia, id);


    if (!ado->Execute(q)) {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    setKorroziaSezon(m_sezon_korrozia, true);
    Invalidate();
    return true;
}



void CGidrView::korroziaAr(const char* tn, CCoordList& m_NP)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);
    CString s;

    s.Format("Удалить выбранные индикаторы коррозии из отопительного сезона %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);

    if (AfxMessageBox(s, MB_YESNO) != IDYES) return;

    CString q;

    q.Format(
        "DELETE FROM indikator_korrozii_po_godam \n"
        "WHERE (YEAR(data_planirovaniya) = %d OR YEAR(data_ustanovki) = %d)\n"
        "AND id_i IN \n"
        "(\n"
        "SELECT id FROM indikator_korrozii WHERE shape.STWithin (%s) = 1\n"
        ") \n", m_sezon_korrozia, m_sezon_korrozia, str);



    if (!ado->Execute(q)) {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return;
    }

    setKorroziaSezon(m_sezon_korrozia, true);
    Invalidate();
}



#include "geomenu.h"




void CGidrView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    m_dbl = 0;

    if (m_nTimerDbl)
    {
        KillTimer(m_nTimerDbl);
        m_nTimerDbl = 0;
    }

    int shift = GetKeyState(VK_SHIFT) & 0x80;
    if (shift) return;

    int ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;

    switch (m_nRegim) {
    case ID_MARK_REV:
    case ID_DEL:
        return;
    }

    if (regim == R_INIT && m_nRegim == ID_SET_UCH_MS) {
        SetMs();
        return;
    }


    if (regim == R_INIT) {
        CFPoint pt = ScreenToCoordOb(point);

        //    if (!find(pt)) {

        info2(pt);

        if (1) {
            char tn[256];
            int num;

            CGeoMenu menu(this, IDD_GEO_MENU);

            CNode2* node = m_cxema->m_graph->find(m_parent_id, pt, geom.masx * D5 / mas_otn);
            CLINE2* line = m_cxema->m_graph->findLine(m_parent_id, pt, geom.masx * D5 / mas_otn);

            if (node) {
                //        menu.Add(_TR("Узел"), -1);
            }
            if (node) {
                //        menu.Add(_TR("Участок"), -2);
            }

            int ret;

            if (m_parent_id != 0) return;

            ret = m_geofile->findAllXY(&menu, this, pt, D5 * geom.masx / mas_otn   /*, tn, num*/);
            if (!ret) return;

            EndDrawLine();
            EndDrawNode();

            //      CDC *dc1 = SaveImage();

            ret = menu.DoModal();
            menu.close();

            num = -1;

            if (ret != IDCANCEL) {
                strcpy(tn, menu.m_strText);
                num = menu.m_num;
            }
            //      CloseImage(dc1);

            switch (ret) {
            case IDOK:
                if (num) {
                    //            setIsEdit(isEditGeo(tn));
                    GeoInfo(tn, num, false);
                }
                break;
            case IDC_ISPR:
                if (isEditGeo(tn)) GeoIsm(tn, num);
                return;
            case IDC_ISPR_PODP:
                if (isEditGeo(tn)) GeoIsmPodp(tn, num);
                return;
            case IDC_DEL:
                if (isEditGeo(tn)) GeoDel(tn, num);
                return;
            case IDC_TU:
                if (isEditGeo(tn)) GeoTU(tn, num);
                return;
            }
        }
    }
    else if (regim == R_BEGIN_DRAW_LINE && m_nRegim == ID_NEW_GEO) {
        if (m_geofile && isEditGeo(m_kls->nazv)) {
            CString tn = m_kls->nazv;
            tn.MakeLower();

            if (tn == EL_LINE) {
                AfxMessageBox("Линия электропередач должна заканчиваться на приемнике электроснабжения", MB_OK | MB_ICONINFORMATION);
            }
            else {
                m_NP.pop_back();

                int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);

                regim = R_INIT;
                m_nRegim = ID_INFO;
                m_NP.clear();
                GetDocument()->SetModifiedFlag(true);
                InvalidateG();
                if (nn) {
                    GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                }
            }
        }
    }

    else if (regim == R_BEGIN_LEN_OKR_VYD_GEO_2) {   // после выделения гео объектов делает что-то с ними
//        m_nRegim = ID_FRAGMENT_POLY_GEO;
//        CCoordList::iterator it = m_NP.begin();
//        m_NP.push_back(*it);
//        regim = R_BEGIN_MOVE_LINE;
//        vydGeo(REM_INDICATOR, m_NP);
//        korroziaPlan(REM_INDICATOR, m_NP);

        if (m_regim_geo == ID_FRAGMENT_POLY_KORROZIA_AR) {
            korroziaAr(REM_INDICATOR, m_NP);
        }
        else if (m_regim_geo == ID_FRAGMENT_POLY_KORROZIA_PLAN) {
            korroziaPlan(REM_INDICATOR, m_NP);
        }

        m_NP.clear();
        regim = R_INIT;
        m_nRegim = ID_INFO;
        Invalidate();
    }

    CScroll::OnLButtonDblClk(nFlags, point);
}

void CGidrView::Out(BOOL anal)
{
    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;

    int fileID = m_cxema->m_cur_file->id;

    CMMenu menu(this, IDD_MENU3, _TR("Выберите таблицу объекта с результатами расчета"));
    CString str, q, query;

    bool mark = m_cxema->m_graph->isMark();

    bool all = false;
    int ret = 0;

    do {
        for (int i = 0; menudocres[i].name; i++)
        {
            str = menudocres[i].name;

            //        inGraphLineVyd
            bool y = true;

            if (!all) {
                if (menudocres[i].node_line == 2) {
                    y = false;

                    int typ = getLineTypOut(menudocres[i].name);

                    if (mark && m_cxema->m_graph->inGraphLineVyd(typ, m_parent_id) || !mark && m_cxema->m_graph->inGraphLine(typ, m_parent_id, fileID)) {
                        y = true;
                    }

                    if (typ == TIP_RD && !y) {
                        if (mark && m_cxema->m_graph->inGraphLineVyd(TIP_RR, m_parent_id) || !mark && m_cxema->m_graph->inGraphLine(TIP_RR, m_parent_id, fileID)) {
                            y = true;
                        }
                        else if (mark && m_cxema->m_graph->inGraphLineVyd(TIP_RP, m_parent_id) || !mark && m_cxema->m_graph->inGraphLine(TIP_RP, m_parent_id, fileID)) {
                            y = true;
                        }
                    }
                }
                else if (menudocres[i].node_line == 1) {
                    y = false;
                    int typ = getNodeTypOut(menudocres[i].name);
                    if (mark && m_cxema->m_graph->inGraphNodeVyd(typ, m_parent_id) || !mark && m_cxema->m_graph->inGraphNode(typ, m_parent_id, fileID)) {
                        y = true;
                    }
                    if (!y && !strcmp(menudocres[i].name, "DR_OUT")) {
                        if (mark && m_cxema->m_graph->inGraphNodeVyd(TIP_PR, m_parent_id) || !mark && m_cxema->m_graph->inGraphNode(TIP_PR, m_parent_id, fileID)) {
                            y = true;
                        }
                    }

                    if (!y && !strcmp(menudocres[i].name, "PT_OUT")) {
                        if (mark && m_cxema->m_graph->inGraphNodeVyd(TIP_PR, m_parent_id) || !mark && m_cxema->m_graph->inGraphNode(TIP_PR, m_parent_id, fileID)) {
                            y = true;
                        }
                        if (mark && m_cxema->m_graph->inGraphNodeVyd(TIP_PO, m_parent_id) || !mark && m_cxema->m_graph->inGraphNode(TIP_PO, m_parent_id, fileID)) {
                            y = true;
                        }
                    }


                }
            }
            if (y) {
                menu.Add(menudocres[i].kode, (void*)i);
            }
        }


        menu.Add(_TR("Узлы с внутренними схемами"), (void*)1003);


        menu.Add(_TR("Участки магистральных фрагментов ТС"), (void*)1000);
        menu.Add(_TR("Участки распределительных фрагментов ТС"), (void*)1001);
        //    menu.Add(_TR("Нагрузки и расходы по фрагментам"), (void*)1002);

        ret = menu.DoModal();

        if (ret == IDC_DBALL) {
            all = !all;
        }
    } while (ret == IDC_DBALL);

    if (ret == IDOK && menu.m_Index != -1)
    {
        int id = (int)menu.getV();


        if (id == 1003) {
            CString q = readFile(argpath() + "\\sql\\out4.sql");
            CString ss;
            ss.Format("%d", fileID);
            q.Replace("$fileID$", ss);

            viewQ2(ado, m_cxema->m_ado->m_schema, q, "", menu.m_strText, virt_data_unknown, "", menu.m_strText);
        }
        else if (id == 1000) {
            CString q = readFile(argpath() + "\\sql\\pipeSectionsMAG.sql");
            viewQ2(ado, m_cxema->m_ado->m_schema, q, "pipeSections", menu.m_strText, virt_data_edit, "", menu.m_strText);
        }
        else if (id == 1002) {
            CString q = readFile(argpath() + "\\sql2\\potreb.sql");

            if (fileID >= 0) {
                CString ss;
                ss.Format("%d", fileID);
                q.Replace("$fileID$", ss);
                int calculationID = getOutID(ado, fileID);
                ss.Format("%d", calculationID);
                q.Replace("$calculationID$", ss);
                viewQ2(ado, m_cxema->m_ado->m_schema, q, "fragments", menu.m_strText, virt_data_unknown, "", menu.m_strText);
            }
        }
        else if (id == 1001) {
            CString q =
                /*
                "SELECT ps.id, m.naimenovanie_uchastka_rs AS 'Наименование распредсети', "
                " ec1.name AS 'Код РС начальной камеры', nn1.externalNodeName AS 'Наименование начальной камеры', "
                " ec2.name AS 'Код РС конечной камеры', nn2.externalNodeName AS 'Наименование конечной камеры' "
                " FROM pipeSections ps  "
                " JOIN nodes nn1 ON nn1.id=ps.nodeID1 "
                " JOIN nodes nn2 ON nn2.id=ps.nodeID2 "
                " join externalCodes ec1 on ec1.id = nn1.externalCodeID "
                " join externalCodes ec2 on ec2.id = nn2.externalCodeID"
                " JOIN ps.uchastok_rs m ON m.id=ps.distSite";

                */


                "SELECT p.id, m.naimenovanie_uchastka_rs AS 'Наименование распредсети', "
                " ec1.name AS 'Код РС начальной камеры', nn1.externalNodeName AS 'Наименование начальной камеры', "
                " ec2.name AS 'Код РС конечной камеры', nn2.externalNodeName AS 'Наименование конечной камеры' , "
                " p.pipeLength,p.organizationID, o.name, "
                " tubeTypes.name,p.firstPICdateHP,p.lastTransDate " //,p.sectExploitPeriod "
                " FROM pipeSections p "
                " JOIN nodes nn1 ON nn1.id=p.nodeID1 "
                " JOIN nodes nn2 ON nn2.id=p.nodeID2 "
                " join externalCodes ec1 on ec1.id = nn1.externalCodeID "
                " join externalCodes ec2 on ec2.id = nn2.externalCodeID "
                " JOIN uchastok_rs m ON m.id=p.distSite "
                " LEFT JOIN organizations o ON o.id=p.organizationID "
                " LEFT JOIN tubeTypes ON tubeTypes.id=p.tubeTypeID";


            viewQ2(ado, m_cxema->m_ado->m_schema, q, "pipeSections", menu.m_strText, virt_data_edit, "", menu.m_strText);
        }
        else if (id >= 0) {
            char sel[] = "select ";


            if (menudocres[id].query) {
                CString ss;
                CString q = menudocres[id].query;
                CString qq = q;
                qq.MakeUpper();

                if (qq.Find("SELECT ") != 0) {
                    q = readFile(argpath() + q);
                    ss.Format("%d", fileID);
                    q.Replace("$fileID$", ss);
                }
                viewQ2(ado, m_cxema->m_ado->m_schema, q, menudocres[id].name, menu.m_strText, menudocres[id].node_line == 1 ? virt_data_node_out : menudocres[id].node_line == 2 ? virt_data_line_out : virt_data_unknown_out, "",menu.m_strText, all);
            }
            else {
                viewTable2(m_cxema->m_ado->m_schema, menudocres[id].name, menudocres[id].name, menu.m_strText, menudocres[id].node_line == 1 ? virt_data_node_out : menudocres[id].node_line == 2 ? virt_data_line_out : virt_data_unknown_out, all);
            }
        }
    }
}

void CGidrView::OnOut()
{
    Out(1);
}


void CGidrView::OnOut2()
{
    Out(0);
}

bool CGidrView::reset()
{
    return m_cxema->m_graph->reset();
}


void CGidrView::setOpresNull()
{
    m_id_opr = -1;
    m_id_remont2 = -1;
    m_id_osvidet2 = -1;
    m_id_osmotr2 = -1;
}


void CGidrView::OnEsc()
{
    if (m_nRegim == ID_F4) {
        m_NP.clear();
    }

    if (m_nRegim == ID_MARK_REV || m_nRegim == ID_BEGIN_CANAL_2) {
        m_nRegim = ID_INFO;
    }

    if (m_id_opr > 0) {
        m_id_opr_old = m_id_opr;
        Invalidate();
    }

    if (m_id_remont2 > 0) {
        m_id_remont2_old = m_id_remont2;
        Invalidate();
    }

    if (m_id_osvidet2 > 0) {
        m_id_osvidet2_old = m_id_osvidet2;
        Invalidate();
    }


    if (m_id_osmotr2 > 0) {
        m_id_osmotr2_old = m_id_osmotr2;
        Invalidate();
    }


    if (m_linePTS_ID > 0) {
        m_linePTS_ID = -1;
        Invalidate();
    }

    setOpresNull();

    //  m_geofile->setVisibleAllRemonts();

    if (m_isNeOpr) {
        m_isNeOpr = false;
        Invalidate();
    }


    if (m_dcSave) {
        CloseImage(m_dcSave);
        m_dcSave = NULL;
    }

    if (m_kod_vyd != -1) {
        m_kod_vyd = -1;
        Invalidate();
    }

    m_magistral_kod = 0;
    m_ms_kod = 0;
    m_rs_kod = 0;

    m_pts_ut_kod = 0;


    //  m_kod0_vyd = "";
    m_node_first = NULL;
    m_node_last = NULL;


    if (m_mag || m_ms || m_rs) {
        m_mag = 0;
        m_ms = 0;
        m_rs = 0;
        Invalidate();
    }


    m_CurNode = NULL;
    m_CurLine = NULL;
    m_napr1 = NULL;
    if (reset()) {
        InvalidateG();
    }

    m_Magistral1 = "";
    m_MagistralID1 = -1;
    m_Magistral2 = "";
    m_MagistralID2 = -1;

    Status(3, m_MagistralTyp == 1 ? m_Magistral1 : m_Magistral2);


    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.isMarkAnalyze) {
            n->node.isMarkAnalyze = false;
            n->node.colorP = 0xFFFFFFFF;
        }
        CLINE2* l;
        for (l = n->lines; l; l = next(l)) {
            if (n->node.colorP != 0xFFFFFFFF) {
                n->node.colorP = 0xFFFFFFFF;
                InvalidateG();
            }

            CLine2* ll = bline(l);
            if (ll->line.colorP != 0xFFFFFFFF || ll->line.colorO != 0xFFFFFFFF) {
                ll->line.colorP = ll->line.colorO = 0xFFFFFFFF;
                InvalidateG();
            }
        }
        p++;
    }

    if (m_geofile) {
        if (m_geofile->ClearMark()) {
            Invalidate();
        }
    }

    if (m_geofile->m_old_remont > 0) {
#if NOMYSQL
        allGeoVisible(m_geofile->m_old_TN, true);
        m_geofile->m_old_remont = 0;
        refreshRemont();
        Invalidate();
#endif
    }

    if (m_bIsFragment) {
        //    Invalidate();
    }

    Invalidate();

    CScroll::OnEsc();
}

void CGidrView::checkPjezo()
{
    /*
    Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();
    CView *pView;

    m_CurLine = NULL;
    m_CurNode = NULL;


    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
    pView = pDoc->GetNextView(pos);
    if (pView->IsKindOf(RUNTIME_CLASS(CPjezo))) {
    ((CMDIChildWnd *) pView->GetParent())->MDIDestroy();
    break;
    }
    }
    m_cxema->m_graph->reset();
    m_napr1 = NULL;

    pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
    pView = pDoc->GetNextView(pos);
    if (pView->IsKindOf(RUNTIME_CLASS(CGidView))) {
    ((CMDIChildWnd *) pView->GetParent())->MDIDestroy();
    }
    }
    */
}

void CGidrView::OnPjezo()
{
    if (m_cxema->m_graph->list_pjezo.empty()) return;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    pDoc->m_cxema = m_cxema;
    //  pDoc->m_graph = m_cxema->m_graph;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        CPjezo* pView = (CPjezo*)pDoc->GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CPjezo))) {
            //          CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
            //          pFrame->MDIActivate(pView->GetParent());
            CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
            mf->setActiveView(pView);
            pView->initPjezo();
            return;
        }
    }

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_PJEZO);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }

}

void CGidrView::OnUpdatePjezo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_cxema->m_graph->list_pjezo.empty());
}


void CGidrView::OnProtocol()
{
#if 0
    /*
    CProtocol pr(this, GetFName(GetDocument()->GetPathName())+".pro");
    pr.DoModal();
    */

    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)pApp->m_pMainWnd;

    if (pm_wndProtBar->IsVisible()) {
        pMainFrame->ShowControlBar(pm_wndProtBar, FALSE, FALSE);
    }
    else {
        pMainFrame->ShowControlBar(pm_wndProtBar, TRUE, FALSE);
        pm_cProt->setFile(GetFName(GetDocument()->GetPathName()) + ".pro");
    }
#endif
}


void CGidrView::OnUpdateProtocol(CCmdUI* pCmdUI)
{
#if 0
    if (pm_wndProtBar->IsVisible())
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
#endif
}


void CGidrView::OnOpengl()
{
#ifdef COOLDIAL


    CString ss, str;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    pDoc->m_cxema = m_cxema;
    //  pDoc->m_graph = m_cxema->m_graph;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_4);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
#endif
}



void CGidrView::OnZona()
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    double h_min = 1e10, h_max = -1e10;
    CNode2* n_min = NULL, * n_max = NULL;

    m_cxema->m_graph->reset();
    m_cxema->m_graph->WS1(p->second, 0);

    while (p != m_cxema->m_graph->map_node.end()) {

        CNode2* n = p->second;

        if (n->node.len != -1) {
            if (n->node.geoMarkTopTube + n->node.hz > h_max) {
                h_max = n->node.geoMarkTopTube + n->node.hz;
                n_max = n;
            }
            //    if (n->data.geodz < h_min && n->data.geodz != 0) {
            if (n->node.geoMarkTopTube < h_min) {
                h_min = n->node.geoMarkTopTube;
                n_min = n;
            }
        }
        p++;
    }

    m_cxema->m_graph->reset();
    m_cxema->m_graph->WS1_h(n_min, 0, 60);

    CString str;

    str.Format(_TR("Полный статический напор %g м.\n"
        "Пьезометрический статический напор %g м.\n"
        "Минимальная геодезическая отметка верха трубы %g м. у узла %s %s\n"
        "Объекты нижней зоны окрашены зеленым цветом"),
        h_max + 5, h_max + 5 - h_min, h_min, n_min->node.node_name.kod, n_min->node.name);
    MessageBox(str, _TR("Гидростатические зоны"));

    moveNode(n_min);

    InvalidateG();
}

void CGidrView::OnFindGeo()
{
    CFPoint pt;

    CDC* dc = SaveImage();
    pt = m_geofile->find();
    CloseImage(dc);

    if (!(pt == CFPoint(0, 0))) moveXY(pt);
}


bool CGidrView::tabGeo(Klassif* kls)
{
    if (kls) {
        //        RunTableMap(kls->m_database, kls->nazv, NULL, getHelp_Map(), 0, 1, FALSE, FALSE, FALSE, FALSE);

        viewTable2(kls->m_database, kls->nazv, kls->nazv, kls->RusName(), virt_data_geo);

    }
    return true;
}

void CGidrView::OnGeoFindNext()
{
    CFPoint pt = m_geofile->findNext();
    if (!(pt == CFPoint(0, 0))) moveXY(pt);
}


bool CGidrView::geoInfo(CFPoint pt)
{
    char tn[256];
    int n = m_geofile->findXY(pt, D5 * geom.masx, tn);
    //  setIsEdit(isEditGeo(tn));

    cur_geo_tn = tn;
    cur_geo_n = n;

    int ret = GeoInfo(tn, n, false);

    return ret != 0;
}

void CGidrView::OnFon()
{
    COLORREF rgb[] = {
        0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
        0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
        0x00FFFF, 0xFFFFFF,
    };
    CColorDialog dlgColor(m_bk_color);
    dlgColor.m_cc.lpCustColors = rgb;

    if (dlgColor.DoModal() == IDOK)
    {
        m_bk_color = dlgColor.GetColor();
        InvalidateG();
    }
}


void CGidrView::OnUpdateKam(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_nRegim == pCmdUI->m_nID);
    pCmdUI->Enable(IsMain());
}

void CGidrView::OnUpdateKam2(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!IsMain());
}


void CGidrView::OnGeoGeodez()
{
    m_bIsGeoGeodez = !m_bIsGeoGeodez;
    InvalidateG();
}

void CGidrView::OnUpdateGeoGeodez(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsGeoGeodez);
    pCmdUI->Enable(IsMain() && m_isGeobaza && m_cxema->isGeobaza() && m_geofile->isOpen());
}


void CGidrView::OnUkrup()
{
    m_isUkrup = !m_isUkrup;
    InvalidateG();
}

void CGidrView::OnUpdateUkrup(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isUkrup);
}


void CGidrView::OnGidinf()
{
    m_cxema->m_bIsGidrInf = !m_cxema->m_bIsGidrInf;
    InvalidateG();
}

void CGidrView::OnUpdateGidinf(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_cxema->m_bIsGidrInf);
}

#include "UnvisualTree.h"


void CGidrView::OnNovisual()
{
    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CUnvisualTree dlg(this, m_cxema->m_graph, ado, m_cxema->m_cur_file->id);
    if (dlg.DoModal() == IDOK) {
    }

    m_cxema->initGidTree();
    Invalidate();

}


//#include "AnalEdit.h"
//#include "ColorLine.h"
//#include "ColorLine1.h"

int ado_info_find(CWnd* wnd, CAdoFile* ado, const char* tn, const char* capt);

/*
1 - edit
2 - readonly
3 - PTS
*/

void CGidrView::setMenu(TypMenu idMenu)
{
    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    TypMenu isMain = TYP_MENU_REZHIM; // isMDBMain(m_cxema->m_cur_file->m_dbf);

    if (idMenu == TYP_MENU_REZHIM && m_cxema->isReadOnly() && !/* pDoc->i */ isEditGid()) {
        AfxMessageBox(_TR("Вы не имеете прав на редактирование этого файла."), MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (idMenu == TYP_MENU_REZHIM && m_cxema->isReadOnly() && (/* pDoc->i */ isRegim() && isMain || /* pDoc->i */ isAdmin() && !isMain)) {
        AfxMessageBox(_TR("Файл только для чтения"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    //  setIsEdit(idMenu == 1);

    m_idMenu = idMenu;

    if (idMenu == TYP_MENU_REZHIM) {
        m_menuType = IDR_GIDRTYPE;
    }
    else if (idMenu == TYP_MENU_READONLY) {
        m_menuType = IDR_GIDRTYPE_READONLY;
    }
    //  else if (idMenu == 3) {
    //    m_menuType = IDR_GIDRTYPE_PTS;
    //  }
    else if (idMenu == TYP_MENU_PTS) {
        m_menuType = IDR_GIDRTYPE_PTS_NEW;
    }

#ifdef COOLDIAL  
    Cgid6Doc* pdoc = GetDocument();

    pdoc->m_DefaultMenu =
        ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(m_menuType));
    if (pdoc->m_DefaultMenu == NULL)
        return;

    CMDIFrameWnd* frame = ((CMDIChildWnd*)GetParent())->GetMDIFrame();
    frame->MDISetMenu(CMenu::FromHandle(pdoc->m_DefaultMenu), NULL);
    frame->DrawMenuBar();
#endif

#if 0
    Cgid6Doc* pdoc = GetDocument();

    CMenu menu;

    //    m_menuType = IDR_GIDRTYPE;

    /*
    //    if (menu.LoadMenu(MAKEINTRESOURCE(m_menuType)))
    //    {
            CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
            CMFCMenuBar* m = &mf->m_wndMenuBar;

            if (m) {

                m->InsertButton(CMFCToolBarMenuButton(0, menu, -1, _T("&MyMenu")), -1);
                m->AdjustLayout();
                m->AdjustSizeImmediate();
            }
    */


    CMDIFrameWnd* frame = ((CMDIChildWnd*)GetParent())->GetMDIFrame();
    frame->MDISetMenu(CMenu::FromHandle(menu), NULL);
    frame->DrawMenuBar();

    //    }
#endif


    return;


    Cgid6Doc* pdoc = GetDocument();

    //  pdoc->m_DefaultMenu =
    //     ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(m_menuType));
    //  if (pdoc->m_DefaultMenu == NULL)
    //     return;

    //  CMenu *menu = CMenu::FromHandle(pdoc->m_DefaultMenu);

    CMenu menu1;
    menu1.CreatePopupMenu();

    menu1.AppendMenu(MF_STRING, ID_LEFT_BAR, "Проводник карты");
    menu1.AppendMenu(MF_STRING, ID_MAG_ALMA, "Проводник структуры сети");
    menu1.AppendMenu(MF_STRING, ID_BOTTOM_REMONT, "Проводник мониторинга");

    menu1.AppendMenu(MF_STRING, ID_EDIT_FIND_NODE, "!!!!");

    menu1.AppendMenu(MF_STRING, ID_TUBING_1, "Канальная");
    menu1.AppendMenu(MF_STRING, ID_TUBING_2, "Бесканальная");
    menu1.AppendMenu(MF_STRING, ID_TUBING_4, "Надземная");
    menu1.AppendMenu(MF_STRING, ID_TUBING_3, "Подвальная");
    menu1.AppendMenu(MF_STRING, ID_TUBING_5, "Обвязка узлов и насосных станций");

    menu1.AppendMenu(MF_STRING, ID_GOOGLE_ELEVATION, "Высота по Google");
    menu1.AppendMenu(MF_STRING, ID_MAP_SEARCH, "Поиск");
    //    menu1.AppendMenu(MF_STRING, ID_MAP_CITY, "Найти город");
    //    menu1.AppendMenu(MF_STRING, ID_ADD_CITY, "Установить город");
    menu1.AppendMenu(MF_STRING, ID_MAP_PATH, "Каталог");
    menu1.AppendMenu(MF_STRING, ID_MAP_YES, "Восстанавливать подключение");

    CMenu menu;
    menu.CreateMenu();
    menu.AppendMenu(MF_POPUP | MF_STRING, (UINT_PTR)menu1.GetSafeHmenu(), "!!!!");

    CMDIFrameWnd* frame = ((CMDIChildWnd*)GetParent())->GetMDIFrame();
    frame->MDISetMenu(&menu, NULL);
    frame->DrawMenuBar();
}



void CGidrView::OnEditor1()
{
    if (m_idMenu == 1) return;

    m_colors.load1();
    //    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    //  if (!pApp || !pApp->Login(TRUE)) {
    //    return;
    //  }
#if 0
    if (!/* pDoc->i */ isEditMain()) {
        CString str = _TR("Нельзя");
        Cgid6App* pApp = (Cgid6App*)AfxGetApp();
        str.Format("%s не имеет прав на редактирование", pDoc->m_user_gid);
        AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
    }
    else {
#endif
        setMenu(TYP_MENU_REZHIM);
//    }

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    //  mf->SetControlBar2(0, 3);
    ::SetBar(this);
    m_colors.load(isPassport());

    Invalidate();
}

void CGidrView::OnEditor2()
{
    if (m_idMenu == TYP_MENU_READONLY) return;

    m_colors.load1();
    setMenu(TYP_MENU_READONLY);

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    //  mf->SetControlBar2(0);
    m_colors.load(isPassport());

    ::SetBar(this);
}

#if 0

void CGidrView::OnEditor3()
{
    if (m_idMenu == 3) return;

    m_colors.load1();

    setMenu(3);
    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    //  mf->SetControlBar2(-1, -1, 4);
    ::SetBar(this);

    m_colors.load(isPassport());

    Invalidate();
}
#endif

void CGidrView::OnEditor4()
{
    m_bIsRezhim = TRUE;

    if (m_idMenu == TYP_MENU_PTS) return;

    setMenu(TYP_MENU_PTS);

    m_colors.set_defPS();

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    //  mf->SetControlBar2(-1, -1, 4, -1, 6);
    ::SetBar(this);
    m_colors.load(isPassport());

    Invalidate();
}

void CGidrView::OnUpdateEditor1(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_idMenu == 1);
}

void CGidrView::OnUpdateEditor2(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_idMenu == 2);
}

void CGidrView::OnUpdateEditor3(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_idMenu == 3);
}

void CGidrView::OnUpdateEditor4(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_idMenu == 4);
}


void CGidrView::OnBmp()
{
    CFileDialog fd(TRUE, _T("BMP"), NULL,
        OFN_HIDEREADONLY,
        //    _T("JPEG-файл|*.jpg|GIF-файл|*.gif|BMP-файл|*.bmp|"), this);
        _TR("Картинки (*.jpg;*.gif;*.tif;*.bmp;*.dxf)|*.jpg; *.tif; *.gif; *.bmp; *.dxf|"), this);

    if (fd.DoModal() != IDOK) return;

    m_strPicture = fd.GetPathName();
    openBmp(m_strPicture);
    m_bIsPicture = TRUE;
    OnBmpFind();

    InvalidateG();
}

void CGidrView::OnSetLength()
{
    CLINE2* l;
    CString str;

    if (!isEditGid() || !m_cxema->isGeobaza() || !IsMain()) return;

    if (MessageBox(_TR("Установить длины всех участков?"), _TR("Установить длины"), MB_YESNO) != IDYES) return;

    str.Format(_TR("Внимание!\nПри выполнении этой операции длины всех участков будут изменены\nВыполнить операцию?"));

    if (MessageBox(str, _TR("Установить длины"), MB_YESNO) != IDYES) return;

    int nn = 0, n1 = 0;


    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (l = n->lines; l; l = next(l)) {
            if (IsBegin(l)) {
                nn++;
            }
        }
        p++;
    }
    {
        Percent2 pc;

        p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            for (l = n->lines; l; l = next(l)) {
                if (IsBegin(l)) {
                    m_cxema->setLength(l, is_google_map & GOOGLE_LEN, false);
                    n1++;
                    pc.SetValue(n1 * 100. / nn);
                }
            }
            p++;
        }
    }

    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);

    GetDocument()->SetModifiedFlag(true);
}


void CGidrView::OnUpdateSetLength(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(isEditGid() && m_cxema->isGeobaza() && (m_isGeobaza || is_google_map) && IsMain());
}


void CGidrView::OnExportDxf()
{
    CFileDialog fd(FALSE, _T("dxf"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("Dxf-файлы|*.dxf|"), this);

    if (fd.DoModal() == IDOK) m_cxema->exportDxf(fd.GetPathName(), m_isGeobaza);
}


int CGidrView::move_geo(const char* tn, int n)
{
    if (!m_geofile || !m_geofile->m_kl_list) return 0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (kls && kls->ins) {
        int id2 = kls->GetNom2(n);
        if (id2 > 0) {
            kls->geo4[id2 - 1]->mark = true;
            CCoordList cl = kls->geo4[id2 - 1]->cl;

            CFPoint pt = cl.centroid();
            moveXY(pt);
        }
        return 1;
    }

    return 0;
}

// Переход из протокола

void CGidrView::move_line(CNodeName& nn1, CNodeName& nn2)
{
    AfxMessageBox(_TR("Временно удалено 504"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::drawMark(bool redr)
{
    if (m_CurNode || m_CurLine) {
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            if (m_CurNode) {
                //        drawNode0(m_dc, m_CurNode1, redr);
                drawNode0(m_dc, m_CurNode, redr);
            }
            if (m_CurLine) {
                drawLine1(m_dc, m_CurLine, redr, false, false);
            }
            EndPaint(m_dc);
        }
    }
}

void drawText(CDC* dc, int cx, int cy, const char* str);


void CGidrView::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == m_nTimer)
    {
        static bool redr = true;
        drawMark(redr);
        redr = !redr;
    }

    if (nIDEvent == m_nTimer2) // OPC
    {
        drawOPC2();
    }

    extern COPCThread2* pOPCThread;

    if (0 && nIDEvent == m_nTimer3 && pOPCThread)
        //  if (1 && nIDEvent == m_nTimer3)
        //  if (nIDEvent == m_nTimer3)
    {
        CPoint point;
        CFPoint pt;

        GetCursorPos(&point);
        ScreenToClient(&point);

        pt = ScreenToCoord(point);

        regim = R_INIT;
        m_nRegim = ID_INFO;

        CMapGraph::const_iterator p;
        p = m_cxema->m_graph->map_node.begin();

        CFRect rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

        static int num = 0;
        int i = 0;

        while (p != m_cxema->m_graph->map_node.end()) {

            CNode2* n = p->second;

            if (i == num) {
                point = CoordToScreen(n->node.coord);
                PostMessage(WM_LBUTTONDOWN, (WPARAM)0, MAKELONG(point.x, point.y));
            }
            i++;

            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                if (IsBegin(l)) {

                    if (i == num) {
                        CNode2* n2 = other(l);

                        CFPoint pf;

                        pf.x = (n->node.coord.x + n2->node.coord.x) / 2;
                        pf.y = (n->node.coord.y + n2->node.coord.y) / 2;

                        point = CoordToScreen(pf);
                        //          ClientToScreen(&point);
                        PostMessage(WM_LBUTTONDOWN, (WPARAM)0, MAKELONG(point.x, point.y));
                    }
                    i++;
                }
            }
            p++;
        }

        num++;

        if (num > i) num = 0;

        //    find(pt);

        //    PostMessage(WM_LBUTTONDOWN,(WPARAM)0,MAKELONG(point.x, point.y));
        /*
        if (m_cxema->m_dbf->openTable("UT_OUT")) {
        try {
        COutDialog od(this, m_cxema->m_dbf, "UT_OUT", "UT_OUT", 2, 1);
        od.DoModal();
        }
        catch (...) {
        AfxMessageBox(_TR("Неизвестная ошибка"), MB_OK|MB_ICONINFORMATION);
        }
        }
        */
    }
    CScroll::OnTimer(nIDEvent);
}

void CGidrView::moveGeo(const char* tn, int ID)
{
    CFPoint pt;
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int n = kls->geo4.size();

        for (int i = 0; i < n; i++) {
            if (kls->geo4[i]->nom == ID) {
                if (isLine(kls->loc)) {
                    double x1, y1, x2, y2;
                    kls->geo4[i]->cl.napr(x1, y1, x2, y2);
                    pt.x = (x1 + x2) / 2;
                    pt.y = (y1 + y2) / 2;
                }
                else {
                    pt = kls->geo4[i]->cl.centroid();
                }
                moveXY(pt);
                break;
            }
        }
    }
}


void CGidrView::editGeo(const char* tn, int ID)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int n = kls->geo4.size();

        for (int i = 0; i < n; i++) {
            if (kls->geo4[i]->nom == ID) {
                GeoInfo(tn, ID, false);
                break;
            }
        }
    }
}

LRESULT CGidrView::OnFindSelGeoObject(WPARAM wParam, LPARAM lParam)
{
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
    if (pMainFrame) pMainFrame->MDINext();
    return 0;
}

LRESULT CGidrView::OnFindZh(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT CGidrView::OnFindZd(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CGidrView::moveNode(CNode2* n)
{
    if (n->node.internalNodeID != m_parent_id) {
        if (m_parent_id) KamObr();
        if (n->node.internalNodeID) {
            CNode2* nn = m_cxema->m_graph->find(n->node.internalNodeID);
            if (nn) moveXY(CFPoint(nn->node.coord));

            Kam(n->node.internalNodeID);
        }
    }

    moveXYmas(CFPoint(n->node.coord));
    BeginDrawNode(n);
}


LRESULT CGidrView::OnFindNode(WPARAM wParam, LPARAM lParam)
{
    long id = (long)wParam;
    CNode2* n = m_cxema->m_graph->find(id);
    if (n) {
        if (IsRezhim(n)) {
            if (AfxMessageBox("Данный узел не отображен на схеме. Включить вспомогательные участки?", MB_YESNO) == IDYES) {
                m_bIsRezhim = FALSE;
                moveNode(n);
                InvalidateG();
            }
        }
        else {
            moveNode(n);
        }
    }

    return 0;
}


LRESULT CGidrView::OnRefresh(WPARAM wParam, LPARAM lParam)
{
    Invalidate();

    return 0;
}


CLINE2* CGidrView::moveLine(long id)
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (l && IsBegin(l) && (bline(l)->line.nomP == id || bline(l)->line.nomO == id)) {
                    if (n->node.internalNodeID != m_parent_id) {
                        if (m_parent_id) KamObr();
                        if (n->node.internalNodeID) {
                            CNode2* nn = m_cxema->m_graph->find(n->node.internalNodeID);
                            if (nn) moveXY(CFPoint(nn->node.coord));
                            Kam(n->node.internalNodeID);
                        }
                    }
                    moveXY(centroid(l));
                    BeginDrawLine(l);
                    return l;
                }
            }
        }
        p++;
    }
    return nullptr;
}


CLINE2* CGidrView::moveLineBig(long id)
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (l && IsBegin(l) && (bline(l)->line.pipeSectionID == id)) {
                    if (n->node.internalNodeID != m_parent_id) {
                        if (m_parent_id) KamObr();
                        if (n->node.internalNodeID) {
                            CNode2* nn = m_cxema->m_graph->find(n->node.internalNodeID);
                            if (nn) moveXY(CFPoint(nn->node.coord));
                            Kam(n->node.internalNodeID);
                        }
                    }
                    moveXY(centroid(l));
                    //                  BeginDrawLine(l);
                    return l;
                }
            }
        }
        p++;
    }
    return nullptr;
}

/**

CLINE2* CGidrView::moveLineBig2(long id)
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (l && IsBegin(l) && (bline(l)->line.pipeSectionID == id)) {
                    if (n->node.internalNodeID != m_parent_id) {
                        if (m_parent_id) KamObr();
                        if (n->node.internalNodeID) {
                            CNode2* nn = m_cxema->m_graph->find(n->node.internalNodeID);
                            if (nn) move554XY(CFPoint(nn->node.coord));
                            Kam(n->node.internalNodeID);
                        }
                    }
                    moveXY(centroid(l));
                    //                  BeginDrawLine(l);
                    return l;
                }
            }
        }
        p++;
    }
    return nullptr;
}
*/


void no_fragment(CAdoFile *ado, int id)
{
    CString q;
    q.Format("SELECT n1.nodeName,n2.nodeName,fr.name FROM pipeSections ps1 JOIN nodes n1 ON n1.id=ps1.nodeID1 JOIN nodes n2 ON n2.id=ps1.nodeID2 JOIN fragments fr ON fr.id=n1.fileID WHERE ps1.id=%d", id);

    CString s;
    vector<CString> vv;
    if (readTableValues(ado, q, 3, vv)) {
        s.Format("Отобразить участок ПТС %s - %s невозможно, так как фрагмент %s не подключен. Подключите фрагмент командой: \"Файл>Отобразить дополнительную схему\"", vv[0], vv[1], vv[2]);
        //          s.Format("Не могу найти участок ПТС %s - %s\nОткройте фрагмент %s", vv[0], vv[1], vv[2]);
        AfxMessageBox(s);
    }
    else {
        s.Format("Не могу найти участок ПТС");
        AfxMessageBox(s);
    }
}


LRESULT CGidrView::OnFindLine(WPARAM wParam, LPARAM lParam)
{
    //    if (m_parent_id != 0) return 0;

    long id = (long)wParam;

    moveLine(id);

    return 0;
}


// Поиск участка ПТС по id heatPipeSection

LRESULT CGidrView::OnFindLineBig(WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = OnFindLine(wParam, lParam);

    long id = (long)wParam;

    CLINE2* l = moveLineBig(id);

    if (l) {
        int idbig = bline(l)->line.pipeSectionID;
        if (idbig > 0) {
            //          m_cxema->m_graph->set_pipeSectionID(idbig);
            //          MoveVyd();
            m_linePTS_ID = idbig;
            MoveLinePTS();
            Invalidate();
        }
    }
    else {
        no_fragment(m_cxema->m_ado, id);
    }
    return 0;
}


// Поиск участка ПТС по id через осмотр

LRESULT CGidrView::OnFindLineBigOsmotr(WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = OnFindLine(wParam, lParam);

    long id = (long)wParam;
    
    CString q;

    q.Format("SELECT hps.pipeSectionID,directionID FROM osmotrDeployed osd JOIN heatPipeSections hps ON hps.lineID=osd.lineID WHERE osd.id=%d", id);

    vector<CString> vv;
    bool r = readTableValues(m_cxema->m_ado, q, 2, vv);

    if (!r) return 0;
    
    id = atoi(vv[0]);
    int directionID = atoi(vv[1]);

    CLINE2* l = moveLineBig(id);

    if (l) {
        int idbig = bline(l)->line.pipeSectionID;
        if (idbig > 0) {
            m_linePTS_ID = idbig;

            bool setOsmotr(CAdoFile * ado, CString opres, CCxema * m_cxema, int directionID);

            setOpresNull();

            m_id_osmotr2 = directionID;
            m_id_osmotr2_old = directionID;;

            LastOpres("osmotr", _TR("Участок осмотра"), m_id_osmotr2, m_id_osmotr2_old, false);

            MoveLinePTS();
            Invalidate();
        }
    }
    else {
        no_fragment(m_cxema->m_ado, id);
    }
    return 0;
}



// Поиск участка ПТС по id pipeSection

LRESULT CGidrView::OnFindLineBig2(WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = OnFindLine(wParam, lParam);

    long id = (long)wParam;

    CLINE2* l = moveLineBig(id);

    if (l) {
        int idbig = bline(l)->line.pipeSectionID;
        if (idbig > 0) {
            //          m_cxema->m_graph->set_pipeSectionID(idbig);
            //          MoveVyd();
            m_linePTS_ID = idbig;
            MoveLinePTS();
            Invalidate();
        }
    }

    return 0;
}

LRESULT CGidrView::OnFindGeoTu(WPARAM wParam, LPARAM lParam)
{
    long id = (long)wParam;
//    CString fn = (LPCTSTR)lParam;
    CString fn = "zdaniya_tu";

//    CString q;

    Klassif* kls = m_geofile->m_kl_list->findKlN(fn);
    if (kls) {
//        q.Format("SELECT zdanie FROM tehnicheskie_usloviya WHERE id=%d", id);
//        id = readTableValueInt(kls->m_ado, q);
        
        CGeoObject* geo = kls->getGeoObjectById(id);

        if (geo) {
            geo->mark = true;

            if (kls->loc == LOC6) {
                MoveRect(CFRect(geo->cl.first(), geo->cl.first()));
            }
            else {
                MoveRect(geo->cl.GetFRect());
            }
        }
    }
    return 0;

}


LRESULT CGidrView::OnFindGeo(WPARAM wParam, LPARAM lParam)
{
    long id = (long)wParam;
    CString fn = (LPCTSTR)lParam;

    //  if (fn == "osmotr") {   ё
    if (fn.Find("Анализ осмотра контуров.") == 0) {
        m_id_osmotr2 = id;
        LastOpres("osmotr", _TR("Участок осмотра"), m_id_osmotr2, m_id_osmotr2_old, false);
        return 0;
    }

    if (fn == "remont2") {
        m_id_remont2 = id;
        LastOpres("remont2", _TR("Контуры ремонта"), m_id_remont2, m_id_remont2_old, false);
//        OnLastRemont2();
        return 0;
    }
    if (fn == "osmotr") {
        m_id_osmotr2 = id;
        LastOpres("osmotr", _TR("Контур осмотра"), m_id_osmotr2, m_id_osmotr2_old, false);
//        OnLastOsmotr2();
        return 0;
    }
    if (fn == "opres") {
        m_id_opr = id;
        LastOpres("opres", _TR("Контуры опрессовок"), m_id_opr, m_id_opr_old, false);
//        OnLastOpres();
        return 0;
    }

    Klassif* kls = m_geofile->m_kl_list->findKlN(fn);
    if (kls) {
        CGeoObject* geo = kls->getGeoObjectById(id);

/**
        if (!geo && fn == "defect") {
            CString q;
            q.Format("SELECT diagID FROM defectsForDiag WHERE defectID=%d", id);
            id = readTableValueInt(kls->m_ado, q);
            kls = m_geofile->m_kl_list->findKlN("diag");
            if (kls) {
                geo = kls->getGeoObjectById(id);
                kls->ins = 1;
            }
        }
*/
        if (geo) {
            geo->mark = true;

            if (kls->loc == LOC6) {
                MoveRect(CFRect(geo->cl.first(), geo->cl.first()));
            }
            else {
                MoveRect(geo->cl.GetFRect());
            }
        }
    }
    return 0;
}

CPropertyGrid* ado_info(CWnd* wnd, CAdoFile* ado, const char* tn, int id, const char* capt, TYP_OF_P typ_of_p, const char* tn_txt, bool autodelete);


void CGidrView::geoEdit(CWnd *wnd, CString tn, int id)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        ado_info(wnd, m_cxema->m_ado, tn, id, kls->RusName(), TP_GEO, NULL, false);
    }
}


LRESULT CGidrView::OnFindGeoEdit(WPARAM wParam, LPARAM lParam)
{
    long id = (long)wParam;
    CString tn = (LPCTSTR)lParam;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        ado_info(this, m_cxema->m_ado, tn, id, kls->RusName(), TP_GEO, NULL, false);
    }


/*
    Klassif* kls = m_geofile->m_kl_list->findKlN(fn);
    if (kls) {
        CGeoObject* geo = kls->getGeoObjectById(id);

        if (geo) {
            
            geo->mark = true;

            if (kls->loc == LOC6) {
                MoveRect(CFRect(geo->cl.first(), geo->cl.first()));
            }
            else {
                MoveRect(geo->cl.GetFRect());
            }
        }
    }
*/
    return 0;
}



LRESULT CGidrView::OnFindGeoBigLine(WPARAM wParam, LPARAM lParam)
{
    long id = (long)wParam;
    CString fn = (LPCTSTR)lParam;

    Klassif* kls = m_geofile->m_kl_list->findKlN(fn);
    if (kls) {
        CGeoObject* geo = kls->getGeoObjectById(id);

        if (geo) {
            geo->mark = true;

            if (isPoint(kls->loc)) {
                MoveRect(CFRect(geo->cl.first(), geo->cl.first()));

                CLINE2* line = m_cxema->m_graph->findLine(m_parent_id, geo->cl.first(), geom.masx * D5 / mas_otn);

                if (line) {
                    m_linePTS_ID = bline(line)->line.pipeSectionID;
                }

                Invalidate();
            }
            else {
                MoveRect(geo->cl.GetFRect());
            }
        }
    }
    return 0;
}


void CGidrView::OnColors()
{
    CLineStyle colors(this, isPassport());

    colors.m_color_uzel = m_color_uzel;

    if (colors.DoModal() == IDOK) {
        m_ring = colors.m_ring;
        m_color_uzel = colors.m_color_uzel;

        m_colors.save1();

        InvalidateG();
    }
}

void CGidrView::OnZapros()
{
    struct menuitem1 {
        char* name;
        int i;
    };

    menuitem1 menudoc[] = {
        "Объем сети", 1,
        "Длина теплопроводов", 2,
        //   "Теплопотребление общее", 3,
        //   "Теплопотребление в закрытых системах", 4,
        //   "Теплопотребление в открытых системах", 5,
        "Отключенные потребители", 6,
        "Длины теплопроводов по диаметрам", 7,
        NULL, 0
    };

    CMMenu menu(this);

    for (int i = 0; menudoc[i].name; i++) menu.lst.Add(menudoc[i].name);

    if (menu.DoModal() == IDOK) {
        switch (menudoc[menu.m_Index].i) {
        case 1:
            //      Zap1(m_cxema);
            break;
        }
    }
}


void CGidrView::OnZima()
{
    CString str;
    str.Format(_TR("Переключить все потребители на %s режим"), m_zima ? _TR("летний") : _TR("зимний"));

    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        m_zima = !m_zima;
        m_cxema->zima_leto(m_cxema->m_cur_file->id, m_zima);

        if (m_parent_id_old) {
            AfxMessageBox(_TR("Открыть заново"), MB_OK | MB_ICONINFORMATION);
            //      m_cxema->reopenCxema(m_cxema->m_cur_file, m_parent_kam);
            InvalidateG();
        }
    }
}

void CGidrView::OnUpdateZima(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_zima);
}

double getTn()
{
    return atof(AfxGetApp()->GetProfileString(szSection, "Tn", "-32"));
}

void CGidrView::OnChName()
{
    AfxMessageBox(_TR("Смена имен узлов, отменили"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnGeodz()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString text2 = "0";
    CInputDialog inD2(this, _TR("Ввод геодезических отметок узлов на одной горизонтали"), _TR("Введите геодезическую отметку"), text2);
    if (inD2.DoModal() == IDOK) {
        double g = atof(inD2.m_strInput);

        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все геодезические отметки в выбранной области будут установлены в %g\nВыполнить операцию?"), g);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkTopTube", (const char*)inD2.m_strInput, false);
            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkNodeArea", (const char*)inD2.m_strInput, false);

            CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                    if (n->node.isPjezo) {
                        n->node.geoMarkTopTube = n->node.geoMarkNodeArea = g;
                    }
                }
                p++;
            }
            GetDocument()->SetModifiedFlag(true);
        }
    }
    regim = R_INIT;
    m_nRegim = ID_INFO;
    InvalidateG();
}

// Трубы

void CGidrView::OnGeodz1()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString text2 = "0";
    CInputDialog inD2(this, _TR("Ввод геодезических отметок узлов на одной горизонтали"), _TR("Введите геодезическую отметку оси трубы, м"), text2);
    if (inD2.DoModal() == IDOK) {
        double g = atof(inD2.m_strInput);

        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все геодезические отметки оси трубы в выбранной области будут установлены в %g\nВыполнить операцию?"), g);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkTopTube", (const char*)inD2.m_strInput, false);
//            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkNodeArea", (const char*)inD2.m_strInput, false);

            CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                    if (n->node.isPjezo) {
//                        n->node.geoMarkTopTube = n->node.geoMarkNodeArea = g;
                        n->node.geoMarkTopTube = g;
                    }
                }
                p++;
            }
            GetDocument()->SetModifiedFlag(true);
        }
    }
    regim = R_INIT;
    m_nRegim = ID_INFO;
    InvalidateG();
}

// Земля

void CGidrView::OnGeodz2()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString text2 = "0";
    CInputDialog inD2(this, _TR("Ввод геодезических отметок узлов на одной горизонтали"), _TR("Введите геодезическую отметку поверхночти земли, м"), text2);
    if (inD2.DoModal() == IDOK) {
        double g = atof(inD2.m_strInput);

        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все геодезические отметки поверхности земли в выбранной области будут установлены в %g\nВыполнить операцию?"), g);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
//            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkTopTube", (const char*)inD2.m_strInput, false);
            setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "geoMarkNodeArea", (const char*)inD2.m_strInput, false);

            CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                    if (n->node.isPjezo) {
//                        n->node.geoMarkTopTube = n->node.geoMarkNodeArea = g;
                        n->node.geoMarkNodeArea = g;
                    }
                }
                p++;
            }
            GetDocument()->SetModifiedFlag(true);
        }
    }
    regim = R_INIT;
    m_nRegim = ID_INFO;
    InvalidateG();
}






void CGidrView::OnUpdateVyd(CCmdUI* pCmdUI)
{
    if (!isEditGid()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        if (p->second->node.isPjezo) {
            pCmdUI->Enable(TRUE);
            return;
        }
        p++;
    }
    pCmdUI->Enable(FALSE);
}


void CGidrView::OnUpdateVydRemont(CCmdUI* pCmdUI)
{
    if (!isRemont()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        if (p->second->node.isPjezo) {
            pCmdUI->Enable(TRUE);
            return;
        }
        p++;
    }
    pCmdUI->Enable(FALSE);
}




void CGidrView::OnUpdateGeodzInternet(CCmdUI* pCmdUI)
{
    if (!isEditGid()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        if (p->second->node.isPjezo) {
            pCmdUI->Enable(TRUE);
            return;
        }
        p++;
    }
    pCmdUI->Enable(FALSE);
}




void CGidrView::OnPodp()
{
    m_bIsPodp = !m_bIsPodp;

    if (m_bIsPodp) {
        m_bIsPTS = FALSE;
    }

    InvalidateG();
}


void CGidrView::OnPodpNiz()
{
    if (m_bIsPodpVerh) {
        AfxMessageBox(_TR("Наименование узлов Тепловой сети будут отключены"), MB_OK | MB_ICONINFORMATION);
    }

    m_bIsPodpNiz = !m_bIsPodpNiz;
    m_bIsPodpVerh = FALSE;

    InvalidateG();
}

void CGidrView::OnPodpVerh()
{
    if (m_bIsPodpNiz) {
        AfxMessageBox(_TR("Наименование узлов Гидравлики будут отключены"), MB_OK | MB_ICONINFORMATION);
    }

    m_bIsPodpVerh = !m_bIsPodpVerh;
    m_bIsPodpNiz = FALSE;

    InvalidateG();
}
void CGidrView::OnUpdatePodpNiz(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPodpNiz);
}
void CGidrView::OnUpdatePodpVerh(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPodpVerh);
}

void CGidrView::OnUpdatePodp(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPodp);
}

void CGidrView::OnRezhim()
{
    m_bIsRezhim = !m_bIsRezhim;

    if (m_idMenu == 4) {
        m_bIsRezhim = TRUE;
    }

    InvalidateG();
}


void CGidrView::OnUpdateRezhim(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(!m_bIsRezhim);
}


//void CGidrView::OnUpdatePodp2(CCmdUI* pCmdUI)
//{
//  pCmdUI->SetCheck(!m_bIsPodp);
//}


void CGidrView::OnPodpPO()
{
    m_bIsPTS = !m_bIsPTS;
    InvalidateG();
}

void CGidrView::OnUpdatePodpPO(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPTS);
}


void CGidrView::OnPodpRes()
{
    m_bIsPodpRes = !m_bIsPodpRes;
    if (m_bIsPodpRes) {
        m_bIsPodp = TRUE;
        m_bIsPTS = FALSE;
    }

    InvalidateG();
}

void CGidrView::OnUpdatePodpRes(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPodpRes);
}


void CGidrView::OnPribor()
{
    m_bIsPribor = !m_bIsPribor;

    InvalidateG();
}

void CGidrView::OnUpdatePribor(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPribor);
}

void CGidrView::OnFlag()
{
    m_bIsFlag = !m_bIsFlag;

    InvalidateG();
}

void CGidrView::OnUpdateFlag(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsFlag);
}

void CGidrView::OnUpdatePicture(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsPicture);
}

void CGidrView::OnPicture()
{
    m_bIsPicture = !m_bIsPicture;

    if (m_bIsPicture && !m_bIsOpenPicture) {
        openBmp(m_strPicture);
    }
    InvalidateG();
}

bool CGidrView::addGeo(Klassif* kls, bool init)
{
    if (kls) {
        if (!isEditGeo(kls->nazv, true)) {
/*
            CString str;
            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
            if (pDoc) {
                str.Format("%s не имеет прав на редактирование %s", pDoc->m_user_gid, kls->RusName());
                AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
            }
*/
            return false;
        }

        if (init) initGeoValue();
        kls->ins = 1;
        m_isGeobaza = TRUE;
        m_kls = kls;
        regim = R_INIT;
        m_nRegim = ID_INFO;
        SetCur(m_nRegim);
        m_nRegim = ID_NEW_GEO;
    }

    return true;
}

bool CGidrView::addGeoTab(const char* fn)
{
    return true;
}

bool CGidrView::delGeoTab(Klassif* kls)
{
    /*
    if (!isEditGeo(kls->TableName())) return false;

    CString str;
    str.Format(_TR("Удалить %s из %s?"), kls->TableName(), kls->FileName());
    if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return false;
    if (MessageBox(str, _TR("Еще раз подтвердить удаление"), MB_YESNO) != IDYES) return false;

    if (m_geofile->delLayer(kls->FileName(), kls->TableName())) {
    if (pm_cSheet->m_left1) {
    m_geofile->initGeoTree(pm_cSheet->m_left1->m_ctlTree);
    }
    initLeft();
    }
    */
    return true;
}

void CGidrView::OnNewUch2()
{
    AfxMessageBox(_TR("Старые дефекты, удалили"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnUpdateNewUch2(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_nRegim == pCmdUI->m_nID);
    pCmdUI->Enable(isEditGid());
}

void CGidrView::OnExportShape()
{
    //  CFileDialog fd(FALSE, _T("SHP"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    //    _T("Shape-файлы|*.shp|"), this);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString fn = "";  //  GetTitle(pDoc->m_strMDB_real);

    CString strDir = "";
    SelDirDial(this, strDir);

    if (strDir != "") {
        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
        ChDir cd(strDir);
        m_cxema->exportShape(pDoc->m_bd_gid);
    }
}

void CGidrView::OnPrPo()
{
    if (!isEditGid()) return;
    CString str = _TR("Внимание!\nПри выполнении этой операции все реальные потребители станут обобщенными%s\nВыполнить операцию?");

    str = _TR("ВНИМАНИЕ!!!  Выполнение команды может  уничтожить данные по реальным потребителям, если Вы ошибочно нажали кнопку ДА. В любом случае перед нажатием ДА необходимо сделать страховочную копию редактируемого файла.");

    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        str = _TR("Вам предлагается повторная попытка запустить команду преобразования потребителей. Проверьте наличие страховочной копии файла  после первого предупреждения. Если копия имеется, то запускайте команду на исполнение");

        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            //            m_cxema->PrPo();

            int n = 0, i = 0;
            CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* nn = p->second;
                if (nn->node.typ == TIP_PR) n++;
                p++;
            }

            Percent pc;

            p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* nn = p->second;
                if (nn->node.typ == TIP_PR) {
                    renameNode(nn, TIP_PO);
                    pc.SetValue(100. * i / n);
                    i++;
                }
                p++;
            }
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}

void CGidrView::OnTemp()
{
#if 0
    CString q;
    char s[256];

    int y = 2008, m1 = 10;

    q = "Select [begin_year] From [Система теплоснабжения]";

    CDbFile* dbf = m_cxema->m_cur_file->m_dbf;

    if (!dbf->openTable(q)) {
        return;
    }

    if (!dbf->rs->IsEOF()) {
        dbf->rs->MoveFirst();
    }

    dbf->read(0, s);

    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);

    dbf->closeTable();

    q.Format("Select * From [Температуры воздуха и грунта]"
        " Where ([year]=%d AND [month] >= %d) OR ([year]=%d AND [month] < %d) "
        " Order by [year], [month]"
        , y, m1, y + 1, m1);

    RunTable(_TR("Температуры воздуха и грунта"), q, -1, 1, FALSE, FALSE, FALSE);
#endif
}


int RunTableDialog(CWnd* wnd, CAdoFile* ado, const TCHAR* table, const TCHAR* query, const TCHAR* title);


void CGidrView::OnRasprMag()
{
    CString tn = "externalCodes";
    CString q;
    //  q.Format("SELECT id, name FROM %s WHERE fileID=%d AND removed = 0", tn, m_cxema->m_cur_file->id);

    q.Format(
        " SELECT top 2147483647"
        " ec.id, fr.name AS 'Фрагмент', ec.name AS 'Расчетная схема', hs.name AS 'Источник', ot.name AS 'Объект РС'"
        " FROM externalCodes ec"
        " JOIN fragments fr ON fr.id=ec.fileID"
        " LEFT JOIN heatSources hs ON hs.id=ec.heatSourceID"
        " LEFT JOIN objectTypes ot ON ot.id=ec.objectID"
        " WHERE ec.removed=0 AND ec.fileID IN (%s)"
        " ORDER BY fr.name, ec.name", m_cxema->m_par);


    int externalCodeID = RunTableDialog(this, m_cxema->m_ado, tn, q, _TR("Выберите Расчетную схему для выделения"));

    if (externalCodeID > 0) {
        m_cxema->m_graph->reset();

        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.id_kod == externalCodeID && n->node.internalNodeID == 0) {
                CLINE2* l;
                n->node.isPjezo = 1;
                for (l = n->lines; l; l = next(l)) {
                    bline(l)->line.isPjezo = 1;
//                    where(l)->node.isPjezo = 1;
//                    other(l)->node.isPjezo = 1;
                }
            }
            p++;
        }
        MoveVydIf();
        InvalidateG();
    }
}


/*
void CGidrView::OnRasprMag()
{
    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    CString tn = "externalCodes";
    CString q;
    q.Format("SELECT id, name FROM %s WHERE fileID=%d AND removed = 0", tn, m_cxema->m_cur_file->id);

    int externalCodeID = RunTableDialog(this, getAdo(getGidAdoName()), tn, q, _TR("Выберите Расчетную схему для выделения"));

    m_cxema->m_graph->reset();

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.id_kod == externalCodeID) {
            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                bline(l)->line.isPjezo = 1;
                where(l)->node.isPjezo = 1;
            }
        }
        p++;
    }
    MoveVydIf();
    InvalidateG();
}
*/

void CGidrView::OnRasprMagNew()   // Выделить РС красным цветом
{

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CString tn = "externalCodes";
    CString q;
    q.Format("SELECT id, name FROM %s WHERE fileID=%d AND removed = 0", tn, m_cxema->m_cur_file->id);

    int externalCodeID = RunTableDialog(this, m_cxema->m_ado, tn, q, _TR("Выберите Расчетную схему для выделения"));

    m_kod_vyd = externalCodeID;

    //     = externalCodeID;
//  AfxMessageBox(_TR("Временно удалено 505"), MB_OK|MB_ICONINFORMATION);

    InvalidateG();
}

void CGidrView::OnRasprMag2()   // Выделить объекты системы теплоснабжения
{

    CMMenu menu1(this, IDD_MENU, _TR("Выберите Объект системы теплоснабжения"));

    CAdoFile* ado = getAdo("vsp");

    if (!ado) return;

    CString q;

    q.Format("SELECT * FROM %s", transl("B10_Виды объектов расчетной схемы"));

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s = ado->readStr(2);
            long id = ado->read_long(1);
            ado->MoveNext();
            menu1.Add(s, (void*)id);
        }
        ado->closeTable();
    }

    if (menu1.DoModal() != IDOK) return;

    long id = (long)menu1.getV2();

    m_cxema->m_graph->reset();

    CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();

    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.coord.x != 0 && p->node.coord.x != 0) {
            map<long, RasCx>::const_iterator it = m_cxema->map_rs.find(p->node.id_kod);
            if (it != m_cxema->map_rs.end()) {
                RasCx cx = it->second;

                if (id == cx.objectID) {
                    CLINE2* l;
                    for (l = p->lines; l; l = next(l)) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                        other(l)->node.isPjezo = 1;
                    }
                }
            }
        }
        pp++;
    }
    MoveVydIf();
    InvalidateG();
}

void CGidrView::OnUpdateIsMas(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsMas);
}

void CGidrView::OnIsMas()
{
    m_bIsMas = !m_bIsMas;
    InvalidateG();
}

void CGidrView::OnMasPic()
{
    CString str;
    str.Format("%g", m_MasPic);

    //  CInputDialog inD(this, _TR("Коэффициент масштабирования"), _TR("Введите коэффициент масштабирования"), str);
    CInputDialog inD(this, _TR("Стандартный масштаб"), _TR("Введите стандартный масштаб"), str);
    if (inD.DoModal() == IDOK) {
        m_MasPic = atof(inD.m_strInput);
        if (m_MasPic < 10) m_MasPic = m_MasPic = 1500;
        InvalidateG();
    }
}

void CGidrView::OnMasPodp()
{
    CString str;
    str.Format("%g", m_MasPodp);

    CInputDialog inD(this, _TR("Масштаб откл.надписей"), _TR("Введите масштаб откл.надписей"), str);
    if (inD.DoModal() == IDOK) {
        m_MasPodp = atof(inD.m_strInput);
        if (m_MasPodp < 10) m_MasPodp = 5000;
        InvalidateG();
    }
}

void CGidrView::OnNaprGid()
{
    m_bIsNaprGid = !m_bIsNaprGid;
    InvalidateG();
}

void CGidrView::OnUpdateNaprGid(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsNaprGid);
}

void CGidrView::OnAlma()
{
    m_bIsAlma = !m_bIsAlma;
    InvalidateG();
}

void CGidrView::OnUpdateAlma(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bIsAlma);
}

#if 0

void CGidrView::OnIstSety()     // Выделить источник
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите Источник для выделения его участков"));

    CAdoFile* ado = getAdo(getGidAdoName());

    CString q;
    q.Format("SELECT * FROM heatSources");

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            CString s = ado->readStr("sourceName");
            menu.Add(s, (void*)id);

            ado->MoveNext();
        }
        ado->closeTable();
    }


    if (menu.DoModal() == IDOK) {
        CString q;

        long id = (long)menu.getV2();

        m_cxema->m_graph->reset();

        CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();

        while (pp != m_cxema->m_graph->map_node.end()) {
            CNode2* p = pp->second;
            if (p->node.coord.x != 0 && p->node.coord.x != 0) {
                map <long, long>::const_iterator it;

                it = m_cxema->map_ist.find(p->node.id_kod);

                if (it != m_cxema->map_ist.end() && it->second == id) {
                    CLINE2* l;
                    for (l = p->lines; l; l = next(l)) {
                        CNode2* n2 = other(l);
                        it = m_cxema->map_ist.find(n2->node.id_kod);
                        if (it != m_cxema->map_ist.end() && it->second == id) {
                            bline(l)->line.isPjezo = 1;
                            where(l)->node.isPjezo = 1;
                            other(l)->node.isPjezo = 1;
                        }
                    }
                }
            }
            pp++;
        }
        InvalidateG();
    }
}



void CGidrView::OnIstSety()     // Выделить источник
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите Источник для выделения его участков"));

    CAdoFile* ado = getAdo(getGidAdoName());

    CString q;
    q.Format("SELECT id, naimenovanie AS sourceName FROM %s.istochniki_tepla", getPsAdoName());

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            CString s = ado->readStr("sourceName");
            menu.Add(s, (void*)id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        long id = (long)menu.getV2();

        CString q;

        q.Format("SELECT l.id FROM linesobj l "
            "JOIN heatPipeSections hps ON hps.lineID=l.id "
            "LEFT JOIN ps_almaty.magistrali mag ON mag.id=hps.magistral "
            "LEFT JOIN ps_almaty.uchastok_rs rs ON rs.id=hps.distSite "
            "LEFT JOIN ps_almaty.magistrali mag2 ON mag2.id=rs.magistral "
            "LEFT JOIN ps_almaty.istochniki_tepla ist ON ist.id=mag.istochnik_tepla OR ist.id=mag2.istochnik_tepla "
            "WHERE ist.id=%d", id);

        set<int> set_id;
        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                int id = ado->read_long("id");
                set_id.insert(id);
                ado->MoveNext();
            }
            ado->closeTable();
            m_cxema->m_graph->vyd(set_id);
        }

        InvalidateG();
    }
}
#endif

void CGidrView::OnBalans()
{
    //    CString tn = "Organizatsii_vladeltsy";

    CString q;
    //    q.Format("SELECT id,naimenovanie FROM %s", tn);

    CString tn = "organizations";
    q.Format("SELECT id,name FROM %s", tn);

    CAdoFile* ado = m_cxema->m_ado;

    int ID = RunTableDialog(this, ado, tn, q, _TR("Выберите Балансовую принадлежность для выделения"));

    m_cxema->m_graph->reset();

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.internalNodeID == 0) {
            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                if (bline(l)->line.owner == ID) {
                    bline(l)->line.isPjezo = 1;
                    where(l)->node.isPjezo = 1;
                }
            }
        }
        p++;
    }
    MoveVydIf();
    InvalidateG();
}


void CGidrView::OnIstSety()     // Выделить источник
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите Источник для выделения его участков"));

    CAdoFile* ado = m_cxema->m_ado;

    CString q;
    q.Format("SELECT * FROM heatSources ORDER BY name");

    q.Format(
        "SELECT  "
        "hs.id, "
        "fr.name AS fr_name, "
        "hs.name AS hs_name, "
        "n.externalNodeName AS n_name "
        "FROM heatSources hs "
        "JOIN nodes n ON n.id=hs.nodeID "
        "JOIN externalCodes ec ON ec.id=n.externalCodeID "
        "JOIN fragments fr ON fr.id=n.fileID "
        " WHERE fr.id IN (%s)"

        "ORDER BY fr.name, hs.name,n.externalNodeName ", m_cxema->m_par);



    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            //            CString s = ado->readStr("sourceName");
            //            CString s = ado->readStr("name");

            CString s;

            CString s1 = ado->readStr("fr_name");
            CString s2 = ado->readStr("hs_name");
            CString s3 = ado->readStr("n_name");

            if (s2 != "") {
                s.Format("%s (%s)", s2, s1);
            }
            else {
                s.Format("%s (%s)", s3, s1);
            }

            menu.Add(s, (void*)id);

            ado->MoveNext();
        }
        ado->closeTable();
    }


    if (menu.DoModal() == IDOK) {
        CString q;

        long id = (long)menu.getV2();

        m_cxema->m_graph->reset();

        CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();

        while (pp != m_cxema->m_graph->map_node.end()) {
            CNode2* p = pp->second;
            if (p->node.coord.x != 0 && p->node.coord.x != 0 && p->node.internalNodeID == 0) {
                //                map <long, long>::const_iterator it;
                //                it = m_cxema->map_ist.find(p->node.id_kod);

                int kod_id = -1;

                map<long, RasCx>::const_iterator it = m_cxema->map_rs.find(p->node.id_kod);
                if (it != m_cxema->map_rs.end()) {
                    RasCx cx = it->second;
                    if (cx.objectID == 2) {
                        map<long, RasCx>::const_iterator it = m_cxema->map_rs.find(cx.belongMagistral);
                        if (it != m_cxema->map_rs.end()) {
                            RasCx cx = it->second;
                            kod_id = cx.heatSourceID;
                        }
                    }

                    if (kod_id == -1 /*cx.objectID == 1 || cx.objectID == 8*/) {
                        kod_id = cx.heatSourceID;
                    }
                }

                if (id == kod_id) {
                    CLINE2* l;
                    for (l = p->lines; l; l = next(l)) {
                        bline(l)->line.isPjezo = 1;
                        where(l)->node.isPjezo = 1;
                        other(l)->node.isPjezo = 1;
                    }
                }
            }
            pp++;
        }
        MoveVydIf();

        InvalidateG();
    }
}



void CGidrView::OnClose()
{
    log1("CGidrView::OnClose");

//    m_pMapsThread

    //    if (m_ask_close) if (AfxMessageBox("Закрыть базу данных?", MB_YESNO) != IDYES) return;



        /*
            CMainFrame *mf = (CMainFrame *) AfxGetMainWnd();
            mf->setMainBar(1);

            if (m_pMapsThread) {
                m_pMapsThread->Exit();
                while (!m_pMapsThread->isOff());
                m_pMapsThread = NULL;
            }

        */
    CScroll::OnClose();

}

void CGidrView::OnOtklonenia()
{
    try {
        excel(AfxGetApp()->GetProfileString(szSection, "path_excel", ""));
    }
    catch (...) {
    }
}


void CGidrView::savePjezo()
{
    int fileID = 0;

    deque<long>::const_iterator it = m_cxema->m_graph->list_pjezo_min.begin();
    for (; it != m_cxema->m_graph->list_pjezo_min.end(); it++) {
        long id = *it;
        CNode2* node = m_cxema->m_graph->find(id);
        if (node) {
            fileID = node->node.fileID;
            break;
        }
    }

    if (fileID == 0) return;


    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;
    CString text = "";
    CInputDialog inD(this, _TR("Название направления"), _TR("Введите название направления"), text);
    if (inD.DoModal() == IDOK) {

        ado->AddNew("directions");
        ado->write("name", inD.m_strInput);
        ado->write("fileID", (long)fileID);
        long directionID = ado->Update();


        CString qq;
        bool first = true;

        q.Format("INSERT INTO deployedDirections (directionID,nodeID) VALUES ");

        deque<long>::const_iterator it = m_cxema->m_graph->list_pjezo_min.begin();
        for (; it != m_cxema->m_graph->list_pjezo_min.end(); it++) {
            long id = *it;
            //          ado->AddNew("deployedDirections");
            //          ado->write("directionID", directionID);
            //          ado->write("nodeID", id);
            //          ado->Update();
            if (!first) q += ",";  first = false;
            qq.Format("(%d,%d)", directionID, id);
            q += qq;
        }

        if (!first) {
            long affected;
            long n = ado->ExecuteInsert(q, &affected);
        }


    }
}


void CGidrView::OnSavePjezo()
{
    savePjezo();
}

void CGidrView::OnUpdateSavePjezo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_cxema->m_graph->list_pjezo_min.empty());
}

void CGidrView::OnListPjezo()
{
    CMMenu menu(this, IDD_MENU7, _TR("Выберите направление"));


    //  if (!curFile()) {
    //      AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
    //      return;
    //  }


    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    CString q;
    //  q.Format("Select id, name From dirctions WHERE fileID=%d", m_cxema->m_cur_file);
    q.Format("Select id, name From directions WHERE fileID in (%s)", m_cxema->m_par);


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            CString name = ado->readStr("name");
            menu.Add(name, (void*)id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.lst.GetSize() <= 0) {
        AfxMessageBox(_TR("Нет сохраненных направлений"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    int ret = menu.DoModal();

    if (ret == IDC_DEL) {
        CString q;
        if (menu.m_Index >= 0) {
            q.Format(_TR("Удалить направление %s?"), menu.m_strText);
            if (AfxMessageBox(q) == IDOK) {
                q.Format("DELETE FROM directions WHERE id=%d", menu.getV());
                long affected;
                ado->Execute(q, &affected);
            }
        }
    }
    else if (ret == IDOK) {
        long directionID = (long)menu.getV();

        if (directionID >= 0) {
            CString q;

            q.Format("Select nodeID From deployedDirections Where directionID = %d", directionID);

            m_napr1 = NULL;
            m_cxema->m_graph->reset();
            if (ado->openTable0(q)) {
                while (!ado->isEOF()) {
                    long id = ado->read_long("nodeID");

                    CNode2* node = m_cxema->m_graph->find(id);
                    if (node) {
                        if (m_napr1 != NULL) {
                            m_cxema->m_graph->link(m_napr1, node);
                            InvalidateG();
                        }
                        m_napr1 = node;
                        node->node.isPjezo = true;
                        CDC* m_dc;
                        if ((m_dc = BeginPaint()) != NULL) {
                            drawNode0(m_dc, node, false);
                            EndPaint(m_dc);
                        }
                    }

                    ado->MoveNext();
                }
                ado->closeTable();
            }
            //      OnPjezo();
        }
    }

}

void CGidrView::OnTrio()
{
    AfxMessageBox(_TR("Временно удалено 506"), MB_OK | MB_ICONINFORMATION);
}

//void CGidrView::OnSaveOut() {}

#include "MenuExcel.h"

void CGidrView::OnExcel3()
{
    ChDir cd(argpath() + "Excel2");

    m_cxema->Excel2List("Excel2", "OUT_Тепловые потери.lst2");
}


void CGidrView::OnExcel4()
{
    ChDir cd(argpath() + "Excel2");
    m_cxema->Excel2List("Excel2", "OUT_Тепловые потери фактические.lst2");
}

void vydMsRsMag(CGraph2* graph, int m_ms, int m_rs, int m_mag);

void CGidrView::OnExcel2()
{
    CFileFind finder;
    BOOL bWorking;
    ChDir cd(argpath() + "Excel2");

    CMenuExcel menu(this, _TR("Выберите таблицу"), m_cxema);

    bWorking = finder.FindFile(_T("*.lst"));
    while (bWorking) {
        bWorking = finder.FindNextFile();
        menu.lst.Add(finder.GetFileTitle());
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет данных"), MB_OK | MB_ICONINFORMATION);
    }
    else if (menu.DoModal() == IDOK) {
        CAdoFile* ado = m_cxema->m_ado;
        //        m_cxema->create_vyd_table(ado);

        CString slst = menu.m_strText + ".lst";
        m_cxema->m_graph->save_pjezo();
        vydMsRsMag(m_cxema->m_graph, m_ms, m_rs, m_mag);

        m_cxema->Excel2List("Excel2", slst);

        m_cxema->m_graph->restore_pjezo();
    }
}


bool CGidrView::mark_vyd()
{
    bool ret = false;
    if (m_mark_regim == ID_MARK_GEO) {
        ret = m_geofile->setMark(m_NP);
    }
    else if (m_mark_regim == ID_MARK_DEL) {
        ret = m_cxema->m_graph->setMark(m_NP, false);
    }
    else {
        ret = m_cxema->m_graph->setMark(m_NP, true);
    }
    m_NP.clear();
    InvalidateG();
    return ret;
}

#include "ColorLine.h"
#include "ColorLine1.h"

void CGidrView::OnColorLine()
{
    m_cxema->m_graph->reset();

    CAdoFile* ado = m_cxema->m_ado;

    CColorLine utd(AfxGetMainWnd(), m_cxema->m_ado, "", "");

    int ret = utd.DoModal();

    if (ret == IDOK) {
        CString q;


        if (utd.m_out) {
            q.Format("SELECT id, externalSignLineID, %s FROM UT_OUT", utd.m_fn);

            q.Format(
                "SELECT id, externalSignLineID, %s FROM UT_OUT"
                " JOIN"
                " ("
                "     SELECT"
                "     c.fileID,"
                "     max(c.id) AS cid"
                "     FROM CALCULATION c"
                "     LEFT JOIN fragments fr ON fr.id = c.fileID"
                "     GROUP BY c.fileID"
                " ) calc ON UT_OUT.calculationID = calc.cid"

                " WHERE fileID IN (%s)", utd.m_fn, m_cxema->m_par);


            //          int nr = getCountTable2(ado, q);
            //          CString sss;
            //          sss.Format("r = %d", nr);

            m_cxema->m_graph->init_find_line_nom_out(TIP_UT);

            if (ado->openTable0(q)) {
                while (!ado->isEOF()) {
                    int id = ado->read_long("id");
                    int po = ado->read_long("externalSignLineID");
                    CString txt = ado->readStr(utd.m_fn);

                    double d = atof(txt);

                    CLINE2* ll = m_cxema->m_graph->find_line_nom_out(id);
                    if (ll) {
                        CLine2* l = bline(ll);

                        if (!strcmp(utd.m_fn, "a13")) d = fabs(d);

                        if (po == 2) {
                            if (d <= utd.m_dd[0]) l->line.colorP = utd.m_color[0];
                            else if (d <= utd.m_dd[1]) l->line.colorP = utd.m_color[1];
                            else l->line.colorP = utd.m_color[2];
                        }
                        else {
                            if (d <= utd.m_dd[3]) l->line.colorO = utd.m_color[3];
                            else if (d <= utd.m_dd[4]) l->line.colorO = utd.m_color[4];
                            else l->line.colorO = utd.m_color[5];
                        }
                    }
                    ado->MoveNext();
                }
            }
            ado->closeTable();
        }
        else {

            q.Format(
                "SELECT l.id, l.externalSignLineID, hps.%s FROM heatPipeSections hps\n"
                "join linesobj l on l.id = hps.lineID\n"
                "join nodes n on n.id = l.nodeID1\n"
                " WHERE n.fileID IN (%s)", utd.m_fn, m_cxema->m_par);

            m_cxema->m_graph->init_find_line_nom();

            if (ado->openTable0(q)) {
                while (!ado->isEOF()) {
                    int id = ado->read_long("id");
                    int po = ado->read_long("externalSignLineID");
                    CString txt = ado->readStr(utd.m_fn);

                    double d = atof(txt);

                    CLINE2* ll = m_cxema->m_graph->find_line_nom(id);
                    if (ll) {
                        CLine2* l = bline(ll);

                        if (po == 1 || po == 2 || po == 4) {
                            if (d <= utd.m_dd[0]) l->line.colorP = utd.m_color[0];
                            else if (d <= utd.m_dd[1]) l->line.colorP = utd.m_color[1];
                            else l->line.colorP = utd.m_color[2];
                        }
                        if (po == 1 || po == 3 || po == 5) {
                            if (d <= utd.m_dd[3]) l->line.colorO = utd.m_color[3];
                            else if (d <= utd.m_dd[4]) l->line.colorO = utd.m_color[4];
                            else l->line.colorO = utd.m_color[5];
                        }
                    }
                    ado->MoveNext();
                }
            }
            ado->closeTable();
        }
    }

    InvalidateG();
}


void CGidrView::OnColorLine1()
{
    m_cxema->m_graph->reset();

    CAdoFile* ado = m_cxema->m_ado;

    CColorLine1 utd(AfxGetMainWnd(), m_cxema->m_ado, "", "");

    int ret = utd.DoModal();

    if (ret == IDOK) {
        CString q;

        q.Format(
            "SELECT id, %s FROM PT_OUT"
            " JOIN"
            " ("
            "     SELECT"
            "     c.fileID,"
            "     max(c.id) AS cid"
            "     FROM CALCULATION c"
            "     LEFT JOIN fragments fr ON fr.id = c.fileID"
            "     GROUP BY c.fileID"
            " ) calc ON PT_OUT.calculationID = calc.cid"

            " WHERE fileID IN (%s)", utd.m_fn, m_cxema->m_par);


        m_cxema->m_graph->init_find_node_nom_pt_out();


        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                int id = ado->read_long("id");
                CString txt = ado->readStr(utd.m_fn);

                double d = atof(txt);

                CNode2* n = m_cxema->m_graph->find_pt_out(id);

                if (n) {
                    if (d <= utd.m_dd[0]) n->node.colorP = utd.m_color[0];
                    else if (d <= utd.m_dd[1]) n->node.colorP = utd.m_color[1];
                    else n->node.colorP = utd.m_color[2];
                }
                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    InvalidateG();
}

bool CGidrView::setNodeColorById(int id, int color) {
    if (!m_cxema->m_graph) return false;

    if (CNode2* n = m_cxema->m_graph->find(id)) {
        n->node.colorP = color;
        n->node.isMarkAnalyze = true;
        return true;
    }

    return false;
}

void CGidrView::applyNodeColor(const std::vector<int>& ids, int color) {
    if (ids.empty()) return;
    
    for (int id : ids) {
        setNodeColorById(id, color);
    }
    InvalidateG();
}

void CGidrView::applyNodeColors(const std::unordered_map<int, int>& idToColor) {
    if (idToColor.empty()) return;

    for (const auto& kv : idToColor) {
        setNodeColorById(kv.first, kv.second);
    }
    InvalidateG();
}


void CGidrView::OnZam()
{
    ChDir cd(argpath() + "Excel2");

    CAdoFile* ado = m_cxema->m_ado;

    m_cxema->create_vyd_table(ado);
    m_cxema->Excel2List("Excel2", "OUT_Время остывания.lst2");
}


bool CGidrView::MoveFragm(double dx, double dy)
{
    if (!isEditGid()) return false;

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            if (m_NP.find3(n->node.coord)) {
                n->node.coord.x += dx;
                n->node.coord.y += dy;

                setXY(n, n->node.coord);

                for (CLINE2* l = n->lines; l; l = next(l)) {
                    if (l && IsBegin(l) && bline(l)->line.m_NP.size() > 0) {
                        bline(l)->line.m_NP.move(dx, dy);
                        setCoord(l, bline(l)->line.m_NP);
                    }
                }
            }
        }
        p++;
    }

    /*
        CString par;
        par.Format("%d", m_n2->node.fileID);
        CWaitCursor wait;

        CAdoFile* ado = getAdo(getPsAdoName());

        if (ado) {
            set<int>  v;
            v.insert(m_n2->node.fileID);
            m_cxema->m_graph->clearFile(v);
            m_cxema->openGid(ado, par);
            Invalidate();
        }
    */

    return true;
}

void CGidrView::OnMainCxema()
{
    CMMenu menu(this, IDD_MENU8, _TR("Выберите файл активной расчетной схемы"));

    map<long, CGidFile*>::iterator it = m_cxema->m_file_map.begin();
    for (; it != m_cxema->m_file_map.end(); it++) {
        menu.Add(it->second->name, (void*)it->second);
    }

    if (menu.DoModal() == IDOK) {
        CGidFile* gf = (CGidFile*)menu.getV();

        if (gf) {
            setActiv(gf->id);
        }
    }
}

CString getPar(set<int>& v);

void CGidrView::OnAddDopFile()
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    CAdoFile* ado = m_cxema->m_ado;

    set<int> v, v0, v2, v2d;
    CString par;

    map<long, CGidFile*>::iterator it = m_cxema->m_file_map.begin();
    for (; it != m_cxema->m_file_map.end(); it++) {
        v0.insert(it->first);
    }

    if (m_cxema->initGidFiles(ado, v)) {

        set<int>::const_iterator it1, it2;

        it1 = v.begin();
        for (; it1 != v.end(); it1++) {
            int id = *it1;
            it2 = v0.find(id);
            if (it2 == v0.end()) {
                v2.insert(id);
            }
        }

        it2 = v0.begin();
        for (; it2 != v0.end(); it2++) {
            int id = *it2;
            it1 = v.find(id);
            if (it1 == v.end()) {
                v2d.insert(id);
            }
        }

        if (v2d.size() > 0) {
            m_cxema->m_par = getPar(v2d);
            m_cxema->m_graph->clearFile(v2d);
        }

        if (v2.size() > 0) {
            m_cxema->m_par = getPar(v2);
            CWaitCursor wait;
            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

            m_cxema->openGid(ado, m_cxema->m_par, pDoc->m_user_gid);
        }

        m_cxema->initGidTree();

        CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
        SetBorder(rect);

        Invalidate();
    }
}

void CGidrView::OnEmptyTemplate()
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();


    pDoc->m_view_color_col = -1;
    //    pDoc->m_view_color_col = 28;

    pDoc->m_view_head = "My Browser Table";

    pDoc->m_view_is_edit = true;
    pDoc->m_view_is_add = true;
    pDoc->m_view_is_del = true;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    CWebViewTable* pView;


    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        pView = (CWebViewTable*)pDoc->GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CWebViewTable))) {
            CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
            pFrame->MDIActivate(pView->GetParent());

        }
    }


    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE_NEW);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
}

void CGidrView::OnC3Add()
{
    AfxMessageBox(_TR("Временно удалено 507"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnC3Del()
{
    AfxMessageBox(_TR("Временно удалено 508"), MB_OK | MB_ICONINFORMATION);
}

bool CGidrView::findIS(const char* name)
{
    map <CString, CNode2*>::const_iterator it = m_cxema->map_ist2.find(CString(name));

    if (it != m_cxema->map_ist2.end()) {
        const CNode2* n = it->second;

        if (n) {
            moveXY(n->node.coord);
        }
    }
    return false;
}

void setRSGr(CCxema* cxema, const char* kod, int ch)
{
    CGraph2* graph = cxema->m_graph;

    CMapGraph::iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (!strcmp(n->node.node_name.kod, kod)) {
            n->node.isHide = (ch == 1);

            n->node.isPjezo = (ch != 0);
            for (CLINE2* ll = n->lines; ll; ll = next(ll)) {

                bline(ll)->line.isPjezo = (ch == 1);
                //        bline(ll)->isHide = (ch == 1);
            }
        }
        p++;
    }
}

bool CGidrView::setRS(const char* kod, int ch)
{
    setRSGr(m_cxema, kod, ch);
    InvalidateG();
    return true;
}

CFRect GetLineRect(CFPoint pt1, CFPoint pt2, const CCoordList& cl);


void conv_obr(CGraph2* m_graph)
{
    CMapGraph::iterator p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.y != 0) {
            n->node.coord = conv_obr(n->node.coord);
        }
        p++;
    }

    p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (IsBegin(l)) {
                    bline(l)->line.m_NP.conv_obr();
                    bline(l)->line.rect = GetLineRect(where(l)->node.coord, other(l)->node.coord, bline(l)->line.m_NP);
                }
            }
        }
        p++;
    }

}

void conv(CGraph2* m_graph)
{
    CMapGraph::iterator p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.y != 0) {
            n->node.coord = conv(n->node.coord);
        }
        p++;
    }

    p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (IsBegin(l)) {
                    bline(l)->line.m_NP.conv();
                    bline(l)->line.rect = GetLineRect(where(l)->node.coord, other(l)->node.coord, bline(l)->line.m_NP);
                }
            }
        }
        p++;
    }
}


void CGidrView::set_coord_gid(double x0, double y0, double alpha, bool convert)
{
    if (alpha == 0) {
        if (convert) {
            conv_obr(m_cxema->m_graph);
        }
        set_coord(x0, y0, alpha);
    }
    else {
        set_coord(x0, y0, alpha);
        if (convert) {
            conv(m_cxema->m_graph);
        }
    }

    InvalidateG();
}


void CGidrView::OnPovorot2()
{
    if (geom.m_alpha != 0) {
        regim = R_INIT;
        m_nRegim = ID_INFO;
        geom.m_alpha = 0;
        set_coord_gid(0, 0, 0);
        InvalidateG();
    }
}

void CGidrView::OnUpdatePovorot2(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(geom.m_alpha != 0);
}


struct Nagr
{
    Nagr() : o(0), g(0), v(0) {};
    double o, g, v;
};


void CGidrView::OnResetGeo()
{


    //  if (m_geofile) m_geofile->reset_geo();
//  InvalidateG();
    Nagr nagr;
    double Gmax = -1;

    CNode2* n_max = nullptr;


    for (auto& p : m_cxema->m_graph->map_node) {
        CNode2* node = p.second;
        if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {
            nagr.o += node->node.Qot;
            nagr.g += node->node.Qgvs;
            nagr.v += node->node.Qvent;
        }

        double G = 0;
        for (CLINE2* l = node->lines; l; l = next(l)) {
            G += bline(l)->line.pod.q * napr(l);
        }
        G = fabs(G);
        if (G > Gmax) {
            Gmax = G;
            n_max = node;

            G = 0;
            for (CLINE2* l = node->lines; l; l = next(l)) {
                G += bline(l)->line.pod.q * napr(l);
            }
        }
    }


    CString str;

    double Q = nagr.o + nagr.v + nagr.g;

    str.Format("%s\nGmax=%g Q=%g o=%g v=%g g=%g", n_max->getName(), Gmax, Q, nagr.o, nagr.v, nagr.g);
    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);

}


// Пересчет G в Q для Михаила из Алматы

double CGidrView::getQG()
{


    //  if (m_geofile) m_geofile->reset_geo();
//  InvalidateG();
    Nagr nagr;
    double Gmax = -1;

    CNode2* n_max = nullptr;


    for (auto& p : m_cxema->m_graph->map_node) {
        CNode2* node = p.second;
        if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {
            double GG = 0;
            for (CLINE2* l = node->lines; l; l = next(l)) {
                GG += bline(l)->line.pod.q * napr(l);
            }
            if (fabs(GG) > 0.0001) {
                nagr.o += node->node.Qot;
                nagr.g += node->node.Qgvs;
                nagr.v += node->node.Qvent;
            }
        }

        double G = 0;
        for (CLINE2* l = node->lines; l; l = next(l)) {
            G += bline(l)->line.pod.q * napr(l);
        }
        G = fabs(G);
        if (G > Gmax) {
            Gmax = G;
            n_max = node;

            G = 0;
            for (CLINE2* l = node->lines; l; l = next(l)) {
                G += bline(l)->line.pod.q * napr(l);
            }
        }
    }


    CString str;

    double Q = nagr.o + nagr.v + nagr.g;

    //    str.Format("%s\nGmax=%g Q=%g o=%g v=%g g=%g", n_max->getName(), Gmax, Q, nagr.o, nagr.v, nagr.g);
    //  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    return Q / Gmax;
}




#include "OperDate.h"
#include "LoginDialog.h"

void CGidrView::OnIsprLast()
{
    CAdoFile* ado = m_cxema->m_ado;
    CString q = readQ(ado, "history\\history", "", -1);
    CString tn = "";
    
    RunTable("", q, -1, virt_data_unknown, "", "История изменений");


#if 0
    COperDate menu(this);
    CString  q;
    CString all = _TR("Все пользователи");
    CString tn = ST_UT;

    menu.m_date = CTime::GetCurrentTime();
    menu.Add(all, (void*)-1);

    map <CString, CUser>::const_iterator it;

    for (it = list_user.begin(); it != list_user.end(); it++) {
        menu.Add(it->first, (void*)it->second.id);
    }


    for (int i = 0; menudoc[i].name; i++)
    {
        if (!strcmp(menudoc[i].name, ST_UT)) {
            menu.m_Index = i;
        }
        menu.lst_t.Add(menudoc[i].name);
    }

    int ret = menu.DoModal();


    if (ret == IDOK) {

        CTime t = menu.m_date;
        CString q, str;

        tn = menu.m_strTextTable;


        q.Format("SELECT * FROM linesobj WHERE archiveChangeDate >= '%04d%02d%02d'", t.GetYear(), t.GetMonth(), t.GetDay());

        if (menu.m_strText != all) {
            str.Format(" AND operatorID=%d", (int)menu.getV());
            q += str;
        }

        //      AfxMessageBox(strQuery, MB_OK|MB_ICONINFORMATION);
        CAdoFile* ado = m_cxema->m_ado;


        set<int> set_id;

        CString qid;

        qid.Format("SELECT id FROM (%s\n) __T", q);

        if (ado->openTable0(qid)) {
            while (!ado->isEOF()) {
                int id = ado->read_long("id");
                set_id.insert(id);
                ado->MoveNext();
            }
            ado->closeTable();

            CString tn = "linesobj";

            m_cxema->m_graph->vyd_lines(set_id);
            Invalidate();
            //          RunTable(tn, q, -1, virt_data_line);
            //          viewTable2(getGidAdoName(), linef[i - nodefsize].code, linef[i - nodefsize].table, virt_data_line, false);

            viewTable2(m_cxema->m_ado->m_schema, "UT", tn, ST_UT, virt_data_line, true);
        }

        //      Query(tn, strQuery);
    }



    /*
        if (ret == IDOK) {
            CTime t = menu.m_date;
            CString q, str;

            tn = menu.m_strTextTable;


            q.Format("SELECT * FROM linesobj WHERE archiveChangeDate >= '%04d%02d%02d'", t.GetYear(), t.GetMonth(), t.GetDay());

            if (menu.m_strText != all) {
                str.Format(" AND operatorID=%d", (int)menu.getV());
                q += str;
            }

            //      AfxMessageBox(strQuery, MB_OK|MB_ICONINFORMATION);
            CAdoFile* ado = m_cxema->m_ado;


            set<int> set_id;

            CString qid;

            qid.Format("SELECT id FROM (%s\n) __T", q);

            if (ado->openTable0(qid)) {
                while (!ado->isEOF()) {
                    int id = ado->read_long("id");
                    set_id.insert(id);
                    ado->MoveNext();
                }
                ado->closeTable();

                CString tn = "linesobj";

                m_cxema->m_graph->vyd(set_id);
                RunTable(tn, q, -1, virt_data_line);
            }

            //      Query(tn, strQuery);
        }
    */
#endif
}

BOOL CGidrView::isReadOnly()
{
    if (geom.m_alpha) return TRUE;
    return m_cxema->isReadOnly() || (m_idMenu == TYP_MENU_READONLY);
};


BOOL CGidrView::isEditGid()
{
    if (geom.m_alpha) return FALSE;
    return !m_cxema->isReadOnly() && (m_idMenu != TYP_MENU_READONLY) && isEditMain();
};

BOOL CGidrView::isEditPS()
{
    if (geom.m_alpha) return FALSE;
    return !m_cxema->isReadOnly() && m_idMenu == 1 || !m_cxema->isReadOnly() && m_idMenu == 4;
};

BOOL isRemont(const char* tn)
{
    if (
/*
        !strcmp(tn, STR_REMONT_DEFECT) ||
        !strcmp(tn, STR_REMONT_DEFECT2) ||
        !strcmp(tn, STR_REMONT_SHURF) ||
        !strcmp(tn, STR_REMONT_SHURF2) ||
        !strcmp(tn, STR_REMONT_OSMOTR) ||
        //  !strcmp(tn, STR_REMONT_KORROZ  ) ||
        !strcmp(tn, STR_REMONT_ISPYT) ||
        !strcmp(tn, STR_REMONT_KAPITAL) ||
        !strcmp(tn, STR_REMONT_CONTROL_TU) ||

        !strcmp(tn, "diag") ||
        !strcmp(tn, "remont") ||
        !strcmp(tn, "opressovka_sektsioniruyuschaya_zadvizhka") ||
        !strcmp(tn, "podpis") ||
*/

        !strcmp(tn, "defect") ||
        !strcmp(tn, "opres")  ||
        !strcmp(tn, "shurfy") ||
        !strcmp(tn, "remont2") ||
        !strcmp(tn, "osmotr")
        ) {
        return TRUE;
    }
    return FALSE;
}

BOOL isAkt(const char* tn0)
{
    CString tn = tn0;
    tn.MakeLower();

    return tn == "act";
}

BOOL isEditGeo(const char* tn, bool isMessage)
{
    CString str;
    CGidrView* pView = getView();

    if (!pView) return FALSE;
    Cgid6Doc* pDoc = (Cgid6Doc*)pView->GetDocument();
    if (!pDoc) return FALSE;
/*
    if (!pView->isEditGid()) {
        if (isMessage) {
            str.Format("Редактирование в данном режиме невозможно");
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return FALSE;
    }
*/

    if (pView->isReadOnly()) {
        if (isMessage) {
            str.Format("Редактирование в данном режиме невозможно");
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return FALSE;
    }

    if (isAkt(tn)) {
        if (isAkt()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str.Format("%s не имеет прав на редактирование Актов раздела", pDoc->m_user_gid);
                AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
            }
            return FALSE;
        }
    }

    if (!strcmp(tn, "indikator_korrozii")) {
        if (isIndicator()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str.Format("%s не имеет прав на редактирование Индикаторов коррозии", pDoc->m_user_gid);
                AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
            }
            return FALSE;
        }
    }



    bool is_pts(const CString &tn);

    

/**
    bool is_pts = false;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        CAdoFile* ado = pView->m_cxema->m_ado;
        kls->m_ado, ado)) {
            is_pts = true;
        }
    }
*/

    if (is_pts(tn)) {
        if (isProiz()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str.Format("%s не имеет прав на редактирование объектов ПТС", pDoc->m_user_gid);
                AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
            }
            return FALSE;
        }
    }

    if (isRemont(tn)) {
        if (isRemont()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str.Format("%s не имеет прав на редактирование объектов Ремонтов", pDoc->m_user_gid);
                AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
            }
            return FALSE;
        }
    }

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        if (isGeo()) {
            return TRUE;
        }
        else {
            if (isMessage) {
                str.Format("%s не имеет прав на редактирование объектов Геобазы", pDoc->m_user_gid);
                AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
            }
            return FALSE;
        }
    }

    return TRUE;
};

//#include "UserList.h"

/*
void CGidrView::OnUsers()
{
Cgid6App* pApp = (Cgid6App*) AfxGetApp();

//  if (pApp->isAdmin() || pApp->isRegim() && getMDBright(m_cxema->m_dbf)) {
if (pApp->isAdmin()) {
CUserList dlg(m_cxema->m_cur_file->m_dbf, this);
dlg.DoModal();
//    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
}
else {
AfxMessageBox(_TR("Вы не имеете прав редактировать этот файл!"), MB_OK|MB_ICONINFORMATION);
}
}
*/

BOOL CGidrView::isAdminFile()
{
    if (m_cxema) return m_cxema->isAdminFile;

    return TRUE;
}

CString getPodpis(int isMain);


void CGidrView::SetWindowText2()
{
    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    Cgid6Doc* pDoc = GetDocument();

    CString capt = pDoc->m_bd_gid;
    CString s;

    if (m_sezon_korrozia > 0 && 1
        /*
                (
                mf->isToolbar(ID_VIEW_TOOLBAR_KORROZIA) ||
                mf->isToolbar(ID_VIEW_TOOLBAR2_REMONT) ||
                mf->isToolbar(ID_VIEW_TOOLBAR_CONTROL_TU) ||
                mf->isToolbar(ID_VIEW_TOOLBAR_OPRESSOVKA) ||
                mf->isToolbar(ID_VIEW_TOOLBAR_OSMOTR) ||
                mf->isToolbar(ID_TOOLBAR_REMONT2_DEFECT)
                )
        */

        ) {
        s.Format(" [%d-%d отопительный сезон]", m_sezon_korrozia, m_sezon_korrozia + 1);
    }
    else {
        s = " [Все сезоны]";
    }

    capt += s;

    GetParentFrame()->SetWindowText(capt);

    CString ss, name;
    name.LoadString(IDR_MAINFRAME);

    s.Format("%s          Пользователь: %s", name, pDoc->m_user_gid);

    mf->SetTitle(s);
}

void CGidrView::InvalidateG(BOOL bErase)
{
    if (m_geofile) {
        m_geofile->setExit();
    }
    Invalidate(bErase);
}

void CGidrView::OnAktCreate()
{
    if (!isEditGeo("act")) return;

    Klassif* kls = m_geofile->m_kl_list->findKlN("Act");

    if (kls) {
        addGeo(kls);
    }
}

void CGidrView::OnUpdateAktCreate(CCmdUI* pCmdUI)
{
    //  pCmdUI->SetCheck(m_isGeobaza);

    if (!m_geofile || !m_geofile->m_kl_list || !IsMain() || !m_geofile->isOpen()) {
        pCmdUI->Enable(FALSE);
        pCmdUI->SetCheck(FALSE);
    }
    else {
        Klassif* kls = NULL;
        if (m_geofile && m_geofile->m_kl_list) {
            kls = m_geofile->m_kl_list->findKlN("Act");
        }
        pCmdUI->Enable(kls != NULL && kls->ins);
        pCmdUI->SetCheck(m_nRegim == ID_NEW_GEO && kls && m_kls && kls == m_kls);
    }
}

void CGidrView::OnPassportTpCreate()
{
    if (!isEditGeo("")) return;

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_PASPORT_OBJ));

    if (kls) {
        addGeo(kls);
    }
}


void CGidrView::OnUpdatePassportTpCreate(CCmdUI* pCmdUI)
{
    if (!m_geofile || !m_geofile->m_kl_list || !IsMain() || !m_geofile->isOpen()) {
        pCmdUI->Enable(FALSE);
        pCmdUI->SetCheck(FALSE);
    }
    else {
        Klassif* kls = NULL;
        if (m_geofile && m_geofile->m_kl_list) {
            kls = m_geofile->m_kl_list->findKlN(transl(STR_PASPORT_OBJ));
        }
        pCmdUI->Enable(kls != NULL && kls->ins);
        pCmdUI->SetCheck(m_nRegim == ID_NEW_GEO && kls && m_kls && kls == m_kls);
    }
}

void CGidrView::deleteLeftAll()
{
#ifdef COOLDEF
    set <CString> set_db;
    CString str;
    int i;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    int nGeo = AfxGetApp()->GetProfileInt(NameForProfile(pDoc->GetPathName()), _T("NDop"), 0);

    CTreeCtrl* m_ctlTree = NULL;

    if (&pm_cSheet->m_left2) {
        m_ctlTree = &pm_cSheet->m_left2->m_ctlTree;
    }

    if (m_ctlTree && m_ctlTree->m_hWnd) {
        for (i = 0; i < nGeo; i++) {
            str.Format(_T("Dop %d"), i);
            str = AfxGetApp()->GetProfileString(NameForProfile(pDoc->GetPathName()), str, "");

            CString fn = GetTitle(str);

            HTREEITEM i2 = FindChild(m_ctlTree, 0, fn);

            if (i2) {
                m_ctlTree->DeleteItem(i2);
            }
        }
        CString fn = GetTitle(pDoc->GetPathName());

        if (m_ctlTree->m_hWnd) {
            HTREEITEM i2 = FindChild(m_ctlTree, 0, fn);

            if (i2) {
                m_ctlTree->DeleteItem(i2);
            }
        }
    }
#endif
}

void CGidrView::initLeft()
{
    return;
#ifdef COOLDIAL  
    //  pm_cSheet->AddAllPages();

    if (pm_cSheet->m_left1) {
        pm_cSheet->m_left1->SetKls(m_geofile->m_kl_list);
        m_geofile->initGeoTree(pm_cSheet->m_left1->m_ctlTree);
    }
#endif

    setAddr();

    m_cxema->initGidTree();
}

double CGidrView::mas1()
{
    //  if (IsMain() && m_cxema->isGeobaza()) return 1;

    return 1;
}

double CGidrView::mas2()
{
    return 1e12;
    return 1e8;

    if (is_google_map) return 1e8;

    return 250000;
}

int copyRow(CAdoFile* ado, CString tn, int id, const CCoordList& cl, int loc)
{

    CString s;
    int i, n = ado->NColsMySQL_S(ado->m_schema, tn);

    CString params1 = "";
    CString params2 = "";

    for (i = 0; i < n; i++) {
        MySQLColumns* col = ado->findMySQLColumnsOrdS(ado->m_schema, tn, i + 1);
        if (col) {

            if (col->strCOLUMN_NAME != "id") {
                if (params1 != "") params1 += ",";
                if (params2 != "") params2 += ",";

                params1 += col->strCOLUMN_NAME;

                if (col->strCOLUMN_NAME == "shape") {
                    s.Format("%s", cl.getWKT(loc, ado->m_type_of_net));
                    params2 += s;
                }
                else {
                    params2 += col->strCOLUMN_NAME;
                }
            }
        }
    }

    CString q;

    long affected;

    q.Format("insert into %s (%s) select %s from %s where id = %d", tn, params1, params2, tn, id);
    int id2 = ado->ExecuteInsert(q, &affected);
    id2 = getLastID(ado, tn);

    return id2;
}


void CGidrView::copyAkt(int n_akt, CFPoint point)
{
    m_nRegim = ID_INFO;
    regim = R_INIT;
    m_nRegim = ID_INFO;

    CString tn = "Act";

    if (!isEditGeo(tn)) return;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (kls) {
        m_NP.push_back(point);
        int n = copyRow(kls->m_ado, tn, n_akt, m_NP, kls->loc);

        CGeoObject* g = new CGeoObject(n, m_NP);
        kls->geo4.push_back(g);
        m_NP.clear();
    }

    InvalidateG();
}

void CGidrView::FindFileList(const char* fn)
{
    CMMenu menu(this);
    char s[256];

    FILE* f = fopenexe(fn, "r");
    if (f) {
        while (1) {
            fgetstr(s, 255, f);
            if (feof(f)) break;
            s[34] = 0;
            menu.lst.Add(s);
        }
        fclose(f);

        if (menu.DoModal() == IDOK) {

            f = fopenexe(fn, "r");
            if (f) {
                while (1) {
                    fgetstr(s, 255, f);
                    if (feof(f)) break;
                    if (!strncmp(s, menu.m_strText, 34)) {
                        double x, y;
                        //            MessageBox(&s[35]);
                        sscanf(&s[34], "%lg %lg", &x, &y);
                        moveXY(CFPoint(lon2x(x) / 0.0000001, lat2y(-y) / 0.0000001));
                    }
                }
                fclose(f);
            }
        }
    }
}

bool CGidrView::addCity(const CFPoint& pt)
{
    CString str;
    //  str.Format("%g %g", pt.x, -pt.y);
    //  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

    CInputDialog inD(this, "", _TR("Введите название"), str);
    if (inD.DoModal() == IDOK) {
        FILE* f = fopenexe("city.dat", "a");
        //    double lat = pt.y/180.*M_PI;
        //    double B = (atan(exp(lat))*360/M_PI - 90);

        double H = x2lon(pt.x * 0.0000001);
        double B = y2lat(-pt.y * 0.0000001);
        fprintf(f, "%-34s %.4f %.4f\n", (const char*)inD.m_strInput, H, B);
        fclose(f);
    }

    return true;
}

void CGidrView::OnMapCity()
{
    FindFileList("city.dat");
}

void CGidrView::OnInvert()
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        n->node.isPjezo = !n->node.isPjezo;

        for (CLINE2* ll = n->lines; ll; ll = next(ll)) {
            if (IsBegin(ll)) {
                bline(ll)->line.isPjezo = !bline(ll)->line.isPjezo;
            }
        }
        p++;
    }

    Invalidate();
}

//#include "MagUzel.h"

bool getVydRect(CGraph2* graph, CFRect& rect)
{
    CMapGraph::const_iterator p = graph->map_node.begin();

    double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.x != 0 && n->node.internalNodeID == 0) {
            if (n->node.isPjezo) {
                x1 = min(x1, n->node.coord.x);
                y1 = min(y1, n->node.coord.y);
                x2 = max(x2, n->node.coord.x);
                y2 = max(y2, n->node.coord.y);


                for (CLINE2* l = n->lines; l; l = next(l)) {
                    if (bline(l)->line.isPjezo) {
                        CCoordList::const_iterator it;
                        it = bline(l)->line.m_NP.begin();
                        for (; it != bline(l)->line.m_NP.end(); ++it) {
                            x1 = min(x1, it->x);
                            y1 = min(y1, it->y);
                            x2 = max(x2, it->x);
                            y2 = max(y2, it->y);
                        }
                    }
                }
            }
        }
        p++;
    }
    if (x1 != 1e10) {
        rect = CFRect(x1, y1, x2, y2);
        return true;
    }

    return false;
}

bool getLinePTSRect(CGraph2* graph, CFRect& rect, int m_linePTS_ID)
{
    CMapGraph::const_iterator p = graph->map_node.begin();

    double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 && n->node.coord.x != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (bline(l)->line.pipeSectionID == m_linePTS_ID) {


                    x1 = min(x1, where(l)->node.coord.x);
                    y1 = min(y1, where(l)->node.coord.y);
                    x2 = max(x2, where(l)->node.coord.x);
                    y2 = max(y2, where(l)->node.coord.y);

                    x1 = min(x1, other(l)->node.coord.x);
                    y1 = min(y1, other(l)->node.coord.y);
                    x2 = max(x2, other(l)->node.coord.x);
                    y2 = max(y2, other(l)->node.coord.y);


                    CCoordList::const_iterator it;
                    it = bline(l)->line.m_NP.begin();
                    for (; it != bline(l)->line.m_NP.end(); ++it) {
                        x1 = min(x1, it->x);
                        y1 = min(y1, it->y);
                        x2 = max(x2, it->x);
                        y2 = max(y2, it->y);
                    }
                }
            }
        }
        p++;
    }
    if (x1 != 1e10) {
        rect = CFRect(x1, y1, x2, y2);
        return true;
    }

    return false;
}



bool getVydRectAll(CGraph2* graph, CFRect& rect0)
{
    CFRect rect;
    CFRect r(1e10, 1e10, -1e10, -1e10);

    if (getVydRect(graph, rect)) {
        r = rect;
    }
    rect0 = r;
    return r.left != 1e10;
}

bool getVydRectMSRS(CGraph2* graph, CFRect& rect, int mag_typ, int mag_id)
{
    CMapGraph::const_iterator p = graph->map_node.begin();

    double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.x != 0) {

            CNode2* n = p->second;

            for (CLINE2* l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);
                if (ll->line.isPjezo) {
                    CNode2* n1 = where(l);
                    CNode2* n2 = other(l);
                    x1 = min(x1, n1->node.coord.x);
                    y1 = min(y1, n1->node.coord.y);
                    x2 = max(x2, n1->node.coord.x);
                    y2 = max(y2, n1->node.coord.y);
                    x1 = min(x1, n2->node.coord.x);
                    y1 = min(y1, n2->node.coord.y);
                    x2 = max(x2, n2->node.coord.x);
                    y2 = max(y2, n2->node.coord.y);

                    CCoordList::const_iterator it;
                    it = bline(l)->line.m_NP.begin();

                    for (; it != bline(l)->line.m_NP.end(); ++it) {
                        x1 = min(x1, it->x);
                        y1 = min(y1, it->y);
                        x2 = max(x2, it->x);
                        y2 = max(y2, it->y);
                    }
                }
            }
        }
        p++;
    }
    if (x1 != 1e10) {
        rect = CFRect(x1, y1, x2, y2);
        return true;
    }

    return false;
}

bool getVydRectAllMSRS(CGraph2* graph, CFRect& rect0, int mag_typ, int mag_id)
{
    CFRect rect;
    CFRect r(1e10, 1e10, -1e10, -1e10);

    if (getVydRectMSRS(graph, rect, mag_typ, mag_id)) {
        r = rect;
    }
    rect0 = r;
    return r.left != 1e10;
}

bool CGidrView::MoveVyd()
{
    CFRect rect;

    if (getVydRect(m_cxema->m_graph, rect)) {
        MoveRect(rect);
        return true;
    }
    return false;
}

bool CGidrView::MoveLinePTS()
{
    CFRect rect;

    if (m_linePTS_ID > 0) {
        if (getLinePTSRect(m_cxema->m_graph, rect, m_linePTS_ID)) {
            MoveRect(rect);
            return true;
        }
    }
    return false;
}



bool CGidrView::MoveVydIf()
{
    CFRect rect;

    if (!m_cxema->m_graph->isMark()) {
        AfxMessageBox("Не найдено", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    if (getVydRect(m_cxema->m_graph, rect)) {
        MoveRect(rect);
        return true;
    }
    return false;
}


bool CGidrView::MoveMSRS(int mag_typ, int mag_id)
{
    CFRect rect;

    if (getVydRectAllMSRS(m_cxema->m_graph, rect, mag_typ, mag_id)) {
        MoveRect(rect);
        return true;
    }
    return false;
}


CString read_db(const char* mdb, const char* tn, const char* q, int nom, const char* fn);


void CGidrView::beginMS1(int id, const char* rn)
{
    m_nRegim = ID_MS_UZEL1;

    m_Magistral = rn;
    m_MagistralID = id;
}

void CGidrView::beginMS2(int id, const char* rn)
{
    m_nRegim = ID_MS_UZEL2;

    m_Magistral = rn;
    m_MagistralID = id;
}


void CGidrView::beginRas(int id, const char* rn)
{
    m_nRegim = ID_RAS_UZEL;
    m_ras_node.clear();

    m_Raspr = rn;
    m_RasprID = id;
}

void CGidrView::beginRas2(int id, const char* rn)
{
    m_nRegim = ID_RAS_UZEL2;
    m_ras_node.clear();

    m_Raspr = rn;
    m_RasprID = id;

}

void CGidrView::beginRasAll(int id, const char* rn)
{
    /*

    m_nRegim = ID_RAS_UZEL_ALL;
    m_ras_node.clear();

    m_Raspr = rn;
    m_RasprID = id;
    */
}

bool CGidrView::setMag(int typ_mag, int id, const char* name, int move)
{
    m_Magistral1 = name;
    m_MagistralID1 = id;
    m_MagistralTyp = typ_mag;

    m_mag = typ_mag == 1 ? id : 0;
    m_ms = typ_mag == 2 ? id : 0;
    m_rs = typ_mag == 3 ? id : 0;

    m_mag_old = m_mag;
    m_ms_old = m_ms;
    m_rs_old = m_rs;

    bool ret = true;

    if (move) {
        ret = MoveMSRS(typ_mag, id);
    }
    Status(3, m_Magistral1);

    return ret;
}

void CGidrView::OnProt()
{
    CAdoFile* ado = m_cxema->m_ado;

    m_cxema->create_vyd_table(ado);
    m_cxema->Excel2List("Excel2", "OUT_Протяженность трубопроводов.lst");
}


struct QUE {
    CString q;
    CString tab;
    CString mdb;
};

static map <int, QUE> map_q;

void initPS()
{
    FILE* f = fopenexe("tabs\\PS.txt", "r");

    int n = 0;

    if (f) {
        map_q.clear();

        CString tn;
        char s[1024];
        while (!feof(f)) {
            QUE que;
            fgetstr(s, 1023, f);
            tn = que.tab = s;
            fgetstr(s, 1023, f);
            que.mdb = s;

            if (que.mdb == "MAP") {
                fgetstr(s, 1023, f);
                que.tab = s;
            }

            fgetstr(s, 1023, f);
            que.q = s;

            map_q[n] = que;
            n++;
        }

        fclose(f);
    }
}


void CGidrView::OnSprav1(UINT nID)
{
    //  if (nID != ID_NOMAP && !isInternet) {
    int n = nID - ID_SPRAV_1;

    if (n < 0) return;

    initPS();

    map <int, QUE>::const_iterator it = map_q.find(n);

    if (it == map_q.end()) return;

    QUE que = it->second;

    CString q = que.q, mdb = que.mdb;

    if (q == "") {
        q.Format("SELECT * FROM [%s]", que.tab);
    }
#if 0
    if (mdb == "MapSprav") {
        mdb = getMapSprav();
    }
    if (mdb == "PS") {
        mdb = getPS();
    }
    if (mdb == "PS2") {
        mdb = argpath() + "PS2.mdb";
    }

#endif
    TypMenu idMenu = m_idMenu;

    m_idMenu = TYP_MENU_REZHIM;

    if (mdb == "MAP") {
        Klassif* kls = m_geofile->m_kl_list->findKlN(que.tab);

        if (kls) {
            RunTableMap(kls->m_database, que.tab, q, -1);
        }
    }
    else if (mdb == "MAIN") {
        RunTable(que.tab, q, -1, virt_data_edit_add_del, "", "");
    }
    else {
        RunTableDB(mdb, que.tab, q, -1, virt_data_edit_add_del, "", "");
    }
    m_idMenu = idMenu;
}

void CGidrView::OnOwner()
{
    //  RunTable(getPS(), _TR("Владельцы"), "SELECT * FROM [Владельцы]", NULL, -1, 0, FALSE, TRUE, TRUE, FALSE);
}

void CGidrView::OnCheckPO()
{
    CString tn = "Zdaniya";

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) {
        //    AfxMessageBox(_TR("Не подключен файл Договорных нагрузок"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CMapGraph::const_iterator p;
    p = m_cxema->m_graph->map_node.begin();

    FILE* f = fopen("E:\\Alma-2015-08-13\\yandex\\al\\1\\qqq.txt", "w");
    if (!f) return;

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;

        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            if (n->node.typ == TIP_PO) {
                int ng = m_geofile->findXY3(n->node.coord, 10, 3, tn);
                if (ng == 0) {
                    ng = m_geofile->findXY3(n->node.coord, 1000, 2, tn);
                }

                if (ng > 0) {
                    fprintf(f, "%d %d\ng", n->node.nomP, ng);
                }
                else {
                    fprintf(f, "%d\ng", n->node.nomP);
                }
            }
        }
        p++;
    }

    fclose(f);

}


void CGidrView::OnNagrZd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Договорных нагрузок"), MB_OK | MB_ICONINFORMATION);
        return;
    }


    RunTableMap(kls->m_database, _TR("Жилье"), NULL, 0);

}

void CGidrView::OnNagrOrg()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Договорных нагрузок"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    RunTableMap(kls->m_database, _TR("Организации"), NULL, -1);
}


void CGidrView::OnNagrZdNeiz()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Договорных нагрузок"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CString q;

    q =
        "SELECT ID, Адрес, [Площадь дома], [Нагрузка Отопление], [Нагрузка ГВС], [Административный район], [Признак наличия ОПУ], [Эксплуатационный район] "
        "FROM Жилье WHERE Здание = 0 OR Здание IS NULL";


    RunTableMap(kls->m_database, _TR("Ненайденные здания"), q, -1);


}

void CGidrView::OnNagrOrgNeiz()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Договорных нагрузок"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CString q;

    q =
        "SELECT ID, [Номер договора], [Наименование объекта], [Адрес объекта], [Назначение объекта], [Наименование контрагента], Площадь, Этажность, [Нагрузка \"Отопление\"], [Нагрузка \"Вентиляция\"], [Нагрузка \"ГВС\"], [Нагрузка \"Пар\"], [Номер(а) прибора учета], [Эксплуатационный район по объекту], [Эксплуатационный участок по объекту], [Административный район по объекту] "
        "FROM Организации WHERE  Здание = 0 OR Здание IS NULL";

    RunTableMap(kls->m_database, _TR("Ненайденные здания"), q, -1);
}



void CGidrView::OnProverka()
{
    //  CMenuProverka mp(this);
    //  mp.DoModal();
}


void CGidrView::OnKm()
{
    m_isKm = TRUE;
}

void CGidrView::OnMetr()
{
    m_isKm = FALSE;
}

void CGidrView::OnUpdateKm(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isKm);
}

void CGidrView::OnUpdateMetr(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(!m_isKm);
}


void viewZdan(CWnd* wnd, const char* tn1)
{
    CGidrView* view = (CGidrView*)wnd;

    if (!view->cur_node) return;

    double md = D5 * view->geom.masx;
    //  char tn[256];
    //  tn[0] = 0;

    int n = m_geofile->findXY3(view->cur_node->node.coord, md, 3, tn1);

    if (n) {
        view->GeoInfo(tn1, n, false);
    }
}


int getZdan(CWnd* wnd, char* tn1)
{
    CGidrView* view = (CGidrView*)wnd;

    if (!view || !view->cur_node) return 0;

    double md = D5 * view->geom.masx;

    return m_geofile->findXYmin(view->cur_node->node.coord, tn1);
}

void viewUzel(CWnd* wnd, bool readonly)
{
    CGidrView* view = (CGidrView*)wnd;

    view->viewCurGeoUzel(view, readonly);
}

void CGidrView::viewCurGeoUzel(CWnd* parent, bool readonly)
{
    viewGeoUzel(parent, cur_geo_tn, cur_geo_n, readonly);
}

CNode2* CGidrView::getGeoNode(const char* tn, int nom)
{
    AfxMessageBox(_TR("Временно удалено 509"), MB_OK | MB_ICONINFORMATION);
    return NULL;
}

void CGidrView::viewGeoUzel(CWnd* parent, const char* tn, int nom, bool readonly)
{
    AfxMessageBox(_TR("Временно удалено 510"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnMasAllAll()
{
    if (m_clExpl.size() > 0) {
        ChMas(m_clExpl.GetFRect());
    }
    else {
        CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
        ChMas(rect);
    }
    InvalidateG();
}

bool CGidrView::moveCurrent() {
    //main_window->setCurrent(this); !!! Нужно сделать перемещение на карту
    OnMasAllAll();
    return true;
}

void moveGeoId(CGidrView* gidr, const char* tn, int id)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (kls) {
        CGeoObject* geo = kls->getGeoObjectById(id);
        if (geo) {
            gidr->moveXYmas(geo->cl.centroid());
        }
    }
}

void CGidrView::setRS(int id)
{
    m_id_RS = id;
    m_id_MS = -1;
}

void CGidrView::setMS(int id)
{
    m_id_RS = -1;
    m_id_MS = id;
}

CFRect CGidrView::getMaxRect()
{
    CFRect r, rgeo, rgid = m_cxema->m_graph->getBorder(m_parent_id);

    r = rgid;

    if (m_parent_id == 0) {

        m_geofile->GetBorder(rgeo);

        if (rgeo.left != LONG_MAX) {
            r.UnionRect(&rgeo, &rgid);
        }
    }
    return r;
}

void savePNG(HDC& hdc, int nWidth, int nHeight, const char* fn);

const int DD = 512;
const double mas_coef = 1.5;

void setGidView(CGidrView* view);


void initMsRect(CGidrView* gidr_view, int id);
void initRsRect(CGidrView* gidr_view, int id);
int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);

CString num_MSRS(const char* s);


struct lessMSRS
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() (const CString& arg1, const CString& arg2) const
    {
        CString n1 = num_MSRS((const char*)arg1);
        CString n2 = num_MSRS((const char*)arg2);

        if (n1 == n2 && arg1 != arg2) {
            return arg1 < arg2;
        }

        return n1 < n2;
    }
};

void initMSRE(map<int, UMSRS>& mapMSRS);
void initMSRE2(map<int, UMSRS>& mapMSRS, map<CString, UMSRS, lessMSRS>& mapMS, int is_mag);


#if 0

void CGidrView::OnSavePNG()
{
    CString str;

    m_CurNode = NULL;
    m_CurLine = NULL;

    setGidView((CGidrView*)this);

    double x, y;
    int i, k, m;

    CWaitCursor cur;

    CClientDC oDC(this);

    CDC dc;
    dc.CreateCompatibleDC(&oDC);

    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&oDC, DD, DD);
    dc.SelectObject(bitmap);

    CFRect rectMax = getMaxRect();

    CString m_szTilesDir = AfxGetApp()->GetProfileString("WebServer2", "Tiles Dir", "");

    int j = 0;


    CString tileName(int dpi, int m, int r, int c, int internalNodeID, int layer, int gid, int geo, int ms, int rs);

    CGraph2* m_graph = m_cxema->m_graph;

    CMMenu menu(this, IDD_MENU2, _TR("Магистрали"));

    CGraph2* graph = m_cxema->m_graph;

    map<int, UMSRS> mapMSRS;
    map<CString, UMSRS, lessMSRS> mapMS;

    int ms_rs = 0;

    initMSRE(mapMSRS);
    initMSRE2(mapMSRS, mapMS, ms_rs);

    map<CString, UMSRS, lessMSRS>::const_iterator it = mapMS.begin();

    for (; it != mapMS.end(); it++) {
        UMSRS ms = it->second;

        graph->reset();

        if (ms_rs == 0) {
            viewUchastok(graph, make_tuple(ms.id, 0), 1, true);
        }
        else {
            viewUchastok(graph, make_tuple(0, ms.id), 2, true);
        }

        if (graph->isMark()) {
            menu.Add(ms.opis, (void*)ms.id);
        }
    }

    int id = 0;

    if (menu.DoModal() == IDOK) {
        id = (int)menu.getV();
    }

    double mas = 200000;


    //    int ms = 43;
    int ms = 0;
    int rs = 0;

    if (ms_rs == 0) {
        ms = id;
    }
    else {
        rs = id;
    }


    //    graph->save_pjezo();
    //    graph->reset();
    //    if (ms) {
    //      initMsRect(this, ms);
    //      vydMsN(graph, ms);
    //    }
    //    if (rs) {
    //      initRsRect(this, rs);
    //      vydRsN(graph, rs);
    //    }


    long dt1 = 0, dt0 = 0, dt2 = 0;

    clock_t t0_1 = clock();

    int nnn = 0;

    int mm = 12;

    if (rs > 0) mm = 17;
    if (ms > 0) mm = 14;

    if (ms) {
        initMsRect(this, ms);
    }
    if (rs) {
        initRsRect(this, rs);
    }


    int n_t = 0;
    mas = 200000;
    j = 0;

    for (m = 1; m <= mm; m++) {
        for (i = 0, y = rectMax.top; y < rectMax.bottom; y += mas * DD / geom.dmas, i++) {
            for (k = 0, x = rectMax.left; x < rectMax.right; x += mas * DD / geom.dmas, k++) {
                if (ms == 0 && rs == 0) {
                    n_t++;
                }
                else {
                    CString fn0 = tileName(m, i, k, 0, 1, 1, 0, 0);
                    CString fn = tileName(m, i, k, 0, 1, 1, ms, rs);

                    if (fn != fn0) {
                        n_t++;
                    }
                }
                j++;
            }
        }
        mas /= mas_coef;
    }

    //    str.Format("%d", n_t);
    //    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

    Percent2 pc;

    mas = 200000;
    j = 0;

    for (m = 1; m <= mm; m++) {
        for (i = 0, y = rectMax.top; y < rectMax.bottom; y += mas * DD / geom.dmas, i++) {
            for (k = 0, x = rectMax.left; x < rectMax.right; x += mas * DD / geom.dmas, k++) {
                CString fn0 = tileName(m, i, k, 0, 1, 1, 0, 0);
                CString fn = tileName(m, i, k, 0, 1, 1, ms, rs);

                if ((ms == 0 && rs == 0) || fn != fn0) {

                    if (pc.SetValue(nnn * 100 / n_t)) goto QUIT;
                    nnn++;

                    if (1 || !IsFile(fn)) {
                        if (ms || rs) {
                            clock_t t1_1 = clock();

                            graph->save_pjezo();
                            graph->reset();

                            if (ms) {
                                initMsRect(this, ms);
                                viewUchastok(graph, make_tuple(ms, 0), 1, true);
                            }
                            if (rs) {
                                initRsRect(this, rs);
                                viewUchastok(graph, make_tuple(0, rs), 2, true);
                            }
                            dt1 += (clock() - t1_1);
                        }

                        clock_t t2_1 = clock();

                        mkdir_for_file(fn);
                        CopyBlockPng2(dc, mas, x, y, DD, DD, fn, k, i, m);


                        dt2 += (clock() - t2_1);

                        if (ms || rs) {
                            graph->restore_pjezo();
                        }
                    }
                }
                j++;
            }
        }
        mas /= mas_coef;
    }

    dt0 = clock() - t0_1;

    str.Format("%g\n%g секунд\n%g секунд\n%g на одну", (double)dt0 / CLOCKS_PER_SEC, (double)dt1 / CLOCKS_PER_SEC, (double)dt2 / CLOCKS_PER_SEC, ((double)dt0) / nnn / CLOCKS_PER_SEC);
    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);


QUIT:
    return;

    //    graph->restore_pjezo();
}
#endif


/*
void CGidrView::OnExportMSQL()
{
CFileDialog fd(FALSE, _T("mdb"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
_T("mdb-файлы|*.mdb|"), this);

if (fd.DoModal() == IDOK) {
m_cxema->exportMSSQL(fd.GetFileName());
}
}
*/



void print_index1(FILE* f, const char* title);
void print_index2(FILE* f);
void print_index(FILE* f, const char* title, const char* fn);
void print_doc_end(FILE* f);
FILE* print_doc(FILE* fi, const char* fn, const char* title, void(*filtr) (CString& s1) = NULL);


void editD3() {}


void CGidrView::OnDialog3()
{
    editD3();
}

// Закрылась таблица

void open_opressovki();

void CGidrView::OnDestroy()
{
    log1("CGidrView::OnDestroy");

    if (m_pMapsThread) {
//        CEvent event2;
        m_pMapsThread->Exit();
        // 
//                   ::WaitForSingleObject(event2.m_hObject, INFINITE);
//
//        PostThreadMessage(m_pMapsThread->m_nThreadID, WM_QUIT, 0, 0);

//        ::WaitForSingleObject(m_pMapsThread->m_hThread, INFINITE);
//        ::WaitForSingleObject(&event2, INFINITE);
        ::WaitForSingleObject(m_pMapsThread->m_event, INFINITE);

        m_pMapsThread = nullptr;
    }
    
    //    CMainFrame *mf = (CMainFrame *) AfxGetMainWnd();
    //    if (mf) {
    //        mf->setMainBar(1);
    //    }


//    if (m_ask_close) if (AfxMessageBox("Закрыть базу данных?", MB_YESNO) != IDYES) return;


    CString str;
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    deleteLeftAll();

    if (m_bIsOpen) {
        m_colors.load1();
        //  m_colors.save();

        if (m_parent_id) {
            m_parent_id = 0;

            geom.masx = old_masx;
            geom.masy = old_masy;
            geom.m_bx = old_m_bx;
            geom.m_by = old_m_by;
        }

        str.Format("%lg", geom.m_by);
        AfxGetApp()->WriteProfileInt(szSection, "isFlag", m_bIsFlag);
        AfxGetApp()->WriteProfileInt(szSection, "idMenu", m_idMenu);
        AfxGetApp()->WriteProfileInt(szSection, "IsPodp", m_bIsPodp);
        AfxGetApp()->WriteProfileInt(szSection, "IsPodpNiz", m_bIsPodpNiz);
        AfxGetApp()->WriteProfileInt(szSection, "IsPodpVerh", m_bIsPodpVerh);
        AfxGetApp()->WriteProfileInt(szSection, "IsRezhim", m_bIsRezhim);
        AfxGetApp()->WriteProfileInt(szSection, "IsPodpPO", m_bIsPTS);
        AfxGetApp()->WriteProfileInt(szSection, "IsPodpRes", m_bIsPodpRes);
        AfxGetApp()->WriteProfileInt(szSection, "IsPribor", m_bIsPribor);
        AfxGetApp()->WriteProfileInt(szSection, "IsPicture", m_bIsPicture);
        AfxGetApp()->WriteProfileInt(szSection, "IsGeobaza", m_isGeobaza);
        AfxGetApp()->WriteProfileInt(szSection, "IsGidrInf", m_cxema->m_bIsGidrInf);
        AfxGetApp()->WriteProfileInt(szSection, "IsGeoGeodez", m_bIsGeoGeodez);
        AfxGetApp()->WriteProfileInt(szSection, "IsMas", m_bIsMas);
        AfxGetApp()->WriteProfileInt(szSection, "IsNaprGid", m_bIsNaprGid);
        AfxGetApp()->WriteProfileInt(szSection, "m_bk_color", m_bk_color);
        AfxGetApp()->WriteProfileInt(szSection, "IsAlma", m_bIsAlma);

        AfxGetApp()->WriteProfileInt(szSection, "sezon_korrozia", m_sezon_korrozia);

        str.Format("%lg", m_MasPic);
        AfxGetApp()->WriteProfileString(szSection, "MasPic", str);

        str.Format("%lg", m_MasPodp);
        AfxGetApp()->WriteProfileString(szSection, "MasPodp", str);

        CString pn = pDoc->m_bd_gid;
        if (pn == "") pn = "MySQL";

        AfxGetApp()->WriteProfileString(pn, "Password", m_password);

        for (int i = 0; i < sizeof(m_cxema->m_bIsInfo) / sizeof(m_cxema->m_bIsInfo[0]); i++) {
            CString sss;
            sss.Format("IsInfo%d", i);
            AfxGetApp()->WriteProfileInt(szSection, sss, m_cxema->m_bIsInfo[i]);
        }

        AfxGetApp()->WriteProfileString(pn, "picture", m_strPicture);
        str.Format("%.8lf", geom.masx * geom.dmas);
        AfxGetApp()->WriteProfileString(pn, "mas", str);
        str.Format("%.8lf", geom.m_bx);
        AfxGetApp()->WriteProfileString(pn, "m_bx", str);
        str.Format("%.8lf", geom.m_by);
        AfxGetApp()->WriteProfileString(pn, "m_by", str);

        if (m_tmpPicN[0]) DeleteFile(m_tmpPicN);

        if (m_dxf) {
            delete m_dxf;
            m_dxf = NULL;
        }
    }

    CScroll::OnDestroy();
}

void resetMS2();

void CGidrView::changeTable(const char* fn, const char* tn)
{
    //  if (!stricmp(tn, _TR("Очередь опрессовок"))) {
    //    open_opressovki();
    //  }
    //  else 
    //  if (!stricmp(tn, _TR("Участок РС")) || !stricmp(tn, _TR("Участок МС"))) {

#if NOMYSQL

    ps_alma.init();


    resetMS2();

#endif

    //  }
}

#include "MagNode.h"

void CGidrView::vydP(list<CNode2*>& list_node)
{
    CNode2* node0 = NULL;

    list<CNode2*>::iterator it = list_node.begin();

    for (; it != list_node.end(); it++) {

        CNode2* node = *it;

        if (node0 == NULL) {
            node->node.m_graph->reset();
        }
        if (node0 != NULL) {
            node0->node.m_graph->link(node0, node);
        }

        node0 = node;
        node->node.isPjezo = true;
        m_cxema->m_graph->list_pjezo_min.push_back(node->id);
    }

    InvalidateG();
}

void CGidrView::OnMagNode()
{
    CMagNode dlg(this, -1);

    if (dlg.DoModal() == IDOK) {
        //    vydP(dlg.map_node2);
    }
}

void CGidrView::OnTextOpr()
{
    if (!isEditGid()) return;

    CString text;
    CInputDialog inD(this, _TR("Исходный текст"), _TR("Введите текст"), text);

    if (inD.DoModal() == IDOK && inD.m_strInput != "") {
        m_strText = inD.m_strInput;
        m_nRegim = ID_INFO;
        regim = R_BEGIN_DRAW_TEXT1;
    }
}


void CGidrView::OnOdnolin()
{
    m_cxema->m_graph->reset();

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            if (bline(l)->line.nomP == -1 || bline(l)->line.nomO == -1) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }

    Invalidate();
}


void CGidrView::OnRaznolin()
{
    m_cxema->m_graph->reset();

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1 && bline(l)->line.nomP != bline(l)->line.nomO) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
    Invalidate();
}

bool CGidrView::isPassport()
{
    return m_idMenu == 4;
}

bool CGidrView::isToolbar(int id)
{
    if (m_isPrint) return false;

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    bool b = mf->isToolbar(id);

    return b;
}

void CGidrView::OnCtrlAstr()
{
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    //    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

    toPoint(ScreenToCoord(pt));
}

//-----------------------------------------------
//  Нахождение ближайшей к курсору точки на карте
//-----------------------------------------------

void CGidrView::toPoint(CFPoint pt) {
    CFRect rect;
    CCoordList cl;
    CFPoint pt0;
    double minlen0 = 2000000000;
    double len, minlen = minlen0;

    if (m_bIsGeoGeodez) {
        KlList::const_iterator itk = m_geofile->m_kl_list->begin();

        for (; itk != m_geofile->m_kl_list->end(); itk++) {
            const Klassif* kl = &itk->second;
            if (!kl->ins) continue;

            m_geofile->SetKls(kl->nazv);

            int n = kl->NObj();

            for (int i = 1; i <= n; i++) {
                m_geofile->GetFRect(kl, i, rect);
                rect.InflateRect(minlen, minlen);

                m_geofile->GetCoord(kl, i, cl);

                CCoordList::const_iterator it = cl.begin();

                for (; it != cl.end(); ++it) {
                    len = Length(*it, pt);
                    if (len < minlen) {
                        minlen = len;
                        pt0 = *it;
                    }
                }
            }
        }
    }
    /*
    CCoordList::const_iterator it;

    for (it = m_NP.begin(); it != m_NP.end(); ++it) {
    len = Length(*it, pt);
    if (len < minlen) {
    minlen = len;
    pt0 = *it;
    }
    }
    */

    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            len = Length(n->node.coord, pt);
            if (len < minlen) {
                minlen = len;
                pt0 = n->node.coord;
            }
        }
        p++;
    }

    if (minlen != minlen0) {
        moveXY(pt0);

        m_is_ob = true;
        m_pt_ob = pt0;
    }
}


CString getProfileString(const char* section, const char* reg)
{
    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    return pApp->GetProfileString(section, reg, "");
}

//bool getColorOpressovka(int id, unsigned long &color) { return true; }


/*
void CGidrView::setMagNomer(int mag, int ms, int rs)
{
m_ms_kod0 = ms;
m_rs_kod0 = rs;
m_magistral_kod0 = mag;
}

*/


void CGidrView::setNavigator(bool redr)
{
    CMapDlg* a = (CMapDlg*)getDockablePane(IDD_MAP_DLG);
    if (a) {
        if (redr) a->RedrawWindow();
        a->moveRect();
        a->curRect();
    }
}

bool ado_execute(CAdoFile* ado, const char* q, long* affected);

void CGidrView::OnSetLineID()
{
    KlList::iterator it = m_geofile->m_kl_list->begin();
    CMMenu menu(this, IDD_MENU2, "");

    for (; it != m_geofile->m_kl_list->end(); ++it) {
        Klassif* kl = &it->second;
        if (kl && isPoint(kl->loc)) {
            //      kl->m_ado->MySQLTableInfo(kl->nazv);

            MySQLColumns* col = kl->m_ado->findMySQLColumns(kl->nazv, "lineID");
            if (col) {
                menu.Add(kl->nazv, (void*)kl);
            }
        }
    }

    if (menu.DoModal() == IDOK) {
        Klassif* kl = (Klassif*)menu.getV();
        CString q;

        q.Format("UPDATE %s SET lineID=NULL", kl->nazv);
        long affected;
        if (ado_execute(kl->m_ado, q, &affected) && affected > 0) {
        }
        for (int i = 0; i < kl->geo4.size(); i++) {
            CGeoObject* geo = kl->geo4[i];
            CFPoint pt = geo->cl.first();
            CLINE2* line = m_cxema->m_graph->findLine(m_parent_id, pt, geom.masx * D5 / mas_otn);
            if (line) {

                int lineID = bline(line)->line.nomP;
                if (lineID <= 0) lineID = bline(line)->line.nomO;

                if (lineID > 0) {
                    q.Format("UPDATE %s SET lineID=%d WHERE ID=%d", kl->nazv, bline(line)->line.nomP, geo->nom);
                    if (ado_execute(kl->m_ado, q, &affected) && affected > 0) {
                    }
                }
            }
        }
    }
}

#include "CalcTree.h"

void CGidrView::OnRasList()
{
    CAdoFile* ado = m_cxema->m_ado;

    CCalcTree dlg(this, m_cxema->m_graph, ado);

    dlg.DoModal();

}

bool CGidrView::delMark()
{
    int nn = 0;

    {
        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.isPjezo) {
                for (CLINE2* l = n->lines; l; ) {
                    CLINE2* l2 = next(l);
                    nn ++;
                    l = l2;
                }
            }
            p++;
        }


        p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            p++;

            if (n->node.isPjezo) {
                if ( n->node.isP && n->node.typ != TIP_PR ) {
                   nn ++;
                }
            }
        }


        p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            p++;

            if (n->node.isPjezo) {
               nn ++;
            }
        }
    }

    int sz = nn;
    nn = 0;

    Percent2 pc;

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.isPjezo) {
            for (CLINE2* l = n->lines; l; ) {
                CLINE2* l2 = next(l);
                delLine(l, false);

                pc.SetValue(100. * nn / sz );

                nn ++;
                l = l2;
            }
        }
        p++;
    }


    p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        p++;

        if (n->node.isPjezo) {
            if ( n->node.isP && n->node.typ != TIP_PR ) {
                delCxema(n);
                n->node.isP = false;
                pc.SetValue(100. * nn / sz );
            }

            nn ++;
        }
    }


    p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        p++;

        if (n->node.isPjezo) {
            delNode(n, false);

            pc.SetValue(100. * nn / sz );

            nn ++;
        }
    }

    return true;
}


#include "table.h"


void CGidrView::viewTable(const char* tn)
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    CAdoListData* nd = new CAdoListData();
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    CString q;

    /**
         if (query && query[0]) {
           q = query;
         }
         else {
           q.Format("SELECT * FROM %s", table);
         }
         nd->setGeo(table);
    */
    q.Format("SELECT * FROM %s", tn);


    nd->setGeo(tn);

    if (nd->initQpat(ado, q) > 0) {

        pDoc->m_cxema = m_cxema;
        pDoc->m_data = nd;


        CFrameWnd* pWndSecondChild;
        CMultiDocTemplate* pDocTemplate;

        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL)
        {
            CTable* pView = (CTable*)pDoc->GetNextView(pos);
            if (pView->IsKindOf(RUNTIME_CLASS(CTable))) {
                //              CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
                //              pFrame->MDIActivate(pView->GetParent());
                CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
                mf->setActiveView(pView);
                pView->init(pDoc->m_data);
                return;
            }
        }

        pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE);
        if (pDocTemplate != NULL)
        {
            pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
            ASSERT(pWndSecondChild != NULL);
            pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
        }
    }

}



void CGidrView::viewQ(const char* q, VIRT_DATA typ)
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    CAdoListData* nd = new CAdoListData();
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    if (nd->initQ(ado, q)) {
        nd->setType(typ);
        pDoc->m_cxema = m_cxema;
        pDoc->m_data = nd;

        CFrameWnd* pWndSecondChild;
        CMultiDocTemplate* pDocTemplate;

        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL)
        {
            CTable* pView = (CTable*)pDoc->GetNextView(pos);
            if (pView->IsKindOf(RUNTIME_CLASS(CTable))) {
                //              CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
                //              pFrame->MDIActivate(pView->GetParent());

                CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
                mf->setActiveView(pView);

                pView->init(pDoc->m_data);
                return;
            }
        }

        pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE);
        if (pDocTemplate != NULL)
        {
            pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
            ASSERT(pWndSecondChild != NULL);
            pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
        }
    }
}


CString readSql(const char* path, const char* fn, bool include, char* xn, int& nline, int& ntab, char* ist);


CString getExcelQ(const char* path, const char* fn)
{
    char s[256], txt[256], nt[256], xn[256], ist[256], xn1[256];
    FILE* f;
    CString strQ = "";

    sprintf(s, "%s%s/%s", (LPCTSTR)argpath(), path, fn);

    f = fopen(s, "r");

    if (f) {
        fgetstr(xn, 255, f);

        while (!feof(f)) {
            int nline = 0, ntab = -1, n1, n2;
            fgetstr(txt, 255, f);
            fgetstr(nt, 255, f);

            if (feof(f)) break;

            sscanf(nt, "%d", &nline);

            if (sscanf(nt, "%d %d", &nline, &ntab) != 2) {
                ntab = -1;
            }

            strQ = readSql(argpath() + path, txt, true, xn1, n1, n2, ist);
        }

        fclose(f);
    }

    return strQ;
}



void CGidrView::OnTeplopoteriExcel()
{
#ifndef TEPLOPOTERI_YES 
    return;
#endif

    CFileFind finder;
    BOOL bWorking;
    ChDir cd(argpath() + "Excel3");

    CMenuExcel menu(this, _TR("Выберите таблицу"), m_cxema);

    bWorking = finder.FindFile(_T("*.lst"));
    while (bWorking) {
        bWorking = finder.FindNextFile();
        menu.lst.Add(finder.GetFileTitle());
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет данных"), MB_OK | MB_ICONINFORMATION);
    }
    else if (menu.DoModal() == IDOK) {
        CWaitCursor wait;

        CAdoFile* ado = m_cxema->m_ado;
        m_cxema->create_vyd_table(ado);

        CString q = getExcelQ("Excel3", menu.m_strText + ".lst");
        viewQ2(ado, ado->m_schema, q, "", menu.m_strText, virt_data_unknown,"", menu.m_strText);
    }
}


void CGidrView::KamObr()
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    m_parent_id = m_parent_id_old;
    SetBorder(m_rect_old);
}

void CGidrView::KamObr(KamState& ks)
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    m_parent_id = ks.internalNodeID;
    SetBorder(ks.rect);
}

KamState CGidrView::Kam(int internalNodeID)
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    KamState ks;

    old_masx = geom.masx;
    old_masy = geom.masy;
    old_m_bx = geom.m_bx;
    old_m_by = geom.m_by;
    old_rect = m_rectBorder;

    m_parent_id_old = m_parent_id;
    m_parent_id = internalNodeID;

    if (internalNodeID == 0) {
        SetBorder(m_rect_old);
    }
    else {
        m_rect_old = m_rectBorder;

        CFRect rect = m_cxema->m_graph->getBorder(internalNodeID);
        SetBorder(rect);
        ChMas(rect);
    }

    //    ks.fileID = fileID;

    ks.rect = m_rect_old;
    ks.internalNodeID = m_parent_id_old;


    return ks;
}

void CGidrView::vyd_tubing(int tubingID)
{
    CLINE2* l;
    CMapGraph::iterator pp = m_cxema->m_graph->map_node.begin();

    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;

        for (l = p->lines; l; l = next(l)) {
            bline(l)->line.isPjezo = (bline(l)->line.tubingTypeID == tubingID) ? 1 : 0;
            where(l)->node.isPjezo = bline(l)->line.isPjezo;
            other(l)->node.isPjezo = bline(l)->line.isPjezo;
        }
        pp++;
    }
    Invalidate();
}


void CGidrView::vydLineQ(CString q)
{
    CAdoFile* ado = m_cxema->m_ado;
  
    set<int> set_id;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    m_cxema->m_graph->vyd_lines(set_id);
    Invalidate();
}


void CGidrView::OnPtVydKv()
{
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "varCoefficients";
        CString q;
        q.Format("SELECT id, kodkv FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Коэффициент вариации"));

        if (kodv > 0) {
            q.Format(

"select distinct l.id  from nodes n\n"
"join generalizedConsumers gc on gc.nodeID=n.id and gc.varCoeffID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0\n"
"union\n"
"select distinct l.id  from nodes n\n"
"join realConsumers rc on rc.nodeID=n.id and rc.varCoeffID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0;\n"

               , kodv, kodv);
                vydLineQ(q);
        }
    }
}

void CGidrView::OnPtVydUr()
{
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "specExpends";
        CString q;

        q.Format("SELECT id, specExpendID FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Код удельных расходов для реальных потребителей"));

        if (kodv > 0) {
            q.Format(

"select distinct l.id  from nodes n\n"
"join generalizedConsumers gc on gc.nodeID=n.id and gc.specExpendID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0\n"
"union\n"
"select distinct l.id  from nodes n\n"
"join realConsumers rc on rc.nodeID=n.id and rc.specExpendID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0;\n"

               , kodv, kodv);
                vydLineQ(q);
        }
    }
}

void CGidrView::OnPtVydTr()
{
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "calcTemperatures";
        CString q;
        q.Format("SELECT id, calcTemperatureID FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Код расчетных температур"));

        if (kodv > 0) {
            q.Format(

"select distinct l.id  from nodes n\n"
"join generalizedConsumers gc on gc.nodeID=n.id and gc.calcTemperatureID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0\n"
"union\n"
"select distinct l.id  from nodes n\n"
"join realConsumers rc on rc.nodeID=n.id and rc.calcTemperatureID=%d\n"
"join linesobj l on l.nodeID1=n.id or l.nodeID2=n.id and l.removed=0\n"
"where n.removed=0;\n"
               , kodv, kodv);
                vydLineQ(q);
        }
    }
}


void CGidrView::OnTubing1() { vyd_tubing(1); }
void CGidrView::OnTubing2() { vyd_tubing(2); }
void CGidrView::OnTubing3() { vyd_tubing(3); }
void CGidrView::OnTubing4() { vyd_tubing(4); }
void CGidrView::OnTubing5() { vyd_tubing(5); }



void CGidrView::OnFindKti()
{
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    CMMenu menu(this, IDD_MENU2, _TR("Выберите коэффициент"));

    CString q;
    q.Format("SELECT DISTINCT heatTestsCoeff FROM heatPipeSections ORDER BY heatTestsCoeff");

    map<int, double> map_d;

    if (ado->openTable0(q)) {
        int n = 0;
        while (!ado->isEOF()) {
            CString s;
            double d = ado->read_double(0);
            ado->MoveNext();
            s.Format("%g", d);

            map_d[n++] = d;

            menu.Add(s, (void*)n);
        }
        ado->closeTable();
    }
    if (menu.DoModal() != IDOK) return;

    set<int> set_id;
    q.Format("SELECT l.id FROM heatPipeSections hps JOIN linesobj l ON l.id=hps.lineID JOIN nodes n ON l.nodeID1=n.id WHERE heatTestsCoeff=%s AND n.fileID in (%s)", menu.m_strText, m_cxema->m_par);
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long("id");
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    m_cxema->m_graph->vyd_lines(set_id);

    Invalidate();
}



void CGidrView::OnSetAddr()
{
    AfxMessageBox(_TR("Расставить адреса реальных потребителей"), MB_OK | MB_ICONINFORMATION);
}


//////////////////////////////////////////////////////////////

void CGidrView::SetMs() { AfxMessageBox(_TR("Отменили"), MB_OK | MB_ICONINFORMATION); }


int editAlmaGeo(CWnd* parent, const char* fn, const char* tn, long kk, int ro) { return 0; }
int editDefectMin(CWnd* parent, const char* fn, const char* tn, long kk, int ro) { return 0; }
int editWithDoc(CWnd* parent, const char* fn, const char* tn, const char* elem, long kk, int ro) { return 0; }

void CGidrView::OnSetKodOpr() { AfxMessageBox("!!! 1", MB_OK | MB_ICONINFORMATION); }

void CGidrView::OnUpdateSetOpressovka(CCmdUI* pCmdUI) { }
void CGidrView::OnRemont() { AfxMessageBox("!!! 5", MB_OK | MB_ICONINFORMATION); }
void CGidrView::OnRepW() { AfxMessageBox("!!! 6", MB_OK | MB_ICONINFORMATION); }
int CGeoFile::createObjKapital(const char* tN, const CCoordList& cl, CLINE2* l) { return 0; }
int CGeoFile::createObjKapital0(const char* tN, const CCoordList& cl, CLINE2* l) { return 0; }
int CGeoFile::createObjOpressovka(const char* tN, const CCoordList& cl, CLINE2* l) { return 0; }
int CGeoFile::createObjOpressovka0(const char* tN, const CCoordList& cl, CLINE2* l) { return 0; }
void CGidrView::OnPopupPasVV() { AfxMessageBox("!!! 7", MB_OK | MB_ICONINFORMATION); }

void CGidrView::OnRepR() { AfxMessageBox("!!! 8", MB_OK | MB_ICONINFORMATION); }
void CGidrView::OnRep() { AfxMessageBox("!!! 9", MB_OK | MB_ICONINFORMATION); }

void CGidrView::pasport_US(class CNode2*) { AfxMessageBox("!!! 10", MB_OK | MB_ICONINFORMATION); }
void CGidrView::pasport_VV(class CFPoint) { AfxMessageBox("!!! 11", MB_OK | MB_ICONINFORMATION); }

//-----------------------------------------

void CGidrView::setOprNode(CNode2* node, int first_last)
{
    if (!node) return;

    if (!node->node.isPjezo) return;

    if (first_last == 1) {
        m_node_first = node;
    }
    else {
        m_node_last = node;
    }
    InvalidateG();
}



int mark(CNode2* n, bool zakr);


void CGidrView::OnSetIst()
{

    CMMenu menu(this, IDD_MENU2, _TR("Выберите узел"));

    CMapGraph::iterator pp = m_cxema->m_graph->map_node.begin();

    int i = 0;

    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.coord.x != 0 && p->node.coord.x != 0 && p->node.internalNodeID == 0) {
            if (!p->node.isPjezo) {
                //        p->node.m_graph->link2(p, NULL);

                int k = mark(p, false);

                if (k > 1) {
                    menu.Add(p->getName(), (void*)p->id);
                }
            }
        }
        pp++;
    }

    Invalidate();

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();

        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            moveXY(n->node.coord);
        }
    }
}

bool readWKB(COleVariant& var, CCoordList& cl);


static int ii = 0;
static int nn = 14;

void setPsMap(Percent2* pc, const char* tn, CGraph2* graph)
{
    pc->SetValue(100. * ii / nn);
    ii++;


    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;
    if (!ado) return;
    if (!isPoint(kls->loc)) return;

    CString q;
    q.Format("SELECT id, ST_AsWKB(shape) AS __wkb FROM %s", tn);

    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Format("SELECT id, shape.STAsBinary() AS __wkb FROM %s", tn);
    }



    map<int, int> map_1;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long("id");
            COleVariant var = ado->read("__wkb");
            CCoordList cl;
            readWKB(var, cl);
            CFPoint pt = cl.first();

            CLINE2* l = graph->findLine(0, pt, 50.);

            if (l) {
                int idl = bline(l)->line.nomP > 0 ? bline(l)->line.nomP : bline(l)->line.nomO;
                map_1[id] = idl;
                //              CLine2 *l = bline(l);
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }

    map<int, int>::const_iterator it = map_1.begin();

    for (; it != map_1.end(); it++) {
        long affected;
        q.Format("UPDATE %s SET lineID=%d WHERE id=%d", tn, it->second, it->first);
        ado->Execute(q, &affected);
    }
}


void CGidrView::OnSetPsMap()
{
    Percent2 pc;

    nn = 15;
    ii = 0;

    setPsMap(&pc, "capital2", m_cxema->m_graph);
    setPsMap(&pc, "defekt", m_cxema->m_graph);
    setPsMap(&pc, "defekt2", m_cxema->m_graph);
    setPsMap(&pc, "drenazhnyy_kran", m_cxema->m_graph);
    setPsMap(&pc, REM_INDICATOR, m_cxema->m_graph);
    setPsMap(&pc, "kompensator", m_cxema->m_graph);
    setPsMap(&pc, "kontrol_tehnicheskogo_sostoyaniya", m_cxema->m_graph);
    setPsMap(&pc, "opora", m_cxema->m_graph);
    setPsMap(&pc, "perehod_diametra", m_cxema->m_graph);
    setPsMap(&pc, "shurf", m_cxema->m_graph);
    setPsMap(&pc, "shurf2", m_cxema->m_graph);
    setPsMap(&pc, "ugol_povorota_truboprovoda", m_cxema->m_graph);
    setPsMap(&pc, "visualMetricControl", m_cxema->m_graph);
    setPsMap(&pc, "vozdushnik", m_cxema->m_graph);
    setPsMap(&pc, "zapornaya_armatura", m_cxema->m_graph);
}


void CGidrView::OnCalcs()
{
    AfxMessageBox("!", MB_OK | MB_ICONINFORMATION);
}

void newFragment(CWnd* wnd);


void CGidrView::OnFileNew()
{
    newFragment(this);
}


BOOL CGidrView::PreTranslateMessage(MSG* pMsg)
{
    //  if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
    //    if (pMsg->wParam != VK_RETURN) return TRUE;
    //      return TRUE;
    //  }
    return CScroll::PreTranslateMessage(pMsg);
}


void CGidrView::OnTableDan()
{
    CString str, s1, p;
    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_PROFILE, 1);


    p.Format("%s\\AppData\\Local\\Programs\\table-vue\\table.exe", path1);
    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", p, "", "", SW_SHOW);

}


void CGidrView::OnDaniil()
{
    CString str, s1, p;
    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_PROFILE, 1);


    p.Format("%s\\AppData\\Local\\Programs\\table-vue\\table.exe", path1);
    CString param = "\"Table Name\" 1234 800 500 form";


    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", p, param, "", SW_SHOW);

}


void CGidrView::OnMagFragment()
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите магистральный фрагмент присоединения"));

    CAdoFile* ado = m_cxema->m_ado;

    if (ado->openTable0("SELECT id, name FROM fragments WHERE removed = 0")) {

        while (!ado->isEOF()) {
            int id = ado->read_long("id");
            CString name = ado->readStr("name");
            menu.Add(name, (void*)id);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    if (menu.DoModal() == IDOK) {
        CString str;
        str.Format(_TR("Установить магистральный фрагмент присоединения во всех фрагментах распределительных сетей \"%s\"?"), menu.m_strText);
        if (MessageBox(str, _TR("Установить"), MB_YESNO) == IDYES) {
            str.Format("UPDATE setPressNodes SET fragment_resultID=%d", (int)menu.getV());
            bool r = ado->Execute(str);
            if (!r) {
                AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
            }

        }
    }

}

void CGidrView::OnIspravl()
{
    if (!isEditGid()) return;

    CString str;
    str.Format(_T("Внимание!\nПри выполнении этой операции будут изменены внутреннние схемы во всех реальных потребителях\nВыполнить операцию?"));

    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        //      CWaitCursor wait;
        //    m_cxema->ispravl(m_cxema->m_cur_file);

        int n = 0, i = 0;
        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* nn = p->second;
            if (nn->node.typ == TIP_PR) n++;
            p++;
        }

        Percent2 pc;

        CAdoFile* ado = m_cxema->m_ado;

        p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* nn = p->second;
            if (nn->node.typ == TIP_PR) {
                //          rename(nn, TIP_PO);
                create_b5(ado, nn, m_cxema);

                pc.SetValue(100. * i / n);
                i++;
            }
            p++;
        }
    }
}


void CGidrView::OnMasMarkAll()
{
    if (!m_cxema->m_graph->isMark()) return;

    CFRect rect;

    if (m_cxema->m_graph->GetMarkRect(rect)) {
        MoveRect(rect);
    }
}

void CGidrView::OnUpdateMasMarkAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_cxema->m_graph->isMark());
}

void CGidrView::OnAstr()
{
    m_dX = (m_dX == 0) ? 10 * 20 : 0;
    InvalidateG();
}

void CGidrView::OnUpdateAstr(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_dX != 0);
}


double CGidrView::okr(double x, int dx)
{
    return okr3(x, dx, m_parent_id != 0);
}


void saveTableRusName2(CString database, CString klfn);
void initColumnRusName(CString database, CString database2);


void CGidrView::On1000()
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    CString database = pDoc->m_bd_geo;

    CFileDialog fd(FALSE, _T("txt2"), NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _TR("*.txt2|*.txt2|"), this);

    if (fd.DoModal() == IDOK) {
        saveTableRusName2(database, fd.GetPathName());
    }
}

void CGidrView::On1001()
{
    HINSTANCE hRun = ShellExecute(GetParent()->GetSafeHwnd(), "open", "https://ivs.lmedia.pro/#/shape", NULL, NULL, SW_SHOW);
}

void CGidrView::On1002()
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    CString database = pDoc->m_bd_geo;

    CString path = "";
    PWSTR path1 = NULL;

    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path1);
    if (SUCCEEDED(hr)) {
        path = path1;
        path += "\\";

        CoTaskMemFree(path1);
    }

    CFileDialog fd(TRUE, _T("txt2"), path,
        OFN_HIDEREADONLY,
        _TR("*.txt2|*.txt2|"), this);

    if (fd.DoModal() == IDOK) {
        CString fn = fd.GetPathName();
        //        CString database = g_kls->m_ado->m_schema;

        CString fn2;

        fn2.Format("%s%s.txt2", argpath_2(), database);

        if (CopyFile(fn, fn2, 0)) {
            initColumnRusName(database, database);

            if (database == m_cxema->m_ado->m_schema) {
                initColumnRusName(database, "GID");
            }
            AfxMessageBox(_TR("Выполнено! Перезагрузите программу"), MB_OK | MB_ICONINFORMATION);
        }
    }
}


void CGidrView::initGeoValue()
{
    m_map_geo_value.clear();
}

void CGidrView::AddGeoValue(const char* name, const COleVariant& var)
{
    m_map_geo_value[name] = var;
}



void CGidrView::OnCoord()
{
    CPropGridDlg* pm_cEditDlg = dialog_pr(this, "coord", WM_USER_COORD_UPDATED, "Настройка системы координат", -1);

    CString s;

    if (pm_cEditDlg) {
        s = AfxGetApp()->GetProfileString(szSection, "COORD_Z3", "0");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("Z3", s);

        s = AfxGetApp()->GetProfileString(szSection, "False_Easting", "0");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("False_Easting", s);

        s = AfxGetApp()->GetProfileString(szSection, "False_Northing", "0");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("False_Northing", s);

        s = AfxGetApp()->GetProfileString(szSection, "Latitude_Of_Origin", "0");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("Latitude_Of_Origin", s);

        s = AfxGetApp()->GetProfileString(szSection, "Scale_Factor", "1");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("Scale_Factor", s);

        s = AfxGetApp()->GetProfileString(szSection, "Angle", "0");
        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("Angle", s);


    }
}


LRESULT CGidrView::OnCoordUpdated(WPARAM wParam, LPARAM)
{
    CString s;

    double False_Easting;
    double False_Northing;
    double Central_Meridian;
    double Latitude_Of_Origin;
    double Scale_Factor;
    double Angle;

    s = AfxGetApp()->GetProfileString(szSection, "COORD_Z3", "0");
    Central_Meridian = atof(s);
    s = AfxGetApp()->GetProfileString(szSection, "False_Easting", "0");
    False_Easting = atof(s);
    s = AfxGetApp()->GetProfileString(szSection, "False_Northing", "0");
    False_Northing = atof(s);
    s = AfxGetApp()->GetProfileString(szSection, "Latitude_Of_Origin", "0");
    Latitude_Of_Origin = atof(s);
    s = AfxGetApp()->GetProfileString(szSection, "Scale_Factor", "1");
    Scale_Factor = atof(s);
    s = AfxGetApp()->GetProfileString(szSection, "Angle", "0");
    Angle = atof(s);

    if (getPropValue(WM_USER_COORD_UPDATED, "Z3", s)) {
        AfxGetApp()->WriteProfileString(szSection, "COORD_Z3", s);
        Central_Meridian = atof(s);
    }

    if (getPropValue(WM_USER_COORD_UPDATED, "False_Easting", s)) {
        AfxGetApp()->WriteProfileString(szSection, "False_Easting", s);
        False_Easting = atof(s);
    }

    if (getPropValue(WM_USER_COORD_UPDATED, "False_Northing", s)) {
        AfxGetApp()->WriteProfileString(szSection, "False_Northing", s);
        False_Northing = atof(s);
    }

    if (getPropValue(WM_USER_COORD_UPDATED, "Latitude_Of_Origin", s)) {
        AfxGetApp()->WriteProfileString(szSection, "Latitude_Of_Origin", s);
        Latitude_Of_Origin = atof(s);
    }

    if (getPropValue(WM_USER_COORD_UPDATED, "Scale_Factor", s)) {
        AfxGetApp()->WriteProfileString(szSection, "Scale_Factor", s);
        Scale_Factor = atof(s);
    }

    if (getPropValue(WM_USER_COORD_UPDATED, "Angle", s)) {
        AfxGetApp()->WriteProfileString(szSection, "Angle", s);
        Angle = atof(s);
    }

    //  set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Angle);
    Invalidate();

    return 0L;
}


void CGidrView::OnBmpFind()
{
    if (m_bIsPicture && m_strPicture != "") {
        if (GetExt(m_strPicture) == "dxf") {
            if (m_dxf) {
                if (m_dxf->extent_x || m_dxf->extent_y || m_dxf->extent_x || m_dxf->extent_y) {
                    double x1 = min(m_dxf->extent_x, m_dxf->extent_mx);
                    double x2 = max(m_dxf->extent_x, m_dxf->extent_mx);
                    double y1 = min(m_dxf->extent_y, m_dxf->extent_my);
                    double y2 = max(m_dxf->extent_y, m_dxf->extent_my);

                    x1 = max(x1, m_dxf->l_xmin);
                    x2 = min(x2, m_dxf->l_xmax);
                    y1 = max(y1, m_dxf->l_ymin);
                    y2 = min(y2, m_dxf->l_ymax);

                    moveXY(CFPoint((x1 + x2) / 2 * m_dxf->p25400, -(y1 + y2) / 2 * m_dxf->p25400));
                }
                else {
                    moveXY(CFPoint((m_dxf->l_xmin + m_dxf->l_xmax) / 2 * m_dxf->p25400, -(m_dxf->l_ymin + m_dxf->l_ymax) / 2 * m_dxf->p25400));
                }
            }
        }
        else {
            moveXY(CFPoint(1000000, -1000000));
        }
    }
}

void CGidrView::OnUpdateBmpFind(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsMain() && m_bIsPicture);
}


void CGidrView::OnRebuildIndexes()
{
    if (!isEditGid()) {
        CString str;
        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
        if (pDoc) {
            str.Format("%s не имеет прав на редактирование", pDoc->m_user_gid);
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return;
    }

    if (AfxMessageBox(_TR("Провести перстройку индексов?"), MB_YESNO) == IDYES) {
        CAdoFile* ado = m_cxema->m_ado;
        long affected;
        CString q;
        q = readQ(ado, "system\\rebuild_all_indexes_database", "", -1);
        q.Replace("$BAZA$", m_cxema->m_ado->m_schema);

        CWaitCursor wait;
        if (ado && ado->Execute(q, &affected)) {
            AfxMessageBox("Готово", MB_OK | MB_ICONINFORMATION);
        }
    }
}

void CGidrView::OnClearOut()
{
    if (!isEditGid()) {
        CString str;
        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
        if (pDoc) {
            str.Format("%s не имеет прав на редактирование", pDoc->m_user_gid);
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return;
    }

    if (AfxMessageBox(_TR("Удалить старые гидравлические расчеты?"), MB_YESNO) == IDYES) {
        CAdoFile* ado = m_cxema->m_ado;
        long affected;
        CString q = "DELETE FROM CALCULATION WHERE NOT id IN ( SELECT max(c.id) AS cid FROM CALCULATION c LEFT JOIN fragments fr ON fr.id=c.fileID GROUP BY c.fileID,c.user_gid)";

        CWaitCursor wait;
        if (ado && ado->Execute(q, &affected)) {
            AfxMessageBox("Готово", MB_OK | MB_ICONINFORMATION);
        }
    }
}


#include "ElemList.h"

bool create_vyd_line_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn);
bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big);



/*
Спрятать всех кроме нужного начальника
*/

void setNachVisible(int id)
{

}

void CGidrView::setNachalnik(int nachalnikID)
{
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    CString q;

    q.Format(
        "SELECT\n"

        "ms.id AS ms_id,\n"
        "rs.id AS rs_id\n"


        "FROM nachalniki_uchastkov nach\n"
        "JOIN uchastki_ekspluatatsii ue ON ue.nachalnik_uchastka = nach.id\n"
        "LEFT JOIN uchastok_ms ms ON ms.nomer_uchastka = ue.id\n"
        "LEFT JOIN uchastok_rs rs ON rs.nomer_uchastka = ue.id\n"
        "WHERE nach.id = %d\n", nachalnikID);
    set<int> set_ms;
    set<int> set_rs;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long idm = ado->read_long(0);
            if (idm) set_ms.insert(idm);
            long idr = ado->read_long(1);
            if (idr) set_rs.insert(idr);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    m_cxema->m_graph->visibleMSRS(set_ms, set_rs);
    Invalidate();
}


int getNachalnik(CAdoFile* ado, CWnd* wnd, bool all)
{
    CString q =
        "SELECT \n"
        "nach.id,\n"
        "nach.fio\n"
        "FROM nachalniki_uchastkov nach\n"

        //      "FROM uchastki_ekspluatatsii uch\n"
        //      "LEFT JOIN nachalniki_uchastkov nach ON nach.id=uch.nachalnik_uchastka\n"
        "ORDER BY nach.fio  \n";

    CMMenu menu(wnd, IDD_MENU2, "Список начальников участков");

    if (all) {
        menu.Add("Все начальники", (void*)-1);
    }


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString name = ado->readStr(1);
            menu.Add(name, (void*)id);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    if (menu.DoModal() != IDOK) return -1;

    //  setNachVisible((int)menu.getV());

    int id_nach = (int)menu.getV();

    return id_nach;

}


void CGidrView::OnNachalnik()
{
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    m_set_nachalnikID.clear();

    m_nachalnikID = getNachalnik(ado, this, true);

    if (m_nachalnikID == -1) {
        m_cxema->m_graph->allVisible();
    }
    else {
        setNachalnik(m_nachalnikID);
    }
    Invalidate();
}


void CGidrView::OnListOsvid()
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString tn = "defect";

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) {
        CString s;
        s.Format("Нет объекта Повреждения");
        AfxMessageBox(s);
        return;
    }

    if (m_id_opr <= 0) {
        CString s;
        s.Format("Выберите дату освидетельствования");
        AfxMessageBox(s);
        return;
    }


    CString fStr;

    fStr.Format("%ssql\\objects\\defect\\defectsByOpresID.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        fStr.Format("objects\\defect\\defectsByOpresID");
        CString q = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

        CString ss;
        ss.Format("%d", m_id_opr);

        q.Replace("$objID$", ss);

        ss.Format("SELECT name FROM opres WHERE id=%d", m_id_opr);
        ss = readTableValue(ado, ss);

        ss.Format("Дефекты контура опрессовок %s", ss);

        //        RunTableNiz(ado, tn, q, kls->RusName(), true, true);
        RunTableNiz(ado, tn, q, ss, true, false, virt_data_geo);
    }

    //    viewNizGeoQ("defect", "stateID", "data_osmotra", true);
}


// Установить дату освидетельствования

void CGidrView::OnSetOsvid()
{
    CInputDialogDate dlg(this, "", "Введите дату планирования освидетельствования");

    if (dlg.DoModal() != IDOK) return;

    CString q;
    CString tn = "diag";

    q.Format("INSERT INTO %s (sredstvoDiagID, data_prov) VALUES (3,'%s')", tn, dlg.m_date1.Format("%Y%m%d"));

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    long affected;

    int diagID = ado->ExecuteInsert(q, &affected);


    //    q.Format("INSERT INTO osvidetelForLine (diagID, lineID) VALUE(%d %d)", diagID, lineID);

    CString q0;

    q = "";

    q0.Format("INSERT INTO osvidetelForLine (diagID, lineID) VALUES ");

    auto p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* pp = p->second;
        for (CLINE2* l = pp->lines; l; l = next(l)) {
            if (bline(l)->line.isPjezo) {
                CString qq;
                if (bline(l)->line.nomP != -1) {
                    if (q != "") q += ",";
                    qq.Format("(%d,%d)", diagID, bline(l)->line.nomP);
                    q += qq;
                }
                if (bline(l)->line.nomO != -1 && bline(l)->line.nomO != bline(l)->line.nomP) {
                    if (q != "") q += ",";
                    qq.Format("(%d,%d)", diagID, bline(l)->line.nomO);
                    q += qq;
                }
            }
        }
        p++;
    }

    q = q0 + q;

    if (!ado->ExecuteInsert(q, &affected)) {
        AfxMessageBox(ado->getError());
    }
}


void CGidrView::OnSetupProxy()
{
    AfxMessageBox("1");
}


void CGidrView::OnExportCxe()
{
    CFileDialog fd(FALSE, _T("coord"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("coord-файлы|*.coord|"), this);

    if (fd.DoModal() == IDOK) m_cxema->exportCxe(fd.GetPathName());
}

void CGidrView::OnImportCxe()
{
    CFileDialog fd(TRUE, _T("coord"), NULL, OFN_HIDEREADONLY,
        _T("*.coord|*.coord|"), AfxGetMainWnd());

    if (fd.DoModal() == IDOK) {
        CString fn = fd.GetPathName();
        m_cxema->importCxe(fd.GetPathName());
    }
}

void CGidrView::OnNoPts()
{
    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.isPjezo = (bline(l)->line.pipeSectionID == 0);
        }
        p++;
    }
    Invalidate();
}


void opres_utverdit(CWnd *wnd, int id)
{
//    if (!wnd->check(true, false)) return;

    CPropGridDlg* pm_cEditDlg = dialog_pr(wnd, "opres_utverdit", WM_USER_OPRES_UTVERDIT, _TR("Утверждение плана опрессовок"), -1);

    if (pm_cEditDlg) {
        CTime theTime = CTime::GetCurrentTime();
        CString data_utverzhdeniya_plana = theTime.Format("%d.%m.%Y");

        setPropValueInit(pm_cEditDlg, WM_USER_OPRES_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);
    }
}


void remont_utverdit(CWnd *wnd, int id)
{
//    if (!wnd->check(true, false)) return;

    CPropGridDlg* pm_cEditDlg = dialog_pr(wnd, "remont_utverdit", WM_USER_REMONT_UTVERDIT, _TR("Утверждение плана ремонтов"), -1);

    if (pm_cEditDlg) {
        CTime theTime = CTime::GetCurrentTime();
        CString data_utverzhdeniya_plana = theTime.Format("%d.%m.%Y");

        setPropValueInit(pm_cEditDlg, WM_USER_REMONT_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);
    }
}

bool remont_utverdit_ok(CWnd* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return false;

    if (AfxMessageBox("Вы уверены, что хотите утвердить данный контур ремонта?", MB_YESNO) != IDYES) {
        return false;
    }

    CString data_utverzhdeniya_plana;
    getPropValue0(WM_USER_REMONT_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);


    CString q;
    q.Format("UPDATE remont2 SET data_utverzhdeniya_plana='%s', stateID=2, utverdit = 1 WHERE id=%d", 
        data_utverzhdeniya_plana, id);
    
    long affected;

    if (ado->Execute(q, &affected)) {
        return true;
    }
    return false;
}

bool opres_utverdit_ok(CWnd* wnd, int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return false;

    if (AfxMessageBox("Вы уверены, что хотите утвердить данный контур опрессовок?", MB_YESNO) != IDYES) {
        return false;
    }

    CString data_utverzhdeniya_plana;
    getPropValue0(WM_USER_OPRES_UTVERDIT, "data_utverzhdeniya_plana", data_utverzhdeniya_plana);


    CString q;
//    q.Format("UPDATE opres SET data_utverzhdeniya_plana='%s', stateID=2, utverdit = 1 WHERE id=%d", 
    q.Format("UPDATE opres SET data_utverzhdeniya_plana='%s', sostoyanie_opresID = 2, utverdit = 1 WHERE id=%d", 
        data_utverzhdeniya_plana, id);
    
    long affected;

    if (ado->Execute(q, &affected)) {
        return true;
    }
    return false;
}


void CGidrView::OnGoogleElevationConfig()
{
    CString site = "https://api.open-elevation.com";
    site = AfxGetApp()->GetProfileString(szSection, "elevation_url", site);

    CInputDialog inD(this, _TR("Ввод сервера геодезических отметок"), _TR("Введите сервер геодезических отметок"), site);
    if (inD.DoModal() == IDOK) {
        AfxGetApp()->WriteProfileString(szSection, "elevation_url", inD.m_strInput);
    }
}

void CGidrView::OnInitSezon()
{
    CAdoFile* ado = m_cxema->m_ado;
    
    CString q = "SELECT * FROM heating_seasons";

    map<int, pair<COleDateTime, COleDateTime> > map_sezon;

    int y0 = COleDateTime::GetCurrentTime().GetYear();

    int y1 = 2000, y2 = y0+2;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            COleDateTime date1 = ado->read_date("date1");
            COleDateTime date2 = ado->read_date("date2");

            int y = date1.GetYear();

            map_sezon[y] = pair<COleDateTime, COleDateTime>(date1, date2);

            if (y < y1)  y1 = y;
            if (y > y2)  y2 = y;

            ado->MoveNext();
        }
        ado->closeTable();
    }


    if (y1 < 1990) {
        y1 = 1990;
    }
    if (y2 > y0 + 10) {
        y2 = y0 + 10;
    }

    q.Format("TRUNCATE TABLE heating_seasons");
    ado->Execute(q);

    CAdoInsert ins(ado, "heating_seasons", "date1,date2");

    for (int y = y1; y <= y2; y++) {
        CString sql, s1, s2;

        auto it = map_sezon.find(y);

        COleDateTime date1(y, 10, 15, 0, 0, 0);
        COleDateTime date2(y+1, 4, 15, 0, 0, 0);

        if (it != map_sezon.end()) {
            date1 = it->second.first;
            date2 = it->second.second;
            date2.SetDate(y+1, date2.GetMonth(), date2.GetDay());
        }
        s1 = date1.Format("%Y%m%d");
        s2 = date2.Format("%Y%m%d");
        sql.Format("'%s','%s'", s1, s2);
        if (!ins.ado_insert(sql)) break;
    }
    ins.ado_insert();
}

bool executeSqlWithGo(CAdoFile *ado, CString q)
{
    CString q0 = q;

    int nn = 0, n = 0;

    int i1 = 0;

    CString p1 = "\nGO\n";

    while (1) {
        int i2 = q0.Find(p1, i1);
        if (i2 == -1) break;

//        CString q1 = q0.Left(i);
//        q1 = q.Mid(i1, i2-i1);

        i1 = i2 + p1.GetLength();

        nn ++;
    }

    i1 = 0;

    Percent2 pc;

    while (1) {
        pc.SetValue(n*100./nn);
        int i2 = q.Find(p1, i1);

        if (i2 == -1) break;

        CString q1 = q.Mid(i1, i2-i1);


        vector<CString> match;

        while (1) {
            int l = regex_match("[\\n]SET IDENTITY_INSERT [^\\n]+", q1, match);
            if (l == 0) l = regex_match("^SET IDENTITY_INSERT [^\\n]+", q1, match);

            if (l == 0) break;
            CString m = match[0];
            q1.Replace(m, "");
        }

        int l = regex_match("[\\n]INSERT \\[dbo\\]\\.\\[([^\\]]+)\\]", q1, match);
        if (l > 0) {
            CString tn = match[1];

            CString s1, s2;

            s1.Format("SET IDENTITY_INSERT [dbo].[%s] ON\n", tn);
            s2.Format("SET IDENTITY_INSERT [dbo].[%s] OFF\n", tn);

            q1 = s1 + q1 + "\n" + s2;
        }


        q1 = q1.TrimLeft().TrimRight();

        if (q1 != "" && q1 != "GO" && !ado->Execute(q1)) {
            AfxMessageBox(ado->getError());
            return false;
        }

        i1 = i2 + p1.GetLength();
        n ++;
    }
    return true;
}

void CGidrView::OnRemontIspr()
{
    CString str;

    if (!isEditGid()) {
        CString str;
        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
        if (pDoc) {
            str.Format("%s не имеет прав на редактирование", pDoc->m_user_gid);
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return;
    }

    str.Format(_TR("Выполнить корректировку контуров ремонтов?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {

        CAdoFile* ado = m_cxema->m_ado;
        CString q = readQ(ado, "system\\correct_remont2", "", -1);

        CWaitCursor wait;
        if (ado && executeSqlWithGo(ado, q)) {
            AfxMessageBox("Готово", MB_OK | MB_ICONINFORMATION);
        }
    }
}

void CGidrView::OnIndexes()
{
    CString tn1 = argpath() + "\\sql3\\update_index.sql";
    CString q = readFile(tn1);
    q.Replace("$BAZA$", m_cxema->m_ado->m_schema);

    CAdoFile* ado = m_cxema->m_ado;

    CWaitCursor wait;

    if (ado->Execute(q)) {
        AfxMessageBox(_TR("Готово"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        AfxMessageBox(_TR("Ошибка"), MB_OK | MB_ICONINFORMATION);
    }
}

#include "CZdanieNagruzki.h"

void CGidrView::OnNagrAlmaty()
{
    CZdanieNagruzki dlg(true, false, this);

    dlg.DoModal();
}


/*

void CGidrView::AlsecoNagr(int id)
{
    CZdanieNagruzki dlg(true, this);

    if (dlg.DoModal() == IDOK) {
        dlg.m_OTOP;
        dlg.m_GVS;
        dlg.m_VENT;
        dlg.m_PAR;
    }
}

*/
void CGidrView::OnNagrAlmatyOnOff()
{
    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (a && m_geofile) {
        CString tn,  str,  raj,  nomer;
        if (a->Addr(tn,  str,  raj,  nomer)) {
            tn = ZDANIYA_2;

            Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
            if (kls) {
                if (kls->codlabel == "txt") {
//                    kls->codlabel = nomer;
                    kls->codlabel = "number_1";
                    kls->lf.lfItalic = FALSE;
                    kls->label = 1;
                }
                else {
                    kls->codlabel = "txt";
                    kls->lf.lfItalic = TRUE;
                    kls->label = 1;
                }

                Percent2 pc;
                m_geofile->readMySql_Shape_table(kls->m_ado, kls, &pc);
                m_geofile->m_kl_list->save2();

                Invalidate();
            }
        }
    }
}


void CGidrView::OnUpdateElevation2(CCmdUI* pCmdUI)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN("gps");
    pCmdUI->Enable(kls != NULL);
}


void CGidrView::OnUpdateNagrAlmatyOnOff(CCmdUI* pCmdUI)
{
    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (a) {
        CString tn,  str,  raj,  nomer;
        if (a->Addr(tn,  str,  raj,  nomer)) {
            tn = ZDANIYA_2;
            Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
            if (kls) {
                pCmdUI->SetCheck(kls->codlabel == "txt");
            }
        }
    }
}

void CGidrView::OnNagrAlmaXlsx()
{
    CString alseco_xls = AfxGetApp()->GetProfileString(szSection, "alseco_xls", "");

    if (alseco_xls == "" || !IsFile(alseco_xls)) {

        CFileDialog fd(TRUE, _T("xlsx"), NULL,
            OFN_HIDEREADONLY,
            //    _T("JPEG-файл|*.jpg|GIF-файл|*.gif|BMP-файл|*.bmp|"), this);
            _TR("Файлы Excel (*.xlsx)|*.xlsx|"), this);
          
          if (fd.DoModal() == IDOK) {
              alseco_xls = fd.GetPathName();
              AfxGetApp()->WriteProfileString(szSection, "alseco_xls", alseco_xls);
          }
    }

    if (IsFile(alseco_xls)) {
        HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", "\"" + alseco_xls + "\"" , NULL, SW_SHOWMAXIMIZED);
    }
}


void CGidrView::OnNagrAlmaty2()
{
    CZdanieNagruzki dlg(true, true, this);
    dlg.init(m_mkr, m_street, m_dom);

    dlg.DoModal();
}

void CGidrView::OnNagrAlmatyTable()
{
    CString tn1 = "nagruzki";

    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (!a || !m_geofile) return;
    CString tn, str, raj, nomer;
    if (!a->Addr(tn, str, raj, nomer)) return;

    tn = ZDANIYA_2;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString q = readQ(kls->m_ado, "alseco\\alseco", "", -1);

    RunTableAdo(kls->m_ado, tn1, q, -1, virt_data_edit_add_del, "", "Alseco");
}


void CGidrView::OnNagrAlmaNenaid1()
{
    CString tn1 = "nagruzki";

    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (!a || !m_geofile) return;
    CString tn, str, raj, nomer;
    if (!a->Addr(tn, str, raj, nomer)) return;

    tn = ZDANIYA_2;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString q = readQ(kls->m_ado, "alseco\\nenaid1", "", -1);

    RunTableAdo(kls->m_ado, tn1, q, -1, virt_data_unknown, "", "Ненайденные объекты МЖД");
}

void CGidrView::OnNagrAlmaNenaid2()
{
    CString tn1 = "nagruzki";

    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (!a || !m_geofile) return;
    CString tn, str, raj, nomer;
    if (!a->Addr(tn, str, raj, nomer)) return;

    tn = ZDANIYA_2;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString q = readQ(kls->m_ado, "alseco\\nenaid2", "", -1);

    RunTableAdo(kls->m_ado, tn1, q, -1, virt_data_unknown, "", "Ненайденные объекты прочие");
 
}


void CGidrView::OnNagrAlmaNenaid3()
{
    CString tn1 = "nagruzki";

    CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
    if (!a || !m_geofile) return;
    CString tn, str, raj, nomer;
    if (!a->Addr(tn, str, raj, nomer)) return;

    tn = ZDANIYA_2;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString q = readQ(kls->m_ado, "alseco\\nenaid3", "", -1);

    RunTableAdo(kls->m_ado, tn, q, -1, virt_data_geo, "", "Здания АЛСЕКО без потребителей");
  
}


void CGidrView::OnSqlQuery()
{
    CString str = "";

    static bool psw_yes = false;

    if (!psw_yes) {
        CInputDialog inD(this, _T(""), "Введите технический пароль",str, IDD_PASSWORDDIALOG);
        if (inD.DoModal() != IDOK) return;
        if (inD.m_strInput != "TsqlTgidAdmin.kz") {
            AfxMessageBox("Пароль неверный");
            return;
        }
        psw_yes = true;
    }

    CInputDialog inQ(this, _T(""), "Введите запрос",str, IDD_INPUTDIALOG_MULTY);
    if (inQ.DoModal() != IDOK) return;

    CAdoFile* ado = m_cxema->m_ado;
    long affected;

    CString q = inQ.m_strInput;
                               
    if (!ado->Execute(q, &affected)) {
        AfxMessageBox(ado->getError());
    }
    else {
        CString s;
        s.Format("Запрос выполнен.\nОбработано %d строк", affected);
        AfxMessageBox(s);
    }
}


void CGidrView::OnSetFocus(CWnd* pOldWnd)
{
    CScroll::OnSetFocus(pOldWnd);

    CString ss;
    ss.Format("OnSetFocus %d -> %d", pOldWnd, this);
    log1(ss);

    setGidView((CGidrView*)this);

    // Отправляем пользовательское сообщение
//    GetParent()->SendMessage(WM_USER_FOCUS_VIEW, (WPARAM)this);
}


void read_address_ini(CString tn, CString& raj, CString& str, CString& dom);

void CGidrView::OnAddrConfig()
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите объект для адресного поиска"));

    CString raj = "", str = "", dom = "";

    for (auto & it : *m_geofile->m_kl_list) {
        read_address_ini(it.first, raj, str, dom);
        if (str != "") {
            menu.Add(it.second.RusName(), &it.second);
        }
    }
    if (menu.DoModal() == IDOK) {
        Klassif * kls = (Klassif*)menu.getV();

        if (kls) {
            CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
            mf->LeftBar(true);

            CAddressLeft *a = (CAddressLeft *)getDockablePane(IDD_ADDRESS3);
            if (a) {
                m_isGeobaza = TRUE;
                kls->ins = 1;
                read_address_ini(kls->nazv, raj, str, dom);
                a->Init("", kls->nazv, raj, str, dom);
                BOOL delay = TRUE;
                a->ShowPane(TRUE, delay, TRUE);
                Invalidate();
            }
        }
        m_is_geo = true;
    }
}

void CGidrView::OnGeoZdan()
{
    CFiltr3 menu(this, _TR("Выберите отображаемые здания"));

    CString raj = "", str = "", dom = "";

    for (auto & it : *m_geofile->m_kl_list) {
        read_address_ini(it.first, raj, str, dom);
        if (str != "") {
            read_address_ini(it.first, raj, str, dom);
            Klassif *kls = &it.second;
            menu.Add(kls->RusName(), (int) kls, kls->ins);
        }
    }
    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_is_geo = true;
        Invalidate();
    }
}


void CGidrView::OnFindGeoAddr()
{
     CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
     mf->LeftBar(true);
     CAddressLeft *a = (CAddressLeft *)getDockablePane(IDD_ADDRESS3);
     if (a) {
         BOOL delay = TRUE;
         a->ShowPane(TRUE, delay, TRUE);
     }
}
