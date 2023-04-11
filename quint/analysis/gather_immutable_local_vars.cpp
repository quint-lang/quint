//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"

namespace quint::lang {

    class GatherImmutableLocalVars : public BasicStmtVisitor {
    private:
        using BasicStmtVisitor::visit;

        enum class AllocaStatus { kCreated = 0, kStoredOnce = 1, kInvalid = 2 };
        std::unordered_map<Stmt *, AllocaStatus> alloca_status_;

    public:
        explicit GatherImmutableLocalVars() {
            invoke_default_visitor = true;
        }

        void default_visit(Stmt *stmt) {
            for (auto &op : stmt->get_operands()) {
                if (op != nullptr && op->is<AllocaStmt>()) {
                    auto status_iter = alloca_status_.find(op);
                    QUINT_ASSERT(status_iter != alloca_status_.end())
                    status_iter->second = AllocaStatus::kInvalid;
                }
            }
        }

        void visit(Stmt *stmt) override {
            default_visit(stmt);
        }

        void preprocess_container_stmt(Stmt *stmt) override {
            default_visit(stmt);
        }


        static std::unordered_set<Stmt *> run(IRNode *node) {
            GatherImmutableLocalVars pass;
            node->accept(&pass);
            std::unordered_set<Stmt *> result;
            for (auto &[k, v] : pass.alloca_status_) {
                if (v == AllocaStatus::kStoredOnce) {
                    result.insert(k);
                }
            }
            return result;
        }
    };

    namespace irpass::analysis {

        std::unordered_set<Stmt *> gather_immutable_local_vars(IRNode *root) {
            return GatherImmutableLocalVars::run(root);
        }

    }

}