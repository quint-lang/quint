//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_DAG_GRAPH_H
#define QUINT_DAG_GRAPH_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "data_structure.h"


namespace quint {
    struct Edge;

    struct Node
    {
        std::shared_ptr<Register> reg;
        int index;
        std::string op;
        std::string variable_name;
        int value_id;
        int copy_id;
        std::vector<std::shared_ptr<Edge>> ctrl_edges_in;
        std::vector<std::shared_ptr<Edge>> non_ctrl_edges_in;
        std::vector<std::shared_ptr<Edge>> edges_out;
        std::shared_ptr<Edge> consume_edge_in;
        std::shared_ptr<Edge> consume_edge_out;

        Node(std::shared_ptr<Register> reg, int index, const std::string& op, int value_id = 0, int copy_id = 0);

    };

    struct Edge
    {
        enum EdgeType {
            A,
            C,
            D,
            U
        };
        std::shared_ptr<Node> node_from;
        std::shared_ptr<Node> node_to;
        EdgeType type;

        Edge(std::shared_ptr<Node> node_from, std::shared_ptr<Node> node_to, EdgeType type);
    };

    struct DAG
    {
        std::unordered_map<std::string, std::vector<std::vector<std::shared_ptr<Node>>>> nodes;

        DAG() {}

        void addNode(std::shared_ptr<Node> node);

        void auxTopologicalSort(std::vector<std::shared_ptr<Node>> &sorted_nodes,
                                std::map<std::shared_ptr<Node>, int> &seen_nodes, std::shared_ptr<Node> node);

        std::shared_ptr<Node> latestCopy(std::shared_ptr<Node> node);

        bool hasCycleWith(std::shared_ptr<Node> origin_node);

        std::vector<std::shared_ptr<Node>> nodesInTopologicalOrder();

        std::shared_ptr<Edge> connectAvailabilityNodes(std::shared_ptr<Node> from, std::shared_ptr<Node> to);

        std::vector<std::shared_ptr<Edge>> connectConsumedNodes(std::shared_ptr<Node> from, std::shared_ptr<Node> to);

        std::vector<std::shared_ptr<Edge>> connectDependencyNodes(std::shared_ptr<Node> from, std::shared_ptr<Node> to);

        std::vector<std::shared_ptr<Edge>> updateAvailabilityEdges(std::shared_ptr<Edge> edge);
    };
}


#endif //QUINT_DAG_GRAPH_H
