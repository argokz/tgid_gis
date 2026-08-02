#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>


// Количество выделенных участков рядом с узлом

int nVydL(class CNode2 *n)
{
    int nn = 0;

    for (CLINE2 *l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.isPjezo) {
            nn++;
        }
    }
    return nn;
}


bool vydel4(CGraph2 *graph, CNode2* node, CLINE2* line)
{
    if (!node && !line) return false;

    graph->reset();

    if (!node) node = where(line);

    if (node) {
        node->node.m_graph->link2(node, NULL, true);
    }
    return true;
}


bool vydel(CNode2* node)
{
    CGraph2 *graph = node->node.m_graph;

    vydel4(graph, node, NULL);

    set<CLINE2*> set_p;

    {
        auto p = graph->map_node.begin();

        while (p != graph->map_node.end()) {
            CNode2* pp = p->second;

            if (!pp->node.internalNodeID) {
                CLINE2* l;
                for (l = pp->lines; l; l = nextl(l)) {
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
        auto p = graph->map_node.begin();

        while (p != graph->map_node.end()) {
            CNode2* pp = p->second;

            if (!pp->node.internalNodeID) {
                CLINE2* l;
                for (l = pp->lines; l; l = nextl(l)) {
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
