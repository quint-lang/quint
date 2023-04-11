//
// Created by BY210033 on 2023/3/28.
//

#ifndef QUINT_STMT_OP_TYPES_H
#define QUINT_STMT_OP_TYPES_H

namespace quint::lang {

    enum class UnaryOpType : int {
#define PER_UNARY_OP(x) x,
#include "quint/inc/unary_op.inc.h"
#undef PER_UNARY_OP
    };


    inline bool constexpr unary_op_is_cast(UnaryOpType op) {
        return op == UnaryOpType::cast_value || op == UnaryOpType::cast_bits;
    }

    enum class BinaryOpType : int {
#define PER_BINARY_OP(x) x,
#include "quint/inc/binary_op.inc.h"
#undef PER_BINARY_OP
    };

    enum class TernaryOpType : int { select, ifte, undefined };

    enum class AtomicOpType : int { add, sub, min, max, bit_and, bit_or, bit_xor };

    enum class SNodeOpType : int {
        is_active,
        length,
        get_addr,
        activate,
        deactivate,
        append,
        allocate,
        clear,
        undefined
    };
}

#endif //QUINT_STMT_OP_TYPES_H
