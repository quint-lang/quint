//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class EliminateImmutableLocalVars : public BasicStmtVisitor {
    private:
        using BasicStmtVisitor::visit;

        std::unordered_set<Stmt *> immutable_local_vars_;
        std::unordered_map<Stmt *, Stmt *> immutable_local_var_to_value_;
        ImmediateIRModifier immediate_modifier_;
        DelayedIRModifier delayed_modifier_;

    public:
        explicit EliminateImmutableLocalVars(
                const std::unordered_set<Stmt *> &immutable_local_vars,
                IRNode *node)
                : immutable_local_vars_(immutable_local_vars), immediate_modifier_(node){
        }

        static void run(IRNode *node) {
            EliminateImmutableLocalVars pass(
                    irpass::analysis::gather_immutable_local_vars(node), node);
            node->accept(&pass);
            pass.delayed_modifier_.modify_ir();
        }
    };

    namespace irpass {
        void eliminate_immutable_local_vars(IRNode *root) {
            QUINT_AUTO_PROF
            EliminateImmutableLocalVars::run(root);
        }
    }

}
