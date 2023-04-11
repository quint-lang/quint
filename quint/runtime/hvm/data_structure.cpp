//
// Created by BY210033 on 2023/2/7.
//
#include "quint/runtime/hvm/data_structure.h"
#include "quint/runtime/hvm/dag_graph.h"
#include "quint/runtime/hvm/uncomputation.h"

#include <utility>
#include <cassert>
#include <algorithm>

using namespace std;

namespace quint {

    string markedName(int addr)
    {
        return "Reg" + to_string(addr);
    }

    std::string System::to_string() const {
        std::string ret = fmt::format("{} ", amplitude);
        for (size_t i = 0; i < name_register_map.size(); ++i)
        {
            if (System::status_of(i))
            {
                auto& reg = registers[i];
                ret += reg.to_string(name_register_map[i]);
            }
        }
        return ret;
    }

    Operation::Operation(bool free, const std::string &name, std::shared_ptr<Register> target,
                         const std::vector<std::shared_ptr<Register>> &aux)
                         : name(name), target_bit(std::move(target)), aux_bit(aux), free(free) {}

    const std::string &Operation::getName() const {
        return name;
    }

    const std::shared_ptr<Register> &Operation::getTargetBit() const {
        return target_bit;
    }

    const std::vector<std::shared_ptr<Register>> &Operation::getAuxBit() const {
        return aux_bit;
    }

    const std::vector<std::shared_ptr<Register>> &Operation::getControlBit() const {
        return control_bit;
    }

    void Operation::add_controls(std::vector<std::shared_ptr<Register>> controls) {
        control_bit.insert(control_bit.end(), controls.begin(), controls.end());
    }

    Function::Function(const string &name, const std::vector<std::shared_ptr<Register>> &targets,
                       const std::vector<std::shared_ptr<Register>> &consts)
                       : targets(targets), consts(consts) {}

    const string &Function::getName() const {
        return name;
    }

    const vector <std::shared_ptr<Register>> &Function::getTargets() const {
        return targets;
    }

    const vector <std::shared_ptr<Register>> &Function::getConsts() const {
        return consts;
    }

    const vector <std::shared_ptr<Register>> &Function::getControls() const {
        return control_bit;
    }

    void Function::add_controls(std::vector<std::shared_ptr<Register>> controls) {
        control_bit.insert(control_bit.end(), controls.begin(), controls.end());
    }

    void Module::execute(std::vector<System> &state) {
        DAG dag = modConvertToDAG();
        auto sorted_nodes = dag.nodesInTopologicalOrder();
        vector<shared_ptr<Node>> needs_to_uncomp;
        for (auto iter = sorted_nodes.rbegin(); iter != sorted_nodes.rend(); iter++) {
            auto n = iter->get();
            if (n->reg->local && n->value_id > 0)
                needs_to_uncomp.push_back(make_shared<Node>(*n));
        }

        UnComputation uncomp;
        if (uncomp.unComputeListNode(dag, needs_to_uncomp)) {
            auto full_nodes = dag.nodesInTopologicalOrder();
            reverse(full_nodes.begin(), full_nodes.end());
            vector<shared_ptr<Node>> dealloc_nodes;
            for (auto node : full_nodes) {
                if (node->op == "init") {
                    if (node->reg->local)
                        dealloc_nodes.push_back(node);
                    cout << node->op << " target: " << node->variable_name;
                } else {
                    cout << node->op << " target: " << node->consume_edge_in->node_from->variable_name
                         << ", controlled_by: ";
                }
                for (auto ctrl_edge : node->ctrl_edges_in) {
                    cout << ctrl_edge->node_from->variable_name << ", ";
                }
                cout << "." << endl;
            }
            for (auto node : dealloc_nodes) {
                cout << "dealloc target: " << node->variable_name << endl;
            }
        }
    }

    void Module::execute_dump(std::vector<System> &state) {
        for (const auto& item : regs) {
            cout << (item->local ? "LocalRegister: " : "GlobalRegister: ");
            cout << item->addr << endl;
        }
        for (const auto& item : ops) {
            cout << item->getName() << ": Reg" << item->getTargetBit()->addr << ", ";
            vector<std::shared_ptr<Register>> auxs = item->getAuxBit();
            for (auto aux : auxs) {
                cout << "Reg" << aux->addr << ", ";
            }
            cout << "Controlled_by: ";
            vector<std::shared_ptr<Register>> controls = item->getControlBit();
            for (auto con : controls) {
                cout << "Reg" << con->addr << ", ";
            }
            cout << "." << endl;
        }
    }

    DAG Module::modConvertToDAG() {
        DAG dag = DAG();
        map<string, shared_ptr<Node>> latest_node;

        for (auto reg : regs) {
            auto node = make_shared<Node>(reg, reg->addr, "init");
            latest_node[node->variable_name] = node;
            dag.addNode(node);
        }

        for (auto op : ops) {
            string wire_name = markedName(op->getTargetBit()->addr);
            if (!latest_node.count(wire_name))
                cout << "Can not find " << wire_name << endl;
            assert(latest_node.count(wire_name));
            auto previous_node = latest_node[wire_name];
            auto node = make_shared<Node>(op->getTargetBit(), op->getTargetBit()->addr, op->getName(),
                                          previous_node->value_id+1, 0);
            dag.addNode(node);
            dag.connectConsumedNodes(previous_node, node);
            latest_node[node->variable_name] = node;
            for (auto qarg : op->getAuxBit()) {
                string wire_dep_name = markedName(qarg->addr);
                auto aux_node = latest_node[wire_dep_name];
                dag.connectDependencyNodes(aux_node, node);
            }

            for (auto qarg : op->getControlBit()) {
                string wire_dep_name = markedName(qarg->addr);
                auto ctrl_node = latest_node[wire_dep_name];
                dag.connectDependencyNodes(ctrl_node, node);
            }
        }
        return dag;
    }

}