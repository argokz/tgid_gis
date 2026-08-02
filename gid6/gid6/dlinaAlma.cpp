#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "mmenu.h"
#include "css.h"
#include "html.h"

#include <locale.h>

typedef int DIAM;

#include "ps_alma.h"

static CSS css;

static int id_map = 0;

int num_mag(const char *s);

static map<int, CString> map_mag;


//CGidrView *getView();

void initMagistral();

struct PARA
{
/**
  PARA () {
    dlP = 0;
    dlO = 0;
  };
*/
  double dlP, dlO;
};

void Add(map<double, PARA> &map1, map<double, PARA> &map2)
{
  map<double, PARA>::const_iterator it2 = map2.begin();

  for (; it2 != map2.end(); it2++) {
    map<double, PARA>::iterator it1 = map1.find(it2->first);
    if (it1 == map1.end()) {
      map1[it2->first] = it2->second;
    }
    else {
      PARA p1 = it1->second;
      PARA p2 = it2->second;

      p1.dlP += p2.dlP;
      p1.dlO += p2.dlO;

      it1->second = p1;
    }
  }
}



void print_truba(FILE *f){}

static double d1000 = 1000.;

static BOOL isKm = FALSE;


CString sDlina(double d)
{
  CString str;

  isKm = FALSE;

  if (isKm) {
    str.Format("%.4g", d/d1000);
  }
  else {
    str.Format("%.0f", d);
  }

  return str;
}

/**

bool isKod(const CNode2 *n, const char *kod, int isP)
{
  if (isP) return true;

  if (!strcmp(n->node.node_name.kod, kod)) return true;
  return false;
}

bool isKod(const CLINE2 *l, const char *kod, int isP)
{
  if (isP) return true;
  
  if (isKod(where(l), kod, isP)) return true;
  if (isKod(other(l), kod, isP)) return true;
  return false;
}
*/

void getLenD(CGraph2* graph, int isAll, map<double, PARA>& map_len)
{

    set<int> set_diam;


    auto pp = graph->map_node.begin();
    int nnn = 0;
    map_len.clear();


#if 0
    while (pp != graph->map_node.end()) {
        CNode2* p = pp->second;

        if (p->node.isPjezo || isAll) {
            for (CLINE2* l = p->lines; l; l = next(l)) {
                if (IsBegin(l) && !IsRezhim(l)) {
                    if (bline(l)->line.isPjezo || isAll) {
                        UTNapr naprP, naprO;
                        naprP = bline(l)->line.pod;
                        naprO = bline(l)->line.obr;

                        set_diam.insert((int)naprP.diam_usl);
                        set_diam.insert((int)naprO.diam_usl);
                    }
                }
            }
        }
        pp++;
        nnn++;
    }

//    map<double, PARA> map_len1;


    set<int>::const_iterator it1 = set_diam.begin();

    for (; it1 != set_diam.end(); it1++) {
        int d = *it1;
        int qqq;
        qqq = 1;

//        int d = *it1;

//        map_len1[d] = para;
    }

#endif 

//    for (: set_diam) {
//        PARA para;
//        map_len1[diam] = para;
//    }




    while (pp != graph->map_node.end()) {
        CNode2* p = pp->second;

        if (p->node.isPjezo || isAll) {
            for (CLINE2* l = p->lines; l; l = next(l)) {
                if (IsBegin(l) && !IsRezhim(l)) {
                    if (bline(l)->line.isPjezo || isAll) {
                        UTNapr naprP, naprO;

                        naprP = bline(l)->line.pod;
                        naprO = bline(l)->line.obr;

                        if (naprP.diam_usl == 0) naprP.diam_usl = naprO.diam_usl;
                        if (naprO.diam_usl == 0) naprO.diam_usl = naprP.diam_usl;
                        
                        PARA para;
                        double dP = naprP.diam_usl;
                        double dO = naprO.diam_usl;

                        if (bline(l)->line.nomP > 0) {
                            auto it = map_len.find(naprP.diam_usl);
                            if (it == map_len.end()) {
                                para.dlP = naprP.dl;
                                para.dlO = 0;
                                map_len[dP] = para;
                            }
                            else {
                                it->second.dlP += naprP.dl;
                            }
                        }

                        if (bline(l)->line.nomO > 0) {
                            auto it = map_len.find(naprO.diam_usl);
                            if (it == map_len.end()) {
                                para.dlP = 0;
                                para.dlO = naprO.dl;
                                map_len[dO] = para;
                            }
                            else {
                                it->second.dlO += naprO.dl;
                            }
                        }
                    }
                }
            }
        }
        pp++;
        nnn++;
    }
}


