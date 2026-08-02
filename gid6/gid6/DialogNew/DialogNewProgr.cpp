#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"

#include "TableDialog.h"

#include "Dialog3.h"


#include "dbi.h"
#include "win.h"

#include "mmenu.h"
#include "mmenu2.h"
#include "MagNode.h"

#include "DialogNew2.h"
#include "FormDialog2.h"

bool GotoKey(CAdoFile *dbf, int n1);

MAG *getMag2(int id) ;
CGidrView *getView();

CString getPath(const char *reg);
CString TrimPath(CString val, CString path);


double getLenD(CGraph *graph, int isAll, int m_mag, int m_ms, int m_rs)
{
  CMapGraph::iterator p = graph->set_node.begin();

  double dl = 0;

  while (p != graph->set_node.end()) {
    if (p->isVyd(m_mag, m_ms, m_rs) || isAll) {
      for ( CLINE *l = p->lines; l; l = next(l)) {
        if (IsBegin(l) && (bline(l)->isVyd(m_mag, m_ms, m_rs) || isAll)) {
          UTNapr naprP, naprO;
          
          naprP = bline(l)->pod;
          naprO = bline(l)->obr;

          if (bline(l)->nomP != -1) {
            dl += naprP.dl;
          }

          if (bline(l)->nomO != -1) {
            dl += naprO.dl;
          }
        }
      }
    }
    p++;
  }

  return dl;
}

double getLenD(CGraph *graph)
{
  CMapGraph::iterator p = graph->set_node.begin();

  double dl = 0;

  while (p != graph->set_node.end()) {
    if (1) {
      for ( CLINE *l = p->lines; l; l = next(l)) {
        if (IsBegin(l) && bline(l)->isPjezo) {
          UTNapr naprP, naprO;
          
          naprP = bline(l)->pod;
          naprO = bline(l)->obr;

          if (bline(l)->nomP != -1) {
            dl += naprP.dl;
          }

          if (bline(l)->nomO != -1) {
            dl += naprO.dl;
          }
        }
      }
    }
    p++;
  }

  return dl;
}





double getLength(CAdoFile *m_dbf)
{
#if XX0
  CGidrView *pView = getView();

  if (!pView) return 0;
  
  int m_ms = m_dbf->read_long("nomer");

  const MAG *ms = getMag2(m_ms);
  CCxema *cxema = pView->m_cxema;

  double dl = 0;

  if (ms) {
    dl += getLenD(cxema->m_graph, 0, 0, m_ms, 0);
  }

  return dl;
#endif

  return 0;

}

//  CDialogNew(CWnd* pParent, CAdoFile *dbf, const char *tn, const char *q, int n, int readOnly, int idd = CDialogNew::IDD);   // standard constructor
#include "DialogNew2.h"

void init_menu(CAdoFile *dbf, CMMenu2 *menu);

void edit1(CMMenu2 *menu, CAdoFile *dbf, const char *tn, int nom)
{
  if (openPS_Alma(dbf)) {

    CDialogNew2 dlg(menu, dbf, tn, 0);
    dlg.addQ(tn, NULL, nom);

    if (dlg.DoModal() != IDCANCEL) {
      dlg.saveDB2();

      menu->Reset();
      init_menu(dbf, menu);
      menu->init();
    }
  }
}

void edit(CMMenu2 *menu, CMenuRow &row, void *pp1 = NULL, void *pp2 = NULL, void *pp3 = NULL, void *pp4 = NULL)
{
  int n = (int)row.v;

  if (n > 0) {
    CAdoFile *dbf = (CAdoFile *) pp1;
    
    edit1(menu, dbf, (const char *) pp2, n);
    menu->Reset();
    init_menu(dbf, menu);
    menu->init();
  }
}

void add(CMMenu2 *menu, CMenuRow &row, void *pp1 = NULL, void *pp2 = NULL, void *pp3 = NULL, void *pp4 = NULL)
{
  edit1(menu, (CAdoFile *) pp1, (const char *) pp2, 0);
}



void del(CMMenu2 *menu, CMenuRow &row, void *pp1 = NULL, void *pp2 = NULL, void *pp3 = NULL, void *pp4 = NULL)
{
  int n = (int)row.v;
  CAdoFile *dbf = (CAdoFile *) pp1;
  const char *tn = (const char *) pp2;

  if (n > 0) {
    CString s;
    s.Format("Удалить %s?", row.cols[0]);

    if (AfxMessageBox(s, MB_YESNO) == IDYES) {
     if (openPS_Alma(dbf)) {
       CString q;
       q.Format("DELETE FROM [%s] WHERE id=%d", tn, n);
       dbf->Execute(q);
       dbf->closeMDB();

       menu->Reset();
       init_menu(dbf, menu);
       menu->init();
     }
    }
  }
}

