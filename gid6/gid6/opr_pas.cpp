#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "win.h"
#include "ado.h"

#include <locale.h>

#include "TreeCtrlX.h"

#include "mmenu.h"
#include "mmenu2.h"
#include "dialog_pr.h"



#define UCH_OPR transl("Опрессовки участок очереди")
#define OPR_OPR transl("Очередь опрессовок")

#define ID_UOO transl("Участок очереди опрессовки")


void setOpr(CGraph2* m_graph, int typ, const char* kod0, int val);

#define UCH_OPR transl("Опрессовки участок очереди")
#define OPR_OPR transl("Очередь опрессовок")
#define ID_UOO transl("Участок очереди опрессовки")

CString DateToString(const COleDateTime& Date);




static int id_map = 0;

struct OPR
{
  int id;
  CString nomer;
  unsigned long color;
  COleDateTime date1, date2;
};

struct UCH
{
  int id;
  int opr;
  CString name;
  double Du;
  double dlina;
};

static map<int, int> map_OPR_color;

static map<int, OPR> map_OPR;
static map<int, UCH> map_UCH;

static void initOpr(CTreeCtrlX &m_ctrlTree, HTREEITEM i1)
{
  map<int, OPR>::const_iterator it = map_OPR.begin();
  for (; it != map_OPR.end(); it++) {
    OPR opr = it->second;
    HTREEITEM i = m_ctrlTree.InsertItem(opr.nomer, i1);
    m_ctrlTree.SetItemColor(i, opr.color);
    m_ctrlTree.SetItemData(i, (DWORD) opr.id);
  }

  HTREEITEM i = m_ctrlTree.InsertItem("Без очереди", i1);
  m_ctrlTree.SetItemColor(i, 0);
  m_ctrlTree.SetItemData(i, (DWORD) 0);
}

void initOpr0()
{
    map_OPR.clear();
    map_UCH.clear();

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString q;
    q.Format("SELECT * FROM %s", transl(OPR_OPR));

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            OPR opr;

            opr.id = ado->read_long("ID");
            opr.nomer = ado->read(transl("Номер очереди опрессовки"));
            opr.color = ado->read_long(transl("Цвет"));
            opr.date1 = ado->read_date(transl("Планируемая дата проведения, начало"));
            opr.date2 = ado->read_date(transl("Планируемая дата проведения, окончание"));

            map_OPR[opr.id] = opr;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    q.Format("SELECT * FROM %s", transl(UCH_OPR));

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            UCH uch;

            uch.id = ado->read_long("ID");
            uch.opr = ado->read_long(transl("Номер очереди опрессовки"));
            uch.Du = ado->read_double(transl("Диаметр условный, мм"));
            uch.name = ado->read(transl("Наименование"));
            uch.dlina = ado->read_double(transl("Общая протяженность, м"));

            map_UCH[uch.id] = uch;
            ado->MoveNext();
        }
        ado->closeTable();
    }
}


bool getColorOpressovka(int id, unsigned long &color)
{
  map<int, UCH>::const_iterator itU = map_UCH.find(id);
  if (itU != map_UCH.end()) {
    UCH uch = itU->second;
    map<int, OPR>::const_iterator itO = map_OPR.find(uch.opr);
    if (itO != map_OPR.end()) {
      OPR opr = itO->second;
      color = opr.color;
      return true;
    }
  }
  return false;
}


#include "css.h"
#include "html.h"

static CSS css;


struct CELL
{
  int x, y;
  int cx, cy;
  int sx, sy;
  CString text;
  CSS css;
};

class EXCEL
{
public:
  EXCEL();
  void put(int x, int y, const char *text, CSS &css);
  void put(int x, int y, int val, CSS &css);
  void print(HTML *h);
  CELL *get(int x, int y);
  void set_colspan(int n);
  void set_rowspan(int n);

private:
  int cx, cy;
  int sx, sy;

  map<int, map<int, CELL> > map_CELL;  
};

EXCEL::EXCEL()
{
  cx = 0;
  cy = 0;
  sx = 1;
  sy = 1;
}

 void EXCEL::set_colspan(int n)
 {
   sx = n;
 }

 void EXCEL::set_rowspan(int n)
 {
   sy = n;
 }


CELL *EXCEL::get(int x, int y)
{
  map<int, map<int, CELL> >::iterator itx = map_CELL.find(x);

  if (itx != map_CELL.end()) {
    map<int, CELL>::iterator ity = itx->second.find(y);
    if (ity != itx->second.end()) {
      return &ity->second;
    }
  }

  return NULL;
}


void EXCEL::put(int x, int y, const char *text, CSS &css)
{
  CELL cell;
  cell.x = x;
  cell.y = y;

  cell.sx = sx;
  cell.sy = sy;
  
  cell.text = text;

  cell.css = css;

  map_CELL[x][y] = cell;

  if (x+1 > cx) cx = x+1;
  if (y+1 > cy) cy = y+1;
}

void EXCEL::put(int x, int y, int val, CSS &css)
{
  CString s;
  s.Format("%d", val);

  put(x, y, s, css);
}


void EXCEL::print(HTML *h)
{
  fprintf(h->f, "<table>");

  for (int y = 0; y < cy; y++) {
    h->print_tr();
    for (int x = 0; x < cx; x++) {
      CELL *cell = get(x, y);

      if (cell) {
        h->set_colspan(cell->sx);
        h->set_rowspan(cell->sy);
        x += cell->sx-1;

        h->css->css  =  cell->css.css;

        h->print_td(cell->text);
      }
      else {
        h->css->css.border = 0;
        h->set_colspan(1);
        h->print_td("&nbsp;");
      }

    }
//    h->print_tr2();
    fprintf(h->f, "</tr>\n");

  
  }

  fprintf(h->f, "</table>");

}


