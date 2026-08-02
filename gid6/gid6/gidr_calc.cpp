/*
    Запуск расчета
*/

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"

#include "ado.h"
#include "win.h"


#include "param.h"
#include "param1.h"
#include "param2.h"
//#include "ParamAnaliz.h"
#include "prot.h"

const char* sety4 = "sety4.exe";

static int m_reread_sety = false;

extern int readonly_version;
extern int no_raschet;


DWORD FindProcessId(const char* processname);
void killProcessByName(const char* filename);




//-----------------------------------------------
// Расчет
//-----------------------------------------------

#include "Protocol.h"

CString GetFName(const char* path);


void Run(CWnd* win, const char* cmd, const char* prtn, int id)
{
    CProt* pr = (CProt*)getDockablePane(IDD_PROTOCOL);
    if (!pr) return;

    BOOL delay = TRUE;
    pr->ShowPane(TRUE, delay, TRUE);

    Cgid6App* pApp = (Cgid6App*)AfxGetApp();
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)pApp->m_pMainWnd;

    mkdir_for_file(prtn);
    HANDLE hHandle = pr->start_proc(win, cmd, prtn, id);
}


void CGidrView::RunDoIt(const char* cmd1, const char* txt)
{
    CString cmd = cmd1;

    CInputDialog inD(this, "", txt, cmd);
    if (inD.DoModal() != IDOK) return;

    m_wait = new CWaitCursor();

    if (FindProcessId(sety4)) {
        killProcessByName(sety4);
    }
    else {
        Run(this, cmd, argpath_2() + "protocol.txt", ID_END_DOIT);
    }
}



extern int m_exit_code;

/// перечтение заново данных после теплогидравлического расчета


BOOL CGidrView::reopen()
{
    if (!m_cxema->m_cur_file) return FALSE;

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (m_cxema->m_cur_file && (n->node.fileID == m_cxema->m_cur_file->id || n->node.fileID == m_fileID2)) {
            n->node.nomgP = -1;
            n->node.nomgO = -1;
            for (CLINE2* l = n->lines; l; l = next(l)) {
                bline(l)->line.nomgP = -1;
                bline(l)->line.nomgO = -1;
                bline(l)->line.pod.q = 0;
                bline(l)->line.obr.q = 0;
            }
        }
        p++;
    }
    CString par;
    if (m_fileID2 > 0) {
        par.Format("%d,%d", m_cxema->m_cur_file->id, m_fileID2);
    }
    else {
        par.Format("%d", m_cxema->m_cur_file->id);
    }

    CAdoFile* ado = this->m_cxema->m_ado;

    //    if (m_reread_sety) {
    //        m_cxema->openGid(ado, par);
    //    }
    //    else {

    m_cxema->m_graph->clean_out(m_cxema->m_cur_file->id);

    m_cxema->openLine(ado, getLineTableMySQL(TIP_DR), "stateID", TIP_DR, par, true);    

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    m_cxema->openOut(ado, par, pDoc->m_user_gid);
    //    }

    if (m_wait) delete m_wait;  // 
    m_wait = NULL;

    return TRUE;
}


BOOL CGidrView::reopen_all()
{
    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

    //    if (!m_cxema->m_cur_file) return FALSE;

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (m_cxema->m_cur_file && n->node.fileID == m_cxema->m_cur_file->id) {
            n->node.nomgP = -1;
            n->node.nomgO = -1;
            for (CLINE2* l = n->lines; l; l = next(l)) {
                bline(l)->line.nomgP = -1;
                bline(l)->line.nomgO = -1;
                bline(l)->line.pod.q = 0;
                bline(l)->line.obr.q = 0;
            }
        }
        p++;
    }

    CAdoFile* ado = getAdo(getGidAdoName());

    m_cxema->openLine(ado, getLineTableMySQL(TIP_DR), "stateID", TIP_DR, m_cxema->m_par, true);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    m_cxema->openOut(ado, m_cxema->m_par, pDoc->m_user_gid);

    if (m_wait) delete m_wait;  // 
    m_wait = NULL;

    return TRUE;
}


