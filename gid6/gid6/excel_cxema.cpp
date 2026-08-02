#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"
#include "MainFrm.h"

#include "gid6Doc.h"
#include <math.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>

#include "mmenu.h"
#include "win.h"

#include "ado.h"


#include "excel.h"

void AtoN(const char* A, int& r, int& c);

CString readSql(const char* path, const char* fn, bool include, char* xn, int& nline, int& ntab, char* ist)
{
    CString strQ = "", qq;
    char q[2550], s[256];

    FILE* f;

    sprintf(s, "%s/%s", path, fn);


    CString GetPath(CString path);

    CString path1 = GetPath(s);

    //  CString qq1;
    //  qq1.Format("%s\n%s\n%s", path, s, path1);
    //  AfxMessageBox(qq1, MB_OK|MB_ICONINFORMATION);

    f = fopen(s, "r");

    if (f) {
        fgetstr(q, 2550, f);

        for (int i = 0; q[i + 1]; i++) {
            if (q[i] == '/' && q[i + 1] == '/') {
                q[i] = 0;
                break;
            }
        }

        strQ = q;
        strQ += " ";

        if (q[0] == '#' || include) {
            if (!include) strQ = "";
            while (!feof(f)) {
                fgetstr(q, 2550, f);
                qq = q;
                qq.TrimLeft();
                if (q[0] == '#') {
                    if (q[1] == 'i') {
                        char x1[256], ist[256];
                        int n1, n2;
                        qq = readSql(path1, &q[strlen("#include ")], true, x1, n1, n2, ist);
                    }
                    else {
                        break;
                    }
                }
                strQ += qq;
                strQ += " ";
            }
        }

        if (!include) {
            fgetstr(xn, 255, f);
            fgetstr(s, 255, f);

            sscanf(s, "%d", &nline);

            if (sscanf(s, "%d %d", &nline, &ntab) != 2) {
                ntab = -1;
            }
            fgetstr(s, 255, f);
            sscanf(s, "%s", ist);
        }

        fclose(f);
    }

    strQ.TrimLeft();

    return strQ;
}

long getCountTable2(CAdoFile* ado, const char* q);

CString GetName(const char* path);

CString NtoA(int n);


static double m_sum[10][256];

static CString m_str_sum = "";

void InitExcelSum()
{
    m_str_sum = "";
}

void InitExcelSum0()
{
    for (int num = 0; num < 10; num++) {
        for (int k = 0; k < 256; k++) {
            m_sum[num][k] = 0;
        }
    }
}


void InitExcelSum(int num)
{
    for (int k = 0; k < 256; k++) {
        m_sum[num][k] = 0;
    }
}

double to_double(COleVariant& var);


void AddExcelSum(int c, int* summ, COleVariant varValue)
{
    double d = to_double(varValue);

    for (int i = 0; summ[i]; i++) {
        if (summ[i] == c) {
            for (int num = 0; num < 10; num++) {
                m_sum[num][i] += d;
            }
            break;
        }
    }
}



void ExcelSum(CExcel& ex, int r, int r_first, int r_last, int nline, int* summ)
{
    CString s;

    for (int i = 0; summ[i]; i++) {
        int col = summ[i];
        CString B = NtoA(col);

        s.Format("=СУММ(%s%d:%s%d)", B, r_first + nline, B, r_last + nline);
        ex.set_arr_str(CRange(r, col), s);
    }

    s.Format("$%d:$%d", r_first + nline, r_last + nline);
    if (m_str_sum != "") m_str_sum += ";";
    m_str_sum += s;
}


void ExcelSumAll(CExcel& ex, int r, int* summ)
{
    for (int i = 0; summ[i]; i++) {
        CString s;
        int col = summ[i];
        CString B = NtoA(col);

        s.Format("=СУММ(%s)", m_str_sum);
        s.Replace("$", B);
        ex.set_arr_str(CRange(r, col), s);
    }
}

void ExcelItogo(int num, CExcel& ex, int r, int* summ)
{
    if (summ && summ[0]) {
        ex.set_arr_str(CRange(r, 1), _TR("Итого"));

        for (int i = 0; summ[i]; i++) {
            int col = summ[i];
            COleVariant val(m_sum[num][i]);
            ex.set_arr(CRange(r, col), val);
        }
    }
}