void init_menu(CAdoFile *dbf, CMMenu2 *menu)
{
    CString q;
//    q.Format("SELECT ID, [Название], [Телефон организации], [Телефон руководителя] FROM [Организация]");
    q.Format("SELECT * FROM [Организации владельцы]");
    
  if (openPS_Alma(dbf)) {
    if (dbf->openTable(q)) {
      if (!dbf->rs->IsEOF()) {
        dbf->rs->MoveFirst();

        while ( !dbf->rs->IsEOF() ) {
          long id = dbf->read_long("ID");
          CString s1 = dbf->read("Наименование");
          CString s2 = dbf->read("Телефон организации");
          CString s3 = dbf->read("Телефон руководителя");
          CString s4 = dbf->read("Улица");
          CString s5 = dbf->read("Номер дома");
          menu->Add(1, s1);
          menu->Add(2, s2);
          menu->Add(3, s3);
          menu->Add(4, s4);
          menu->Add(5, s5);

          menu->Add((void*)id);

          dbf->rs->MoveNext();
        }
      }
      dbf->closeTable();
    }
  }
}

struct DIAM
{
  double d1, d2, d3, tol;
};


DIAM *diametr(const char *app1)
{
//  CMMenu2 menu(AfxGetMainWnd(), IDD_MENU10, "");
  CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_DIAM, "");

  CAdoFile dbf;

  if (dbf.openMDB(getStandObor())) {
    CString q;

    CString app = app1;
    if (app == "") app = "Россия";

    q.Format("SELECT * FROM [TP_Трубы стандартные] WHERE stand='%s'", app);

    menu.AddColumn("Условный");
    menu.AddColumn("Внешний");
    menu.AddColumn("Внутренний");
    menu.AddColumn("Толщина");

    dbf.openTable(q, TRUE);
    if (!dbf.rs->IsEOF()) {
//      dbf.rs->MoveFirst();
      while (!dbf.rs->IsEOF()) {
//        CString s1 = dbf.read("diametr_usl");
//        CString s2 = dbf.read("diamVne");
//        CString s3 = dbf.read("diametr");
//        CString s4 = dbf.read("tol");
        DIAM *d = new DIAM;

        d->d1 = dbf.read_double("diametr_usl");
        d->d2 = dbf.read_double("diamVne");
        d->d3 = dbf.read_double("diametr");
        d->tol = dbf.read_double("tol");

        CString s1 = dbf.read("diametr_usl");
        CString s2 = dbf.read("diamVne");
        CString s3 = dbf.read("diametr");
        CString s4 = dbf.read("tol");

        menu.Add(1, s1);
        menu.Add(2, s2);
        menu.Add(3, s3);
        menu.Add(4, s4);

        menu.setWidth(1, 100);
        menu.setWidth(2, 100);
        menu.setWidth(3, 100);
        menu.setWidth(4, 100);

        menu.Add((void*) d);
        dbf.rs->MoveNext();
      }
    }
    dbf.closeTable();
  }

  if (menu.DoModal() == IDOK) {
    return (DIAM *) menu.getV();
  }

  return NULL;
}


double getLengthAll()
{
  CGidrView *pView = getView();

  if (!pView) return 0;

  CCxema *cxema = pView->m_cxema;
  CGraph *graph = cxema->m_graph;

  double dlP = 0;
  double dlO = 0;

  CMapGraph::iterator p = graph->set_node.begin();

  while (p != graph->set_node.end()) {
    if (1 || p->isPjezo) {
      for ( CLINE *l = p->lines; l; l = next(l)) {
        if (IsBegin(l) && !IsRezhim(l)) { 
          if (bline(l)->isPjezo) {
            dlP += bline(l)->pod.dl;
            dlO += bline(l)->obr.dl;
          }
        }
      }
    }
    p++;
  }


  return max(dlP, dlO);
}


