//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

#include <unordered_set>

namespace quint::lang {

    class DIE : public IRVisitor {
    public:
        std::unordered_set<int> used;
        int phase; // 0: mark usage 1: eliminate
        DelayedIRModifier modifier;
        bool modified_ir;

        explicit DIE(IRNode *node) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
            modified_ir = false;
            while (true) {
                bool modified = false;
                phase = 0;
                used.clear();
                node->accept(this);
                phase = 1;
                while (true) {
                    node->accept(this);
                    if (modifier.modify_ir()) {
                        modified = true;
                        modified_ir = true;
                        continue;
                    }
                    break;
                }
                if (!modified)
                    break;
            }
        }
    };

    namespace irpass {
        bool die(IRNode *root) {
            QUINT_AUTO_PROF
            DIE instance(root);
            return instance.modified_ir;
        }
    }

}