void ExcelQ2_MAG_n(int nCol0, CAdoFile* ado, CExcel& ex, const char* strQ, const char* xls_file, int nline, int ntab, int* summ)
{
    InitExcelSum0();

    int nRow = getCountTable2(ado, strQ);

    if (ado->openTable0(strQ)) {
        if (!ex.IsOpen()) {
            ex.open(xls_file);
        }
        CString typs_old = "e334354t5";

        COleVariant v_old[256];


        int r_first = -1;
        int r_last = -1;

        if (ex.IsOpen()) {
            int nCol = ado->NFlds();

            nRow += 150;

            ex.setTab(ntab);
            ex.init_arr(nRow, nCol - nCol0);

            int r = 0;

            while (!ado->isEOF()/* && r < nRow*/) {

                CString typs = ado->readStr(0);

                for (int c = 0; c < nCol0; c++) {
                    COleVariant varValue = ado->read(c);

                    CString s1 = CCrack::strVARIANT2(varValue);

                    CString s2 = CCrack::strVARIANT2(v_old[c]);

                    if (s1 != s2) {

                        if (r > 0) {
                            for (int c0 = nCol0 - 1; c0 >= c; c0--) {
                                r++;
                                ExcelItogo(c0, ex, r, summ);
                            }
                            r++;
                        }

                        for (; c < nCol0; c++) {
                            InitExcelSum(c);

                            COleVariant varValue = ado->read(c);

                            v_old[c] = varValue;
                            r++;
                            ex.set_arr(CRange(r, 1), varValue);
                        }
                    }
                }

                r++;
                for (int c = nCol0; c < nCol; c++) {
                    COleVariant varValue = ado->read(c);
                    ex.set_arr(CRange(r, c - nCol0 + 1), varValue);
                    AddExcelSum(c - nCol0 + 1, summ, varValue);
                }
                ado->MoveNext();
            }

            for (int c0 = nCol0 - 1; c0 >= 0; c0--) {
                r++;
                ExcelItogo(c0, ex, r, summ);
            }
            r++;


            while (r < nRow) {
                r++;
                for (int c = 0; c < nCol; c++) {
                    ex.set_arr_str(CRange(r, c), "");
                }
            }


            ex.example2(CRange(nline, 0), ntab);
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
}





void ExcelQ2_MAG(CAdoFile* ado, CExcel& ex, const char* strQ, const char* xls_file, int nline, int ntab, int* summ)
{
    InitExcelSum();

    int nRow = getCountTable2(ado, strQ);

    if (ado->openTable0(strQ)) {
        if (!ex.IsOpen()) {
            ex.open(xls_file);
        }
        CString typs_old = "e334354t5";

        int r_first = -1;
        int r_last = -1;

        if (ex.IsOpen()) {
            int nCol = ado->NFlds();

            ex.setTab(ntab);
            ex.init_arr(nRow + 0 + 150, nCol - 1);

            int r = 0;

            while (!ado->isEOF()/* && r < nRow*/) {

                CString typs = ado->readStr(0);

                if (typs != typs_old) {
                    typs_old = typs;
                    if (r > 0) {
                        ExcelSum(ex, r + 1, r_first, r, nline, summ);
                        r += 2;
                    }
                    r++;
                    COleVariant varValue = ado->read(0);
                    ex.set_arr(CRange(r, 1), varValue);
                    r_first = r + 1;
                }

                r++;
                for (int c = 1; c < nCol; c++) {
                    COleVariant varValue = ado->read(c);
                    ex.set_arr(CRange(r, c), varValue);
                }
                ado->MoveNext();
            }

            ExcelSum(ex, r + 1, r_first, r, nline, summ);
            ExcelSumAll(ex, r + 2, summ);


            while (r < nRow + 0 + 20) {
                r++;
                for (int c = 0; c < nCol; c++) {
                    ex.set_arr_str(CRange(r, c), "");
                }
            }

            ex.example2(CRange(nline, 0), ntab);
        }
        ado->closeTable();


    }
    else {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
}



void ExcelQ2(CAdoFile* ado, CExcel& ex, const char* strQ, const char* xls_file, int nline, int ntab, list<CDop> *list_dop)
{
    int nRow = getCountTable2(ado, strQ);

    if (ado->openTable0(strQ)) {
        if (!ex.IsOpen()) {
            ex.open(xls_file);
        }

        if (ex.IsOpen()) {

            int nCol = ado->NFlds();

            ex.setTab(ntab);

            if (list_dop) {
                for (auto it : *list_dop) {
                    ex.init_arr(1, nCol);
                    ex.set_arr_str(CRange(1, 1), it.name);
                    ex.example2(CRange(it.r-1, it.c-1), ntab);
                }
            }



            if (nRow == 0) {
                ex.init_arr(1, nCol);
            }
            else {
                ex.init_arr(nRow + 0 + 20, nCol);
            }



#if 1
            int r = 0;

            while (!ado->isEOF() && r < nRow) {
                r++;
                for (int c = 0; c < nCol; c++) {
                    COleVariant varValue = ado->read(c);
                    ex.set_arr(CRange(r, c + 1), varValue);
                }
                ado->MoveNext();
            }

            for (int c = 0; c < nCol; c++) {
                CAdoField fld = ado->Field(c);
                ex.set_typ2(CRange(nline + 1, c + 1, nRow + 0 + 20, c + 1), fld.typ);
            }
#endif
            ex.example2(CRange(nline, 0), ntab);
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
}

CString getTableNameFromSQL(CString q);
CString getFullName(CString cx, CString tn, CString fn);


void ExcelQ1(CAdoFile* ado, CExcel& ex, const char* strQ, const char* tn0, list<int>& list_col, list<CString>& list_col_name)
{
    int ntab = 0;
    int nline = 1;
    int nRow = getCountTable2(ado, strQ);

    CString tn = getTableNameFromSQL(strQ);
    CString fn = ado->FieldName(0);
    if (tn == "") {
        tn = tn0;
    }

    MySQLColumns* col = ado->findMySQLColumns(tn, fn);

    if (ado->openTable0(strQ)) {

        if (ex.IsOpen()) {
            int nCol0 = ado->NFlds();
            int nCol = list_col.size();

            ex.init_arr(nRow + 0 + 20, nCol);

            int r = 0;

            list<int>::const_iterator it = list_col.begin();
            list<CString>::const_iterator itS = list_col_name.begin();

            for (int c = 0; c < nCol; c++) {
                int cc = *it++;
                CString ss = *itS++;

                CString s;
                s.Format("%d", c + 1);

                CAdoField fld = ado->Field(cc);
                CString fn = fld.name;

                //                s = getFullName(ado->m_schema, tn, fn);
                s = ss;

                //                s = ado->getFullName1(fn);

                //                ex.set_hv_align(CRange(1, c + 1), 0, 5);

                ex.set_hv_align(CRange(1, c + 1), -4108, 2);
                ex.set_val(CRange(1, c + 1), L"WrapText", COleVariant(1L));


                ex.set_border(CRange(1, c + 1), 5);

                ex.set_bold(CRange(1, c + 1));


                ex.set_arr_str(CRange(1, c + 1), s);
                //                ex.set_typ(CRange(1, c + 1, nRow + 0 + 20, c+1), fld.typ);
            }

            r++;

            int rr = 0;

            while (!ado->isEOF() && rr < nRow) {
                rr++;
                r++;
                list<int>::const_iterator it = list_col.begin();
                for (int c = 0; c < nCol; c++) {
                    int cc = *it++;
                    COleVariant varValue = ado->read(cc);
                    ex.set_arr2(CRange(r, c + 1), varValue);
                }
                ado->MoveNext();
            }


            it = list_col.begin();

            for (int c = 0; c < nCol; c++) {
                int cc = *it++;
                CAdoField fld = ado->Field(cc);
                ex.set_typ2(CRange(1, c + 1, nRow + 0 + 20, c + 1), fld.typ);
            }

            //            ex.example2(CRange(nline, 0), -1);
            ex.example2(CRange(0, 0), -1);
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
    }
}



void CCxema::ExcelQ(CAdoFile* ado, CExcel& ex, const char* strQ, const char* xn, int nline, int ntab, const char* fn, list<CDop> *list_dop)
{
    CString xn1, xls_file;
    xn1.Format("%sExcel2\\%s", argpath(), xn);
    xls_file.Format("%s\\%s", getenv("TMP"), GetName(xn));

    CopyFile(xn1, xls_file, 0);

    ExcelQ2(ado, ex, strQ, xls_file, nline, ntab, list_dop);
}

int getOutID(CAdoFile* ado, int fileID);


bool create_vyd_line_table_new_simple(CAdoFile* ado, CGraph2* graph, CString& tn);
bool create_vyd_line_table_new_all(CAdoFile* ado, CGraph2* graph, CString& tn, int fileID);
bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big);
bool create_vyd_node_table_BIG_all(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big, int fileID);

void CCxema::Excel2List(const char* path, const char* fn)
{
    int fileID = m_cur_file ? m_cur_file->id : 0;

    CAdoFile* ado = m_ado;

    if (!ado) {
        return;
    }

    if (fileID <= 0) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    CString s, s_order;
    char sn[256], txt[256], nt[256], xn[256], dop[256]; //  , ist[256], xn1[256];
    FILE* f;

    sprintf(sn, "%s%s/%s", (LPCTSTR)argpath(), path, fn);

    //AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);

    f = fopen(sn, "r");

    if (f) {
        CExcel ex;
        //    Sleep(1000);

        fgetstr(xn, 255, f);

        dop[0] = 0;

        while (!feof(f)) {
            int nline = 0, ntab = -1; // , n1, n2;
            
            if (dop[0] != 0) {
                strcpy(txt, dop);
            }
            else {
                fgetstr(txt, 255, f);
            }
            fgetstr(nt, 255, f);


            int l;

            list<CDop> list_dop;

            do {
                fgetstr(dop, 255, f);
                vector<CString> match;

                l = regex_match("^DOP\\s+([A-Z0-9]+)\\s+(.+)$", dop, match);

                if (l > 0) {
                    CString a = match[1];
                    CDop d;

                    AtoN(a, d.r, d.c);

                    d.name = match[2];

                    if (d.name == "$out_name") {
                        if (m_Tn != -300) {
                            d.name = m_out_name;
                        }
                        else {
                            d.name = "";
                        }
                    }
                    else if (d.name == "$out_date") {
                        if (m_Tn != -300) {
                            d.name = m_out_date.Format(_T("%d.%m.%Y_"));
                        }
                        else {
                            d.name = "";
                        }
                    }
                    else if (d.name == "$out_time") {
                        if (m_Tn != -300) {
                            d.name = m_out_date.Format(_T("%H:%M"));
                        }
                        else {
                            d.name = "";
                        }
                    }
                    else if (d.name == "$out_rezhim") {
                        if (m_Tn != -300) {
                            if (m_out_name.Find("фактического") != -1) {
                                d.name = "Фактический";
                            }
                            else {
                                d.name = "Плановый";
                            }
                        }
                        else {
                            d.name = "";
                        }
                    }
                    else if (d.name == "$Tn") {
                        if (m_Tn != -300) {
                            d.name.Format("%g", m_Tn);
                        }
                        else {
                            d.name = "";
                        }
                    }
                    else if (d.name == "$TnZ") {
                        if (m_Tn != -300) {
                            d.name.Format("Tнаруж %g°С", m_Tn);
                        }
                        else {
                            d.name = "";
                        }
                    }

                    list_dop.push_back(d);

                    dop[0] = 0;
                }
            } while (l > 0);

//            if (feof(f)) break;

            if (txt[0] == 0 || txt[0] == '-') continue;

            sscanf(nt, "%d", &nline);

            if (sscanf(nt, "%d %d", &nline, &ntab) != 2) {
                ntab = -1;
            }

            CString fn;

            fn.Format("%s%s\\%s", argpath(), path, txt);

            //          CString strQ = readSql(argpath()+path, txt, true, xn1, n1, n2, ist);

            CString strQ = readFile(fn), ss, vyd;

            if (strQ != "") {
                if (m_graph->isMark()) {
                    if (strQ.Find("n1.fileID=$fileID$") != -1) {
                        create_vyd_line_table_new_simple(ado, m_graph, vyd);

                        s.Format("join %s vyd ON vyd.id2=l.id WHERE", vyd);
                        strQ.Replace("WHERE n1.fileID=$fileID$ AND", s);
                        s_order.Format(" ORDER BY vyd.id1");
                        strQ += s_order;
                    }
                    else if (strQ.Find("n.fileID=$fileID$") != -1) {
                        CString s, s_order;
                        create_vyd_node_table_BIG(ado, m_graph, vyd, false);

                        s.Format("join %s vyd ON vyd.id2=n.id WHERE", vyd);
                        strQ.Replace("WHERE n.fileID=$fileID$ AND", s);
                        s_order.Format(" ORDER BY vyd.id1");
                        strQ += s_order;
                    }
                }
                else {
                    if (strQ.Find("n1.fileID=$fileID$") != -1) {
                        if (strQ.Find("heatPipeSections") != -1) {
                            create_vyd_line_table_new_all(ado, m_graph, vyd, fileID);

                            s.Format("join %s vyd ON vyd.id2=l.id WHERE n1.fileID=", vyd);
                            strQ.Replace("WHERE n1.fileID=", s);
                            s_order.Format(" ORDER BY vyd.id1");
                            strQ += s_order;
                        }
                    }
                    else if (strQ.Find("n.fileID=$fileID$") != -1) {
                        create_vyd_node_table_BIG_all(ado, m_graph, vyd, false, fileID);

                        s.Format("join %s vyd ON vyd.id2=n.id WHERE n.fileID=", vyd);
                        strQ.Replace("WHERE n.fileID=", s);
                        s_order.Format(" ORDER BY vyd.id1");
                        strQ += s_order;
                    }
                }

                if (fileID >= 0) {
                    ss.Format("%d", fileID);
                    strQ.Replace("$fileID$", ss);
                    int calculationID = getOutID(ado, fileID);
                    ss.Format("%d", calculationID);
                    strQ.Replace("$calculationID$", ss);
                }
            }

            //      void write_CT(CExcel &ex, CDbFile *m_dbf, int ntab);

            try {
                ExcelQ(ado, ex, strQ, xn, nline, ntab, fn, &list_dop);
                //        write_CT(ex, gid_file->m_dbf, ntab);
            }
            catch (...) {
            }
        }

        fclose(f);
    }
    //  ShowExcel();
}


int CCxema::isExcel2List(const char* path, const char* fn)
{
    return true;

    int n = 0;
    char s[256], txt[256], nt[256], xn[256], ist[256], xn1[256];
    FILE* f;

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

            CString strQ = readSql(argpath() + path, txt, true, xn1, n1, n2, ist);

            /*

                CString q = readFile(fn), ss;

                if (q != "") {
                  if (fileID >= 0) {
                    ss.Format("%d", fileID);
                    q.Replace("$fileID$", ss);
                  }
                }
            */



            /*

                  if (!strQ.IsEmpty() && gid_file->m_dbf->openTable(strQ)) {
                    n = gid_file->m_dbf->NRecs();
                    gid_file->m_dbf->closeTable();
                    if (n > 0) break;
                  }
            */
        }

        fclose(f);
    }

    return n > 0;


    return true;
}


void write(CExcel& ex, int ntab, int r, int c, const char* str)
{
    //  ex.init_arr(1, 1);
    ex.setTab(ntab);
    ex.set_str(CRange(1, 1), str);
    //  ex.example2(CRange(r, c), ntab);
}

void CCxema::Excel2Pot(CExcel& ex, const char* path, const char* fn, const char* kod_ist)
{
    CString str;
    CString strQ;
    char xn[256], ist[256];

    int nline = 0, ntab = -1;

    strQ = readSql(argpath() + path, fn, false, xn, nline, ntab, ist);

    strQ.Replace("$IST$", kod_ist);

    //  AfxMessageBox(strQ, MB_OK|MB_ICONINFORMATION);
    Sleep(1000);

    CAdoFile* ado = m_ado;

    ExcelQ(ado, ex, strQ, xn, nline, ntab, fn, nullptr);

    int r, c;

    AtoN(ist, r, c);

    if (ex.IsOpen()) {
        str.Format(_TR("Источник тепла : %s"), kod_ist);
        write(ex, ntab, r - 1, c - 1, str);
        //    write_CT(ex, gid_file->m_dbf, ntab);
    }
}

static map<string, string> map_pc;

BOOL getPC(const char* kod_rs, char* kod_ist)
{
    map<string, string>::const_iterator it;

    it = map_pc.find(kod_rs);

    if (it != map_pc.end()) {
        strcpy(kod_ist, it->second.c_str());
        return TRUE;
    }
    return FALSE;
}

