#include "stdafx.h"
#include "Graph.h"
#include "Graph2.h"

#include "ado.h"


#include "crack.h"
#include "Cxema.h"
#include "win.h"


struct NODE {
    CString uzel, pr;
    double x, y;

    bool operator<(const NODE& mc) const
    {
        if (uzel < mc.uzel) return true;
        if (uzel > mc.uzel) return false;

        if (pr < mc.pr) return true;
        if (pr > mc.pr) return false;

        return false;
    }
};


struct PARA {
    NODE n1, n2;
    int nomer_mesto;
    CString destination;
};

#if 0
void write(CDaoRecordset* rs, const char* fn, double v)
{
    if (v != 0) {
        char buf[256];
        rs->SetFieldValue(fn, COleVariant(atof(_gcvt(v, 7, buf))));
    }
}

void write(CDaoRecordset* rs, const char* fn, const char* v)
{
    try {
        COleVariant var;
        CString str(v);
        //    str.OemToAnsi();
        //    str.TrimRight();
        if (str.IsEmpty()) {
            rs->SetFieldValueNull(fn);
        }
        else {
            var = CCrack::Var(str, dbText);
            rs->SetFieldValue(fn, var);
        }
    }
    catch (CDaoException * e) {
        //    wout << fh << " - " ;
        //    wout << v << " " << (const char *) e->m_pErrorInfo->m_strDescription << endl;
        e->Delete();
    }
}

void writeLine(CDaoRecordset* rs, int n, const char* kod, const char* uzel, const char* p)
{
    char* nkod, * nuzel, * npr;

    switch (n) {
    case 0: nkod = "kod";   nuzel = "uzel";   npr = "pr";   break;
    case 1: nkod = "kod1";  nuzel = "uzel1";  npr = "pr1";  break;
    case 2: nkod = "kod2";  nuzel = "uzel2";  npr = "pr2";  break;
    case 3: nkod = "kod_p"; nuzel = "uzel_p"; npr = "pr_p"; break;
    }

    write(rs, nkod, kod);
    write(rs, nuzel, uzel);
    write(rs, npr, p);
}

#endif

static map<string, map<string, list<PARA> > > map_b5;
static map<string, set<NODE> >  map_n_b5;
static map<string, set<NODE> >  map_vp;

int read_cxema(const char* cxema, const char* name1, char p1, const char* name2, char p2)
{
    map<string, map<string, list<PARA> > >::const_iterator it;

    it = map_b5.find(string(cxema));

    if (it != map_b5.end()) {
        map<string, list<PARA> >::const_iterator it2;

        it2 = it->second.find(string(ST_ZD));
        if (it2 != it->second.end()) {
            list<PARA>::const_iterator it3;
            for (it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                const PARA* para = &*it3;
                if (
                    !strcmp(para->n1.uzel, name1) && !strcmp(para->n2.uzel, name2) && para->n1.pr[0] == p1 && para->n2.pr[0] == p2 ||
                    !strcmp(para->n1.uzel, name2) && !strcmp(para->n2.uzel, name1) && para->n1.pr[0] == p1 && para->n2.pr[0] == p1)
                {
                    if (para->nomer_mesto == 27) {
                        return 1;
                    }
                    else if (para->nomer_mesto == 49) {
                        return 1;
                    }
                    else if (para->nomer_mesto == 50) {
                        return 2;
                    }
                    else if (para->nomer_mesto == 39) {
                        return 2;
                    }
                    break;
                }
            }
        }
    }
    return 0;
}


struct S39 {
    double  d;
    double  dy;
    long    date;
    CString    proklad;
    double  tg;
    long    tn;
    double  t2;
    double  t1_1;
    double  t1_2;
    double  t1_3;
    double  t1_4;
    double  qp_1;
    double  qo_1;
    double  qp_2;
    double  qo_2;
    double  qp_3;
    double  qo_3;
    double  qp_4;
    double  qo_4;
    double  qp_1g5000;
    double  qo_1g5000;
    double  qp_2g5000;
    double  qo_2g5000;
    double  qp_3g5000;
    double  qo_3g5000;
    double  qp_4g5000;
    double  qo_4g5000;
};

#include "crack.h"

// Дата     Тип      Диаметр
static map<int, map< char, map<double, S39> > > map_s39;

