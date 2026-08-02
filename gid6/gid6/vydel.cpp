#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrView.h"

// Выделение фрагмента


int vydel(CGidFile* file, CGraph2* graph, const CNode* node, const CLINE2* line);

// Количество выделенных участков рядом с узлом

int nVydL(class CNode2 *n)
{
    int nn = 0;

    for (CLINE2 *l = n->lines; l; l = next(l)) {
        if (bline(l)->line.isPjezo) {
            nn++;
        }
    }
    return nn;
}

bool CGidrView::vydel(CNode2* node)
{
    vydel4(node, NULL);

    set<CLINE2*> set_p;

    {
        auto p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* pp = p->second;

            if (!pp->node.internalNodeID) {
                CLINE2* l;
                for (l = pp->lines; l; l = next(l)) {
                    if (IsBegin(l)) {
                        if (bline(l)->line.isLineOtkl() && !bline(l)->line.isPjezo) {
                            if (
                                nVydL(where(l)) == 1 && inc(where(l)) ||
                                nVydL(other(l)) == 1 && inc(other(l))
                                ) {
                                set_p.insert(l);
//                                bline(l)->line.isPjezo = true;
                            }
                        }
                    }
                }
            }
            p++;
        }
    }

    {
        auto p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* pp = p->second;

            if (!pp->node.internalNodeID) {
                CLINE2* l;
                for (l = pp->lines; l; l = next(l)) {
                    if (IsBegin(l)) {
                        if (bline(l)->line.isLineOtkl() && !bline(l)->line.isPjezo) {
                            auto it2 = set_p.find(l);
                            if (it2 != set_p.end()) {
                                bline(l)->line.isPjezo = true;
                            }
                        }
                    }
                }
            }
            p++;
        }
    }


    return true;
}

bool CGidrView::vydel(const CFPoint& point)
{
    if (isPot()) return false;

    CNode2* node = NULL;
    CLINE2* line = NULL;
    node = m_cxema->m_graph->find(m_parent_id, point, geom.masx * D5 / mas_otn);

    if (!node) {
        line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx * D5 / mas_otn);
        if (line) {
            node = where(line);
        }
    }

    if (node) {
        m_cxema->m_graph->reset();

        vydel(node);
        InvalidateG();
    }
    return true;
}

bool CGidrView::vydel4(CNode2* node, CLINE2* line)
{
    if (!node && !line) return false;

    m_cxema->m_graph->reset();

    if (!node) node = where(line);

    if (node) {
        node->node.m_graph->link2(node, NULL, true);
        InvalidateG();
    }
    return true;
}


// Закрывает участки на которых секционируюая задцижка

void CGidrView::OnCloseOpresZd()
{
    CString q;

    q =
        "SELECT \n"
        "objID, lineID, len\n"
        "FROM (\n"
        "SELECT\n"
        "obj.id AS objID, l.id AS lineID,\n"
        "l.shape.STDistance(obj.shape) AS len,\n"
        "MIN(l.shape.STDistance(obj.shape)) OVER(PARTITION BY l.id ) AS min_len\n"

        "FROM opressovka_sektsioniruyuschaya_zadvizhka obj\n"
        "JOIN linesobj l ON l.removed=0 AND l.shape.STDistance(obj.shape) < 0.1 \n"
        ") _Q\n"
        "WHERE len = min_len\n";

    m_cxema->m_graph->init_find_line_nom();

    CAdoFile* ado = m_cxema->m_ado;
    m_cxema->m_graph->save_otkl();

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int objID = ado->read_long(0);
            int lineID = ado->read_long(1);

            CLINE2 *l = m_cxema->m_graph->find_line_nom(lineID);

            if (l) {
                bline(l)->line.pod.isOtklZD = true;
                bline(l)->line.obr.isOtklZD = true;
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }
    Invalidate();

    m_zadv_yes = true;
}
