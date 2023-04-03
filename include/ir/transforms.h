//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_TRANSFORMS_H
#define QUINT_TRANSFORMS_H

#include <atomic>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "ir/ir.h"


namespace quint::lang {
    namespace irpass {
        void re_id(IRNode *root);
    }
}

#endif //QUINT_TRANSFORMS_H
