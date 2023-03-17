//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_LANG_UTIL_H
#define QUINT_LANG_UTIL_H

#include "common/core.h"

namespace quint::lang {

    struct RuntimeContext;

    using FunctionType = std::function<void(RuntimeContext &)>;

}

#endif //QUINT_LANG_UTIL_H
