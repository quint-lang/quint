//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/statements.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class RemoveRangeAssumption : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        DelayedIRModifier modifier_;

        static bool run(IRNode *root) {
            RemoveRangeAssumption pass;
            root->accept(&pass);
            return pass.modifier_.modify_ir();
        }
    };

    namespace irpass {

        bool remove_range_assumption(IRNode *root) {
            QUINT_AUTO_PROF;
            return RemoveRangeAssumption::run(root);
        }

    }

}