CString getProklMaxAll()
{
  CGidrView *pView = getView();

  if (!pView) return "";

  CCxema *cxema = pView->m_cxema;
  CGraph *graph = cxema->m_graph;

  double diamPmax = 0;
  double diamOmax = 0;

  CMapGraph::iterator p = graph->set_node.begin();

  map<CString, double> map_pr;

  while (p != graph->set_node.end()) {
    if (1 || p->isPjezo) {
      for ( CLINE *l = p->lines; l; l = next(l)) {
        if (IsBegin(l) && !IsRezhim(l)) { 
          if (bline(l)->isPjezo) {
            double dlP = bline(l)->pod.dl;
            double dlO = bline(l)->obr.dl;

            CString pr = bline(l)->name_typ;

            map<CString, double>::iterator it = map_pr.find(pr);

            if (it != map_pr.end()) {
              it->second += (dlP+dlO); 
            }
            else {
              map_pr[pr] = (dlP+dlO);
            }
          }
        }
      }
    }
    p++;
  }

  CString pr = "";

  double dl_max = 0;

  map<CString, double>::iterator it = map_pr.begin();
  for (; it != map_pr.end(); it++) {
    if (it->second > dl_max) {
      dl_max = it->second;
      pr = it->first;
    }
  }

  return pr;
}


double getDiamAll()
{
  CGidrView *pView = getView();

  if (!pView) return 0;

  CCxema *cxema = pView->m_cxema;
  CGraph *graph = cxema->m_graph;

  double diamPmax = 0;
  double diamOmax = 0;

  CMapGraph::iterator p = graph->set_node.begin();

  while (p != graph->set_node.end()) {
    if (1 || p->isPjezo) {
      for ( CLINE *l = p->lines; l; l = next(l)) {
        if (IsBegin(l) && !IsRezhim(l)) { 
          if (bline(l)->isPjezo) {
            double diamP = bline(l)->pod.diam_usl;
            double diamO = bline(l)->obr.diam_usl;

            if (diamP > diamPmax) diamPmax = diamP;
            if (diamO > diamOmax) diamOmax = diamO;

          }
        }
      }
    }
    p++;
  }


  return max(diamPmax, diamOmax);
}




static int getValKorroz(double d, const char *tn)
{
  CString mdb = getPsSprav();

  int n = 0;

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
    if (dbf.openTable(tn)) {
      
      if (!dbf.rs->IsEOF()) {

        dbf.rs->MoveFirst();

        while ( !dbf.rs->IsEOF() ) {
          double d1 = dbf.read_double(2);
          long n = dbf.read_long(3);

          if (d < d1) break;

          dbf.rs->MoveNext();
        }
      }

      dbf.closeTable();
    }
    dbf.closeMDB();
  }
  return n;
}



