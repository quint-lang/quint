//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/visitors.h"

namespace quint::lang {

    class FieldsRegisteredChecker : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        FieldsRegisteredChecker() {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void preprocess_container_stmt(Stmt *stmt) override {
            QUINT_ASSERT(stmt->fields_registered)
        }

        void visit(Stmt *stmt) override {
            QUINT_ASSERT(stmt->fields_registered);
        }

        static void run(IRNode *root) {
            FieldsRegisteredChecker checker;
            root->accept(&checker);
        }

    };

    namespace irpass::analysis {
        void check_fields_registered(IRNode *root) {
            return FieldsRegisteredChecker::run(root);
        }
    }

}