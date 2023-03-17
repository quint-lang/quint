//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_EXCEPTION_H
#define QUINT_EXCEPTION_H

#include "common/core.h"

#include <exception>

namespace quint {

    class ExceptionForPython : public std::exception {
    private:
        std::string msg_;

    public:
        explicit ExceptionForPython(const std::string &msg) : msg_(msg) {
        }

        char const *what() const noexcept override {
            return msg_.c_str();
        }
    };

    void raise_assertion_failure_in_python(const std::string &msg);

}

#endif //QUINT_EXCEPTION_H
