//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/program/program.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class DemoteOperations : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        DelayedIRModifier modifier_;

        DemoteOperations() = default;

        static bool run(IRNode *node, const CompileConfig &config) {
            DemoteOperations pass;
            bool modified = false;
            while (true) {
                node->accept(&pass);
                if (pass.modifier_.modify_ir())
                    modified = true;
                else
                    break;
            }
            if (modified) {
                irpass::type_check(node, config);
            }
            return modified;
        }
    };

    namespace irpass {

        bool demote_operations(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            return DemoteOperations::run(root, config);
        }
    }

}