void CGidrView::OnProtNew()
{
  isKm = m_isKm;

  map<double, PARA> map_len;

  getLenD(m_cxema->m_graph, 0, map_len);

  double max_dl = 0;
  UTNapr max_napr;

  map<double, PARA>::const_iterator it;
  
  CString str;

  CMMenu menu(this, IDD_DIAMLEN2, "Длины теплопроводов");

  double dlP = 0, dlO = 0;

  for (it = map_len.begin(); it != map_len.end(); it++) {
    double napr = it->first;;
    PARA d = it->second;

    if (d.dlP != 0 || d.dlO != 0) {
      str.Format("%4.0f\t%s\t%s",
        napr, 
//        napr.tol,
        sDlina(d.dlP), sDlina(d.dlO));

      dlP += d.dlP;
      dlO += d.dlO;

      menu.lst.Add(str);
    }
  }  

  str.Format("Всего\t%s\t%s", sDlina(dlP), sDlina(dlO) );

  menu.lst.Add(str);

  menu.DoModal();
}

#if XX0
void initRaionE();
RE *getRaionE(int id);
#endif


// Мастер
// Участок
// Подача/обратка
// Диаметр


struct TRUB
{
  CString master;
  CString uch;
  int po;

  bool operator<(const TRUB& mc) const
  {
    int cmp;
    cmp = strcmp(master, mc.master);
    if (cmp < 0) return true;
    if (cmp > 0) return false;
    cmp = strcmp(uch, mc.uch);
    if (cmp < 0) return true;
    if (cmp > 0) return false;

    if (po < mc.po) return true;
    if (po > mc.po) return false;

    return false;

  }
};


static set<DIAM> set_diam;

/*
void print_head(HTML *h, const char *title)
{
  FILE *f = h->f;
  fprintf(f, "<html>\n");
  fprintf(f, "<head>\n");
  fprintf(f, "<title>%s</title>\n", title);
  fprintf(f, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n");
  fprintf(f, "<meta name=Generator content=\"Microsoft Excel 14\">\n");
  fprintf(f, "<link rel=Stylesheet href=style.css>\n");
  fprintf(f, "</head>\n<body>\n");
}
*/

void init_u(map<double, PARA> &map_len)
{
  set_diam.clear();

  map<double, PARA>::const_iterator it;

  for (it = map_len.begin(); it != map_len.end(); it++) {
    double diam = it->first;;
    PARA d = it->second;

    if (d.dlP > 0 || d.dlO > 0) {
      set_diam.insert(diam);
    }
  }
}

void print_u1(HTML *h, const char *txt)
{
  CString str;

  fprintf(h->f, "<table border=1>");

  int n = set_diam.size()+1;

  if (txt) fprintf(h->f, "<tr><th colspan=%d align=\"left\">%s</th></tr>\n", n*2+1, txt);

  fprintf(h->f, "<tr><th>&nbsp;</th><th colspan=%d>подающий</th><th colspan=%d>обратный</th></tr>\n", n, n);

  h->set_tr(n*2+1);
  h->print_tr();
  h->print_td();

  css.css.bold = 1;

  for (int i = 0; i < 2; i++) {
    set<DIAM>::const_iterator it;

    for (it = set_diam.begin(); it != set_diam.end(); it++) {
      double diam = *it;
      css.css.text_align = "right";
      h->print_td("%.0f", diam);
      css.css.text_align = "left";
    }
    h->print_td("Всего");
  }
  h->print_tr2();

  css.css.bold = 0;
}

