#include "stdafx.h"
#include "gid6.h"

#include "ado.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrview.h"

int getNumb(const char *txt);
void insertSortLinesToDb2(CAdoFile* ado, CGraph2* graph, CString tn, bool ms, map<int, int>& map_big, int id);
void set_map_big(CAdoFile* ado, CGraph2* graph, map<int, int>& map_big);
void sortNodeDb(FILE* f, CAdoFile* ado, CGraph2* graph, bool ms, int id);

void copyPTS(CAdoFile *ado);


void reset_ms_rs(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs)
{
    graph->save_pjezo();

    sortNodeDb(nullptr, ado, graph, ms, ms_rs);

    map<int, int> map_big;
    insertSortLinesToDb2(ado, graph, "sortLinesForUchastok", ms, map_big, ms_rs);

    set_map_big(ado, graph, map_big);

    copyPTS(ado);

    graph->init_find_line_nom_hps();

    for (auto p : map_big) {
        CLINE2* l = graph->find_line_nom_hps(p.first);
        if (l) {
            bline(l)->line.pipeSectionID = p.second;
        }
    }

    graph->restore_pjezo();
}

void set_node_PTS(CAdoFile* ado, CNode2* n)
{
    CString q;
    CString name;

    if (n->node.internalNodeID != 0) return;

    int numb = getNumb(n->getKod());

    name.Format("%s(%d)", n->node.name, numb);

    int typPS = PS_NEUST;

    if (n->typPS() == 0 || n->node.namePS == "") {
        if (n->typPS() != 0) {
            typPS = PS_NEUST;
        }
        if (n->node.namePS != "") {
            name = n->node.namePS;
        }

        q.Format("UPDATE nodes SET nodeName='%s', nodeTypeID=%d WHERE id=%d", name, typPS, n->id);

        if (ado->Execute(q)) {
            n->node._typPS = PS_NEUST;
            n->node.namePS = name;
        }
    }
}

bool isMS(CNode2* n, int ms)
{
    for (CLINE2* l = n->lines; l; l = next(l)) {
        if (bline(l)->line.ms == ms) return true;
    }
    return false;
}

bool isRS(CNode2* n, int rs)
{
//    if (isMS(n)) return false;

    for (CLINE2* l = n->lines; l; l = next(l)) {
        if (bline(l)->line.rs == rs) return true;
    }
    return false;
}

void set_nodes_PTS(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs)
{
    CMapGraph::iterator p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        if (ms && isMS(p->second, ms_rs)) {
            set_node_PTS(ado, p->second);
        }
        else if (!ms && isRS(p->second, ms_rs)) {
            set_node_PTS(ado, p->second);
        }
        p++;
    }

    reset_ms_rs(ado, graph, ms, ms_rs);
}

void reset_ms_rs_line(CAdoFile* ado, CGraph2* graph, CLINE2* line)
{
    CLine2* l = bline(line);

    if (l->line.ms > 0) {
        reset_ms_rs(ado, graph, true, l->line.ms);
    }
    else if (l->line.rs > 0) {
        reset_ms_rs(ado, graph, false, l->line.rs);
    }
}

void reset_ms_rs_node(CAdoFile* ado, CGraph2* graph, CNode2* n)
{
    set<int> set_ms;
    set<int> set_rs;

    for (CLINE2* l = n->lines; l; l = next(l)) {
        if (bline(l)->line.ms > 0) {
            set_ms.insert(bline(l)->line.ms);
        }
        else if (bline(l)->line.rs > 0) {
            set_rs.insert(bline(l)->line.rs);
        }
    }

    for (auto p : set_ms) {
        reset_ms_rs(ado, graph, true, p);
    }
    for (auto p : set_rs) {
        reset_ms_rs(ado, graph, false, p);
    }
}

void CGidrView::OnVydMs2()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    if (AfxMessageBox("Обработать узлы участка МС", MB_YESNO) != IDYES) return;

    CAdoFile* ado = getAdo(getGidAdoName());
    CGraph2* graph = where(line)->node.m_graph;

    reset_ms_rs(ado, graph, true, bline(line)->line.ms);
}

void CGidrView::OnVydRs2()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    if (AfxMessageBox("Обработать узлы участка РС", MB_YESNO) != IDYES) return;

    CAdoFile* ado = getAdo(getGidAdoName());
    CGraph2* graph = where(line)->node.m_graph;

    reset_ms_rs(ado, graph, false, bline(line)->line.rs);
}
