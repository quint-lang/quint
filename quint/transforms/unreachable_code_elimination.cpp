//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class UselessContinueEliminator : public IRVisitor {
    public:
        bool modified;

        UselessContinueEliminator() : modified(false) {
            allow_undefined_visitor = true;
        }
    };

    class UnreachableCodeEliminator : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        bool modified;
        UselessContinueEliminator useless_continue_eliminator;
        DelayedIRModifier modifier;

        UnreachableCodeEliminator() : modified(false) {
            allow_undefined_visitor = true;
        }

        void visit(Block *stmt_list) override {

        }

        static bool run(IRNode *root) {
            bool modified = false;
            while (true) {
                UnreachableCodeEliminator eliminator;
                root->accept(&eliminator);
                eliminator.modifier.modify_ir();
                if (eliminator.useless_continue_eliminator.modified || eliminator.modified) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified;
        }
    };

    namespace irpass {
        bool unreachable_code_elimination(IRNode *root) {
            QUINT_AUTO_PROF
            return UnreachableCodeEliminator::run(root);
        }
    }

}