#if 0

template <class T> void read(CDaoRecordset* rs, int fh, T& v)
{
    COleVariant varValue;

    rs->GetFieldValue(fh, varValue);

    switch (varValue.vt) {
    case  VT_I2:
        v = V_I2(&varValue);
        break;
    case  VT_I4:
        v = (T)V_I4(&varValue);
        break;
    case  VT_R8:
        v = (T)V_R8(&varValue);
        break;
    default:
        v = (T)0;
        break;
    }
}



void read(CDaoRecordset* rs, int fh, char* s)
{
    COleVariant varValue;

    rs->GetFieldValue(fh, varValue);

    lstrcpy(s, V_BSTRT(&varValue));
}

#endif

double get_q_S39(int year, char typ_pr, char po, double t1, double t2, double tn, double tg, bool kolvork, const S39& s39)
{
    double* tt = (double*)&s39.t1_1;
    double* qq;
    double tt1, tt2, propor, t;
    int i;

    if (kolvork) {
        qq = (double*)&s39.qp_1g5000;
    }
    else {
        qq = (double*)&s39.qp_1;
    }

    if (typ_pr == 'Н') {
        if (po == COBR) {
            t = t2;
        }
        else {
            t = t1;
        }
    }
    else {
        t = (t1 + t2) / 2;
    }

    if (year == 1) {
        if (typ_pr == 'Н') {
            t -= tn;
        }
        else {
            t -= tg;
        }
    }

    int n = 4;
    if (tt[2] == tt[3]) n = 3;
    if (tt[1] == tt[2]) n = 2;

    for (i = 0; i < n - 1; i++)
    {
        tt1 = tt[i];
        tt2 = tt[i + 1];

        if (year == 1) {
            if (typ_pr == 'Н') {
                tt1 -= s39.tn;
                tt2 -= s39.tn;
            }
            else {
                tt1 -= s39.tg;
                tt2 -= s39.tg;
            }
        }

        if (typ_pr != 'Н') {
            tt1 = (tt[i] + s39.t2) / 2;
            tt2 = (tt[i + 1] + s39.t2) / 2;
        }

        if (t <= tt2) {
            break;
        }
    }

    propor = (t - tt1) / (tt2 - tt1);

    int dp = (po == CPOD) ? 0 : 1;

    if (typ_pr == 'Н') {
        dp = 0;
    }

    double qP, qO;
    int i1 = i * 2;
    int i2 = (i + 1) * 2;

    qP = qq[i1] * (1. - propor) + qq[i2] * propor;
    qO = qq[i1 + 1] * (1. - propor) + qq[i2 + 1] * propor;


    if (typ_pr == 'Н') {
        return qP;
    }
    else {
        return (po == CPOD) ? qP : qO;
    }

    //  q1 = qq[i1+dp];
    //  q2 = qq[i2+dp];
    //  q = q1*(1.-propor)+q2*propor;
    //  return q;

}



double get_q_new(int year, char typ_pr, double diam_usl, double diam, char po, double t1, double t2, double tn, double tg, bool kolvork)
{
    S39 s39, s39_1;
    map<int, map< char, map<double, S39> > >::iterator it1 = map_s39.find(year);

    if (year != 1) diam = diam_usl;

    if (it1 != map_s39.end()) {
        map< char, map<double, S39> >::iterator it2 = it1->second.find(typ_pr);
        if (it2 != it1->second.end()) {
            double d, d1 = 0, propor;
            map<double, S39>::iterator it = it2->second.begin();

            s39 = it->second;
            d = max(s39.d, s39.dy);
            ++it;

            for (; it != it2->second.end(); ++it) {
                s39_1 = s39;
                d1 = d;
                s39 = it->second;

                d = max(s39.d, s39.dy);

                if (diam <= d) {
                    break;
                }
            }

            propor = (diam - d1) / (d - d1);

            double q1, q2;

            q1 = get_q_S39(year, typ_pr, po, t1, t2, tn, tg, kolvork, s39_1);
            q2 = get_q_S39(year, typ_pr, po, t1, t2, tn, tg, kolvork, s39);

            return q1 * (1. - propor) + q2 * propor;
        }
    }
    return 0.;
}

map<CString, map<CString, int> > map_act_columns;

