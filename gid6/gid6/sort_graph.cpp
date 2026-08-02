#include "stdafx.h"
#include "Graph.h"
#include "Graph2.h"

#include "sort_graph.h"

static int ii2 = 1;
static int iii = 1;

static deque<CNode2*> st2;

static map<int, CNode2 *> map_pot;
static map<int, CNode2 *> map_zn;

static bool dfs3(CNode2 *n)
{
  if (n->node.Color == 1) return true;
  if (n->node.Color == 2) return false;

  n->node.Color = 1;

  for (CLINE2 *ll = n->lines; ll; ll = next(ll)) {
    CLine2 *l = bline(ll);
    CNode2 *o = other(ll);

    bool is_next = false;
    int napravl = napr(ll);

    if (l->line.pod.q != 0) {
      is_next = (napravl*l->line.pod.q) < 0;
    }
    else if (l->line.obr.q != 0) {
      is_next = (napravl*l->line.obr.q) < 0;
    }
      
    if (is_next) {
      if (dfs3(o)) return true;
    }
  }

  n->node.n_sort = ii2++;

  st2.push_back(n);

  n->node.Color = 2;

  return false;
}

static void sort_line2(CGraph2 *m_graph, bool is_mark)
{
  CNode2 *nZN = NULL;

//  st2.clear();


  CMapGraph::iterator pp = m_graph->map_node.begin();

  while (pp != m_graph->map_node.end()) {
      CNode2 *p = pp->second;
    p->node.Color = 0;

    for (CLINE2 *l = p->lines; l; l = next(l)) {
      bline(l)->line.n_sort = 0;
    }

    pp++;
  }


  pp = m_graph->map_node.begin();

  bool Cycle;

  while (pp != m_graph->map_node.end()) {
    CNode2 *n = pp->second; 
    Cycle = dfs3(n);

    if (n->node.len != -1 && n->node.nZN != -1) {
      nZN = n;
    }
    pp++;
  }

  pp = m_graph->map_node.begin();

  while (pp != m_graph->map_node.end()) {
    CNode2 *n = pp->second;
    if (n->node.n_sort == 0) {
      n->node.n_sort = ii2++;
    }

    pp++;
  }

  while ( !st2.empty() ) {
    CNode2 *v = st2.front();

    if (v) {
      CLINE2 *max_ll = NULL;
      double max_diam = -1;
      int n_l = 0;

      bool not_first = false;

      for (CLINE2 *ll = v->lines; ll; ll = next(ll)) {
        CLine2 *l = bline(ll);
        
        if (other(ll)->node.n_sort > v->node.n_sort) {
          
          l->line.nZN = nZN;
          
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

      if (not_first) {
        iii++;
        nn = 1;
      }

      if (max_ll) {
        st2.push_front(other(max_ll));

        bline(max_ll)->line.n_sort = iii++;
        bline(max_ll)->line.n_sort2 = nn++;
      }
      else {
        st2.pop_front();
      }
    }
  }

  pp = m_graph->map_node.begin();

  while (pp != m_graph->map_node.end()) {
      CNode2 *p = pp->second;
          for (CLINE2 *l = p->lines; l; l = next(l)) {

      if (bline(l)->line.n_sort == 0) {
        bline(l)->line.n_sort = iii++;
      }
    }

   pp++;
  }


//  map_zn.clear();
//  map_pot.clear();

    pp = m_graph->map_node.begin();

  while (pp != m_graph->map_node.end()) {
    CNode2 *n = pp->second; 

    if (/*n->po() == CPOD && */ n->node.len != -1) {
      
      if (n->node.typ == TIP_PR || n->node.typ == TIP_PO) {

        for (CLINE2 *ll = n->lines; ll; ll = next(ll)) {
          CLine2 *l = bline(ll);
          if ( l->line.n_sort != 0 ) {
            map_pot[l->line.n_sort2] = n;
          }
        }
      }
    }
    pp++;
  }
}

void init_sort()
{
  ii2 = 1;
  iii = 1;
  
  st2.clear();
  map_zn.clear();
  map_pot.clear();
}


void sort_line(CGraph2 *m_graph, bool is_mark)
{
  init_sort();
  sort_line2(m_graph, is_mark);
}

#if 0
bool getMainPot(CGraph2 *m_graph, const CLINE2 *ll, CNodeName &name1, CNodeName &name2, int &nn)
{
  nn = 0;
  string n2;

  const CLine2 *l = bline(ll);

  if (l) {
    if (!l->line.n_sort) {
      CNode2 *n1 = m_graph->findPO(*where(ll));
      CNode2 *n2 = m_graph->findPO(*other(ll));

      if (n1 && n2) {
        l = bline(m_graph->Link(n1, n2));
      }
    }
    if (l && l->line.n_sort) {
      map<int, CNode2 *>::const_iterator it = map_pot.find(l->line.n_sort);

      if (it != map_pot.end()) {

        name1 = it->second->node_name;
        if (l->line.nZN) name2 = l->line.nZN->node_name;
        nn = l->line.n_sort2;
        return true;
      }
    }
  }
  return false;
}
#endif



void VYD_LINES::init(CGraph2 *graph, bool all, bool only_podacha)
{
//  CNode *node = n0;
//  CGraph2 *m_graph = n0->m_graph;

//  sort_line(graph, true);

#if 0
    void sort_line_rs_new(CGraph2 * graph, list<CNode2*> & list_sort, list<CLINE2*> & st_l);

    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    sort_line_rs_new(graph, list_sort, st_l);

    list<CLINE2*>::const_iterator it =  st_l.begin();

    int ii = 0;

    for (; it != st_l.end(); it++) {
        map_line[ii++] = *it;
    }

#else

  list<CNode2*> list_sort;
  list<CLINE2 *> st_l;

//  sort_line_rs_new(graph, list_sort, st_l);
  sort_line(graph, true);

  {
    CMapGraph::iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
      CNode2 *n = p->second;
      if (n->node.isPjezo || all) {
        map_node[n->node.n_sort] = n;

        for (CLINE2 *l = n->lines; l; l = next(l)) {
          CLine2 *ll = bline(l);
          if (ll->line.n_sort && (ll->line.isPjezo || all) && (!only_podacha || ll->line.nomP > 0)) {
            map_line[ll->line.n_sort] = l;
          }
        }
      }
      p++;
    }
  }
#endif
}


void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);

void VYD_LINES::init2(CGraph2 *graph, bool all, bool only_podacha)
{
//  CNode *node = n0;
//  CGraph2 *m_graph = n0->m_graph;

//  sort_line(graph, true);

#if 1

    CGraph2 graph2(NULL);

    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    list<CLINE2*>::const_iterator it =  st_l.begin();

    int ii = 0;

    for (; it != st_l.end(); it++) {
        map_line[ii++] = *it;
    }

#else

  list<CNode2*> list_sort;
  list<CLINE2 *> st_l;

//  sort_line_rs_new(graph, list_sort, st_l);
  sort_line(graph, true);

  {
    CMapGraph::iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
      CNode2 *n = p->second;
      if (n->node.isPjezo || all) {
        map_node[n->node.n_sort] = n;

        for (CLINE2 *l = n->lines; l; l = next(l)) {
          CLine2 *ll = bline(l);
          if (ll->line.n_sort && (ll->line.isPjezo || all) && (!only_podacha || ll->line.nomP > 0)) {
            map_line[ll->line.n_sort] = l;
          }
        }
      }
      p++;
    }
  }
#endif
}
