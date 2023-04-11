//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/visitors.h"
#include "quint/ir/frontend_ir.h"

#include <unordered_set>

namespace quint::lang {

    class DetectForsWithBreak : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        std::vector<Stmt *> loop_stack;
        std::unordered_set<Stmt *> fors_with_break;
        IRNode *root;

        explicit DetectForsWithBreak(IRNode *root) : root(root) {
        }

        std::unordered_set<Stmt *> run() {
            root->accept(this);
            return fors_with_break;
        }
    };

    namespace irpass::analysis {
        std::unordered_set<Stmt *> detect_fors_with_break(IRNode *root) {
            DetectForsWithBreak detective(root);
            return detective.run();
        }
    }

}
