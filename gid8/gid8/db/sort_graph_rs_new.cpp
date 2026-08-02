#include <QApplication>
#include <QtSql>
#include <iostream>

#include "cxema/Graph.h"
#include "cxema/graph2.h"

static int ii2 = 1;
static int iii = 1;

static int nnn = 1;
static int nll = 1;

static bool dfs4(CNode2 *n, list<CLINE2 *> &st_l)
{
  deque<CNode2*> st2;

  st2.push_back(n);

  while (!st2.empty()) {
    n = st2.back();
    st2.pop_back();

    if (n->node.n_sort == 0) n->node.n_sort = nnn ++;

    double max_diam = -1;
    CLINE2 *max_l = NULL;

    CLINE2 *ll;

    for (ll = n->lines; ll; ll = nextl(ll)) {
      CLine2 *l = bline(ll);
      CNode2 *o = other(ll);

      double diam = l->line.nomP > 0 ? l->line.pod.diam : l->line.obr.diam;
      
      if (l->line.n_sort == 0 && diam > max_diam) {
        max_diam = diam;
        max_l = ll;
      }
    }

    if (max_l) {
      st2.push_back(other(max_l));
      st_l.push_back(max_l);
      bline(max_l)->line.n_sort = nll++;
    }

    int ns = 0;

    for (ll = n->lines; ll; ll = nextl(ll)) {
      if (!bline(ll)->line.n_sort) {
        ns = 1;
        break;
      }
    }
    if (ns) {
      st2.push_front(n);
    }
  }

  return false;
}

struct less_sort
{
    using result_type = bool;
    using first_argument_type = const CNode2*;
    using second_argument_type = const CNode2*;
    result_type operator () ( const CNode2 * arg1, const CNode2 * arg2) const
    {
        return arg1->node.n_sort < arg2->node.n_sort;
    }
};

void sort_line_rs_new(CGraph2 *graph, list<CNode2*> &list_sort, list<CLINE2 *> &st_l, int fileID)
{
  st_l.clear();
  
  auto p = graph->map_node.begin();

  while (p != graph->map_node.end()) {
    CNode2 *n = p->second;
    n->node.n_sort = 0;

    for (CLINE2 *l = n->lines; l; l = nextl(l)) {
      bline(l)->line.n_sort = 0;
    }

    p++;
  }

  nnn = 1;
  nll = 1;

  while (1) {
    CNode2 *n0 = NULL;
    p = graph->map_node.begin();
 
    if (!n0) {
      p = graph->map_node.begin();

      while (p != graph->map_node.end()) {
        CNode2 *n = p->second;
        if (n->node.n_sort == 0 && n->node.nZN != -1) {
          n0 = n;
          break;
        }
        if (!n0) {
          if (n->node.n_sort == 0 && inc(n) == 1) {
            n0 = n;
          }
        }
        p++;
      }
    }

    if (!n0) break;
    dfs4(n0, st_l);
  }

  set<CNode2*, less_sort> set_sort;

  p = graph->map_node.begin();
  while (p != graph->map_node.end()) {
    CNode2 *n = p->second; 
    set_sort.insert(n);
    p++;
  }

  list_sort.clear();

  set<CNode2*, less_sort>::const_iterator it2 = set_sort.begin();

  for (; it2 != set_sort.end(); it2++) {
    CNode2 *n = *it2;
    list_sort.push_back(n);
  }

  {
    list<CNode2 *>::iterator it = list_sort.begin();

    int nnn = 0;

    for (; it != list_sort.end(); it++) {
      CNode2 *n1 = *it;

      if (inc(n1) == 1) {
        nnn ++;
      }
    }
  }
}

int mark(CNode2 *n, bool zakr);

