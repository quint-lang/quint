//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/snode_types.h"
#include "quint/common/logger.h"

namespace quint::lang {

    bool is_gc_able(SNodeType t) {
        return (t == SNodeType::pointer || t == SNodeType::dynamic);
    }

}