//
// Created by BY210033 on 2023/3/30.
//
#include "quint/codegen/llvm/llvm_codegen_utils.h"

namespace quint::lang {

    void check_func_call_signature(llvm::FunctionType *func_type,
                                   llvm::StringRef func_name,
                                   std::vector<llvm::Value *> &arglist,
                                   llvm::IRBuilder<> *builder) {

    }

}