//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/statements.h"
#include "quint/ir/visitors.h"

namespace quint::lang {

    class GatherDeactivations : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        std::unordered_set<SNode *> snodes;
        IRNode *root;

        explicit GatherDeactivations(IRNode *root) : root(root) {
        }

        std::unordered_set<SNode *> run() {
            root->accept(this);
            return snodes;
        }
    };

    namespace irpass::analysis {
        std::unordered_set<SNode *> gather_deactivations(IRNode *root) {
            GatherDeactivations gather(root);
            return gather.run();
        }
    }

}