//
// Created by BY210033 on 2023/4/3.
//

#ifndef QUINT_MEM_REQUEST_H
#define QUINT_MEM_REQUEST_H

#include "quint/inc/constant.h"

#if defined(QUINT_RUNTIME_HOST)
namespace quint::lang {
#endif

    struct MemRequest {
        std::size_t size;
        std::size_t alignment;
        uint8 *ptr;
        std::size_t __padding;
    };

    static_assert((sizeof(MemRequest) & (sizeof(MemRequest) - 1)) == 0);

    struct MemRequestQueue {
        MemRequest requests[quint_max_num_mem_requests];
        int tail;
        int processed;
    };

#if defined(QUINT_RUNTIME_HOST)
}
#endif

#endif //QUINT_MEM_REQUEST_H
