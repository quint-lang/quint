//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/analysis/analysis.h"

namespace quint::lang {

    class GatherStatementUsages : public BasicStmtVisitor {
    private:
        using BasicStmtVisitor::visit;

        // maps a stmt to all its usages <stmt, operand>
        std::unordered_map<Stmt *, std::vector<std::pair<Stmt *, int>>> stmt_usages_;

    public:
        explicit GatherStatementUsages() {
            invoke_default_visitor = true;
        }

        void default_visit(Stmt *stmt) {
            auto ops = stmt->get_operands();
            for (int i = 0; i < ops.size(); ++i) {
                auto &op = ops[i];
                if (op != nullptr) {
                    stmt_usages_[op].push_back({stmt, i});
                }
            }
        }

        void preprocess_container_stmt(Stmt *stmt) override {
            default_visit(stmt);
        }

        static std::unordered_map<Stmt *, std::vector<std::pair<Stmt *, int>>> run(IRNode *node) {
            GatherStatementUsages pass;
            node->accept(&pass);
            return pass.stmt_usages_;
        }

    };

    namespace irpass::analysis {

        std::unordered_map<Stmt *, std::vector<std::pair<Stmt *, int>>>
        gather_statement_usages(IRNode *root) {
            return GatherStatementUsages::run(root);
        }

    }

}
