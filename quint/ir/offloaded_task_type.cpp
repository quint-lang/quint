//
// Created by BY210033 on 2023/3/30.
//
#include "quint/ir/offloaded_task_type.h"
#include "quint/common/core.h"

namespace quint::lang {

    std::string offloaded_task_type_name(OffloadedTaskType tt) {
        if (false) {
        }
#define PER_TASK_TYPE(x) else if (tt == OffloadedTaskType::x) return #x;
#include "quint/inc/offloaded_task_type.inc.h"
#undef PER_TASK_TYPE
        else
            QUINT_NOT_IMPLEMENTED
    }

}