int get_act_columns(CString tab, CString col)
{
    map<CString, map<CString, int> >::const_iterator it1 = map_act_columns.find(tab);
    if (it1 != map_act_columns.end()) {
        map<CString, int>::const_iterator it2 = it1->second.find(col);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }

    return -1;
}

void init_act_columns()
{
    CAdoFile* ado = getAdo("vsp");

    if (!ado) return;

    CString q = "SELECT id, tab, col FROM act_columns";

    if (ado->openTable0(q))
    {
        while (!ado->isEOF()) {
            CString tab = ado->readStr("tab");
            CString col = ado->readStr("col");
            int id = ado->read_long("id");

            map_act_columns[tab][col] = id;
            ado->MoveNext();
        }
    }
}

void open_vsp(const char* vsp)
{
    CString nomer_cx;
    CString name_enginery;

    map_b5.clear();
    map_n_b5.clear();
    map_vp.clear();

    CAdoFile* ado = getAdo("vsp");

    if (!ado) return;

    init_act_columns();

    CString b3 = transl("B3_Перечень оборудования схем");
    CString b4 = transl("B4_Место установки оборудования");
    CString b5 = transl("B5_Структура оборудования схемы");

    CString q;

    q.Format("SELECT nomer_cx, uzel1, pr1, uzel2, pr2, name_tabl, x, y, x2, y2, %s.nomer_mesto, destination "
        " FROM %s, %s, %s "
        " WHERE %s.nomer_mesto=%s.nomer_mesto AND %s.name_enginery = %s.name_enginery",
        b5,
        b5, b4, b3,

        b5, b4, b3, b4);

    if (ado->openTable0(q))

    {
        while (!ado->isEOF()) {
            //      ado->read(2, nomer_cx);
            //      ado->read(4, nomer_mesto);
            nomer_cx = ado->readStr("nomer_cx");
            name_enginery = ado->readStr("name_tabl");
            //            name_enginery = ado->readStr("newName");

                        //      sscanf(nomer_mesto, "%d", &nm);

            PARA para;

            para.n1.uzel = ado->readStr("uzel1");
            para.n2.uzel = ado->readStr("uzel2");
            para.n1.pr = ado->readStr("pr1");
            para.n2.pr = ado->readStr("pr2");

            para.n1.x = ado->read_double("x");
            para.n1.y = ado->read_double("y");
            para.n2.x = ado->read_double("x2");
            para.n2.y = ado->read_double("y2");
            para.nomer_mesto = ado->read_long("nomer_mesto");

            para.destination = ado->readStr("destination");

            if (para.n2.uzel[0]) {
                if (para.n1.pr[0] == ' ') {
                    para.n1.pr = para.n2.pr = POD;
                    map_b5[string(nomer_cx)][string(name_enginery)].push_back(para);
                    map_n_b5[string(nomer_cx)].insert(para.n1);
                    map_n_b5[string(nomer_cx)].insert(para.n2);
                    para.n1.pr = para.n2.pr = OBR;
                    map_b5[string(nomer_cx)][string(name_enginery)].push_back(para);
                    map_n_b5[string(nomer_cx)].insert(para.n1);
                    map_n_b5[string(nomer_cx)].insert(para.n2);
                }
                else {
                    map_b5[string(nomer_cx)][string(name_enginery)].push_back(para);
                    map_n_b5[string(nomer_cx)].insert(para.n1);
                    map_n_b5[string(nomer_cx)].insert(para.n2);
                }
            }
            if (name_enginery == _TR("Узел подпитки")) {
                map_vp[string(nomer_cx)].insert(para.n1);
            }
            ado->MoveNext();
        }
    }
}

struct S28 {
    double  pr;
    double  sm;
    double  ps;
    double  pw;
    double  o;
};

struct S30 {
    CString  pr;
    double  diametr;
    double  beta_mag;
    double  beta_rasp;
};

struct S07 {
    long    kod_izol;
    CString    name;
    double  L;
    double  K;
};

/**
struct S39 {
    double  d;
    double  dy;
    long    date;
    char    proklad[51];
    double  tg;
    long    tn;
    double  t2;
    double  t1_1;
    double  t1_2;
    double  t1_3;
    double  t1_4;
    double  qp_1;
    double  qo_1;
    double  qp_2;
    double  qo_2;
    double  qp_3;
    double  qo_3;
    double  qp_4;
    double  qo_4;
    double  qp_1g5000;
    double  qo_1g5000;
    double  qp_2g5000;
    double  qo_2g5000;
    double  qp_3g5000;
    double  qo_3g5000;
    double  qp_4g5000;
    double  qo_4g5000;
};
*/

