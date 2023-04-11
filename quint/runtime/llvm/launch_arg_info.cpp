//
// Created by BY210033 on 2023/3/22.
//
#include "quint/runtime/llvm/launch_arg_info.h"

#include "quint/program/kernel.h"

namespace quint::lang {

    bool LLVMLaunchArgInfo::operator==(const LLVMLaunchArgInfo &other) const {
        return is_array == other.is_array;
    }

    std::vector <LLVMLaunchArgInfo> infer_launch_args(const Kernel *kernel) {
        std::vector<LLVMLaunchArgInfo> res;
        res.reserve(kernel->args.size());
        for (const auto &a : kernel->args) {
            res.push_back(LLVMLaunchArgInfo{a.is_array});
        }
        return res;
    }

}