CString get_progr_val6(CDialogNew2 *wnd, const char *progr, const char *val1, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom, int nn, CString &real1, CString &real2, CString &real3, CString &real4, CString &real5, CString &real6)
{
  CString val = val1;
  CString v = val1;
  CString pr = progr;
//  real1 = real2 = real3 = real4 = real5 = real6 = "";

  CGidrView *view = getView();


  if (pr == "Diametr") {
    DIAM *d = diametr(real5);
    if (d) {
      real1.Format("%g", d->d1);
      real2.Format("%g", d->d2);
      real3.Format("%g", d->d3);
      real4.Format("%g", d->tol);
    }
  }
  else if (pr == "average3") {
    CDialog3 dlg(wnd);
    if (dlg.DoModal() == IDOK) {
      real1.Format("%g", dlg.m_sum);
    }
  }
  else if (pr == "Потеря массы средняя при кислотной обрабоке, г") {
    CDialogNew2 *dlg = (CDialogNew2 *) wnd;

    CString s4 = dlg->getValue("Средний вес пластины при установке, г");
    CString s5 = dlg->getValue("Средний вес пластины после испытаний, г");
    double msr1 = atof(s4);
    double msr2 = atof(s5);
  
    real1.Format("%f", msr1-msr2);
  }
  else if (pr == "Средняя скорость коррозии") {

    CDialogNew2 *dlg = (CDialogNew2 *) wnd;


    CString s1 = dlg->getValue("Радиус круглой пластины, мм");
    CString s2 = dlg->getValue("Толщина пластины, мм");
    CString s3 = dlg->getValue("Количество пластин в сборке");
    CString s4 = dlg->getValue("Средний вес пластины при установке, г");
    CString s5 = dlg->getValue("Средний вес пластины после испытаний, г");
    CString s6 = dlg->getValue("Потеря массы средняя при кислотной обрабоке, г");
    CString s7 = dlg->getValue("Дата установки");
    CString s8 = dlg->getValue("Дата извлечения");

    CString sr = dlg->getValue("Радиус втулки, мм");


    double Rsr = atof(s1);
    double rsr = atof(sr);
    double Dsr = atof(s2);

    int N = atoi(s3);
    double msr1 = atof(s4);
    double M1 = msr1;        // Средняя одной пластины до исп
    double msr2 = atof(s5);

    double M2 = msr2;  // Средняя одной пластины после
    double dm = atof(s6); // Потеря массы средняя при кислотной обрабоке, г
    COleDateTime d1, d2;

    d1.ParseDateTime(s7, VAR_DATEVALUEONLY );
    d2.ParseDateTime(s8, VAR_DATEVALUEONLY );

    COleDateTimeSpan ds1(d1), ds2(d2);

    if (s7 == "") {
      AfxMessageBox("Дата установки не задана");
      return "";
    }
    if (s8 == "") {
      AfxMessageBox("Дата извлечения не задана");
      return "";
    }

    int dt = ds2-ds1; // Время испытаний, сут

    if (dt <= 0) {
      AfxMessageBox("Дата извлечения должна быть позже даты установки");
      return "";
    }

/*
//    if (M1-M2-dm <= 0) {
    if (M1-M2 <= 0) {
      CString str;
      str.Format("Ошибка\n%g-%g <= %g", M1, M2, dm);
      AfxMessageBox("Недопустимые значения");
      return "";
    }
*/

    if (M1-M2 <= 0) {
      CString str;
      str.Format("Ошибка\n%g < %g\nСредний вес пластин увеличился", M1, M2);
      AfxMessageBox(str);
      return "";
    }




//    double Scp = 2*M_PI*Rsr*(Rsr+Dsr); // Средняя поверхность 1 пластины


    double Scp = 2*M_PI*(Rsr*Rsr+Rsr*Dsr-rsr*rsr); // Средняя поверхность 1 пластины

//    double k = (M1-M2-dm)*1000000/(Scp*dt); // интенсивность коррозии
    double k = (M1-M2)*1000000/(Scp*dt); // интенсивность коррозии

    
    
    double P = 0.047*k; // средняя скорость коррозии мм/год

    real1.Format("%g", P);
//    static int getValKorroz(double d, const char *tn)

         if (P <= 0.02) real2 = "1";
    else if (P <= 0.04) real2 = "2";
    else if (P <= 0.05) real2 = "3";
    else if (P <= 0.2)  real2 = "4";
    else                real2 = "5";

         if (P <= 0.03)  real3 = "1";
    else if (P <= 0.085) real3 = "2";
    else if (P <= 0.2)   real3 = "3";
    else                 real3 = "4";

//    real4.Format("%f", M2-M1);


    AfxMessageBox("Выполнено");
  }
  else if (pr == "OprUzel") {
    CDialogNew2 *dlg1 = (CDialogNew2 *) wnd;
    
    CControl *cn = dlg1->getControl("Магистраль/Распредсеть");

    int mag = -1;
    if (cn) {
      mag = atoi(cn->m_value);
    }

    CMagNode dlg(view, mag);

    if (dlg.DoModal() == IDOK) {
      const CNode *n1 = dlg.m_n1;
      const CNode *n2 = dlg.m_n2;

      if (n1) {
        real1 = n1->node_name.kod;
        real2 = n1->node_name.name;
      }
      if (n1) {
        real3 = n2->node_name.kod;
        real4 = n2->node_name.name;
      }
    }
  }
  else if (pr == "uchMag") {
    CDialogNew2 *dlg = (CDialogNew2 *) wnd;

    CControl *cn = dlg->getControl("Магистраль/Распредсеть");

    if (cn) {

      CString q;
      q.Format("SELECT ID, [Описание участка МС], [Проектная организация], [Номер проекта], [Документы регистрации] FROM [Участок МС] WHERE [Магистраль]=%s", cn->m_value);

      CTableDialog tab(wnd, getPSAlma(), q, "ID", "Описание участка МС", "Участки МС");

      if (tab.DoModal() == IDOK) {
        map<int, CString>::const_iterator it = tab.map_val.find(3);
        if (it != tab.map_val.end()) {
          real1 = it->second;
        }
        it = tab.map_val.find(4);
        if (it != tab.map_val.end()) {
          real2 = it->second;
        }
      }
    }
  }


/*
  else if (pr == "Word2") {

    CString path = getPath("PATH_AKT_TU");

    if (val != "") {
      if (val.Find(":") == -1) val = path+val;

      CString ext = GetExt(val);

      if (!IsFile(val) && (ext == "docx" || ext == "doc")) {
        ::CopyFile(argpath()+"new.docx", val, false);
      }

      HINSTANCE hi = ShellExecute(wnd->m_hWnd,"open",val,NULL,NULL,SW_SHOWNORMAL);

      if ((long)hi <= 32) {
        CString str;
        str.Format("Не могу открыть\n%s\nОшибка %d", val, hi);
        AfxMessageBox(str);
      }
    }
    else {
      CFileDialog fd(TRUE, "docx", NULL, OFN_HIDEREADONLY, "Документы|*.docx;*.doc|", wnd);
      if (path != "") fd.m_ofn.lpstrInitialDir = path;

      if (fd.DoModal() == IDOK) {
      
        val = fd.GetPathName();
        val = TrimPath(val, path);

        CGidrView *pView = getView();

        pView->Word2((CDialogNew2 *) wnd, fd.GetPathName(), "Капремонт", m_tn, "Акт капитального ремонта") ;
      }
      real1 = val;
      return real1;
    }
  }
*/

  return real1;
}




