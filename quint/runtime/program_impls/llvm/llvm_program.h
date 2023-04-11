//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_PROGRAM_H
#define QUINT_LLVM_PROGRAM_H

#include "../../../program/compile_config.h"
#include "../../../program/program_impl.h"
#define QUINT_RUNTIME_HOST
#include "../../../program/context.h"
#undef QUINT_RUNTIME_HOST
#include "../../../program/parallel_executor.h"
#include "../../llvm/llvm_offline_cache.h"
#include "../../llvm/llvm_runtime_executor.h"

namespace llvm {
    class Module;
}

namespace quint::lang {
    class Program;

    class LLVMProgramImpl : public ProgramImpl {
    public:
        LLVMProgramImpl(CompileConfig &config, KernelProfilerBase *profiler);

        FunctionType compile(Kernel *kernel) override;

        void prepare_runtime_context(RuntimeContext *ctx) override;

        QuintLLVMContext *get_llvm_context() {
            return runtime_exec_->get_llvm_context();
        }

        LLVMRuntimeExecutor *get_runtime_executor() {
            return runtime_exec_.get();
        }

        void cache_kernel(const std::string &kernel_key,
                          const LLVMCompiledKernel &data,
                          std::vector<LLVMLaunchArgInfo> &&args);

        ParallelExecutor compilation_workers;
    private:
        std::size_t num_snode_trees_processed_{0};
        std::unique_ptr<LLVMRuntimeExecutor> runtime_exec_;
        std::unique_ptr<LLVMOfflineCache> cache_data_;
        std::unique_ptr<LLVMOfflineCacheFileReader> cache_reader_;
    };

    LLVMProgramImpl *get_llvm_program(Program *prog);

}

#endif //QUINT_LLVM_PROGRAM_H
