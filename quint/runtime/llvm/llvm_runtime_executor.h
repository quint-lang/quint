//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_RUNTIME_EXECUTOR_H
#define QUINT_LLVM_RUNTIME_EXECUTOR_H

#include "quint/program/compile_config.h"
#include "quint/rhi/llvm/llvm_device.h"
#define QUINT_RUNTIME_HOST
#include "quint/program/context.h"
#undef QUINT_RUNTIME_HOST
#include "quint/program/kernel_profiler.h"
#include "quint/runtime/llvm/llvm_context.h"

namespace quint::lang {

    class LLVMRuntimeExecutor {
    public:
        LLVMRuntimeExecutor(CompileConfig &config, KernelProfilerBase *profiler);

        QuintLLVMContext *get_llvm_context();

        void initialize_host();

        uint64_t *get_ndarray_alloc_info_ptr(const DeviceAllocation &alloc);

    private:
        CompileConfig *config_;

        std::unique_ptr<QuintLLVMContext> llvm_context_host_{nullptr};
        std::unique_ptr<QuintLLVMContext> llvm_context_device_{nullptr};
        void *llvm_runtime_{nullptr};

        KernelProfilerBase *profiler_ = nullptr;
    };

}

#endif //QUINT_LLVM_RUNTIME_EXECUTOR_H