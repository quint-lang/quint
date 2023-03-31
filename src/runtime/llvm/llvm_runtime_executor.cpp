//
// Created by BY210033 on 2023/3/22.
//
#include "runtime/llvm/llvm_runtime_executor.h"

namespace quint::lang {

    LLVMRuntimeExecutor::LLVMRuntimeExecutor(CompileConfig &config, KernelProfilerBase *profiler): config_(&config) {
        llvm_runtime_ = nullptr;
        llvm_context_host_ = std::make_unique<QuintLLVMContext>(&config);

        if (config.kernel_profiler)
            profiler_ = profiler;

        this->initialize_host();
    }


    QuintLLVMContext *LLVMRuntimeExecutor::get_llvm_context() {
        return llvm_context_host_.get();
    }

    void LLVMRuntimeExecutor::initialize_host() {
        llvm_context_host_->init_runtime_jit_module();
    }

    uint64_t *LLVMRuntimeExecutor::get_ndarray_alloc_info_ptr(const DeviceAllocation &alloc) {
        return nullptr;
    }

}