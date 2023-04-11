//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/ir/statements.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class RemoveLoopUnique : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        DelayedIRModifier modifier_;

        static bool run(IRNode *root) {
            RemoveLoopUnique pass;
            root->accept(&pass);
            return pass.modifier_.modify_ir();
        }
    };

    namespace irpass {

        bool remove_loop_unique(IRNode *root) {
            QUINT_AUTO_PROF
            return RemoveLoopUnique::run(root);
        }

    }

}
