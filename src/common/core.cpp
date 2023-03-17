//
// Created by BY210033 on 2023/3/14.
//
#include "common/core.h"

namespace quint {

    CoreState &CoreState::get_instance() {
        static CoreState state;
        return state;
    }

}