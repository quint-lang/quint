//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_RUNTIME_EXECUTOR_H
#define QUINT_LLVM_RUNTIME_EXECUTOR_H

#include "program/compile_config.h"
#include "program/context.h"
#include "program/kernel_profiler.h"

namespace quint::lang {

    class LLVMRuntimeExecutor {
    public:
        LLVMRuntimeExecutor(CompileConfig &config, KernelProfilerBase *profiler);

    private:
        CompileConfig *config_;
        KernelProfilerBase *profiler_ = nullptr;
    };

}

#endif //QUINT_LLVM_RUNTIME_EXECUTOR_H
