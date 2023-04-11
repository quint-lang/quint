//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/transforms.h"

namespace quint::lang::irpass {

    bool replace_statements(IRNode *root,
                            std::function<bool(Stmt *)> filter,
                            std::function<Stmt *(Stmt *)> finder) {
        return transform_statements(
                root, std::move(filter), [&](Stmt *stmt, DelayedIRModifier *modifier) {
                    auto existing_new_stmt = finder(stmt);
                    irpass::replace_all_usages_with(root, stmt, existing_new_stmt);
                    modifier->erase(stmt);
                });
    }

}