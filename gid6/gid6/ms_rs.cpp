#include "stdafx.h"
#include "gid6.h"

#include "gidrView.h"
#include "InputDia.h"
#include "mmenu.h"

#include "ps_alma.h"

//#include "DialogNew2.h"


CString num_MSRS(const char *s);

struct lessMSRS
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() ( const CString & arg1, const CString & arg2) const
    {
        CString n1 = num_MSRS((const char*)arg1);
        CString n2 = num_MSRS((const char*)arg2);

        if (n1 == n2 && arg1 != arg2) {
            return arg1 < arg2;
        }

        return n1 < n2;
    }
};

void initMSRE(map<int, UMSRS> &mapMSRS);

void initMSRE2(map<int, UMSRS> &mapMSRS, map<CString, UMSRS, lessMSRS> &mapMS, int is_mag)
{
  map<int, UMSRS>::const_iterator it = mapMSRS.begin();

  for (; it !=  mapMSRS.end(); it++) {
    UMSRS rs = it->second;
    if (is_mag == rs.mag) {
      CString s = rs.opis;
      if (s == "") s = rs.name;

      mapMS[s] = rs;
    }
  }
}

int vydMagN(CGraph2 *m_graph, int id);
int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);


void CGidrView::setMagNomer(int mag, int ms, int rs)
{
  m_ms_kod0 = ms;
  m_rs_kod0 = rs;
  m_magistral_kod0 = mag;
}



void CGidrView::setMagNew(int mag, int ms, int rs, const char *opis, bool mv)
{
  m_cxema->m_graph->reset();


//  int n = vydMag(m_cxema->m_graph, mag);
//  if (mv) findMag(mag);

  int n = 0;

  if (mag > 0) {
    n = vydMagN(m_cxema->m_graph, mag);
  }
  else if (ms > 0 || rs > 0) {
      n = viewUchastok(m_cxema->m_graph, make_tuple(ms,rs), 3, false);
  }
  else {
    return;
  }

  if (mv) MoveVyd();

  m_node_first = NULL;
  m_node_last = NULL;

  if (ms > 0) {
    UMS *ums = ps_alma.getMs(ms);
    if (ums) {
//      m_node_first = m_cxema->findAll(ums->kod1, ums->uzel1);
//      m_node_last = m_cxema->findAll(ums->kod2, ums->uzel2);;
    }
  }
  if (rs > 0) {
    URS *urs = ps_alma.getRs(rs);
    if (urs) {
//      m_node_first = m_cxema->findAll(urs->kod1, urs->uzel1);
//      m_node_last = m_cxema->findAll(urs->kod2, urs->uzel2);;
    }
  }

  m_ms_kod = ms;
  m_rs_kod = rs;
  m_magistral_kod = mag;

/*
  UMS *ms = ps_alma.getMS(kod);
  if (ms) {
    m_node_first = m_cxema->findAll(ms->kod1, ms->uzel1);
    m_node_last = m_cxema->findAll(ms->kod2, ms->uzel2);;
  }
*/
//  m_kod0_vyd = kod;


  InvalidateG();

  if (n == 0) {
    CString str;
    str.Format(_TR("Магистраль %s не сформирована, Выделите и сформируйте."), opis);

    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  }
}

int vydMsRsN(CGraph2 *m_graph, int ms_rs, int rs, bool is_only_big_pipe);


void CGidrView::setMSNew(int id, const char *opis, int ms_rs, bool mv)
{
/*
  m_cxema->m_graph->reset();
  int n = vydMsRsN(m_cxema->m_graph, ms_rs, id);
//  if (mv) findMS2(kod);
//  MoveVyd();

  m_node_first = NULL;
  m_node_last = NULL;

  if (ms_rs == 0) {
    UMS *ms = ps_alma.getMS(kod);
    if (ms) {
      m_node_first = m_cxema->findAll(ms->kod1, ms->uzel1);
      m_node_last = m_cxema->findAll(ms->kod2, ms->uzel2);;
    }
  }
  if (ms_rs == 0) {
    URS *rs = ps_alma.getRS(kod);
    if (rs) {
      m_node_first = m_cxema->findAll(rs->kod1, rs->uzel1);
      m_node_last = m_cxema->findAll(rs->kod2, rs->uzel2);;
    }
  }

  m_kod0_vyd = kod;
  m_kod0_ms_rs = ms_rs;
  m_magistral_kod = 0;


  InvalidateG();

  if (n == 0) {
    CString str;
    str.Format(_TR("Участок %s не сформирован, Выделите магистраль и сформируйте участок."), opis);

    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  }
*/
}


