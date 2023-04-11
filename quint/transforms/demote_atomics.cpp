//
// Created by BY210033 on 2023/4/9.
//
#include "quint/analysis/analysis.h"
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"


#include <queue>

namespace quint::lang {

    class DemoteAtomics : public BasicStmtVisitor {
    private:
        std::unordered_map<const SNode *, GlobalPtrStmt *> loop_unique_ptr;
        std::unordered_map<int, ExternalPtrStmt *> loop_unique_arr_ptr;

    public:
        using BasicStmtVisitor::visit;

        OffloadedStmt *current_offloaded;
        DelayedIRModifier modifier;

        DemoteAtomics() {
            current_offloaded = nullptr;
        }

        static bool run(IRNode *node) {
            DemoteAtomics demoter;
            bool modified = false;
            while (true) {
                node->accept(&demoter);
                if (demoter.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified;
        }
    };

    namespace irpass {
        bool demote_atomics(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            bool modified = DemoteAtomics::run(root);
            type_check(root, config);
            return modified;
        }
    }

}
