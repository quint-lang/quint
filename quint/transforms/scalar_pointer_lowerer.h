//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_SCALAR_POINTER_LOWERER_H
#define QUINT_SCALAR_POINTER_LOWERER_H

#include <vector>

#include "quint/ir/stmt_op_types.h"

namespace quint::lang {
    class LinearizeStmt;
    class SNode;
    class Stmt;
    class StructForStmt;
    class VecStatement;

    class ScalarPointerLowerer {
    private:
        std::vector<SNode *> snodes_;
        int path_length_{0};

    protected:
        virtual Stmt *handle_snode_at_level(int level,
                                            LinearizeStmt *linearized,
                                            Stmt *last) {
            return last;
        }

        const std::vector<Stmt *> &indices_;
        const SNodeOpType snode_op_;
        const bool is_bit_vectorized_;
        VecStatement *const lowered_;
        const bool packed_;

    public:
        explicit ScalarPointerLowerer(SNode *leaf_snode,
                                      const std::vector<Stmt *> &indices,
                                      const SNodeOpType snode_op,
                                      const bool is_bit_vectorized,
                                      VecStatement *lowered,
                                      const bool packed);

        virtual ~ScalarPointerLowerer() = default;

        void run();
    };

}

#endif //QUINT_SCALAR_POINTER_LOWERER_H
