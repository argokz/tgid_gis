#include "stdafx.h"
#include "stdafx.h"
#include "gid6.h"
#include "ado.h"

#include "gidrView.h"

/*
void setMS(CGraph *m_graph, int typ, const char *kod0, const char *val)
{
  CWaitCursor wait;

//  CGraph *m_graph = m_cxema->m_graph;

  CMapGraph::iterator p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    for (CLINE *l = n->lines; l; l = next(l)) {
      CLine *ll = bline(l);
      if (ll->typ == typ && !ll->isPjezo && IsBegin(l) && ll->kod0 == val) {
        if (dbf->openTable(linef[ll->typ].table)) {

          if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
            dbf->Edit();
            dbf->write(kod0, "");
            dbf->Update();
          }
          if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
            dbf->Edit();
            dbf->write(kod0, "");
            dbf->Update();
          }
        }
        ll->kod0 = "";
      }
    }
    p++;
  }


  p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    if (n->isPjezo) {
      for (CLINE *l = n->lines; l; l = next(l)) {
        CLine *ll = bline(l);
        if (ll->typ == typ && ll->isPjezo && IsBegin(l) && ll->kod0 != val) {
          if (dbf->openTable(linef[ll->typ].table)) {
            if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
            if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
          }
          ll->kod0 = val;
        }
      }
    }
    p++;
  }

  AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
}
*/

void setOpr(CGraph2* m_graph, int typ, const char* kod0, int val)
{
    CWaitCursor wait;
    int ispr = 0;

    CAdoFile* ado = getAdo(getGidAdoName());


    bool create_vyd_line_table(CAdoFile * ado, CGraph2 * graph, CString & tn);


    CString tn, q;
        
    create_vyd_line_table(ado, m_graph, tn);

    q.Format(
        "UPDATE hps"
        " SET hps.crimpingQueSite = IIF(tmp.id IS NULL, 0, %d)"
        " FROM heatPipeSections hps"
        " JOIN linesobj l ON hps.lineID = l.id"
        " LEFT JOIN %s tmp ON tmp.id = l.id", val, tn);


    long affected;

    if (ado->Execute(q, &affected)) {

        CMapGraph::iterator p = m_graph->map_node.begin();

        while (p != m_graph->map_node.end()) {
            CNode2* n = p->second;

            for (CLINE2* l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);
                if (ll->line.typ == typ && !ll->line.isPjezo && IsBegin(l) && ll->line.m_opressovka == val) {
                    ll->line.m_opressovka = 0;
                }
                else if (ll->line.typ == typ && ll->line.isPjezo && IsBegin(l)) {
                    ll->line.m_opressovka = val;
                }
            }
            p++;
        }
    }


#if 0

    CMapGraph::iterator p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;

        for (CLINE2* l = n->lines; l; l = next(l)) {
            CLine2* ll = bline(l);
            if (ll->line.typ == typ && !ll->line.isPjezo && IsBegin(l) && ll->line.m_opressovka == val) {
                if (ado->openTable(linef[ll->line.typ].table)) {

                    if (ll->line.nomP > 0 && ado->GotoKey(ll->line.nomP)) {
                        ado->Edit();
                        ado->write(kod0, 0);
                        ado->Update();
                        ispr++;
                    }
                    if (ll->line.nomO > 0 && ll->line.nomP != ll->line.nomO && ado->GotoKey(ll->line.nomO)) {
                        ado->Edit();
                        ado->write(kod0, 0);
                        ado->Update();
                        ispr++;
                    }
                }
                ll->line.m_opressovka = 0;
            }
        }
        p++;
    }

    p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;

        if (1 /*n->isPjezo*/) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);
                if (ll->line.typ == typ && ll->line.isPjezo && IsBegin(l) && ll->line.m_opressovka != val) {
                    if (ado->openTable(linef[ll->line.typ].table)) {
                        if (ll->line.nomP > 0 && ado->GotoKey(ll->line.nomP)) {
                            ado->Edit();
                            ado->write(kod0, val);
                            ado->Update();
                            ispr++;
                        }
                        if (ll->line.nomO > 0 && ll->line.nomP != ll->line.nomO && ado->GotoKey(ll->line.nomO)) {
                            ado->Edit();
                            ado->write(kod0, val);
                            ado->Update();
                            ispr++;
                        }
                    }
                    ll->line.m_opressovka = val;
                }
            }
        }
        p++;
    }
    if (ispr) {
        AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
    }
#endif

}




#if 0