void CGidrView::setMSNew(int ms_rs, const char *title)
{
  map<int, UMSRS> mapMSRS;
  map<CString, UMSRS, lessMSRS> mapMS;

  initMSRE(mapMSRS);
  initMSRE2(mapMSRS, mapMS, ms_rs);
  
  CGraph2 *m_graph = m_cxema->m_graph;

  CMMenu menu(this, IDD_MENU2, title);

  map<CString, UMSRS, lessMSRS>::const_iterator it = mapMS.begin();

  for (; it !=  mapMS.end(); it++) {
    UMSRS ms = it->second;

    CString s = ms.opis;
    menu.Add(ms.opis, (void*) ms.id2);
  }
  if (menu.DoModal() == IDOK) {
    map<int, UMSRS>::const_iterator it = mapMSRS.find((int)menu.getV());
    if (it != mapMSRS.end()) {
      UMSRS ms = it->second;
//      setMSNew(ms.id, ms.opis, ms.mag, true);
    
      if (ms_rs == 0) {
        setMagNew(0, ms.id, 0, ms.opis, true);
      }
      else if (ms_rs == 1) {
        setMagNew(0, 0, ms.id, ms.opis, true);
      }
    }
  }
}

void CGidrView::OnUpdateSetMSNew(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ms_kod > 0);
}

void CGidrView::OnUpdateSetRSNew(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_rs_kod > 0);
}

void CGidrView::OnSetMSNew()
{
  setMSNew(0, _TR("Выберите Участок МС"));
}

void CGidrView::OnSetRSNew()
{
  setMSNew(1, _TR("Выберите Участок РС"));
}

void setMAG(CGraph2 *m_graph, int typ, const char *kod0, const char *kod0_2, int val, int mag_ms_rs);


void CGidrView::OnSaveMag()
{
  CString str;

  if (!m_cxema->m_graph->isMark()) {
    if (AfxMessageBox(_TR("Участки не выделены? Все равно сохранить?"), MB_YESNO) != IDYES) {
      return;
    }
  }

  if (AfxMessageBox(_TR("Сохранить?"), MB_YESNO) == IDYES) {
      ::setMAG(m_cxema->m_graph, TIP_UT, "magistral", "distSite", m_magistral_kod, 0);
  }


#if 0
  CString tab = _TR("Магистрали");
  int ret = IDCANCEL;
  CString q;
  q.Format("SELECT * FROM [%s] WHERE [ID]=%d", tab, m_magistral_kod);

  
  C1DbFile dbf;
  if (dbf.openMDB(getPSAlma())) {
    CDialogNew2 dlg(this, &dbf, tab, false);
    dlg.addQ(tab, q, -1);

/*
    if (m_node_first) {
      dlg.setValue0("kod1", m_node_first->node_name.kod);
      dlg.setValue0("uzel1", m_node_first->node_name.name);
    }
    if (m_node_last) {
      dlg.setValue0("kod2", m_node_last->node_name.kod);
      dlg.setValue0("uzel2", m_node_last->node_name.name);
    }
*/
    ret = dlg.DoModal();
    if (ret != IDCANCEL) {
      dlg.saveDB2();
    }

    dbf.closeMDB();

    if (ret == IDOK) {

      
      ::setMAG(m_cxema->m_graph, TIP_UT, _TR("Магистраль"), m_magistral_kod);
    }
  }
#endif
}