void print_u2(HTML *h, map<double, PARA> &map_len, int pod)
{
  set<DIAM>::const_iterator it;

  double dd = 0;

  for (it = set_diam.begin(); it != set_diam.end(); it++) {
    double diam = *it;
//    fprintf(f, "<td>%.0f</td>", diam);

    map<double, PARA>::const_iterator it2 = map_len.find(diam);


    if (it2 != map_len.end()) {
      PARA d = it2->second;
    
      if (d.dlP > 0 || d.dlO > 0) {
        double dl = pod ? d.dlP : d.dlO;

        dd += dl;

        if (dl > 0) {
          css.css.text_align = "right";
          h->print_td("%s", sDlina(dl));
          css.css.text_align = "left";
        }
        else {
          h->print_td(0.);
        }
      }
    }
    else {
      h->print_td(0.);
    }
  }
  css.css.text_align = "right";
  h->print_td(dd);
  css.css.text_align = "left";
}

//int vydRS(CGraph *m_graph, const char *kod);
int vydMsRsN(CGraph2 *m_graph, int ms_rs, int rs, bool is_only_big_pipe);


void CGidrView::printUE(HTML *h, int ue, int ms_rs, const char *txt)
{
  isKm = m_isKm;

  set_diam.clear();

  map<double, PARA> map_len;
  map<double, PARA> map_lenAll;

  m_cxema->m_graph->reset();

  map<int, UMSRS>::const_iterator it;

  for (it =  ps_alma.mapMSRS.begin(); it != ps_alma.mapMSRS.end(); it++) {
    if (it->second.mag == ms_rs && it->second.ue == ue) {
      vydMsRsN(m_cxema->m_graph, ms_rs, it->first, true);
    }
  }
  
  getLenD(m_cxema->m_graph, 0, map_len);
  init_u(map_len);

  print_u1(h, txt);

  for (it =  ps_alma.mapMSRS.begin(); it != ps_alma.mapMSRS.end(); it++) {
    UMSRS ms = it->second;
    if (ms.mag == ms_rs && it->second.ue == ue) {
      m_cxema->m_graph->reset();
//      vydRS(m_cxema->m_graph, it->second.kod);


//      int n_rs = it->first;
//      n_rs = 312;

      int n_rs = ms.id;


      vydMsRsN(m_cxema->m_graph, ms_rs, n_rs, true);
    
      getLenD(m_cxema->m_graph, 0, map_len);

      Add(map_lenAll, map_len);

      CString ss;
      ss.Format("%d ", n_rs);

      h->print_tr();
      h->print_td(ss+ms.opis);
 
      print_u2(h, map_len, TRUE);
      print_u2(h, map_len, FALSE);

      h->print_tr2();
    } 
  }

  m_cxema->m_graph->reset();

  for (it =  ps_alma.mapMSRS.begin(); it != ps_alma.mapMSRS.end(); it++) {
    if (it->second.mag == ms_rs && it->second.ue == ue) {
//      vydRS(m_cxema->m_graph, it->second.kod);
      vydMsRsN(m_cxema->m_graph, ms_rs, it->first, true);
    }
  }
  getLenD(m_cxema->m_graph, 0, map_len);

  h->print_tr();
  h->print_td("Всего");

  print_u2(h, map_lenAll, TRUE);
  print_u2(h, map_lenAll, FALSE);

  h->print_tr2();
  fprintf(h->f, "</table>\n");
  fprintf(h->f, "<p>&nbsp;</p>\n");

}

int isUeMsRs(int re, int ms_rs);
int isMagMsRs(int ue, int ms_rs);