void setMS(CGraph *m_graph, int typ, const char *kod0, int val)
{
  CWaitCursor wait;

///  CGraph *m_graph = m_cxema->m_graph;

  CMapGraph::iterator p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    for (CLINE *l = n->lines; l; l = next(l)) {
      CLine *ll = bline(l);
      if (ll->typ == typ && !ll->isPjezo && IsBegin(l) && ll->ms == val) {
        if (dbf->openTable(linef[ll->typ].table)) {

          if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
            dbf->Edit();
            dbf->write(kod0, 0);
            dbf->Update();
          }
          if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
            dbf->Edit();
            dbf->write(kod0, 0);
            dbf->Update();
          }
        }
        ll->ms = 0;
      }
    }
    p++;
  }


  p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    if (1 /*n->isPjezo*/) {
      for (CLINE *l = n->lines; l; l = next(l)) {
        CLine *ll = bline(l);
        if (ll->typ == typ && ll->isPjezo && IsBegin(l) && ll->ms != val) {
          if (dbf->openTable(linef[ll->typ].table)) {
            if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
            if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
          }
          ll->ms = val;
        }
      }
    }
    p++;
  }

  AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
}


void setRS(CGraph *m_graph, int typ, const char *kod0, int val)
{
  CWaitCursor wait;

///  CGraph *m_graph = m_cxema->m_graph;

  CMapGraph::iterator p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    for (CLINE *l = n->lines; l; l = next(l)) {
      CLine *ll = bline(l);
      if (ll->typ == typ && !ll->isPjezo && IsBegin(l) && ll->rs == val) {
        if (dbf->openTable(linef[ll->typ].table)) {

          if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
            dbf->Edit();
            dbf->write(kod0, 0);
            dbf->Update();
          }
          if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
            dbf->Edit();
            dbf->write(kod0, 0);
            dbf->Update();
          }
        }
        ll->rs = 0;
      }
    }
    p++;
  }


  p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    if (1 /*n->isPjezo*/) {
      for (CLINE *l = n->lines; l; l = next(l)) {
        CLine *ll = bline(l);
        if (ll->typ == typ && ll->isPjezo && IsBegin(l) && ll->rs != val) {
          if (dbf->openTable(linef[ll->typ].table)) {
            if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
            if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
              dbf->Edit();
              dbf->write(kod0, val);
              dbf->Update();
            }
          }
          ll->rs = val;
        }
      }
    }
    p++;
  }

  AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
}


void setMagMsRs(CGraph *m_graph, int typ, int mag, int ms, int rs)
{
  CWaitCursor wait;

///  CGraph *m_graph = m_cxema->m_graph;

  CMapGraph::iterator p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    for (CLINE *l = n->lines; l; l = next(l)) {
      CLine *ll = bline(l);
      if (ll->typ == typ && !ll->isPjezo && IsBegin(l) && (ll->mag == mag && mag > 0 || ll->ms == ms && ms > 0 || ll->rs == rs && rs > 0)) {
        if (dbf->openTable(linef[ll->typ].table)) {

          if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
            dbf->Edit();
            if (mag > 0) dbf->write(_TR("Магистраль"), 0);
            if (ms > 0) dbf->write(_TR("Участок МС"), 0);
            if (rs > 0) dbf->write(_TR("Участок РС"), 0);
            dbf->Update();
          }
          if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
            dbf->Edit();
            if (mag > 0) dbf->write(_TR("Магистраль"), 0);
            if (ms > 0) dbf->write(_TR("Участок МС"), 0);
            if (rs > 0) dbf->write(_TR("Участок РС"), 0);
            dbf->Update();
          }
        }
        if (mag > 0) ll->mag = 0;
        if (ms > 0) ll->ms = 0;
        if (rs > 0) ll->rs = 0;
      }
    }
    p++;
  }


  p = m_graph->set_node.begin();

  while (p != m_graph->set_node.end()) {
    CNode *n = &*p;

    CDbFile *dbf = n->file->m_dbf;

    if (1/*n->isPjezo*/) {
      for (CLINE *l = n->lines; l; l = next(l)) {
        CLine *ll = bline(l);
        if (ll->typ == typ && ll->isPjezo && IsBegin(l) && !((ll->mag == mag && mag > 0 || ll->ms == ms && ms > 0 || ll->rs == rs && rs > 0))) {
          if (dbf->openTable(linef[ll->typ].table)) {
            if (ll->nomP > 0 && dbf->GotoKey(ll->nomP)) {
              dbf->Edit();
              if (mag > 0) dbf->write(_TR("Магистраль"), mag);
              if (ms > 0) dbf->write(_TR("Участок МС"), ms);
              if (rs > 0) dbf->write(_TR("Участок РС"), rs);
              dbf->Update();
            }
            if (ll->nomO > 0 && ll->nomP != ll->nomO && dbf->GotoKey(ll->nomO)) {
              dbf->Edit();
              if (mag > 0) dbf->write(_TR("Магистраль"), mag);
              if (ms > 0) dbf->write(_TR("Участок МС"), ms);
              if (rs > 0) dbf->write(_TR("Участок РС"), rs);
              dbf->Update();
            }
          }
          if (mag > 0) ll->mag = mag;
          if (ms > 0) ll->ms = ms;
          if (rs > 0) ll->rs = rs;
        }
      }
    }
    p++;
  }

  AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
}