void CGidrView::passportOpreesovka(FILE *f)
{

    initOpr0();


    int YEAR = 2017;


  CString str;

  int jd1 = jdn(1, 4, YEAR);
  int jd2 = jdn(30, 9, YEAR);

  int w1 = jd1%7;
  jd1 -= (w1);

  int j, jd;
  int D1, M1, Y1, D2, M2, Y2;

  int monsp[12];

  for (j = 0; j < 12; j++) monsp[j] = 0;

  int mm = 0;

  for (jd = jd1; jd < jd2; jd += 7) {
    jdn2(jd, D1, M1, Y1);
    jdn2(jd+4, D2, M2, Y2);

    if (M1 == M2) monsp[M2-1]++; 
    else {
      int jd0 = jdn(1, M2, Y2);
      if (jd0-jd > 2) {
        monsp[M1-1]++; 
      }
      else {
        monsp[M2-1]++; 
      }
    }
    mm ++;
  }


  HTML *h = new HTML(f, &css);
  setlocale(LC_NUMERIC, "eng");

  EXCEL ex;

  css.css.font_family = "\"Times New Roman\", serif";
  ex.set_colspan(2);
  css.css.bold = 1;
  css.css.font_size = "14.0pt";

  ex.set_colspan(10);


  ex.put(0, 0, "УТВЕРЖДАЮ", css);
//  ex.put(0, 1, " Председатель правления", css);
//  ex.put(0, 2, "АО \"Астана-Теплотранзит\"", css);
//  ex.put(0, 4, "_____________ В.В. Курисько", css);
  ex.put(0, 6, "\"_______\" ____________ 2017г.", css);

  ex.put(25, 0, "СОГЛАСОВАНО", css);
//  ex.put(25, 1, "Аким района \"Алматы\"", css);
//  ex.put(25, 2, "г. Астана", css);
//  ex.put(25, 4, "_________________ А. Сарсембаев", css);
  ex.put(25, 6, "\"_______\" ____________ 2017г.", css);
  

  css.css.font_size = "12.0pt";

  ex.set_colspan(25);


  str.Format("График капитального ремонта и испытаний магистральных сетей с учетом объектов инвестиции, реконструкции и строительства %d года.", YEAR);
  css.css.text_align = "center";
  ex.put(0, 8, str, css);
  css.css.text_align = "left";

  css.css.bold = 1;
  css.css.text_align = "center";

  ex.set_colspan(1);

  ex.set_rowspan(2);

  css.css.border = 1;
  
  ex.put(0, 10, "№ ТМ", css);
  ex.put(1, 10, "магистрали", css);
  ex.put(2, 10, "Ду", css);
  ex.put(3, 10, "м трассы", css);

  ex.set_rowspan(1);

  int xx = 4;

  for (j = 0; j < 12; j++) {
    if (monsp[j]) {
      ex.set_colspan(monsp[j]);
      ex.put(xx, 10, mon_name(j+1), css);
      xx += monsp[j];
      ex.set_colspan(1);
    }
  }

  ex.set_colspan(1);

  xx = 0;

  j = 0;
  for (jd = jd1; jd < jd2; jd += 7) {
    jdn2(jd, D1, M1, Y1);
    jdn2(jd+4, D2, M2, Y2);

    CString s;
    s.Format("с %d по %d", D1, D2);
    ex.put(xx, 11, s, css);
    xx ++;;
  }

  css.css.bold = 0;
  css.css.text_align = "left";

  map<int, OPR>::const_iterator itO = map_OPR.begin();

  int nn = 1;

  int yy = 12;

  for (; itO != map_OPR.end(); itO++) {
    OPR opr = itO->second;

    map<int, UCH>::const_iterator itU = map_UCH.begin();

    for (; itU != map_UCH.end(); itU++) {
      UCH uch = itU->second;
      if (uch.opr == opr.id) {
        ex.put(0, yy, nn++, css);
        ex.put(1, yy, uch.name, css);
        ex.put(2, yy, uch.Du, css);
        ex.put(3, yy, uch.dlina, css);

        int jd0 = jdn(opr.date1);
        
//        ex.put(0, 0, DateToString(opr.date1));
//        ex.put(0, 0, DateToString(opr.date2));

        int xx = 4;

        for (jd = jd1; jd < jd2; jd += 7) {
          if (jd <= jd0 && jd0 < jd+7) {

            bool dark = isColorDark(opr.color);
            css.css.bk_color = cssColor(opr.color);

            if (dark) {
              css.css.color = "white";
            }
            else {
              css.css.color = "black";
            }

            ex.put(xx, yy, DateToString(opr.date1), css);
            css.css.bk_color = "white";
            css.css.color = "black";
          }
          else {
            ex.put(xx, yy, "&nbsp;", css);
          }
          xx ++;
        }
      }
      yy ++;
    }
  }

  h->print_head("Испытания");
  ex.print(h);
}


void CGidrView::OnOpressovkaPassport()
{
//  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

  CString tmpName;
  tmpName.Format("%s\\tmp.html", getenv("TMP"));

  FILE *f = fopen(tmpName, "w");
  if (!f) return;

//  printMag(f, RT_MS);
//  printMag(f, RT_RS);

  CWaitCursor wait;
  passportOpreesovka(f);

  fclose(f);
  m_cxema->m_graph->reset();

  setlocale(LC_NUMERIC, "eng");
  CString cssName;
  cssName.Format("%s\\style.css", getenv("TMP"));
  css.print(cssName);
  
  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);
}

