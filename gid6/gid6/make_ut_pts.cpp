#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
//#include "InputDia.h"
//#include "Input2.h"
#include "gidrView.h"

CString getPSTable(int n);




// Объединяет трубы в Участок ПТС

void CGidrView::OnMakeUtPTS()
{
    int nn = 0;
    CLINE2* l1 = nullptr, * l2 = nullptr;

    if (!isEditGid()) return;

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();
    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            for (CLINE2* l = n->lines; l; l = next(l)) {
                if (l && IsBegin(l) && bline(l)->line.isPjezo) {
                    nn += 1;
                    if (!l1) l1 = l;
                    else if (!l2) l2 = l;
                }
            }
        }
        p++;
    }

    if (nn != 2) {
        AfxMessageBox(_TR("Объединить можно только два участка"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (where(l1)->node.fileID != where(l2)->node.fileID) {
        AfxMessageBox(_TR("Объединить можно только участки находящиеся на одном фрагменте"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CNode2* n0 = nullptr;

    if (where(l1) == where(l2)) n0 = where(l1);
    if (where(l1) == other(l2)) n0 = where(l1);
    if (other(l1) == other(l2)) n0 = other(l1);
    if (other(l1) == where(l2)) n0 = other(l1);

    if (!n0) {
        AfxMessageBox(_TR("Объединить можно только два смежных участка"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (n0->typPS() > 0 && n0->typPS() != PS_NEUST) {
        CString s;
        s.Format("%s это %s, объединение невозможно", n0->getName(), getPSTable(n0->typPS()));
        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (bline(l1)->line.pipeSectionID < 0) {
        CLINE2* l = l1; l1 = l2; l2 = l;
    }



    int lp1 = bline(l1)->line.pipeSectionID;
    int lp2 = bline(l2)->line.pipeSectionID;
    
    if (lp1 <= 0 || lp2 <= 0) {
        AfxMessageBox("Оба участка должны быть частьб участков ПТС");
        return;
    }


    {
        set<CNode2*> set_n;

        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();
        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                for (CLINE2* l = n->lines; l; l = next(l)) {
                    if (bline(l)->line.pipeSectionID == lp1) {
                        CNode2* n1 = where(l);
                        int nn = 0;
                        for (CLINE2* ll = n1->lines; ll; ll = next(ll)) {
                            if (bline(ll)->line.pipeSectionID == lp1) {
                                nn += 1;
                            }
                            if (nn == 1) {
                                set_n.insert(n1);
                            }
                        }

                        CNode2* n2 = other(l);
                        nn = 0;
                        for (CLINE2* ll = n1->lines; ll; ll = next(ll)) {
                            if (bline(ll)->line.pipeSectionID == lp1) {
                                nn += 1;
                            }
                            if (nn == 1) {
                                set_n.insert(n2);
                            }
                        }
                    }
                }
            }
            p++;
        }
    }

    CAdoFile* ado = m_cxema->m_ado;

    CString q;
    q.Format("SELECT nodeID1, nodeID2 FROM pipeSections pss WHERE id IN (%d, %d)", lp1, lp2);

    int pnode1, pnode2;

    int np1 = 0, np2 = 0;


    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            while (!ado->isEOF()) {
                int nodeID1 = ado->read_long("nodeID1");
                int nodeID2 = ado->read_long("nodeID2");

                if (np1 == 0) {
                   if (n0->id == nodeID1) {
                       np1 = nodeID2;
                   }
                   else {
                       np1 = nodeID1;
                   }
                }
                else {
                   if (n0->id == nodeID1) {
                       np2 = nodeID2;
                   }
                   else {
                       np2 = nodeID1;
                   }
                }

                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    CNode2* n1 = m_cxema->m_graph->find(np1);
    CNode2* n2 = m_cxema->m_graph->find(np2);

    if (!n1 || !n2) {
        AfxMessageBox("Ошибка");
        return;
    }

    CString ss;

    ss.Format("Создать участок ПТС %s - %s", n1->getName(), n2->getName());

    if (AfxMessageBox(ss, MB_YESNO) != IDYES) {
        return;
    }

    q.Format("UPDATE heatPipeSections SET pipeSectionID=%d WHERE pipeSectionID=%d", lp1, lp2);

    if (!ado->Execute(q)) {
        AfxMessageBox("Ошибка 1");
        return;
    }

    q.Format("UPDATE pipeSections SET nodeID1=%d, nodeID2=%d WHERE id=%d", n1->id, n2->id, lp1);
    if (!ado->Execute(q)) {
        AfxMessageBox("Ошибка 2");
        return;
    }
        
    m_linePTS_ID = lp1;

    {
        CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();
        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                for (CLINE2* l = n->lines; l; l = next(l)) {
                    if (bline(l)->line.pipeSectionID == lp2) {
                        bline(l)->line.pipeSectionID = lp1;
                    }
                }
            }
            p++;
        }
    }



    Invalidate();
}