#endif

void setMAG(CGraph2 *m_graph, int typ, const char *kod0, const char *kod0_2, int val, int mag_ms_rs)
{
    CWaitCursor wait;
    CString tn = getLineTableMySQL(typ);

    CString q;
    long affected;
    CAdoFile *ado = m_graph->m_cxema->m_ado;

    CMapGraph::iterator p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2 *n = p->second;

        for (CLINE2 *l = n->lines; l; l = next(l)) {
            CLine2 *ll = bline(l);

            int lval = 0;
            switch (mag_ms_rs) {
            case 0: lval = ll->line.mag; break;
            case 1: lval = ll->line.ms; break;
            case 2: lval = ll->line.rs; break;
            }

/*
            if (ll->line.typ == typ && !ll->line.isPjezo && IsBegin(l) && lval == val) {
                if (ll->line.nomP > 0) {
                    q.Format("UPDATE %s SET %s=%d WHERE ID=%d", tn, kod0, 0, ll->line.idP2);
                    ado->Execute(q, &affected);
                }
                if (ll->line.nomO > 0 && ll->line.nomP != ll->line.nomO) {
                    q.Format("UPDATE %s SET %s=%d WHERE ID=%d", tn, kod0, 0, ll->line.idO2);
                    ado->Execute(q, &affected);
                }
                switch (mag_ms_rs) {
                  case 0: ll->line.mag = 0; break;
                  case 1: ll->line.ms = 0; break;
                  case 2: ll->line.rs = 0; break;
                }

            }
*/

            if (ll->line.typ == typ && !ll->line.isPjezo && IsBegin(l) && lval == val) {
                if (ll->line.nomP > 0) {
                    q.Format("UPDATE %s SET %s=%d, %s=%d WHERE ID=%d", tn, kod0, 0, kod0_2, 0, ll->line.idP2);
                    ado->Execute(q, &affected);
                }
                if (ll->line.nomO > 0 && ll->line.nomP != ll->line.nomO) {
                    q.Format("UPDATE %s SET %s=%d, %s=%d WHERE ID=%d", tn, kod0, 0, kod0_2, 0, ll->line.idO2);
                    ado->Execute(q, &affected);
                }
                switch (mag_ms_rs) {
                  case 0: ll->line.mag = 0; break;
                  case 1: ll->line.ms = 0; break;
                  case 2: ll->line.rs = 0; break;
                }

            }
        }
        p++;
    }

    p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2 *n = p->second;

        for (CLINE2 *l = n->lines; l; l = next(l)) {
            CLine2 *ll = bline(l);

            int lval = 0;
            switch (mag_ms_rs) {
            case 0: lval = ll->line.mag; break;
            case 1: lval = ll->line.ms; break;
            case 2: lval = ll->line.rs; break;
            }

            if (ll->line.typ == typ && ll->line.isPjezo && IsBegin(l) && lval != val) {
                if (ll->line.nomP > 0) {
                    q.Format("UPDATE %s SET %s=%d WHERE ID=%d", tn, kod0, val, ll->line.idP2);
                    ado->Execute(q, &affected);
                }
                if (ll->line.nomO > 0 && ll->line.nomP != ll->line.nomO) {
                    q.Format("UPDATE %s SET %s=%d WHERE ID=%d", tn, kod0, val, ll->line.idO2);
                    ado->Execute(q, &affected);
                }
                switch (mag_ms_rs) {
                  case 0: ll->line.mag = val; break;
                  case 1: ll->line.ms = val; break;
                  case 2: ll->line.rs = val; break;
                }
            }
        }
        p++;
    }

    AfxMessageBox(_TR("Готово!"), MB_OK|MB_ICONINFORMATION);
}

