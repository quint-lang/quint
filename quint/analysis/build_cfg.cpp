//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/control_flow_graph.h"
#include "quint/program/function.h"
#include "quint/ir/statements.h"


namespace quint::lang {
    struct CFGFuncKey {
        FunctionKey func_key{"", -1, -1};
        bool in_parallel_for{false};

        bool operator==(const CFGFuncKey &other) const {
            return func_key == other.func_key && in_parallel_for == other.in_parallel_for;
        }
    };
}

namespace std {
    template <>
    struct hash<quint::lang::CFGFuncKey> {
        std::size_t operator()(const quint::lang::CFGFuncKey &k) const {
            return std::hash<quint::lang::FunctionKey>()(k.func_key) ^ ((std::size_t)k.in_parallel_for << 32);
        }
    };
}

namespace quint::lang {

    class CFGBuilder : public IRVisitor {
    public:
        CFGBuilder()
                : current_block_(nullptr),
                  last_node_in_current_block_(nullptr),
                  current_stmt_id_(-1),
                  begin_location_(-1),
                  current_offloaded_(nullptr),
                  in_parallel_for_(false) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
            graph_ = std::make_unique<ControlFlowGraph>();
            auto start_node = graph_->push_back();
            prev_nodes_.push_back(start_node);
        }

        static std::unique_ptr<ControlFlowGraph> run(IRNode *node) {
            CFGBuilder builder;
            node->accept(&builder);
            if (!builder.graph_->nodes[builder.graph_->final_node]->empty()) {
                builder.graph_->push_back();
                CFGNode::add_edge(builder.graph_->nodes[builder.graph_->final_node].get(),
                                  builder.graph_->nodes.back().get());
                builder.graph_->final_node = (int) builder.graph_->nodes.size() - 1;
            }
            return std::move(builder.graph_);
        }

    private:
        std::unique_ptr<ControlFlowGraph> graph_;
        Block *current_block_;
        CFGNode *last_node_in_current_block_;
        std::vector<CFGNode *> continues_in_current_loop_;
        std::vector<CFGNode *> breaks_in_current_loop_;
        int current_stmt_id_;
        int begin_location_;
        std::vector<CFGNode *> prev_nodes_;
        OffloadedStmt *current_offloaded_;
        bool in_parallel_for_;
        std::unordered_map<CFGFuncKey, CFGNode *> node_func_begin_;
        std::unordered_map<CFGFuncKey, CFGNode *> node_func_end_;
    };

    namespace irpass::analysis {
        std::unique_ptr<ControlFlowGraph> build_cfg(IRNode *root) {
            return CFGBuilder::run(root);
        }
    }

}