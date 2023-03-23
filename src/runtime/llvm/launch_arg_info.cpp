//
// Created by BY210033 on 2023/3/22.
//
#include "runtime/llvm/launch_arg_info.h"

namespace quint::lang {

    bool LLVMLaunchArgInfo::operator==(const LLVMLaunchArgInfo &other) const {
        return is_array == other.is_array;
    }

}