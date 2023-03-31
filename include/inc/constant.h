//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_CONSTANT_H
#define QUINT_CONSTANT_H

#include <cstddef>

constexpr int quint_max_num_indices = 12;
constexpr int quint_max_num_args_total = 64;
constexpr int quint_max_num_args_extra = 32;

template <typename T, typename G>
T quint_union_cast_with_different_sizes(G g) {
    union {
        T t;
        G g;
    } u;
    u.g = g;
    return u.t;
}

#endif //QUINT_CONSTANT_H