int Raschet(CCxema* cxe);


CString getDoIt(CParam2& param, Cgid6Doc* pDoc, int fileID)
{
    CString str, par = "", qq;

    str.Format("\"%s%s\" "
        " -type_of_net %d -server \"%s\" -database \"%s\" -user \"%s\" -password \"%s\" -port %d -fileID %d "
        " %s -Tn %g  -GWS %d -GWS2 %d",
        argpath(), sety4,

        pDoc->m_type_of_net,

        pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_user, pDoc->m_password, pDoc->m_port, fileID,

        par, param.m_Tn, param.m_nGWS, param.m_nGWS2);


    if (param.m_name != "") {
        str += " -name \"";
        str += param.m_name;
        param.m_name.Replace("\"", "'");
        str += "\"";
    }


    if (param.m_avtomat) str += " -avtomat_yes";
    if (param.m_char_sety) str += " -char_sety";

    if (param.m_veter) {
        str += " -veter";
    }

    if (param.m_detaliz == 1) {
        str += " -a";
    }

    int m_ur = AfxGetApp()->GetProfileInt(szSection, "ur", 1);

    if (!m_ur) {
        //        str += " -no_teplovyd";
        //        str += " -no_teplopoter";
        str += " -tg";
    }

    if (param.m_mag_fn != "") {
        qq.Format(" -mn \"%s\"", param.m_mag_fn);
        str += qq;
    }

    m_reread_sety = false;
    if (param.m_save_po) {
        m_reread_sety = true;
        str += " -save_po";
    }

    if (param.m_leto) {
        str += " -leto";
    }

    if (param.m_mag_fragment) {
        str += " -mag_fragment";
    }

    if (!param.m_kv) {
        str += " -no_kv";
    }

    qq.Format(" -user_gid \"%s\"", pDoc->m_user_gid);
    str += qq;


    map <CString, double>::const_iterator it = param.map_t_ist.begin();

    for (; it != param.map_t_ist.end(); ++it) {
        CString s;
        s.Format(" -Tist %s %g", it->first, it->second);
        str += s;
    }
    return str;
}


void CGidrView::OnDoIt()
{
    if (readonly_version) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (no_raschet) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!isRegim()) {
        AfxMessageBox(_TR("У вас нет прав на расчет режима"), MB_OK | MB_ICONINFORMATION);
        return;
    }


    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    int fileID = m_cxema->m_cur_file->id;

    m_regim_ras = 3;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CParam2 param(this);
    if (param.DoModal() != IDOK) return;

    CString str = getDoIt(param, pDoc, fileID);

    if (m_parent_id) m_parent_id_old = m_parent_id;

    m_CurLine = NULL;
    m_CurNode = NULL;

    m_cur_file_name = m_cxema->m_cur_file->name;

    RunDoIt(str, _TR("Расчет фактического режима"));
}




void CGidrView::OnEndDoIt()
{
    if (!reopen()) return;

    m_cxema->m_bIsGidrInf = true;
    //  m_cxema->m_graph->reset();
    //  m_napr1 = NULL;
    checkPjezo();
    InvalidateG();
    GetDocument()->SetModifiedFlag(true);

    if (m_regim_ras == 256)
    {
        if (m_exit_code) {
            AfxMessageBox(_TR("Расчет тепловых потерь не производится.\nСмотрите файл протокола"), MB_OK | MB_ICONINFORMATION);
        }
        else {
            AfxMessageBox(_TR("Расчет тепловых потерь закончен"), MB_OK | MB_ICONINFORMATION);
            PostMessage(WM_COMMAND, ID_EXCEL3);
        }
        return;
    }

    if (m_regim_ras == 257)
    {
        AfxMessageBox(_TR("Расчет тепловых потерь закончен"), MB_OK | MB_ICONINFORMATION);
        PostMessage(WM_COMMAND, ID_EXCEL4);
        return;
    }

    /*
      if (m_regim_ras == 2 && rezultat) {
        try {
          excel(AfxGetApp()->GetProfileString(szSection, "path_excel", ""));
        }
        catch(...) {
        }
      }
    */
}


