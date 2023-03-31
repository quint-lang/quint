//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_DEVICE_H
#define QUINT_DEVICE_H

#include "common/core.h"

namespace quint::lang {

    class Device;
    struct DevicePtr;

    using DeviceAllocationId = uint64_t;

    struct DeviceAllocation {
        Device *device{nullptr};
        DeviceAllocationId alloc_id{0};

        DevicePtr get_ptr(uint64_t offset = 0) const;

        bool operator==(const DeviceAllocation &other) const {
            return other.device == device && other.alloc_id == alloc_id;
        }

        bool operator!=(const DeviceAllocation &other) const {
            return !(*this == other);
        }
    };

    struct DevicePtr : public DeviceAllocation {
        uint64_t offset{0};

        bool operator==(const DevicePtr &other) const {
            return other.device == device && other.alloc_id == alloc_id &&
                   other.offset == offset;
        }

        bool operator!=(const DevicePtr &other) const {
            return !(*this == other);
        }
    };

    class Device {

    };

}

#endif //QUINT_DEVICE_H
