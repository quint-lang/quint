//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_CONSTANT_H
#define QUINT_CONSTANT_H

#include <cstddef>

constexpr int quint_max_num_indices = 12;
constexpr int quint_max_num_args_total = 64;
constexpr int quint_max_num_args_extra = 32;
constexpr int quint_max_num_snodes = 1024;
constexpr int kMaxNumSnodeTreesLlvm = 512;
constexpr std::size_t quint_global_tmp_buffer_size = 1024 * 1024;
constexpr int quint_max_num_mem_requests = 1024 * 64;
constexpr std::size_t quint_page_size = 4096;
constexpr std::size_t quint_error_message_max_length = 2048;
constexpr std::size_t quint_error_message_max_arguments = 32;

constexpr std::size_t quint_result_buffer_ret_value_id = 0;

constexpr std::size_t quint_result_buffer_error_id = 30;
constexpr std::size_t quint_result_buffer_runtime_query_id = 31;

constexpr int quint_listgen_max_element_size = 1024;

template <typename T, typename G>
T quint_union_cast_with_different_sizes(G g) {
    union {
        T t;
        G g;
    } u;
    u.g = g;
    return u.t;
}

template <typename T, typename G>
T quint_union_cast(G g) {
    static_assert(sizeof(T) == sizeof(G));
    return quint_union_cast_with_different_sizes<T>(g);
}

enum class ExternalArrayLayout { kAOS, kSOA, kNull };

enum class SNodeGradType { kPrimal, kAdjoint, kDual, kAdjointCheckbit };

#endif //QUINT_CONSTANT_H