void CGidrView::OnEndDoIt2()
{
    if (!reopen_all()) return;

    m_cxema->m_bIsGidrInf = true;

    checkPjezo();
    InvalidateG();
    GetDocument()->SetModifiedFlag(true);

    AfxMessageBox(_TR("Расчет закончен"), MB_OK | MB_ICONINFORMATION);
}




CString getDoIt(CParam2& param, Cgid6Doc* pDoc, int fileID);


double getLastCalcTn(CAdoFile *ado, int fileID)
{
    CString q;
    q.Format("SELECT Tn FROM CALCULATION WHERE id IN (SELECT max(id) FROM CALCULATION WHERE fileID=%d)", fileID);
    
    double Tn = -300;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            Tn = ado->read_double("Tn");
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return Tn;
}



CString getDoItDr(CParam1& param, Cgid6Doc* pDoc, CCxema* cxema, int fileID, bool is_mag = false, bool is_dross = true)
{
    CString s, str, par = "", qq;


    if (is_dross && param.m_dross) par += "-dross_yes ";
    if (param.m_avtomat) par += "-avtomat_yes ";

    double Tn = param.m_Tn;

//    if (is_mag) {
//        Tn = cxema->m_Tn_ot;
//    }

    str.Format("\"%s%s\" "
        " -type_of_net %d -server \"%s\" -database \"%s\" -user \"%s\" -password \"%s\" -port %d -fileID %d "
        " %s -iter %d -dross -Tn %g -tp_metod %d -trtp %d",
        argpath(), sety4,

        pDoc->m_type_of_net,

        pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_user, pDoc->m_password, pDoc->m_port, fileID,

        par, param.m_kol_iter, Tn, param.m_metod, param.m_trtp);

    if (is_dross && !is_mag) {
        if (param.m_sav_off) {
            str += " -soff";
        }
        if (param.m_sav_on) {
            str += " -son";
        }
    }

    if (param.m_mag_fn != "") {
        qq.Format(" -mn \"%s\"", param.m_mag_fn);
        str += qq;
    }

    if (param.m_zn0) {
        str += " -zulu_zn0";
    }
    if (param.m_utechki) {
        str += " -zulu_utechki";
    }

    if (param.m_char_sety) str += " -char_sety";

    if (!param.m_teplovyd) {
        str += " -no_teplovyd";
    }

    if (param.m_uf_calc) {
        str += " -uf_calc";
    }


    CString ss = AfxGetApp()->GetProfileString(szSection, "dH_zero", "25");
    double dH_zero = atof(ss);
    ss.Format(" -dH_zero %g", dH_zero);
    str += ss;


    if (param.m_ur == 1) {
        str += " -tg";

        if (!param.m_teplopoter) {
            str += " -no_teplopoter";
        }
        if (is_dross && !is_mag) {
            if (param.m_uf_new) {
                if (AfxMessageBox(_TR("Сохранить коэффициенты смешения?"), MB_YESNO) == IDYES) {
                    str += " -save_uf_new";
                }
            }

        }
    }

    char szDT[256];

    time_t ltime;
    time(&ltime);
    struct tm* newtime = gmtime(&ltime);
    strftime(szDT, 128, "\"%Y-%m-%d %H:%M:%S\"", newtime);
    str += " -time ";
    str += szDT;

    if (param.m_name != "") {
        str += " -name \"";
        str += param.m_name;
        param.m_name.Replace("\"", "'");
        str += "\"";
    }

    if (param.m_leto) {
        str += " -leto";
    }

    if (param.m_mag_fragment) {
        str += " -mag_fragment";
    }

    if (param.m_veter) {
        str += " -veter";
    }

    m_reread_sety = false;
    if (is_dross && !is_mag) {
        if (param.m_save_po) {
            m_reread_sety = true;
            str += " -save_po";
        }
    }

    if (!param.m_kv) {
        str += " -no_kv";
    }

    if (param.m_nagr) {
        s.Format(" -nagr %d", param.m_nagr);
        str += s;
    }

    s.Format(" -user_gid \"%s\"", pDoc->m_user_gid);
    str += s;


    map <CString, double>::const_iterator it = param.map_t_ist.begin();

    for (; it != param.map_t_ist.end(); ++it) {
        s.Format(" -Tist %s %g", it->first, it->second);
        str += s;
    }

//    if (is_mag) {
//        str += " -no_out";
//    }

    return str;
}

