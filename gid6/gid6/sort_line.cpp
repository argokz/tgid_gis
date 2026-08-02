#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrView.h"


static int ii2;

static deque<CNode2*> st2;

bool compare_diam (const CLINE2 *l1, const CLINE2 *l2)
{
  return bline(l1)->line.pod.diam > bline(l2)->line.pod.diam;
}

bool dfs2(CNode2 *n)
{
  if (n->node.Color == 1) return true;
  if (n->node.Color == 2) return false;

  n->node.Color = 1;

  for (CLINE2 *ll = n->lines; ll; ll = next(ll)) {
    CLine2 *l = bline(ll);
    if (l->line.pod.q*napr(ll) < 0 || (l->line.pod.q == 0 && l->line.nomP != -1 && napr(ll) < 0)) {
      if (dfs2(other(ll))) return true;
    }
  }

  n->node.n_sort = ii2++;

  st2.push_back(n);

  n->node.Color = 2;

  return false;
}

// Сортировка участков


void sort_graph(CGraph2 *graph)
{
  CMapGraph::iterator p = graph->map_node.begin();

  bool Cycle;
  ii2 = 1;

  while (p != graph->map_node.end()) {
    CNode2 *n = p->second; 
    Cycle = dfs2(n);
    p++;
  }

  int iii = 1;

  while ( !st2.empty() ) {
    CNode2 *v = st2.front();
//    st2.pop_front();

    if (v) {
      CLINE2 *max_ll = NULL;
      double max_diam = -1;
      int n_l = 0;

      bool not_first = false;

      for (CLINE2 *ll = v->lines; ll; ll = next(ll)) {
        CLine2 *l = bline(ll);
        
        if (l->line.nomP != -1 && other(ll)->node.n_sort > v->node.n_sort) {
          if (l->line.n_sort == 0) {
            n_l ++;
            if (l->line.pod.diam > max_diam) {
              max_diam = l->line.pod.diam;
              max_ll = ll;
            }
          }
          else {
            not_first = true;
          }
        }
      }

      if (not_first) iii++;


      if (max_ll) {
        st2.push_front(other(max_ll));
        bline(max_ll)->line.n_sort = iii;
      }
      else {
        st2.pop_front();
      }
    }
  }
}

/*

void CGidrView::OnUtNapr2()
{
  sort_graph(m_cxema->m_graph);

  GetDocument()->SetModifiedFlag(true);
  InvalidateG();
}

*/


#include "mmenu.h"
#include "sort_graph.h"


void copyVydGraph_new(CGraph2 *graph1, CGraph2 *graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);
CString getPSTable(int n);
bool isPS(CNode2* node);

void CGidrView::OnUtNapr2()
{
    list<CLINE2*> st_l;
    list<CNode2*> list_sort;

    CGraph2 graph2(NULL);

    copyVydGraph_new(m_cxema->m_graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CMMenu menu(this, IDD_MENU8);
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;

    m_cxema->m_graph->reset();
    m_cxema->m_graph->init_find_line_nom();

    list<int> list_ut_min;

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = m_cxema->m_graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = m_cxema->m_graph->find_line_nom(bline(l)->line.nomO);


        //        if (lP) bline(lP)->line.isPjezo = true;
        //        if (lO) bline(lO)->line.isPjezo = true;

        CNode2* n1 = where(l);
        CNode2* n2 = other(l);

        if (n1_old && (isPS(n1) || n1 != n2_old)) {
            //            menu.Add("-------------------", (void*)0);
            if (list_ut_min.size() > 0) {
                list_ut.push_back(list_ut_min);
            }

            list_ut_min.clear();
        }


        CString str;
        str = bline(l)->getNamePT() + " :::: " + bline(l)->getName();
        str = bline(l)->getNamePT();

        CString str1 = n1->getNamePT() == "" ? CString("(") + n1->getName() + CString(")") : n1->getNamePT();
        CString str2 = n2->getNamePT() == "" ? CString("(") + n2->getName() + CString(")") : n2->getNamePT();

        str = str1 + " - " + str2;

        //        menu.Add(str, (void*) 0);


        list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lP)->line.nomO);

        n1_old = n1;
        n2_old = n2;

        //        list_ut.push_back();
    }

    if (list_ut_min.size() > 0) {
        list_ut.push_back(list_ut_min);
    }

    int nB = 0;

    for (auto p1 : list_ut) {

        CString ss = "";

        for (auto p2 : p1) {
            int id = p2;
            CLINE2* l = m_cxema->m_graph->find_line_nom(id);
            if (l) {
                CNode2* n1 = where(l);
                CNode2* n2 = other(l);

                CString str1 = n1->getNamePT() == "" ? CString("(") + n1->getName() + CString(")") : n1->getNamePT();
                CString str2 = n2->getNamePT() == "" ? CString("(") + n2->getName() + CString(")") : n2->getNamePT();

                CString str = str1 + " - " + str2;

                ss += str;
                ss += " | ";

                //                 menu.Add(str, (void*)0);
            }
        }
        //         menu.Add("++++++++++++++", (void*)0);
        menu.Add(ss, (void*)nB);
        nB++;
    }


    if (menu.DoModal() == IDOK) {
        int n = (int)menu.getV();
        if (n >= 0) {

            auto p1 = list_ut.begin();

            for (int i = 0; i < n; i++) {
                p1++;
            }
            if (p1 != list_ut.end()) {

                //                list<int> list_ut_min = *p1;

                CString ss = "";

                for (auto p2 : *p1) {
                    int id = p2;
                    CLINE2* l = m_cxema->m_graph->find_line_nom(id);
                    if (l) {
                        bline(l)->line.isPjezo = true;

                        CNode2* n1 = where(l);
                        CNode2* n2 = other(l);

                        CString str1 = n1->getNamePT() == "" ? CString("(") + n1->getName() + CString(")") : n1->getNamePT();
                        CString str2 = n2->getNamePT() == "" ? CString("(") + n2->getName() + CString(")") : n2->getNamePT();

                        CString str = str1 + " - " + str2;

                        ss += str;
                        ss += "\n";

                    }
                }
                AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
                Invalidate();
            }
        }
    }

    //  sort_graph(m_cxema->m_graph);
    //  GetDocument()->SetModifiedFlag(true);
    //  InvalidateG();
}

void make_big_ut(CGraph2 *graph, list<CLINE2*> &st_l, list<CNode2*> &list_sort, list <list<int> > & list_ut)
{
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    graph->reset();
    graph->init_find_line_nom();

    list<int> list_ut_min;

    for (auto p : st_l) {
        CLINE2* l = p;

        CNode2* n1 = where(l);
        CNode2* n2 = other(l);

        CLINE2* lP = bline(l)->line.nomP > 0 ? graph->find_line_nom(bline(l)->line.nomP) : nullptr;
        CLINE2* lO = bline(l)->line.nomO > 0 ? graph->find_line_nom(bline(l)->line.nomO) : nullptr;

        if (lP || lO) {
            if (n1_old && (isPS(n1) || n1 != n2_old)) {
                if (list_ut_min.size() > 0) {
                    list_ut.push_back(list_ut_min);
                }
                list_ut_min.clear();
            }

            if (lP) {
                list_ut_min.push_back(bline(lP)->line.nomP);
            }
            else if (lO) {
                list_ut_min.push_back(bline(lO)->line.nomO);
            }
        }

        n1_old = n1;
        n2_old = n2;
    }

    if (list_ut_min.size() > 0) {
        list_ut.push_back(list_ut_min);
    }
}
