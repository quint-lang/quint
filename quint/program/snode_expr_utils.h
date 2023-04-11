//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_SNODE_EXPR_UTILS_H
#define QUINT_SNODE_EXPR_UTILS_H

#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

namespace quint::lang {
    class Expr;
    class SNode;
    class FieldExpression;

    using SNodeFieldMap = std::unordered_map< const SNode *, std::shared_ptr<FieldExpression> >;

    void place_child(Expr *expr_arg,
                     const std::vector<int> &offset,
                     int id_in_bit_struct,
                     SNode *parent,
                     SNodeFieldMap *snode_to_exprs);

    void make_lazy_place(SNode *snode,
                         SNodeFieldMap *snode_to_fields,
                         const std::function<void(std::unique_ptr<SNode> &,
                                                  std::vector<Expr> &)> &collect);
}

#endif //QUINT_SNODE_EXPR_UTILS_H