static S28 s28;

static map<char, S30> map_s30;
static map<int, S07> map_s07;
//static map<int, map< char, map<double, S39> > > map_s39;

void open_vsp2(const char* vsp)
{
    CAdoFile* ado = getAdo("sprav");

    if (!ado) return;

    s28.pr = 1.15;
    s28.sm = 1.1;
    s28.ps = 1.25;
    s28.o = 1.;

    map_s07.clear();

    CString tn07 = "07_Теплоизоляционный материал";
    CString tn30 = "30_Коэффициенты местных тепловых потерь";
    CString tn28 = "28_Коэффициенты расчета балансовой нагрузки ГВ";
    CString tn39 = "39_Нормы тепловых потерь";

    CString q;
    q.Format("SELECT * FROM %s", transl(tn07));

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            S07 s07;

            s07.kod_izol = ado->read_long("kod_izol");
            s07.name = ado->readStr("name");
            s07.L = ado->read_double("L");
            s07.K = ado->read_double("K");

            map_s07[s07.kod_izol] = s07;
            ado->MoveNext();
        }
        ado->closeTable();
    }

    map_s30.clear();

    q.Format("SELECT * FROM %s", transl(tn30));

    if (ado->openTable0(q)) { // 30_Коэффициенты местных тепловых потерь
        char t[] = "БКН";
        int n = 0;
        while (!ado->isEOF()) {
            S30 s30;
            s30.pr = ado->readStr("pr");
            s30.diametr = ado->read_double("diametr");
            s30.beta_mag = ado->read_double("beta_magr");
            s30.beta_rasp = ado->read_double("beta_rasp");

            map_s30[t[n++]] = s30;
            ado->MoveNext();
        }
        ado->closeTable();
    }

    q.Format("SELECT * FROM %s", transl(tn28));

    if (ado->openTable0(q)) {  // 28_Коэффициенты расчета балансовой нагрузки ГВ

        if (!ado->isEOF()) {
            s28.pr = ado->read_double("pr");
            s28.sm = ado->read_double("sm");
            s28.ps = ado->read_double("ps");
            s28.pw = ado->read_double("pw");
            s28.o = ado->read_double("o");
        }
        ado->closeTable();
    }


    // Нормы тепловых потерь

    // Дата     Тип   kolvork   Диаметр
    //static map<int, map< char, map<double, S39> > > map_s39;
#if 0
    q.Format("SELECT * FROM %s", transl(tn39));

    map_s39.clear();

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            S39 s39;
            read_xx(ado->rs, &s39);
            double d = max(s39.d, s39.dy);
            map_s39[s39.date][s39.proklad[0]][d] = s39;
            ado->MoveNext();
        }
        ado->closeTable();
    }
#endif
}



void open_vsp3(const char* vsp)
{

    CAdoFile* ado = getAdo("sprav");

    if (!ado) return;

    map_s39.clear();

    CString tn = "39_Нормы тепловых потерь";
    CString q;
    q.Format("SELECT * FROM %s", transl(tn));

    ado->openTable0(q);
    while (!ado->isEOF()) {
        S39 s39;
        //      read_xx(ado->rs, &s39);

        s39.d = ado->read_double("d");
        s39.dy = ado->read_double("dy");
        s39.date = ado->read_long("date");
        s39.proklad = ado->readStr("proklad");
        s39.tg = ado->read_double("tg");
        s39.tn = ado->read_long("tn");
        s39.t2 = ado->read_double("t2");
        s39.t1_1 = ado->read_double("t1_1");
        s39.t1_2 = ado->read_double("t1_2");
        s39.t1_3 = ado->read_double("t1_3");
        s39.t1_4 = ado->read_double("t1_4");
        s39.qp_1 = ado->read_double("qp_1");
        s39.qo_1 = ado->read_double("qo_1");
        s39.qp_2 = ado->read_double("qp_2");
        s39.qo_2 = ado->read_double("qo_2");
        s39.qp_3 = ado->read_double("qp_3");
        s39.qo_3 = ado->read_double("qo_3");
        s39.qp_4 = ado->read_double("qp_4");
        s39.qo_4 = ado->read_double("qo_4");
        s39.qp_1g5000 = ado->read_double("qp_1gt5000");
        s39.qo_1g5000 = ado->read_double("qo_1gt5000");
        s39.qp_2g5000 = ado->read_double("qp_2gt5000");
        s39.qo_2g5000 = ado->read_double("qo_2gt5000");
        s39.qp_3g5000 = ado->read_double("qp_3gt5000");
        s39.qo_3g5000 = ado->read_double("qo_3gt5000");
        s39.qp_4g5000 = ado->read_double("qp_4gt5000");
        s39.qo_4g5000 = ado->read_double("qo_4gt5000");

        double d = max(s39.d, s39.dy);
        map_s39[s39.date][s39.proklad[0]][d] = s39;
        ado->MoveNext();
    }
    ado->closeTable();

}

