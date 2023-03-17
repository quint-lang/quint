//
// Created by BY210033 on 2023/3/16.
//
#include "python/exception.h"

namespace quint {

    void raise_assertion_failure_in_python(const std::string &msg) {
        throw ExceptionForPython(msg);
    }

}