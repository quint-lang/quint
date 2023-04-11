//
// Created by BY210033 on 2023/4/3.
//
#include "../quint/program/program.h"
#include "../quint/util/lang_util.h"

#include <iostream>

using namespace quint::lang;

int main() {
    set_lib_dir("D:/local-dev/project/quint/quint/runtime/llvm/runtime_module");
    Program program = Program();
    std::cout << program.config().offline_cache << std::endl;

}