//
// Created by BY210033 on 2023/4/11.
//
#include "quint/program/snode_expr_utils.h"
#include "quint/ir/snode.h"
#include "quint/ir/frontend_ir.h"

namespace quint::lang {

    void place_child(Expr *expr_arg, const std::vector<int> &offset, int id_in_bit_struct, SNode *parent,
                     SNodeFieldMap *snode_to_exprs) {

    }

    void make_lazy_place(SNode *snode, SNodeFieldMap *snode_to_fields,
                         const std::function<void(std::unique_ptr<SNode> &, std::vector<Expr> &)> &collect) {

    }

}