bool ThereIsAvto(const CGraph2* graph)
{
    CMapGraph::const_iterator pp = graph->map_node.begin();

    while (pp != graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.pr_avtomat == 'А') {
            return true;
        }
        pp++;
    }
    return false;
}



void CGidrView::OnDoItDr1()
{
    if (readonly_version || no_raschet) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!isRegim()) {
        AfxMessageBox(_TR("У вас нет прав на расчет режима"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    m_regim_ras = 1;

    m_regim_geo = -1;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CParam1 param(this);

    if (param.DoModal() != IDOK) return;

    int fileID = m_cxema->m_cur_file->id;

    CString str = getDoItDr(param, pDoc, m_cxema, fileID);

    m_CurLine = NULL;
    m_CurNode = NULL;

    CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();

    CString s = "При наличии автоматизированных тепловых пунктов, расчет дроссельных устройств "
        "рекомендуется производить при температуре наружного воздуха, соответствующего "
        "точке излома эксплуатационного температурного графика качественного реулирования "
        "системы теплоснабжения.";

    //    CString s1 = "При наличии автоматизированных тепловых пунктов, расчет дроссельных устройств "
    //        "рекомендуется производить при температуре наружного воздуха, соответствующего "
    //        "точке излома эксплуатационного температурного графика качественного реулирования "
    //        "системы теплоснабжения.\nРасчет дроссельных устройств выполняется только для неавтоматизированных тепловых пунктов.";

    CString s1 = "Для автоматизированных потребителей расчет дроссельных устройств не производится";

    //    CString s3 = "Расчет дроссельных устройств выполняется только для неавтоматизированных тепловых пунктов.";

    CString s2 = "Расчет автоматизированных тепловых пунктов выполняется только для режима с учетом тепловых потерь в сети.\nДля заданного режима расчет автоматизированных тепловых пунктов не выполнятся.";

    if (ThereIsAvto(m_cxema->m_graph)) {
        CString s = "";
        if ((param.m_sav_off || param.m_sav_on || param.m_dross) && !param.m_avtomat) {
            //            s += s3;
        }
        if ((param.m_sav_off || param.m_sav_on || param.m_dross) && param.m_avtomat) {
            s += s1;
        }

        if ((param.m_avtomat && (param.m_ur == 0 || !param.m_teplopoter))) {
            if (s != "") s += "\n\n";
            s += s2;
            param.m_avtomat = FALSE;
        }
        if (s != "" && AfxMessageBox(s, MB_OKCANCEL | MB_ICONINFORMATION) != IDOK) {
            return;
        }
    }

    RunDoIt(str, _TR("Расчет планового режима"));

}

#include "Filtr3.h"


void CGidrView::OnDoItDrList()
{
    CString sect;
    CAdoFile* ado = getAdo(getGidAdoName());

    sect.Format("Filtr\\%s", ado->m_schema);

    if (readonly_version || no_raschet) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!isRegim()) {
        AfxMessageBox(_TR("У вас нет прав на расчет режима"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CParam1 param(this);

    if (param.DoModal() != IDOK) return;

    CFiltr3 dlg(this, "Выберите фрагменты для расчета");

    for (auto it : m_cxema->map_fragments) {
        bool on = AfxGetApp()->GetProfileInt(sect, it.second.m_name, 0);
        dlg.Add(it.second.m_name, it.second.m_id, on);
    }

    if (dlg.DoModal() != IDOK) return;

    CString tmpName;
    tmpName.Format("%s\\sety.bat", getenv("TMP"));

    FILE* f = fopen(tmpName, "w");

    if (f) {
        for (auto it : dlg.map_item) {
            AfxGetApp()->WriteProfileInt(sect, it.second.name, it.second.on);
            if (it.second.on) {
                CString str = getDoItDr(param, pDoc, m_cxema, it.second.id);
                str.AnsiToOem();
                CString ss;

                ss.Format("Расчет фрагмента %s", it.second.name);
                ss.AnsiToOem();

                fprintf(f, "@echo '%s'\n", ss);
                fprintf(f, "@%s -save_po_yes\n", str);
            }
        }
        fclose(f);

        bool yes = true;

        if (param.m_save_po) {
            yes = false;
            if (AfxMessageBox(_TR("Сохранить нагрузки в обобщенных потребителях?"), MB_YESNO) == IDYES) yes = true;
        }


        if (yes) {
            Run(this, tmpName, argpath_2() + "protocol.txt", ID_END_DOIT2);
        }
    }
}

void CGidrView::OnDoItList()
{
    CString sect;
    CAdoFile* ado = getAdo(getGidAdoName());
    sect.Format("Filtr\\%s", ado->m_schema);

    if (readonly_version || no_raschet) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CParam2 param(this);

    if (param.DoModal() != IDOK) return;

    CFiltr3 dlg(this, "Выберите фрагменты для расчета");

    for (auto it : m_cxema->map_fragments) {
        bool on = AfxGetApp()->GetProfileInt(sect, it.second.m_name, 0);
        dlg.Add(it.second.m_name, it.second.m_id, on);
    }

    if (dlg.DoModal() != IDOK) return;

    CString tmpName;

    tmpName.Format("%s\\sety.bat", getenv("TMP"));

    FILE* f = fopen(tmpName, "w");

    if (f) {
        for (auto it : dlg.map_item) {
            AfxGetApp()->WriteProfileInt(sect, it.second.name, it.second.on);
            if (it.second.on) {
                CString str = getDoIt(param, pDoc, it.second.id);
                str.AnsiToOem();
                CString ss;

                ss.Format("Расчет фрагмента %s", it.second.name);
                ss.AnsiToOem();

                fprintf(f, "@echo '%s'\n", ss);
                fprintf(f, "@%s -save_po_yes\n", str);
            }
        }
        fclose(f);

        bool yes = true;

        if (param.m_save_po) {
            yes = false;
            if (AfxMessageBox(_TR("Сохранить нагрузки в обобщенных потребителях?"), MB_YESNO) == IDYES) yes = true;
        }


        if (yes) {
            Run(this, tmpName, argpath_2() + "protocol.txt", ID_END_DOIT2);
        }
    }
}



static int rezultat;

void CGidrView::OnDoItAnal()
{
    AfxMessageBox(_TR("Временно удалено 503"), MB_OK | MB_ICONINFORMATION);
}


#include "json.h"


CString write_param1(CParam1 &param)
{
    JSON json;

    json.add("{");
    json.add("avar", "0");

/*    

json("m_name", param1.m_name);
json("m_kol_iter", param1.m_kol_iter);
json("m_char_sety", param1.m_char_sety);
json("m_dross", param1.m_dross);
json("m_avtomat", param1.m_avtomat);
json("m_teplopoter", param1.m_teplopoter);
json("m_teplovyd", param1.m_teplovyd);
json("m_uf_calc", param1.m_uf_calc);

json("m_zn0", param1.m_zn0);
json("m_utechki", param1.m_utechki);


json("m_Tn", param1.m_Tn);
json("m_ur", param1.m_ur);
json("m_fiks", param1.m_fiks);
json("m_ekspl", param1.m_ekspl);
json("m_n_fil", param1.m_n_fil);
json("m_sav_off_sys", param1.m_sav_off_sys);
json("m_sav_on_sys", param1.m_sav_on_sys);
json("m_veter", param1.m_veter);
json("m_sav_off", param1.m_sav_off);
json("m_sav_on", param1.m_sav_on);
json("m_mag_fn", param1.m_mag_fn);
json("m_save_po", param1.m_save_po);
json("m_uf_new", param1.m_uf_new);
json("m_leto", param1.m_leto);
json("m_kv", param1.m_kv);
json("m_mag_fragment", param1.m_mag_fragment);
json("m_metod", param1.m_metod);
json("m_trtp", param1.m_trtp);
json("m_nagr", param1.m_nagr);

*/
    json.add("}");


        return json.str();
}

bool AvtomatParam(CParam1 &param, CCxema *m_cxema)
{
    CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();

    CString s = "При наличии автоматизированных тепловых пунктов, расчет дроссельных устройств "
        "рекомендуется производить при температуре наружного воздуха, соответствующего "
        "точке излома эксплуатационного температурного графика качественного реулирования "
        "системы теплоснабжения.";

    //    CString s1 = "При наличии автоматизированных тепловых пунктов, расчет дроссельных устройств "
    //        "рекомендуется производить при температуре наружного воздуха, соответствующего "
    //        "точке излома эксплуатационного температурного графика качественного реулирования "
    //        "системы теплоснабжения.\nРасчет дроссельных устройств выполняется только для неавтоматизированных тепловых пунктов.";

    CString s1 = "Для автоматизированных потребителей расчет дроссельных устройств не производится";

    //    CString s3 = "Расчет дроссельных устройств выполняется только для неавтоматизированных тепловых пунктов.";

    CString s2 = "Расчет автоматизированных тепловых пунктов выполняется только для режима с учетом тепловых потерь в сети.\nДля заданного режима расчет автоматизированных тепловых пунктов не выполнятся.";

    if (ThereIsAvto(m_cxema->m_graph)) {
        CString s = "";
        if ((param.m_sav_off || param.m_sav_on || param.m_dross) && !param.m_avtomat) {
            //            s += s3;
        }
        if ((param.m_sav_off || param.m_sav_on || param.m_dross) && param.m_avtomat) {
            s += s1;
        }

        if ((param.m_avtomat && (param.m_ur == 0 || !param.m_teplopoter))) {
            if (s != "") s += "\n\n";
            s += s2;
            param.m_avtomat = FALSE;
        }
        if (s != "" && AfxMessageBox(s, MB_OKCANCEL | MB_ICONINFORMATION) != IDOK) {
            return false;
        }
    }

    return true;
}

int getFileID2(CParam1 &param, CCxema *m_cxema)
{
    if (!param.m_mag_fragment) return 0;

    int fileID = m_cxema->m_cur_file->id;

    auto it = m_cxema->m_map_fileID_mag.find(fileID);
    if (it != m_cxema->m_map_fileID_mag.end()) {
        int fileID2 = it->second;
        if (fileID != fileID2) {
            return fileID2;
        }
    }
    return 0;
}


void CGidrView::OnDoItDr()
{
    if (readonly_version || no_raschet) {
        AfxMessageBox(_TR("Версия только для просмотра"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!isRegim()) {
        AfxMessageBox(_TR("У вас нет прав на расчет режима"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    m_regim_ras = 1;

    m_regim_geo = -1;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CParam1 param(this);
    if (param.DoModal() != IDOK) return;

    int fileID = m_cxema->m_cur_file->id;

    CString str_param = getDoItDr(param, pDoc, m_cxema, fileID);

    m_CurLine = NULL;
    m_CurNode = NULL;

    if (!AvtomatParam(param, m_cxema)) return;

    int fileID2 = getFileID2(param, m_cxema);

    bool is_mag = false;

    CString fileName = "";

    CString tmpName1, tmpName2, batName;

    batName.Format("%s\\sety.bat", getenv("TMP"));

    if (fileID2) {
        CGidFile* gid = m_cxema->getGidFile(fileID2);

        if (!gid) {
            CString q;
            q.Format("SELECT TOP 1 name FROM fragments WHERE id=%d", fileID);
            fileName = readTableValue(m_cxema->m_ado, q);
        }

        if (gid) {
            fileName = gid->name;
        }

        m_fileID2 = fileID2;

        GLOBAL* global = get_calc_param_global_id(fileID2);
        if (global) {
            double Tn = global->Tn;
            CString ss;
            ss.Format(" -Tn_mag_old %g", Tn);
            str_param += ss;
        }
//        is_mag = true;

        CString str;

        str.Format("Выполнить расчет магистрального фрагмента \"%s\" для заданной температуры наружного воздуха?", fileName);
        int ret = AfxMessageBox(str, MB_YESNOCANCEL | MB_ICONINFORMATION);
//        int ret = AfxMessageBox(str, MB_OKCANCEL | MB_ICONINFORMATION);
        if (ret == IDCANCEL) {
            return;
        }

        if (ret == IDYES) {
            is_mag = true;
        }
    }


    if (param.m_plan) {
        if (AfxMessageBox(_TR(
"Сохранить текущий режим в качестве планового расчетного режима ?\n"
"Примечание: результаты планового расчетного режима магистрального фрагмента используется при расчете режимов внутриквартальных сетей.\n"
        
        ), MB_YESNO) == IDYES) {
            str_param += " -plan";
        }
        else {
            return;
        }
    }

    if (!is_mag) {
        RunDoIt(str_param, _TR("Расчет планового режима"));
        return;
    }


    CString par_mag_1 = getDoItDr(param, pDoc, m_cxema, fileID2, true, false);



    GLOBAL *global = get_calc_param_global_id(fileID2);

    CParam1 param_mag(this, global, param.m_Tn);

    if (param_mag.DoModal() != IDOK) return;

    CString str_param2 = getDoItDr(param_mag, pDoc, m_cxema, fileID2, false, false);

    FILE* f = fopen(batName, "w");

    if (f) {
        fprintf(f, "@echo off\n");

        fprintf(f, "echo ђ бзҐв ¬ ЈЁбва «м­®Ј® да Ј¬Ґ­в \n");
        str_param2.AnsiToOem();
        fprintf(f, "%s\n", str_param2);
//        fprintf(f, "echo R\n");
        fprintf(f, "echo ђ бзҐв Єў ав «м­®Ј® да Ј¬Ґ­в \n");
        str_param.AnsiToOem();
        fprintf(f, "%s\n", str_param);

        fclose(f);

        RunDoIt(batName, _TR("Расчет планового режима"));
    }
}

void CGidrView::OnSetMinPerepad()
{
    CString text = "";

    double dH_zero = 25.;

    text.Format("%g", dH_zero);
    text = AfxGetApp()->GetProfileString(szSection, "dH_zero", "");

    if (text == "0") text = "";

//    CString s = "Введите минимальный перепад";
    CString s = "Располагаемый перепад для расчета Гидравлического сопротивления потребителей, присоединенных в узлах с \"перехлестом\" пьезометра, м";

    CInputDialog inD(AfxGetMainWnd( ), _T(""), s, text, IDD_INPUTDIALOG2);

    if (inD.DoModal() != IDOK) return;

    text = inD.m_strInput;

    if (text != "") {
        dH_zero = atof(text);

        text.Format("%g", dH_zero);
        if (dH_zero <= 0) {
            AfxMessageBox("Недопустимо вводить значение \"Минимальный перепад\" меньше или равным нулю");
            text = "";
        }
    }

    AfxGetApp()->WriteProfileString(szSection, "dH_zero", text);
}
