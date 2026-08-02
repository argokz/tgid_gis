#include "stdafx.h"
#include "gid6.h"

#include "graph2.h"
#include "cxema.h"


//#include "virtlistDoc.h"
#include "virtlistView.h"

#include "virtualnodelist.h"


void sort_graph(CGraph2* graph);

bool is_in_list(const CNode2 *n, bool m_otris, bool m_vyd_potr)
{
    CNode2* nn = n->node.m_graph->m_cxema->m_graph->find(n->node.internalNodeID);

    if (m_vyd_potr) {
        if (n->node.isPjezo && (n->node.typ == TIP_PR || n->node.typ == TIP_PO)) return true;
        return false;
    }

    if (!n->node.isHide && (n->node.internalNodeID == 0 || (nn && nn->node.typ != TIP_PR && nn->node.typ != TIP_PO))) {
        if ((m_otris && (n->node.coord.x != 0 || n->node.coord.y != 0)) || (!m_otris && n->node.coord.x == 0 && n->node.coord.y == 0)) {
            return true;
        }
    }
    return false;
}


void CNodeListData::init(const char* text)
{
    sort_graph(m_graph);

    map<long, map<long, map<long, CNode2*> > > map_n;

    int cnt = 0;
    CMapGraph::const_iterator p;
    p = m_graph->map_node.begin();
    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        //    if (n->node.internalNodeID) {
        //    }
        CNode2* nn = n->node.m_graph->m_cxema->m_graph->find(n->node.internalNodeID);
        if (is_in_list(n, m_otris, m_vyd_potr)) {
//        if (!n->node.isHide && (n->node.internalNodeID == 0 || (nn && nn->node.typ != TIP_PR && nn->node.typ != TIP_PO))) {
//            if ((m_otris && (n->node.coord.x != 0 || n->node.coord.y != 0)) || (!m_otris && n->node.coord.x == 0 && n->node.coord.y == 0)) {
                CString txt1 = n->getName();
                CString txtPTS = n->node.namePS;
                CString txt2 = text;
                CString txt3 = "";


                if (!nn || nn->node.typ != TIP_PR) {
                    if (nn) {
                        txt3 = nn->getName();
                        txt3.MakeLower();
                    }

                    txt1.MakeLower();
                    txt2.MakeLower();
                    txtPTS.MakeLower();

                    if (txt1.Find(txt2) != -1 || txtPTS.Find(txt2) != -1 || txt3.Find(txt2) != -1) {
                        map_n[n->node.fileID][n->node.n_sort][cnt] = n;
                        cnt++;
                    }
                }
//            }
        }
        p++;
    }

    m_v.resize(cnt);

    cnt = 0;

    map<long, map<long, map<long, CNode2*> > >::iterator it1 = map_n.begin();

    for (; it1 != map_n.end(); it1++) {
        map<long, map<long, CNode2*> >::iterator it2 = it1->second.begin();
        for (; it2 != it1->second.end(); it2++) {
            map<long, CNode2*>::iterator it3 = it2->second.begin();
            for (; it3 != it2->second.end(); it3++) {
                m_v[cnt] = it3->second;
                cnt++;
            }
        }
    }
}


CNodeListData::CNodeListData(CGraph2* graph, bool otris, bool vyd_potr)
{
    m_graph = graph;
    m_otris = otris;
    m_vyd_potr = vyd_potr;

    init("");
}

CNodeListData::~CNodeListData()
{
}

CString CNodeListData::GetItemText(int nSubItem, int nIndex, int first, int last)
{
    CString s;

    if (nIndex >= GetCount() || nIndex < 0) {
        return "";
    }

    CNode2* n = m_v[nIndex];

    if (nSubItem == 0)  return n->getKod();
    if (nSubItem == 1)  return n->node.name;

    if (nSubItem == 2)  return n->node.externalSignID == 2 ? POD : n->node.externalSignID == 3 ? OBR : "";

    if (nSubItem == 3)  return n->node.namePS;


    if (n->node.internalNodeID && (nSubItem == 4 || nSubItem == 5)) {
        CNode2* nn = n->node.m_graph->m_cxema->m_graph->find(n->node.internalNodeID);

        if (nSubItem == 4) {
            return nn->getKod();
        }
        if (nSubItem == 5) {
            return nn->node.name;
        }
    }

    if (nSubItem == 6)  return n->getTable();

    if (nSubItem == 7) {
        //    s.Format("%d",n->file);
        CGidFile* gf = n->node.m_graph->m_cxema->getGidFile(n->node.fileID);
        if (gf)
            return gf->name;
    }
    //  if (nSubItem == 3)  {
    //    s.Format("%d",n->node.n_sort);
    //    return s;
    //  }
    return  "";
}



int CNodeListData::GetImage(int nIndex)
{
    return -1;
}

int CNodeListData::GetStateImage(int nIndex)
{
    // Нумерация иконок состояния начинается с единицы
    return 0;
}

int CNodeListData::GetCount()
{
    return m_v.size();
}


int CNodeListData::GetNFlds()
{
    return 4 + 2 + 1 + 1;
}


CString CNodeListData::GetFieldName(int nSubItem)
{
    if (nSubItem == 0)  return "Код";
    if (nSubItem == 1)  return "Наименование";
    if (nSubItem == 2)  return "";
    if (nSubItem == 3)  return "Наименование ПТС";
    if (nSubItem == 4)  return "Код";
    if (nSubItem == 5)  return "Наименование";
    if (nSubItem == 6)  return "Тип";
    if (nSubItem == 7)  return "Фрагмент";

    return "";
}


int CNodeListData::GetFieldWidth(int nSubItem)
{
    if (nSubItem == 0)  return 10;
    if (nSubItem == 1)  return 20;
    if (nSubItem == 2)  return 2;
    if (nSubItem == 3)  return 40;
    if (nSubItem == 4)  return 10;
    if (nSubItem == 5)  return 20;

    return 20;
}


void* CNodeListData::getData(int nIndex)
{
    if (nIndex >= GetCount() || nIndex < 0) {
        return NULL;
    }

    CNode2* n = m_v[nIndex];

    return (void*)n->id;
}

void CNodeListData::setFindText(const char* text)
{
    init(text);
}
