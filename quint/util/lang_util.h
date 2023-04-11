//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_LANG_UTIL_H
#define QUINT_LANG_UTIL_H

#include "quint/common/core.h"

namespace quint::lang {

    struct RuntimeContext;

    using FunctionType = std::function<void(RuntimeContext &)>;

    extern std::string compiled_lib_dir;
    std::string runtime_lib_dir();
    void set_lib_dir(const std::string &dir);

}

#endif //QUINT_LANG_UTIL_H
