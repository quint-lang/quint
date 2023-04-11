//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/control_flow_graph.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/statements.h"
#include "quint/system/profiler.h"

#include <queue>
#include <unordered_set>

namespace quint::lang {

    CFGNode::CFGNode(Block *block, int begin_location, int end_location, bool is_parallel_executed,
                     CFGNode *prev_node_in_same_block)
                     : block(block), begin_location(begin_location), end_location(end_location),
                       is_parallel_executed(is_parallel_executed),
                       prev_node_in_same_block(prev_node_in_same_block),
                       next_node_in_same_block(nullptr) {
        if (prev_node_in_same_block != nullptr) {
            prev_node_in_same_block->next_node_in_same_block = this;
        }
        if (!empty()) {
            QUINT_ASSERT(begin_location >= 0)
            QUINT_ASSERT(block)
            auto parent_block = block;
            parent_blocks_.insert(parent_block);
            while (parent_block->parent_block()) {
                parent_block = parent_block->parent_block();
                parent_blocks_.insert(parent_block);
            }
        }

    }

    CFGNode::CFGNode(): CFGNode(nullptr, -1, -1, false, nullptr) {
    }

    void CFGNode::add_edge(CFGNode *from, CFGNode *to) {
        from->next.push_back(to);
        to->prev.push_back(from);
    }

    bool CFGNode::empty() const {
        return begin_location >= end_location;
    }

    void ControlFlowGraph::erase(int node_id) {

    }

    void ControlFlowGraph::live_variable_analysis(bool after_lower_access,
                                                  const std::optional<LiveVarAnalysisConfig> &config) {

    }

    void ControlFlowGraph::simplify_graph() {

    }

    bool ControlFlowGraph::store_to_load_forwarding(bool after_lower_access, bool autodiff_enabled) {
        return false;
    }

    bool ControlFlowGraph::dead_store_elimination(bool after_lower_access,
                                                 const std::optional<LiveVarAnalysisConfig> &config) {
        QUINT_AUTO_PROF
        live_variable_analysis(after_lower_access, config);
        return false;
    }

    void ControlFlowGraph::determine_ad_stack_size(int default_ad_stack_size) {

    }

}
