//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_CONTROLFLOWGRAPH_H
#define QUINT_CONTROLFLOWGRAPH_H

#include <optional>
#include <unordered_set>

#include "quint/ir/ir.h"

namespace quint::lang {

    class CFGNode {
    private:
        std::unordered_set<Block *> parent_blocks_;

    public:
        Block *block;
        int begin_location, end_location;
        bool is_parallel_executed;

        CFGNode *prev_node_in_same_block;
        CFGNode *next_node_in_same_block;

        std::vector<CFGNode *> prev, next;

        std::unordered_set<Stmt *> reach_gen, reach_kill, reach_in, reach_out;

        std::unordered_set<Stmt *> live_gen, live_kill, live_in, live_out;

        CFGNode(Block *block,
                int begin_location,
                int end_location,
                bool is_parallel_executed,
                CFGNode *prev_node_in_same_block);

        CFGNode();

        static void add_edge(CFGNode *from, CFGNode *to);

        bool empty() const;
    };

    class ControlFlowGraph {
    private:
        void erase(int node_id);

    public:
        struct LiveVarAnalysisConfig {
            std::unordered_set<const SNode *> eliminable_snodes;
        };
        std::vector<std::unique_ptr<CFGNode>> nodes;
        const int start_node = 0;
        int final_node{0};

        template<typename... Args>
        CFGNode *push_back(Args &&... args) {
            nodes.emplace_back(std::make_unique<CFGNode>(std::forward<Args>(args)...));
            return nodes.back().get();
        }

        void live_variable_analysis(
                bool after_lower_access,
                const std::optional<LiveVarAnalysisConfig> &config);

        void simplify_graph();

        bool store_to_load_forwarding(bool after_lower_access, bool autodiff_enabled);

        bool dead_store_elimination(
                bool after_lower_access,
                const std::optional<LiveVarAnalysisConfig> &config);

        void determine_ad_stack_size(int default_ad_stack_size);

    };

}

#endif //QUINT_CONTROLFLOWGRAPH_H
