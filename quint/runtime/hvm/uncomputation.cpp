//
// Created by BY210033 on 2023/2/7.
//
#include "quint/runtime/hvm/uncomputation.h"

#include <iostream>

using namespace std;

namespace quint {

    bool UnComputation::unComputeListNode(DAG &dag, std::vector<std::shared_ptr<Node>> nodes) {
        while (nodes.size() > 0) {
            auto cur_node = nodes[nodes.size()-1];
            nodes.pop_back();
            if (!unComputeNode(dag, cur_node))
                return false;
        }
        return true;
    }

    bool UnComputation::unComputeNode(DAG &dag, shared_ptr<Node> node) {
        if (node->value_id <= 0) {
            cerr << "node.val id <= 0" << endl;
            return false;
        }
        if (!dag.nodes.count(node->variable_name))
            return false;

        if (dag.nodes.at(node->variable_name).size() <= node->value_id)
            return false;

        auto latest_node = dag.nodes[node->variable_name][node->value_id].back();
        if (latest_node->consume_edge_out)
            // The register of the node change later
            return false;

        int copy_id = 1;
        string operation;
        if (node->op == "Push")
            operation = "Pop";
        else
            operation = node->op;
        auto uncomputation_node = make_shared<Node>(node->reg, node->index, operation, node->value_id - 1, copy_id);
        dag.addNode(uncomputation_node);
        auto new_edges = dag.connectConsumedNodes(latest_node, uncomputation_node);

        for (auto ctrl_edge : node->ctrl_edges_in) {
            auto ctrl_latest_node = dag.latestCopy(ctrl_edge->node_from);
            dag.connectDependencyNodes(ctrl_latest_node, uncomputation_node);
        }
        return true;
    }

}