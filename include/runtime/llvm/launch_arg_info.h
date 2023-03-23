//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LAUNCH_ARG_INFO_H
#define QUINT_LAUNCH_ARG_INFO_H

namespace quint::lang {
    struct LLVMLaunchArgInfo {
        bool is_array{false};

        bool operator==(const LLVMLaunchArgInfo &other) const;
        bool operator!=(const LLVMLaunchArgInfo &other) const {
            return !(*this == other);
        }
    };
}

#endif //QUINT_LAUNCH_ARG_INFO_H
