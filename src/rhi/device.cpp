//
// Created by BY210033 on 2023/3/31.
//
#include "rhi/device.h"

namespace quint::lang {

    DevicePtr DeviceAllocation::get_ptr(uint64_t offset) const {
        return DevicePtr{{device, alloc_id}, offset};
    }

}