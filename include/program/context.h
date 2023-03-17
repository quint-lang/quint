//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_CONTEXT_H
#define QUINT_CONTEXT_H

#include "inc/constant.h"

namespace quint::lang {

    struct RuntimeContext {
//        enum class DevAllocType : int8_t {
//            kNone = 0
//        };

        uint64 args[quint_max_num_args_total];
        int32 extra_args[quint_max_num_args_extra][quint_max_num_indices];
        int32 cpu_thread_id;

        uint64 array_runtime_sizes[quint_max_num_args_total]{0};

        uint64 *result_buffer;

//        static constexpr size_t extra_args_size = sizeof(extra_args);

    };

}

#endif //QUINT_CONTEXT_H
