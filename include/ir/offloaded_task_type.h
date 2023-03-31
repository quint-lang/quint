//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_OFFLOADED_TASK_TYPE_H
#define QUINT_OFFLOADED_TASK_TYPE_H

#include <string>

namespace quint::lang {

    enum class OffloadedTaskType : int {
#define PER_TASK_TYPE(x) x,
#include "inc/offloaded_task_type.inc.h"
#undef PER_TASK_TYPE
    };

    std::string offloaded_task_type_name(OffloadedTaskType tt);

}

#endif //QUINT_OFFLOADED_TASK_TYPE_H
