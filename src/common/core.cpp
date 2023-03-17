//
// Created by BY210033 on 2023/3/14.
//
#include "common/core.h"

namespace quint {

    std::string python_package_dir;

    CoreState &CoreState::get_instance() {
        static CoreState state;
        return state;
    }

    std::string get_python_package_dir() {
        return python_package_dir;
    }

    void set_python_package_dir(const std::string &dir) {
        python_package_dir = dir;
    }

}