void copyVydGraph_new1(CGraph2 *graph1, CGraph2 *graph2, bool is_po) 
{
  auto p = graph1->map_node.begin();
  while (p != graph1->map_node.end()) {
    CNode2 *n = p->second;
    if (n->node.isPjezo) {
      CNode2 *n2 = graph2->find_ins(n->id);
      if (n2) {
        n2->node = n->node;
      }
    }
    p++;
  }

  p = graph1->map_node.begin();
  
  while (p != graph1->map_node.end()) {
    CNode2 *n = p->second;

    if (n->node.isPjezo) {
      CNode2 *n2 = graph2->find(n->id);

      if (n2) {
        for (CLINE2 *l = n->lines; l; l = nextl(l)) {
          if (
                (bline(l)->line.isPjezo || bline(l)->line.isPjezoP || bline(l)->line.isPjezoO)
                && IsBegin(l) && (!is_po ||

                                  (
                                      (bline(l)->line.nomP != -1 && (bline(l)->line.isPjezo || bline(l)->line.isPjezoP))
                                   ||
                                   (bline(l)->line.nomO != -1 && (bline(l)->line.isPjezo || bline(l)->line.isPjezoO))
                                   )

                                  )  ) {
            CNode2 *o = other(l);
            CNode2 *o2 = graph2->find(o->id);
            if (o2) {
                CCoordList cl;

              CLINE2 *ll = graph2->insert_line(n2, o2, cl, true);
              if (ll) {
                bline(ll)->line = bline(l)->line;
                bline(ll)->line.isPjezo = false;
              }
            }
          }
        }
      }
    }
    p++;
  }
}


void getKon(CGraph2 *graph, list<CNode2*> &list_k)
{

  auto p = graph->map_node.begin();
  while (p != graph->map_node.end()) {
    CNode2 *n = p->second;

    if (inc(n) == 1) {
      list_k.push_back(n);
    }
    p++;
  }
}

double dist(list<CNode2*> &list_k1, list<CNode2*> &list_k2, CNode2 **nn1, CNode2 **nn2)
{
  list<CNode2*>::const_iterator it1 = list_k1.begin();

  double min_d = -1;

  *nn1 = *nn2 = 0;


  for (; it1 != list_k1.end(); it1++) {
    CNode2 *n1 = *it1;

    list<CNode2*>::const_iterator it2 = list_k2.begin();
    
    for (; it2 != list_k2.end(); it2++) {
      CNode2 *n2 = *it2;

      double d = Length(n1->node.coord, n2->node.coord);

      if (d < min_d || min_d < 0) {
        *nn1 = n1;
        *nn2 = n2;
        min_d = d;
      }
    }
  }

  return min_d;
}



void copyVydGraph_new(CGraph2 *graph1, CGraph2 *graph2, bool is_po) 
{
  copyVydGraph_new1(graph1, graph2, is_po);

LOOP :
  {
    graph2->reset();

    list<CNode2*> list_n;

    auto p = graph2->map_node.begin();
    while (p != graph2->map_node.end()) {
      CNode2 *n = p->second;

      if (!n->node.isPjezo) {
        mark(n, false) ;
        list_n.push_back(n);
      }
      p++;
    }

    if (list_n.size() <= 1) return;

    auto it = list_n.begin();

    double min_d = -1;
    CNode2 *nnn1 = NULL, *nnn2 = NULL;

    for (; it != list_n.end(); it++) {
      CNode2 *nn = *it;
      if (nn) {

        graph2->reset();
        mark(nn, false);
    
        CGraph2 graph3(NULL);
        copyVydGraph_new1(graph2, &graph3, is_po);

        list<CNode2*> list_k1;
        getKon(&graph3, list_k1);
        
        list<CNode2*>::const_iterator it2 = it; it2++;

        for (; it2 != list_n.end(); it2++) {
            CNode2* nn2 = *it2;
            if (nn2) {
                graph2->reset();
                mark(nn2, false);

                CGraph2 graph4(NULL);
                copyVydGraph_new1(graph2, &graph4, is_po);

                list<CNode2*> list_k2;
                getKon(&graph4, list_k2);


                CNode2* n1, * n2;

                double d = dist(list_k1, list_k2, &n1, &n2);
                if (n1 && n2 && (d < min_d || min_d < 0)) {
                    nnn1 = graph2->find(n1->id);
                    nnn2 = graph2->find(n2->id);
                    min_d = d;
                }
            }
        }
      }
    }

    if (nnn1 != NULL && nnn2 != 0) {
      CNode2 *n1 = graph2->find(nnn1->id);
      CNode2 *n2 = graph2->find(nnn2->id);

      if (n1 && n2 && n1 != n2) {
        CLINE2 *ll = graph2->insert_line(n1, n2);
        if (ll) {
          goto LOOP;
        }
      }
    }
  }

  {
    list<CNode2*> list_n;

    auto p = graph2->map_node.begin();
    while (p != graph2->map_node.end()) {
      CNode2 *n = p->second;
      if (inc(n) == 1) {
        list_n.push_back(n);
      }
      p++;
    }
  }
}
