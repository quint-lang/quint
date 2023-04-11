//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_CONTEXT_H
#define QUINT_CONTEXT_H

#include "quint/inc/constant.h"

#if defined(QUINT_RUNTIME_HOST)
namespace quint::lang {
#endif

    struct LLVMRuntime;
    struct RuntimeContext {
        enum class DevAllocType : int8_t {
            kNone = 0,
            kNdarray = 1,
        };

        LLVMRuntime *runtime{nullptr};

        uint64 args[quint_max_num_args_total];
        int32 extra_args[quint_max_num_args_extra][quint_max_num_indices];
        int32 cpu_thread_id;

        uint64 array_runtime_sizes[quint_max_num_args_total]{0};

        uint64 *result_buffer;

//        static constexpr size_t extra_args_size = sizeof(extra_args);

        DevAllocType device_allocation_type[quint_max_num_args_total]{
                DevAllocType::kNone};

#if defined(QUINT_RUNTIME_HOST)
        template <typename T>
        T get_arg(int i) {
            return quint_union_cast_with_different_sizes<T>(args[i]);
        }

        template <typename T>
        void set_arg(int i, T v) {
            args[i] = quint_union_cast_with_different_sizes<uint64>(v);
            set_array_device_allocation_type(i, DevAllocType::kNone);
        }

        void set_array_device_allocation_type(int i, DevAllocType usage) {
            this->device_allocation_type[i] = usage;
        }
#endif
    };

#if defined(QUINT_RUNTIME_HOST)
}
#endif
#endif //QUINT_CONTEXT_H
