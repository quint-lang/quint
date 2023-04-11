//
// Created by BY210033 on 2023/2/7.
//
#include "quint/runtime/hvm/dag_graph.h"

#include <cassert>

using namespace std;

namespace quint {
    Node::Node(shared_ptr<Register> reg, int index, const string &op, int value_id, int copy_id)
            : reg(std::move(reg)), index(index), op(op), value_id(value_id), copy_id(copy_id) {
        variable_name = "Reg" + std::to_string(this->reg->addr);
    }

    Edge::Edge(std::shared_ptr<Node> node_from, std::shared_ptr<Node> node_to, EdgeType type)
            : node_from(std::move(node_from)), node_to(std::move(node_to)), type(type) {}

    void DAG::addNode(std::shared_ptr<Node> node) {
        if (nodes.count(node->variable_name)) {
            auto vector_nodes = nodes.at(node->variable_name);
            if (vector_nodes.size() > node->value_id) {
                assert(vector_nodes[node->value_id].size() == node->copy_id);
                vector_nodes[node->value_id].push_back(node);
            } else {
                assert(vector_nodes.size() == node->value_id && node->copy_id == 0);
                vector_nodes.push_back({node});
            }
            nodes[node->variable_name] = vector_nodes;
        } else {
            vector<vector<shared_ptr<Node>>> item;
            item.push_back({node});
            nodes[node->variable_name] = item;
        }
    }

    void DAG::auxTopologicalSort(vector<shared_ptr<Node>> &sorted_nodes,
                                 map<shared_ptr<Node>, int> &seen_nodes, shared_ptr<Node> node) {
        if (seen_nodes.count(node)) {
            auto seen = seen_nodes.at(node);
            if (seen == 1) {
                assert(false);
            } else {
                return;
            }
        }

        seen_nodes[node] = 1;
        for (auto e : node->edges_out) {
            auxTopologicalSort(sorted_nodes, seen_nodes, e->node_to);
        }
        if (node->consume_edge_out)
            auxTopologicalSort(sorted_nodes, seen_nodes, node->consume_edge_out->node_to);
        seen_nodes[node] = 2;
        sorted_nodes.push_back(node);
    }

    std::shared_ptr<Node> DAG::latestCopy(std::shared_ptr<Node> node) {
        assert(nodes.count(node->variable_name));
        assert(nodes.at(node->variable_name).size() > node->value_id);
        return nodes.at(node->variable_name)[node->value_id].back();
    }

    bool DAG::hasCycleWith(std::shared_ptr<Node> origin_node) {
        return false;
    }

    vector<shared_ptr<Node>> DAG::nodesInTopologicalOrder() {
        vector<shared_ptr<Node>> sorted_nodes;
        map<shared_ptr<Node>, int> seen_nodes;
        for (auto iter = nodes.begin(); iter != nodes.end(); iter++) {
            for (auto list_copies : iter->second) {
                for (auto node : list_copies) {
                    auxTopologicalSort(sorted_nodes, seen_nodes, node);
                }
            }
        }

        return sorted_nodes;
    }

    shared_ptr<Edge> DAG::connectAvailabilityNodes(shared_ptr<Node> from, shared_ptr<Node> to) {
        assert(from->variable_name != to->variable_name);

        auto e = make_shared<Edge>(from, to, Edge::EdgeType::A);
        from->edges_out.push_back(e);
        to->non_ctrl_edges_in.push_back(e);
        return e;
    }

    vector<shared_ptr<Edge>>
    DAG::connectConsumedNodes(shared_ptr<Node> from, shared_ptr<Node> to) {
        assert(from->variable_name == to->variable_name);
        assert(from->value_id == to->value_id + 1 || from->value_id == to->value_id - 1);

        auto e = make_shared<Edge>(from, to, Edge::EdgeType::C);
        from->consume_edge_out = e;
        to->consume_edge_in = e;
        auto avail_edges = updateAvailabilityEdges(e);
        avail_edges.push_back(e);
        return avail_edges;
    }

    vector<shared_ptr<Edge>>
    DAG::connectDependencyNodes(shared_ptr<Node> from, shared_ptr<Node> to) {

//        assert(from->variable_name != to->variable_name);
        if (from->variable_name == to->variable_name) {
            cerr << "Operation " << to->op << " is match from: " << from->variable_name << " to: " << to->variable_name << endl;
            assert(from->variable_name != to->variable_name);
        }
        assert(nodes[from->variable_name][from->value_id][from->copy_id] = from);
        assert(nodes[to->variable_name][to->value_id][to->copy_id] = to);

        auto e = make_shared<Edge>(from, to, Edge::EdgeType::D);
        from->edges_out.push_back(e);
        to->ctrl_edges_in.push_back(e);
        auto avail_edges = updateAvailabilityEdges(e);
        avail_edges.push_back(e);
        return avail_edges;
    }

    vector<shared_ptr<Edge>> DAG::updateAvailabilityEdges(shared_ptr<Edge> edge) {
        vector<shared_ptr<Edge>> new_edges;
        if (edge->type == Edge::EdgeType::C) {
            for (auto edge_out : edge->node_from->edges_out) {
                if (edge_out->type == Edge::EdgeType::D) {
                    auto e = connectAvailabilityNodes(edge_out->node_to, edge->node_to);
                    new_edges.push_back(e);
                }
            }
        } else if (edge->type == Edge::EdgeType::D) {
            if (edge->node_from->consume_edge_out) {
                auto e = connectAvailabilityNodes(edge->node_to, edge->node_from->consume_edge_out->node_to);
                new_edges.push_back(e);
            }
        }
        return new_edges;
    }

}