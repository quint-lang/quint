//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LAUNCH_ARG_INFO_H
#define QUINT_LAUNCH_ARG_INFO_H

#include <vector>

namespace quint::lang {
    struct LLVMLaunchArgInfo {
        bool is_array{false};

        bool operator==(const LLVMLaunchArgInfo &other) const;
        bool operator!=(const LLVMLaunchArgInfo &other) const {
            return !(*this == other);
        }
    };

    class Kernel;

    std::vector<LLVMLaunchArgInfo> infer_launch_args(const Kernel *kernel);
}

#endif //QUINT_LAUNCH_ARG_INFO_H
