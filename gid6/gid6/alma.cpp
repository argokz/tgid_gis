#include "stdafx.h"
#include "gid6.h"

#include "gidrView.h"
#include "ado.h"

#include "graph.h"

#include "mmenu.h"

#include "sort_graph.h"

void sort_line(CGraph2 *m_graph, bool is_mark);

CFRect getRect(CLINE2 *l);

double getLengthLines2(CNode2 *n, CFPoint pt, CLINE2 *vl)
{
    double len_min = 1e20;

    for (CLINE2 *l = n->lines; l; l = next(l)) {
        if (bline(l) == bline(vl)) continue;
        if (where(l) == where(vl) && other(l) == other(vl)) continue;
        if (where(l) == other(vl) && other(l) == where(vl)) continue;

        double len = getLength(&bline(l)->line, pt);
        if (len < len_min) {
            len_min = len;
        }
    }
    return len_min;
}



int find_geo_line(CLINE2 *vl, list<int> &list_geo, Klassif *kls)
{
  if (!kls) return -1;
  
  CFRect rectAll = getRect(vl);

  CNode2 *n = where(vl);
  CNode2 *o = other(vl);

  CFRect rect, r;

//  m_geofile->m_kls000 = kls;

  int nn = kls->NObj();

  for ( int i = 1; i <= nn; i++ ) {
    m_geofile->GetFRect(kls, i, rect);

    if (r.IntersectRect(rect, rectAll)) {
    
      CCoordList cl;
      m_geofile->GetCoord(kls, i, cl);
      CFPoint pt = cl.first();
      double len = getLength(vl, pt);

      int qq = kls->geo4[i-1]->nom;

      double md = LEN100;

      if (len < md) {

        double len1 = getLengthLines2(where(vl), pt, vl);
        double len2 = getLengthLines2(other(vl), pt, vl);

        if (len <= len1 && len <= len2) {
          list_geo.push_back(i);
        }
      }
    }
  }
  return list_geo.size();
}



void CGidrView::viewGeo(const char *tab)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tab));
    if (!kls) return;

    VYD_LINES vyd_l;

    int isMark = m_cxema->m_graph->isMark();

    vyd_l.init(m_cxema->m_graph, !isMark, false);

    map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin();

    list<int> list_geo;

    CString sql;
    long affected;
    CAdoFile *ado = m_cxema->m_ado;
    CString tn = "VYD_LINE";

    if (ado) {
        sql.Format("DROP TABLE %s", tn);
        ado->Execute(sql, &affected);

        sql.Format("CREATE TABLE %s (id1 int, id2 int, id_geo int, nodeID1 int, nodeID2 int )", tn, N3, N10, N3, N10);

        ado->Execute(sql, &affected);

        if (ado->openTable0(tn)) {
            for (; it != vyd_l.map_line.end(); it++) {
                CLINE2 *vl = it->second;
                CLine2 *l = bline(vl);

                list_geo.clear();

                int n = find_geo_line(vl, list_geo, kls);

                if (n) {
                    CNode2 *n1 = where(vl);
                    CNode2 *n2 = other(vl);

                    bool ret = false;

                    list<int>::const_iterator it = list_geo.begin();
                    for (; it != list_geo.end(); it++) {

                        long id1 = l->line.nomP >= 0 ? l->line.nomP : l->line.nomO;
                        long id2 = l->line.n_sort * 2;
                        long id_geo = kls->geo4[*it - 1]->nom;

                        sql.Format("INSERT INTO %s (id1, id2, id_geo, nodeID1, nodeID2) VALUES (%d,%d,%d)", tn, id1, id2, id_geo, n1->id, n2->id);
                        if (ado->Execute(sql, &affected) && affected) {
                            ret = true;
                        }

                    }

                }
            }
            ado->closeTable();
        }
    }

    CString q;
//    q.Format("SELECT * FROM [%s] l INNER JOIN [%s].[%s] v ON l.nomer = v.id1 ORDER BY v.id2", ST_UT, mdb, tn);
//    q.Format("SELECT * FROM [%s] l INNER JOIN [%s].[%s] v ON l.ID = v.id_geo ORDER BY v.id2", tab, mdb, tn);


    //  RunTable(m_cxema->m_cur_file->m_dbf->m_strFileName, ST_UT, q, "Help_Основная.mdb", -1, OSN, FALSE, FALSE, FALSE, FALSE);
