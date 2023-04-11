//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class StatementUsagesReplace : public IRVisitor {
    public:
        Stmt *old_stmt, *new_stmt;

        StatementUsagesReplace(Stmt *old_stmt, Stmt *new_stmt)
            : old_stmt(old_stmt), new_stmt(new_stmt) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void visit(Stmt *stmt) override {

        }

        static void run(IRNode *node, Stmt *old_stmt, Stmt *new_stmt) {
            StatementUsagesReplace pass(old_stmt, new_stmt);
            if (node != nullptr) {
                node->accept(&pass);
                return;
            }

            QUINT_ASSERT(old_stmt->parent != nullptr)
            old_stmt->parent->accept(&pass);
            auto current_block = old_stmt->parent->parent_block();

            while (current_block != nullptr) {
                for (auto &stmt : current_block->statements) {
                    stmt->replace_operand_with(old_stmt, new_stmt);
                }
                current_block = current_block->parent_block();
            }
        }
    };

    namespace irpass {

        void replace_all_usages_with(IRNode *root, Stmt *old_stmt, Stmt *new_stmt) {
            QUINT_AUTO_PROF
            StatementUsagesReplace::run(root, old_stmt, new_stmt);
        }
    }
}
