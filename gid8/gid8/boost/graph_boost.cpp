#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <iostream>
#include <iomanip>

#include <boost/format.hpp>
#include "graph_boost.h"

bool convert(double x, double y, double &xx, double &yy);

// Вывод информации о графе
void GraphB::printNodes() 
{
//    std::cout << "Vertices: " << num_vertices(*this) << std::endl;
//    std::cout << "Edges: " << num_edges(*this) << std::endl;

/*
    dfs_visitor vis;

    std::vector<default_color_type> color_map(boost::num_vertices(g), boost::white_color);

//    boost::depth_first_search(g, boost::visitor(vis));

    // Обход всех узлов
    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
        if (color_map[v] == white_color) {
            depth_first_visit(g, v, vis, boost::make_iterator_property_map(color_map.begin(), boost::get(boost::vertex_index, g)));
        }
    }
*/


//    init_convert();

    for (auto & [id, n] : node_map) {
        VertexProperty vp = (*this)[n];
        if (vp.internalNodeID == 0) {
            double x0 = vp.x/100., y0 = -vp.y/100.;
            double x2, y2;
//            convert(x0, y0, x2, y2);

//            std::cout << std::setprecision (15) << vp.name << " " << vp.internalNodeID << " " << vp.x << " " << vp.y << " -> " << yy << " " << xx << std::endl;
            std::cout << std::setprecision (15) << vp.name << " " << vp.internalNodeID << " " << y0 << " " << x0 << " -> " << y2 << " " << x2 << std::endl << std::flush;
        }
    }
}


void GraphB::printLines() 
{

    for (auto & [id, l] : line_map) {
        EdgeProperty ep = (*this)[l];

        Vertex v1 = source(l, *this);
        Vertex v2 = target(l, *this);

        VertexProperty vp1 = (*this)[v1];
        VertexProperty vp2 = (*this)[v2];

        std::cout << boost::format("%1% - %2%") % vp1.name % vp2.name << std::endl << std::flush;

//        std::cout << vp1.name << " " << vp2.name << std::endl << std::flush;
    }
}


#include <cxema/graph2.h>


GraphBPtr makeGraphB(const CGraph2 *graph)
{
    GraphBPtr g = std::make_shared<GraphB>();

    for (auto& p : graph->map_node) {
        CNode2* pp = p.second;
        VertexProperty vp;
        vp.id = pp->id;

        Vertex v = g->addNode(vp.id, vp);
    }


    for (auto& p : graph->map_node) {
        CNode2* pp = p.second;

        for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l)) {
                CNode2 *n1 = where(l);
                CNode2 *n2 = other(l);
//                if (n1 && n2 && n1->node.coord.x != 0 && n2->node.coord.x != 0 && n1->node.internalNodeID == m_parent_id) {
//                }

                if (n1 && n2) {
                    EdgeProperty ep;
                    Edge e = g->addLine(bline(l)->line.nomP,  n1->id, n2->id, ep);
                }
            
            }
        }
    }



    return g;
}
