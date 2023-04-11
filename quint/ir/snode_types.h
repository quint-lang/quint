//
// Created by BY210033 on 2023/4/10.
//

#ifndef QUINT_SNODE_TYPES_H
#define QUINT_SNODE_TYPES_H

#include <string>

namespace quint::lang {

    enum class SNodeType {
#define PER_SNODE(x) x,
#include "quint/inc/snodes.inc.h"
#undef PER_SNODE
    };

    bool is_gc_able(SNodeType t);
}
#endif //QUINT_SNODE_TYPES_H
