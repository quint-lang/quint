//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_FUNCTION_H
#define QUINT_FUNCTION_H

#include "quint/program/callable.h"
#include "quint/program/function_key.h"

namespace quint::lang {

    class Program;

    class Function : public Callable {
    public:
        FunctionKey func_key;

        Function(Program *program, const FunctionKey &func_key);

        std::optional<std::string> &try_get_ast_serialization_data() {
            return ast_serialization_data_;
        }

    private:
        std::optional<std::string> ast_serialization_data_;
    };

}

#endif //QUINT_FUNCTION_H
