//
// Created by BY210033 on 2023/3/22.
//
#include "quint/codegen/llvm/llvm_compiled_data.h"
#include "llvm/Transforms/Utils/Cloning.h"

namespace quint::lang {

    LLVMCompiledTask LLVMCompiledTask::clone() const {
        return {tasks, llvm::CloneModule(*module), used_tree_ids,
                struct_for_tls_sizes};
    }

    LLVMCompiledKernel LLVMCompiledKernel::clone() const {
        return {tasks, llvm::CloneModule(*module)};
    }

}