//
// Created by BY210033 on 2023/3/22.
//
#include "runtime/llvm/llvm_runtime_executor.h"

namespace quint::lang {

    LLVMRuntimeExecutor::LLVMRuntimeExecutor(CompileConfig &config, KernelProfilerBase *profiler): config_(&config) {
        if (config.kernel_profiler)
            profiler_ = profiler;
    }

}