//void set_nodes_PTS(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs);

int inc_pod_pjezo(const CNode2* n);
int inc_obr_pjezo(const CNode2* n);



/*
Проверяет, что конечные узлы относятся к МС РС
*/

bool isCheckRsMs(CGraph2 *graph)
{
    CMapGraph::iterator p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        if (p->second->node.isPjezo && !p->second->typPS()) {
            int inc = inc_pod_pjezo(p->second);
            if (inc == 0) {
                inc = inc_obr_pjezo(p->second);
            }
            if (inc == 1) {
                return true;
            }
        }
        p++;
    }

    return false;
}



void CGidrView::OnSaveMS()
{
    CString str;

    if (!m_cxema->m_graph->isMark()) {
        if (AfxMessageBox(_TR("Участки не выделены? Все равно сохранить?"), MB_YESNO) != IDYES) {
            return;
        }
    }

    CString msg = _TR("Сохранить?");

    if (isCheckRsMs(m_cxema->m_graph)) {
        msg = _TR("Сохранить? Внимание! Некоторые конечные узлы не являются узлами ПТС");
    }

    if (AfxMessageBox(msg, MB_YESNO) == IDYES) {
//        bool yes = false;
//        if (AfxMessageBox("Сделать выделенные узлы узлами ПТС?", MB_YESNO) == IDYES) {
//            yes = true;
//        }
        ::setMAG(m_cxema->m_graph, TIP_UT, "magistralSite", "distSite", m_ms_kod, 1);
//        if (yes) {
//             set_nodes_PTS(m_cxema->m_ado, m_cxema->m_graph, true, m_ms_kod);
//        }
        Invalidate();
    }
}

void CGidrView::OnSaveRS()
{
  CString str;

  if (!m_cxema->m_graph->isMark()) {
    if (AfxMessageBox(_TR("Участки не выделены? Все равно сохранить?"), MB_YESNO) != IDYES) {
      return;
    }
  }

  CString msg = _TR("Сохранить?");

  if (isCheckRsMs(m_cxema->m_graph)) {
      msg = _TR("Сохранить? Внимание! Некоторые конечные узлы не являются узлами ПТС");
  }

  if (AfxMessageBox(msg, MB_YESNO) == IDYES) {
      ::setMAG(m_cxema->m_graph, TIP_UT, "distSite", "magistralSite", m_rs_kod, 2);
//      if (AfxMessageBox("Сделать выделенные узлы узлами ПТС?", MB_YESNO) == IDYES) {
//           set_nodes_PTS(m_cxema->m_ado, m_cxema->m_graph, false, m_rs_kod);
//      }
      Invalidate();
  }
}


void setMS(CGraph2 *m_graph, int typ, const char *kod0, const char *val);
//int editWithDoc2(CWnd *parent, C1DbFile *dbf, const char *tn, const char *elem, long kk, int ro);
//int editWithDoc2(CWnd *parent, C1DbFile *dbf, const char *tn, const char *elem, long kk, int ro, map<CString, CString> &map_value0);


#if 0