void CGidrView::printMag(FILE *f, int ms_rs)
{
  CString str;

  ps_alma.init();

  isKm = m_isKm;

  set_diam.clear();
  ps_alma.init();

/*
  CString tmpName;
  tmpName.Format("%s\\tmp.html", getenv("TMP"));

//  FILE *f = fopen("D:\\gid\\qq.html", "w");
  FILE *f = fopen(tmpName, "w");
  if (!f) return;
*/
  HTML *h = new HTML(f, &css);
  setlocale(LC_NUMERIC, "eng");

  CWaitCursor wait;

  h->print_head("Протяженность участков магистралей");

  if (ms_rs == RT_MS) {
//    fprintf(h->f, "<h1>Магистрали</h1>\n");
    h->print_title("Магистрали", 10);
  }
  else if (ms_rs == RT_RS) {
    h->print_title("Распределительные сети", 10);
//    fprintf(h->f, "<h1>Распределительные сети</h1>\n");
  }
  else if (ms_rs == RT_KOTEL) {
//    fprintf(h->f, "<h1>Распределительные сети</h1>\n");
  }

  map<double, PARA> map_len;
  map<double, PARA> map_lenAll;

  map<int, CString>::const_iterator itRE = ps_alma.map_RE.begin();

  for (; itRE !=  ps_alma.map_RE.end(); itRE++) {
    CString s = itRE->second;

    if (isUeMsRs(itRE->first, ms_rs)) {
//      fprintf(h->f, "<h2>%s</h2>", itRE->second);
      h->print_title(itRE->second, 10);

      map<int, UE>::const_iterator itUE = ps_alma.map_UE.begin();

      for (; itUE !=  ps_alma.map_UE.end(); itUE++) {
        UE ue = itUE->second;
        if (isMagMsRs(itUE->first, ms_rs)) {
          if (ue.re == itRE->first) {
            CString txt = ue.nomer;

            map<int, NACH>::const_iterator itN = ps_alma.map_NACH.find(ue.nach);
            if (itN !=  ps_alma.map_NACH.end()) {
              CString nach = itN->second.name;
              txt += " ";
              txt += nach;
            }

            printUE(h, itUE->first, ms_rs, txt);
          }
        }
      }
    }
  }

/*
  h->print_td("Всего");

  print_u2(h, map_lenAll, TRUE);
  print_u2(h, map_lenAll, FALSE);

  h->print_tr2();
*/
/*
  fclose(f);
  m_cxema->m_graph->reset();

  setlocale(LC_NUMERIC, "eng");
  CString cssName;
  cssName.Format("%s\\style.css", getenv("TMP"));
  css.print(cssName);
  
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", tmpName, NULL, SW_SHOW);
*/
}

void CGidrView::printMag(int ms_rs)
{
  CString tmpName;
  tmpName.Format("%s\\tmp.html", getenv("TMP"));

  FILE *f = fopen(tmpName, "w");
  if (!f) return;

  printMag(f, ms_rs);

  fclose(f);
  m_cxema->m_graph->reset();

  setlocale(LC_NUMERIC, "eng");
  CString cssName;
  cssName.Format("%s\\style.css", getenv("TMP"));
  css.print(cssName);
  
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);

}


bool isR2(int id, int typ);
bool isR3(int id, int typ);

void CGidrView::OnProtMag()
{
  printMag(RT_MS);
}

void CGidrView::OnProtRs()
{
  printMag(RT_RS);
}


void print_itog(FILE *f, CCxema *cxema, const char *name, int n)
{
  set_diam.clear();

  fprintf(f, "<h2>%s</h2>", name);
//  summ(cxema, 0, "Потребительские сети", 0, 0, 0, 0, n);
}


void CGidrView::OnProtAll()
{

  CString tmpName;
  tmpName.Format("%s\\tmp.html", getenv("TMP"));

  FILE *f = fopen(tmpName, "w");
  if (!f) return;

  printMag(f, RT_MS);
  printMag(f, RT_RS);

  fclose(f);
  m_cxema->m_graph->reset();

  setlocale(LC_NUMERIC, "eng");
  CString cssName;
  cssName.Format("%s\\style.css", getenv("TMP"));
  css.print(cssName);
  
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);
}

void CGidrView::OnProtKot()
{
}
