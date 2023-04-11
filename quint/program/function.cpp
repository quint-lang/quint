//
// Created by BY210033 on 2023/4/11.
//
#include "quint/program/function.h"
#include "quint/program/program.h"
#include "quint/ir/transforms.h"

namespace quint::lang {

    Function::Function(Program *program, const FunctionKey &func_key)
            : func_key(func_key) {
        this->program = program;
    }

}