//    RunTable(kls->m_db->m_strFileName, tn + " _", q, "Help_Основная.mdb", -1, OSN, FALSE, FALSE, FALSE, FALSE);
}

void CGidrView::viewUT()
{
    CGraph2 *graph = m_cxema->m_graph;

    CNode2 *node = NULL;
//    CString tn = "VYD_LINE";

    sort_line(graph, true);

    {

        map<int, CLINE2*> map_line;

        bool isAll = true;

        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

        p = graph->map_node.begin();

        while (p != graph->map_node.end()) {
            CNode2 *n = p->second;

            //    map_node[n->n_sort] = n;

            for (CLINE2 *l = n->lines; l; l = next(l)) {
                CLine2 *ll = bline(l);
                if (ll->line.isPjezo) {
                    isAll = false;
                    break;
                }
            }
            p++;
        }


        p = graph->map_node.begin();

        while (p != graph->map_node.end()) {
            CNode2 *n = p->second;

            //    map_node[n->n_sort] = n;

            for (CLINE2 *l = n->lines; l; l = next(l)) {
                CLine2 *ll = bline(l);
                if (ll->line.n_sort) map_line[ll->line.n_sort] = l;
            }
            p++;
        }

        CString tn = "wrwertwerty1";
        long affected;
        CString sql;
        CAdoFile *ado = m_cxema->m_ado;

        if (ado) {
            sql.Format("DROP TABLE %s", tn);
            ado->Execute(sql, &affected);

            sql.Format("CREATE TABLE %s (id1 int, id2 int, KEY (id1), PRIMARY KEY (id2))", tn);

            ado->Execute(sql, &affected);

            sql.Format("SELECT * FROM %s", tn);

            bool ret = false;

            if (ado->openTable0(sql)) {

                map<int, CLINE2*>::const_iterator it;

                for (it = map_line.begin(); it != map_line.end(); it++) {
                    CLine2 *l = bline(it->second);

                    if (l->line.isPjezo || isAll) {
                        node = where(l);
                        if (l->line.nomP > 0) {
                            sql.Format("INSERT INTO %s (id1, id2) VALUES (%d,%d)", tn, l->line.nomP, l->line.n_sort * 2);
                            if (ado->Execute(sql, &affected) && affected) {
                                ret = true;
                            }
                        }

                        if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
                            sql.Format("INSERT INTO %s (id1, id2) VALUES (%d,%d)", tn, l->line.nomO, l->line.n_sort * 2 + 1);
                            if (ado->Execute(sql, &affected) && affected) {
                                ret = true;
                            }
                        }
                    }
                }
                ado->closeTable();

                sql.Format("SELECT * FROM linesobj l1 JOIN %s tmp ON l1.id=tmp.id1 ORDER BY id2", tn);

                RunTableDB(ado->m_schema, tn, sql, -1, virt_data_unknown, "", "");

            }
        }
    }
}




void CGidrView::OnViewDbAlma()
{
  CMMenu menu(this, IDD_MENU3, _TR("Выберите таблицу объекта с исходной информацией"));

  menu.Add(_TR("Трубопроводы"), (void*)0);

/*
  if (set_typ.size()) {
    for (int i = 0; i < 20 && getPSTable(i+1) != ""; i++) {
      set<int>::const_iterator it = set_typ.find(i+1);
      if (it != set_typ.end()) {
        menu.Add(getPSTable(i+1), (void*) (i+1));
      }
    }
  }
*/

  char *tabs[] = {
  "Воздушник",
  "Дренажный кран",
  "Запорная арматура",
  "Компенсатор",
  "Опора",
  "Переход диаметра",
  "Угол поворота трубопровода"
  };

  for (int i = 0; i < sizeof(tabs)/sizeof(tabs[0]); i++) {
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tabs[i]));
    if (kls) {
      menu.Add(_TR(tabs[i]), (void*) (i+1));
    }
  }


  if (menu.DoModal() == IDOK) {
    int id = (int)menu.getV();
    if (id == 0) {
      viewUT();
    }
    if (id > 0) {
      viewGeo(_TR(tabs[id-1]));
    }
  }
}