CString get_progr_val(CDialogNew2 *wnd, CControl *control, const char *progr, const char *val1, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom, CString &real)
{
  CString pr = progr;
  CString v = val1;
  real = "";
  CString val = val1;

  if (pr == "AktKapRemont" || pr == "Word2" || pr == "WordOpr") {

    if (pr == "WordOpr") control->m_file_path = "PATH_OPRESSOVKA";
    if (pr == "AktKapRemont") control->m_file_path = "PATH_KAPREMONT";


    CString path = getPath(control->m_file_path);

    if (val != "") {
      if (val.Find(":") == -1) val = path+val;

      CString ext = GetExt(val);

      if (!IsFile(val) && (ext == "docx" || ext == "doc")) {
        ::CopyFile(argpath()+"new.docx", val, false);
      }

      HINSTANCE hi = ShellExecute(wnd->m_hWnd,"open",val,NULL,NULL,SW_SHOWNORMAL);

      if ((long)hi <= 32) {
        CString str;
        str.Format("Не могу открыть\n%s\nОшибка %d", val, hi);
        AfxMessageBox(str);
      }
    }


    else {
      CString initF;
      CString str;

      if (m_tn == "Опрессовки участок очереди") {
        CString nom = wnd->getValue("Номер Акта опрессовки");
        CString date = wnd->getValue("Дата выдачи Акта опрессовки");
        initF.Format("Акт опрессовки %s %s.docx", nom, date);
      }
      else {
        initF = "Акт капитального ремонта";
        int id = wnd->getID(m_tn);
        str.Format("_%d.docx", id);
        initF += str;
      
      }


      CFileDialog fd(FALSE, "docx", initF, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "Документы|*.docx;*.doc|", wnd);
      if (path != "") fd.m_ofn.lpstrInitialDir = path;

      if (fd.DoModal() == IDOK) {
      
        val = fd.GetPathName();
        val = TrimPath(val, path);

        CGidrView *pView = getView();

        if (m_tn == "Опрессовки участок очереди") {
          pView->WordOpr((CDialogNew2 *) wnd, fd.GetPathName(), "Опрессовка", m_tn, "Акт опрессовки", "PATH_OPRESSOVKA") ;
        }
        else {
          pView->Word2((CDialogNew2 *) wnd, fd.GetPathName(), "Капремонт", m_tn, "Акт капитального ремонта", "PATH_KAPREMONT") ;
        }
      }
      real = val;
      return real;
    }
  }

  else if (pr == "WordRemont") {

    val = control->m_value;
    CString path = getPath(control->m_file_path);

    if (val != "") {
      if (val.Find(":") == -1) val = path+val;

      CString ext = GetExt(val);

      if (!IsFile(val) && (ext == "docx" || ext == "doc")) {
        ::CopyFile(argpath()+"new.docx", val, false);
      }

      HINSTANCE hi = ShellExecute(wnd->m_hWnd,"open",val,NULL,NULL,SW_SHOWNORMAL);

      if ((long)hi <= 32) {
        CString str;
        str.Format("Не могу открыть\n%s\nОшибка %d", val, hi);
        AfxMessageBox(str);
      }
    }

    else {
      CString fn = control->m_fieldName;

      CString initF;
      int id = wnd->getID(m_tn);

      initF.Format("%s_%d.docx", fn, id);
    


      CFileDialog fd(FALSE, "docx", initF, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, "Документы|*.docx;*.doc|", wnd);
      if (path != "") fd.m_ofn.lpstrInitialDir = path;

      if (fd.DoModal() == IDOK) {
      
        val = fd.GetPathName();
        val = TrimPath(val, path);

        CGidrView *pView = getView();

        pView->Word2((CDialogNew2 *) wnd, fd.GetPathName(), m_tn, m_tn, fn, control->m_file_path) ;
      }
      real = val;
      return real;
    }
  }



  
  else if (pr == "ProklMax") {
    v = getProklMaxAll();
  }

  else if (pr == "DiamMax") {
    double d = getDiamAll();
    v.Format("%g", d);
  }
  else if (pr == "DlinaGr") {
    double d = getLengthAll();
    v.Format("%g", d);
  }
  else if (pr == "DGr") {
    double d = getLengthAll();
    v.Format("%g", d);
  }

  else if (pr == "DlinaGeo") {
    double d = 0;

    CCoordList cl;

    CString que = m_tn;
    if (m_q != "") que = m_q;

    if (m_dbf->openTable(que)) {
      if (m_nom == -1) {
        if (!m_dbf->rs->IsEOF()) {
          while ( !m_dbf->rs->IsEOF() ) {
            m_dbf->readCoord(cl);
            m_dbf->rs->MoveNext();
          }
        }
      }
      else {
        if (GotoKey(m_dbf, m_nom)) {
            m_dbf->readCoord(cl);
        }
      }
    }

    v.Format("%g", cl.Length());
  }


  else if (pr == "Сolor") {
    COLORREF rgb[] = {
      0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
      0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
      0x00FFFF, 0xFFFFFF,
    };
      CColorDialog dlgColor(atoi(val));
      dlgColor.m_cc.lpCustColors = rgb;

      if (dlgColor.DoModal() == IDOK)
      {
        v.Format("%d", dlgColor.GetColor());
      }
      return v;
   }


  else if (pr == "Dlina") {

/*
//    AfxMessageBox("Dlina");
    CString que = m_tn;
    if (m_q != "") que = m_q;
    int n = 0;

    double d = 0;

    if (m_dbf->openTable(que)) {
      if (m_nom == -1) {
        if (!m_dbf->rs->IsEOF()) {
          while ( !m_dbf->rs->IsEOF() ) {
            d = getLength(m_dbf);
            m_dbf->rs->MoveNext();
          }
        }
      }
      else {
        if (GotoKey(m_dbf, m_nom)) {
            d = getLength(m_dbf);
        }
      }
    }

    v.Format("%g", d);
*/
  }
  else if (m_tn == "АКТ" && pr == "abonent"  || pr == "abonent_text") {
    CString tn = "Организации проектные";
    CMMenu2 menu(wnd, IDD_MENU10, "");
    menu.AddColumn("Наименование");
    menu.AddColumn("Телефон организации");
    menu.AddColumn("Телефон руководителя");
    menu.AddColumn("Улица");
    menu.AddColumn("Номер дома");

    CAdoFile dbf;
    init_menu(&dbf, &menu);

    menu.AddProc(IDC_ADD, add, (void*)&dbf, (void*)(const char*)tn);
    menu.AddProc(IDC_DEL, del, (void*)&dbf, (void*)(const char*)tn);
    menu.AddProc(IDC_ED, edit, (void*)&dbf, (void*)(const char*)tn);

    if (menu.DoModal() == IDOK) {
      v = menu.m_strText;
    }
  }

  else if (m_tn == "Участок теплопровода" && pr == "abonent" || pr == "abonent") {
    CString tn = "Организации владельцы";
    CMMenu2 menu(wnd, IDD_MENU10, tn);
    menu.AddColumn("Наименование");
    menu.AddColumn("Телефон организации");
    menu.AddColumn("Телефон руководителя");
    menu.AddColumn("Улица");
    menu.AddColumn("Номер дома");

    CAdoFile dbf;
    init_menu(&dbf, &menu);

    menu.AddProc(IDC_ADD, add, (void*)&dbf, (void*)(const char*)tn);
    menu.AddProc(IDC_DEL, del, (void*)&dbf, (void*)(const char*)tn);
    menu.AddProc(IDC_ED, edit, (void*)&dbf, (void*)(const char*)tn);

    if (menu.DoModal() == IDOK) {
      v.Format("%d", menu.m_row);
      real = menu.m_strText;
    }
  }

  return v;
}

