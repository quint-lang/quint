//
// Created by BY210033 on 2023/4/11.
//
#include <array>

#include "quint/transforms/scalar_pointer_lowerer.h"
#include "quint/inc/constant.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/snode.h"
#include "quint/ir/statements.h"
#include "quint/transforms/utils.h"

namespace quint::lang {

    ScalarPointerLowerer::ScalarPointerLowerer(SNode *leaf_snode, const std::vector<Stmt *> &indices,
                                               const SNodeOpType snode_op, const bool is_bit_vectorized,
                                               VecStatement *lowered, const bool packed)
        : indices_(indices),
          snode_op_(snode_op),
          is_bit_vectorized_(is_bit_vectorized),
          lowered_(lowered),
          packed_(packed) {
        for (auto *snode = leaf_snode; snode != nullptr; snode = snode->parent) {
            snodes_.push_back(snode);
        }

        std::reverse(snodes_.begin(), snodes_.end());

        const int path_inc = (int)(snode_op_ != SNodeOpType::undefined);
        path_length_ = (int)snodes_.size() - 1 + path_inc;
    }

    void ScalarPointerLowerer::run() {

    }

}
