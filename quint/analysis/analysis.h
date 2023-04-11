//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_ANALYSIS_H
#define QUINT_ANALYSIS_H

#include "quint/ir/ir.h"
#include "quint/ir/mesh.h"

namespace quint::lang {

    class DiffRange {

    };

    class ControlFlowGraph;

    namespace irpass::analysis {
        std::unique_ptr<ControlFlowGraph> build_cfg(IRNode *root);
        void check_fields_registered(IRNode *root);
        std::unique_ptr<IRNode> clone(IRNode *root);

        std::unordered_map<Stmt *, std::vector<std::pair<Stmt *, int>>>
        gather_statement_usages(IRNode *root);
        std::unordered_set<Stmt *> gather_immutable_local_vars(IRNode *root);
        std::unordered_set<SNode *> gather_deactivations(IRNode *root);
        std::unordered_set<Stmt *> detect_fors_with_break(IRNode *root);
        std::pair<std::unordered_set<SNode *>, std::unordered_set<SNode *>>
        gather_snode_read_writes(IRNode *root);
        std::vector<Stmt *> gather_statements(IRNode *root,
                                              const std::function<bool(Stmt *)> &test);

        void verify(IRNode *root);
    }

}

#endif //QUINT_ANALYSIS_H
