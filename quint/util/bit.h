//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_BIT_H
#define QUINT_BIT_H

#include "quint/common/core.h"

namespace quint::bit {

    constexpr bool is_power_of_two(int32 x) {
        return x != 0 && (x & (x - 1)) == 0;
    }

    constexpr uint32 log2int(uint64 value) {
        int ret = 0;
        value >>= 1;
        while (value) {
            value >>= 1;
            ret += 1;
        }
        return ret;
    }

}

#endif //QUINT_BIT_H
