//
// Created by BY210033 on 2023/4/6.
//
#include <vector>
#include <unordered_set>

#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/statements.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class IRVerifier : public BasicStmtVisitor {
    private:
        Block *current_block_;
        Stmt *current_container_stmt_;
        std::vector<std::unordered_set<Stmt *>> visible_stmts_;

    public:
        using BasicStmtVisitor::visit;

        explicit IRVerifier(IRNode *root)
            : current_block_(nullptr), current_container_stmt_(nullptr) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
            if (!root->is<Block>())
                visible_stmts_.emplace_back();
            if (root->is<Stmt>() && root->as<Stmt>()->is_container_statement()) {
                current_container_stmt_ = root->as<Stmt>();
            }
        }

        void basic_verify(Stmt *stmt) {
            QUINT_ASSERT_INFO(stmt->parent == current_block_,
                              "stmt({})->parent({}) != current_block({})", stmt->id,
                              fmt::ptr(stmt->parent), fmt::ptr(current_block_))
            for (auto &op : stmt->get_operands()) {
                if (op == nullptr)
                    continue;
                bool found = false;
                for (int depth = (int)visible_stmts_.size() - 1; depth >= 0; depth--) {
                    if (visible_stmts_[depth].find(op) != visible_stmts_[depth].end()) {
                        found = true;
                        break;
                    }
                }
                QUINT_ASSERT_INFO(found,
                                  "IR broken: stmt {} {} cannot have operand {} {}."
                                  " If you are using autodiff, please check out"
                                  " https://docs.quint-lang.org/docs/"
                                  "differences_between_quint_and_python_programs"
                                  " If it doesn't help, please open an issue at"
                                  " https://github.com/quint-lang/quint to help us improve."
                                  " Thanks in advance!",
                                  stmt->type(), stmt->id, op->type(), op->id)
            }
            visible_stmts_.back().insert(stmt);
        }

        void preprocess_container_stmt(Stmt *stmt) override {
            basic_verify(stmt);
        }

        void visit(Stmt *stmt) override {
            basic_verify(stmt);
        }

        void visit(Block *block) override {

        }

        static void run(IRNode *root) {
            IRVerifier verifier(root);
            root->accept(&verifier);
        }

    };

    namespace irpass::analysis {

        void verify(IRNode *root) {
            QUINT_AUTO_PROF
            if (!root->is<Block>() && !root->is<OffloadedStmt>()) {
                QUINT_WARN(
                        "IR root is neither a Block nor an OffloadedStmt."
                        " Skipping verification.");
            } else {
                IRVerifier::run(root);
            }
        }

    }

}