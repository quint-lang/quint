//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class FlagAccess : public IRVisitor {
    public:
        explicit FlagAccess(IRNode *root) {
            allow_undefined_visitor = true;
            invoke_default_visitor = false;
            root->accept(this);
        }

        void visit(Block *stmt) override {

        }
    };

    class WeakenAccess : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        explicit WeakenAccess(IRNode *root) {
            allow_undefined_visitor = true;
            invoke_default_visitor = false;
            current_offload_ = nullptr;
            current_struct_for_ = nullptr;
            root->accept(this);
        }

    private:
        OffloadedStmt *current_offload_;
        StructForStmt *current_struct_for_;
    };

    namespace irpass {

        void flag_access(IRNode *root) {
            QUINT_AUTO_PROF
            FlagAccess pass(root);
            WeakenAccess pass2(root);
        }
    }

}
