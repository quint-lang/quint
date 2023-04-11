//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"

namespace quint::lang {

    class StatementsTransformer : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        StatementsTransformer(
                std::function<bool(Stmt *)> filter,
                std::function<void(Stmt *, DelayedIRModifier *)> transformer)
                : filter_(std::move(filter)), transformer_(std::move(transformer)) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        static bool run(IRNode *root,
                        std::function<bool(Stmt *)> filter,
                        std::function<void(Stmt *, DelayedIRModifier *)> replacer) {
            StatementsTransformer transformer(std::move(filter), std::move(replacer));
            root->accept(&transformer);
            return transformer.modifier_.modify_ir();
        }


    private:
        std::function<bool(Stmt *)> filter_;
        std::function<void(Stmt *, DelayedIRModifier *)> transformer_;
        DelayedIRModifier modifier_;
    };

    namespace irpass {

        bool transform_statements(
                IRNode *root,
                std::function<bool(Stmt *)> filter,
                std::function<void(Stmt *, DelayedIRModifier *)> transformer) {
            return StatementsTransformer::run(root, std::move(filter),
                                              std::move(transformer));
        }

    }

}