void CGidrView::OnSaveMS()
{
  CString str;

  if (!m_cxema->m_graph->isMark()) {
    if (AfxMessageBox(_TR("Участки не выделены? Все равно сохранить?"), MB_YESNO) != IDYES) {
      return;
    }
  }

  CString q;
  
  CString tab = "", tab2 = "";
  int id = 0;

  if (m_ms_kod > 0) {
    tab = _TR("Участок МС"); 
    tab2 = _TR("Паспорт участка МС"); 
    id = m_ms_kod;
  }
  else if (m_rs_kod > 0) {
    tab = _TR("Участок РС");
    tab2 = _TR("Паспорт участка РС"); 
    id = m_rs_kod;
  }
  else {
    return;
  }

  q.Format("SELECT * FROM [%s] WHERE [ID]=%d", tab, id);


  int ret = IDCANCEL;


#if 0
  C1DbFile dbf;
  if (dbf.openMDB(getPSAlma())) {
    CDialogNew2 dlg(this, &dbf, tab, false);
    dlg.addQ(tab, q, -1);

    if (m_node_first) {
      dlg.setValue0("kod1", m_node_first->node_name.kod);
      dlg.setValue0("uzel1", m_node_first->node_name.name);
      dlg.setValue0(_TR("Узел подключения"), PSName(m_node_first));
      dlg.setValue0(_TR("Начальная камера"), PSName(m_node_first));
    }
    if (m_node_last) {
      dlg.setValue0("kod2", m_node_last->node_name.kod);
      dlg.setValue0("uzel2", m_node_last->node_name.name);
      dlg.setValue0(_TR("Конечный узел"), PSName(m_node_last));
      dlg.setValue0(_TR("Конечная камера"), PSName(m_node_last));
    }

    ret = dlg.DoModal();
    if (ret != IDCANCEL) {
      dlg.saveDB2();
    }

    dbf.closeMDB();

    if (ret == IDOK) {
//      ::setMS(m_cxema->m_graph, TIP_UT, tab, id);
  
      void setMagMsRs(CGraph2 *m_graph, int typ, int mag, int ms, int rs);
      setMagMsRs(m_cxema->m_graph, TIP_UT, m_magistral_kod, m_ms_kod, m_rs_kod);
    
    }
  }
#endif

    C1DbFile dbf;

    if (dbf.openMDB(getPSAlma())) {
      map<CString, CString> map_value0;

      if (m_node_first) {
        map_value0["kod1"] = m_node_first->node_name.kod;
        map_value0["uzel1"] = m_node_first->node_name.name;
        map_value0[_TR("Узел подключения")] = PSName(m_node_first);
        map_value0[_TR("Начальная камера")] = PSName(m_node_first);
      }
      if (m_node_last) {
        map_value0["kod2"] = m_node_last->node_name.kod;
        map_value0["uzel2"] = m_node_last->node_name.name;
        map_value0[_TR("Конечный узел")] = PSName(m_node_last);
        map_value0[_TR("Конечная камера")] = PSName(m_node_last);
      }

      int ret = editWithDoc2(this, &dbf, tab, tab2, id, false, map_value0);
      if (ret == IDOK) {
        void setMagMsRs(CGraph2 *m_graph, int typ, int mag, int ms, int rs);
        setMagMsRs(m_cxema->m_graph, TIP_UT, m_magistral_kod, m_ms_kod, m_rs_kod);
      }

      dbf.closeMDB();
    }
}

#endif


void CGidrView::OnUpdatePasp(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_cxema->m_graph->isMark());
//  pCmdUI->Enable(m_ms_kod0 > 0 || m_rs_kod0 > 0);
}

void CGidrView::OnUpdateSaveMS(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_ms_kod > 0);
}

void CGidrView::OnUpdateSaveRS(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_rs_kod > 0);
}


void CGidrView::OnUpdateSaveMag(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_magistral_kod > 0);
}


void CGidrView::setMsNode(CNode2 *node, int first_last)
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

void CGidrView::OnMsPassport()
{
//  if (m_ms_kod0 <= 0 && m_rs_kod0 <= 0) return;

  int id = 0, ms_rs = 0;

  if (m_ms_kod0 > 0) {
    id = m_ms_kod0;
    ms_rs = 0;
  }
  else if (m_rs_kod0 > 0) {
    id = m_rs_kod0;
    ms_rs = 1;
  }

  
  if (m_ms > 0) {
    id = m_ms;
    ms_rs = 0;
  }
  else if (m_rs > 0) {
    id = m_rs;
    ms_rs = 1;
  }


  PaspRSMS(0, 0, ms_rs, id);
}


#if 0
void CGidrView::PaspRSMS(CNode2 *nl, CNode2 *n1, int ms_rs, int id)
{
}
#endif





void CGidrView::OnSaveUtPats()
{
}

void CGidrView::OnUpdateSaveUtPts(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_pts_ut_kod > 0);
}