struct Nomer {
    char nomer[256];
};

static map <CString, Nomer> map_idrp;
static map <string, int> map_El;
static map <string, int> map_Nez;

static map<CString, map<CString, int> > map_potr;

static int nom;

int get_ID2(const char* name, const char* _name2)
{
    map<CString, map<CString, int> >::iterator it = map_potr.find(CString(name));

    if (it == map_potr.end()) return 0;

    CString name2 = _name2;
    name2.MakeLower();

    map<CString, int>::iterator it2 = it->second.find(CString(name2));
    if (it2 == it->second.end()) return 0;

    return it2->second;
}



void open_vsp_PR()
{
    CAdoFile* ado = getAdo(getGidAdoName());

    if (!ado) return;

    open_vsp("vsp");


    int i, n;
    CString cxema;
    char nomer[256];
    map<CString, CString> map_PR;

    CString tn = _TR("Исходные данные реального потребителя по вкладкам");
    CString q;

    q = "SELECT rc.oldName, rc.newName FROM tables_columns as tc join renamed_tables as rt on rt.id = tc.tableID join renamed_columns as rc on rc.id = tc.columnID where rt.ID = 11";
    if (ado && ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString oldName = ado->readStr("oldName");  oldName.MakeLower();
            CString newName = ado->readStr("newName");

            map_PR[oldName] = newName;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    ado = getAdo("vsp");

    q.Format("SELECT * FROM %s", transl(tn));

    if (ado && ado->openTable0(q)) {
        while (!ado->isEOF()) {
            cxema = ado->readStr("cxema");

            map_El[string(cxema)] = ado->read_long("a14");
            map_Nez[string(cxema)] = ado->read_long("otopln");

            for (i = 2; i < ado->NFlds(); i++) {

                CString name = ado->FieldName(i);

                n = ado->read_long(i);
                nomer[i - 2] = n;

                CString nameL = name;
                nameL.MakeLower();

                map<CString, CString>::const_iterator it = map_PR.find(nameL);
                if (it != map_PR.end()) {
                    name = it->second;
                }
                name.MakeLower();

                map_potr[cxema][name] = n;
            }
            memmove(map_idrp[cxema].nomer, nomer, 256);
            ado->MoveNext();
        }
        ado->closeTable();
    }
}

const char* get_idrp(const char* name1)
{
    CString name(name1);

    auto it = map_idrp.find(name);

    if (it != map_idrp.end()) {
        return (const char*)&it->second.nomer;
    }
    return NULL;
}

int get_El(string name)
{
    map <string, int>::const_iterator it;

    it = map_El.find(name);
    if (it != map_El.end()) {
        return it->second;
    }
    return 0;
}

int get_Nez(string name)
{
    map <string, int>::const_iterator it;

    it = map_Nez.find(name);
    if (it != map_Nez.end()) {
        return it->second;
    }
    return 0;
}

int sgn(double d)
{
    return d == 0 ? 0 : 1;
}

int sgn(int d)
{
    return d == 0 ? 0 : 1;
}



#define QQ(a10) if (a10 != 0 && it->second.##a10 == 0 || a10 == 0 && it->second.##a10 != 0 ) continue

CString get_nom_cxema(int gvpr, int gvsm, int gvps, int gvpw, int rez, int gvop, int gvoo, int otopln, int otoplz, int ventil, int elevat)
{
    map <CString, Nomer>::const_iterator it;

    for (it = map_idrp.begin(); it != map_idrp.end(); ++it) {
        if (sgn(it->second.nomer[28]) != sgn(otoplz)) continue;
        if (sgn(it->second.nomer[29]) != sgn(otopln)) continue;
        if (sgn(it->second.nomer[32]) != sgn(ventil)) continue;
        if (sgn(it->second.nomer[37]) != sgn(gvop)) continue;
        if (sgn(it->second.nomer[38]) != sgn(gvoo)) continue;
        if (sgn(it->second.nomer[39]) != sgn(gvpr)) continue;
        if (sgn(it->second.nomer[40]) != sgn(gvsm)) continue;
        if (sgn(it->second.nomer[41]) != sgn(gvps)) continue;
        if (sgn(it->second.nomer[42]) != sgn(gvpw)) continue;

        int a14 = sgn(it->second.nomer[75]);

        if (otoplz) if (sgn(elevat) == a14) continue;

        if (gvop != 0. || gvoo != 0. || gvsm != 0. || gvps != 0. || gvpw != 0. || gvpr != 0.) {
            if (sgn(it->second.nomer[45]) != sgn(rez)) continue;
            //QQ(rez);
        }

        return it->first;
    }

    return CString("");
}


bool readNode(CAdoFile* ado, CNode2* node2, int typ, CGraph2* m_graph, const char* tn);
bool readLine(CAdoFile* ado, CLine2* line, int typ, CGraph2* m_graph, const char* tn, const char* otkr);
long addLine(CAdoFile* ado, int typ, CNode2* n1, CNode2* n2, int externalSignLineID, CCoordList& cl, long& idv);
bool delCxema1(CCxema *m_cxema, const CNode2 *node);
bool delCxemaFull(CCxema *m_cxema, const CNode2 *node);


void create_b5(CAdoFile* ado, CNode2 *nP, CCxema* cxema)
{
    map<string, map<string, list<PARA> > >::const_iterator it0;
    map<string, list<PARA> > ::const_iterator it1;
    list<PARA>::const_iterator itl;

    map<string, set<NODE> >::const_iterator itn;
    map<pair<CString, int>, CNode2*> map_nnn;

    if (!nP) return;

//    delCxema1(cxema, nP);
    delCxemaFull(cxema, nP);

    int id = nP->id;
    int externalCodeID = nP->node.id_kod;
    int fileID = nP->node.fileID;
    string cx = string(nP->node.schemeNum);


    itn = map_n_b5.find(cx);
    if (itn != map_n_b5.end()) {
        set<NODE>::const_iterator it;

        for (it = itn->second.begin(); it != itn->second.end(); ++it) {
            long addNodeUS(CAdoFile * ado, const char* tn, const char* name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt);

            int po = it->pr == CPOD ? 2 : 3;

            int id2 = 0;
            int typ = TIP_US;

            CFPoint point(it->x*10, it->y*10);

            long idn = addNodeUS(ado, "nodes", it->uzel, fileID, po, id, externalCodeID, point);

            if (idn > 0) {

                CString getNodeQ(int typ, const char* tn, const char* qq);

                CNode2* n = cxema->m_graph->ins(idn);
                if (n) {

                    CString qq;
                    qq.Format("nodes.id=%d", idn);
                    CString q = getNodeQ(n->node.typ, n->getTableMySQL(), qq);

                    if (ado->openTable0(q)) {
                        if (!ado->isEOF()) {
                            n->node.nomP = -1;
                            n->node.nomO = -1;
                            n->node.fileID = fileID;
                            readNode(ado, n, n->node.typ, cxema->m_graph, n->getTableMySQL());
                        }
                        ado->closeTable();
                    }

                    pair<CString, int> para(it->uzel, po);

                    map_nnn[para] = n;
                }
            }
        }
    }





    it0 = map_b5.find(cx);
    if (it0 != map_b5.end()) {
        for (it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            CString str;
            int getLineTyp3(const char* tn);
            int idid = getLineTyp3(it1->first.c_str());

            if (idid >= 0) {
                for (itl = it1->second.begin(); itl != it1->second.end(); ++itl) {
                    CCoordList cl;

                    CNode2* n1 = NULL, * n2 = NULL;
                    int po1, po2;

                    po1 = itl->n1.pr == CPOD ? 2 : 3;
                    po2 = itl->n2.pr == CPOD ? 2 : 3;

                    int po;

                    if (po1 == 2 && po2 == 2) po = 2;
                    if (po1 == 2 && po2 == 3) po = 4;
                    if (po1 == 3 && po2 == 2) po = 5;
                    if (po1 == 3 && po2 == 3) po = 3;

                    pair<CString, int> para1(itl->n1.uzel, po1);
                    pair<CString, int> para2(itl->n2.uzel, po2);

                    const auto it1 = map_nnn.find(para1);
                    if (it1 != map_nnn.end()) n1 = it1->second;

                    const auto it2 = map_nnn.find(para2);
                    if (it2 != map_nnn.end()) n2 = it2->second;

                    long idv;
                    long idl = ::addLine(ado, idid, n1, n2, po, cl, idv);

                    CString tn = getLineTableMySQL(idid);

                    ado->Edit(tn, idv);

                    if (idid == TIP_ZD) {
                        ado->write("dispatcherSwitch", itl->destination);
                        if (itl->nomer_mesto == 39 || itl->nomer_mesto == 50) {
                            ado->write("damperArmatureStateID", 2);
                        }
                    }
                    if (idid == TIP_DR) {
                        ado->write("throtDiaphLoc", itl->destination);
                    }
                    ado->Update();

                    if (idl > 0) {
                        CCoordList cl;

                        CLINE2* line = cxema->m_graph->insert_line(n1, n2, cl);
                        if (line) {
                            CLine2* l = bline(line);
                            l->line.typ = idid;

                            if (po == SignLine_ob || po == SignLine_pp || po == SignLine_po) {
                                l->line.nomP = idl;
                                l->line.idP2 = idv;
                            }
                            if (po == SignLine_ob || po == SignLine_oo || po == SignLine_op) {
                                l->line.nomO = idl;
                                l->line.idO2 = idv;
                            }
                            CString getLineQ(int typ, const char* tn, const char* otkr, const char* qq);

                            CString qq;
                            qq.Format("l1.id=%d", idl);

                            CString q = getLineQ(l->line.typ, l->getTableMySQL(), getLineOtkr(l->line.typ), qq);

                            if (ado->openTable0(q)) {
                                if (!ado->isEOF()) {
                                    readLine(ado, l, idid, cxema->m_graph, l->getTableMySQL(), getLineOtkr(l->line.typ));
                                }
                            }
                        }
                    }
                }
            }
        }

          auto itup = map_vp.find(cx);

          if (itup != map_vp.end()) {
            auto it = itup->second.begin();

            for (; it != itup->second.end(); it++) {

                CNode2* n1 = NULL;

                int po1 = it->pr == CPOD ? 2 : 3;
                pair<CString, int> para(it->uzel, po1);

                const auto it1 = map_nnn.find(para);
                if (it1 != map_nnn.end()) n1 = it1->second;

                if (n1) {
                    ado->AddNew("WDOdevices");
                    ado->write("nodeID", n1->id);
                    ado->write("externalSignID", 2);
                    n1->node.nVP = ado->Update();
                }
            }
        }
    }

    nP->node.isP = true;

}


static map<CString, CString> map_typ;
static map<CString, CString> map_typ2;

CString getTyp(const char* name)
{
    map<CString, CString>::const_iterator it = map_typ.find(name);
    if (it != map_typ.end()) {
        return it->second;
    }
    return name;
}

CString getTyp2(const char* name)
{
    map<CString, CString>::const_iterator it = map_typ2.find(name);
    if (it != map_typ2.end()) {
        return it->second;
    }
    return name;
}


void init_A4()
{
    CAdoFile* ado = getAdo("vsp");

    if (!ado) return;

    CString tn = "A4_Типы узлов";
    CString q;
    q.Format("SELECT * FROM %s", transl(tn));


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            CString typ, name_typ, krat_name;

            typ = ado->readStr("typ");
            name_typ = ado->readStr("name_typ");
            krat_name = ado->readStr("krat_name");

            map_typ[typ] = name_typ;
            map_typ[krat_name] = name_typ;
            map_typ2[name_typ] = typ;

            ado->MoveNext();
        }
        ado->closeTable();
    }
}
