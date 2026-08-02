#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <unordered_map>
#include <iostream>


struct VertexProperty {
    int id;
    int internalNodeID;
    int externalCodeID;
    std::string name;
    double x;
    double y;
};

struct EdgeProperty {
    int id;
    int weight;
};

// Объявляем класс GraphB
class GraphB;

// Определяем псевдонимы типов для графа
using GraphBase = boost::adjacency_list<boost::setS, boost::setS, boost::undirectedS, VertexProperty, EdgeProperty>;
using Vertex = boost::graph_traits<GraphBase>::vertex_descriptor;
using Edge = boost::graph_traits<GraphBase>::edge_descriptor;



// Определяем класс GraphB, наследующийся от GraphBase
class GraphB : public GraphBase {
public:
    // Конструктор по умолчанию
    GraphB() = default;

    // Добавление вершины в граф и сопоставление с идентификатором
    Vertex addNode(int id, VertexProperty & vp) {
        Vertex v = add_vertex(vp, *this);
        node_map[id] = v;
        return v;
    }

    // Добавление ребра в граф и сопоставление с идентификатором
    Edge addLine(int id, int nodeID1, int nodeID2, EdgeProperty & ep) {
        Edge e; 
        bool inserted;

        try {
            Vertex n1 = this->node_map.at(nodeID1);
            Vertex n2 = this->node_map.at(nodeID2);
            
            boost::tie(e, inserted) = add_edge(n1, n2, ep, *this);
            if (inserted) {
                line_map[id] = e;
            }
        }
        catch(std::out_of_range) {
            std::cerr << "Error " << nodeID1 << " " << nodeID2 << std::endl;
        }
        return e;
    }

    // Вывод информации о графе
    void printNodes();
    void printLines();

private:
    std::unordered_map<int, Vertex> node_map;
    std::unordered_map<int, Edge> line_map;
};


class dfs_visitor : public boost::default_dfs_visitor {
public:
    void discover_vertex(Vertex u, const GraphB& g) const {
        std::cout << "Discovering vertex " << g[u].id << std::endl;
    }
};


typedef std::shared_ptr<GraphB> GraphBPtr;

