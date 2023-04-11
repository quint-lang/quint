//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"
#include "quint/analysis/analysis.h"

#include <typeindex>

namespace quint::lang {

    class WholeKernelCSE : public BasicStmtVisitor {
    private:
        std::unordered_set<int> visited_;
        std::vector<std::unordered_map<std::size_t , std::unordered_set<Stmt *>>> visible_stmts_;
        DelayedIRModifier modifier_;

    public:
        using BasicStmtVisitor::visit;

        WholeKernelCSE() {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void visit(Block *stmt) override {

        }

        static bool run(IRNode *root) {
            WholeKernelCSE cse;
            bool ir_modified = false;
            while (true) {
                root->accept(&cse);
                if (cse.modifier_.modify_ir()) {
                    ir_modified = true;
                } else {
                    break;
                }
            }
            return ir_modified;
        }
    };

    namespace irpass {

        bool whole_kernel_cse(IRNode *root) {
            QUINT_AUTO_PROF
            return WholeKernelCSE::run(root);
        }
    }

}
