//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_PROGRAM_H
#define QUINT_LLVM_PROGRAM_H

#include "program/compile_config.h"
#include "program/program_impl.h"
#include "program/context.h"
#include "program/parallel_executor.h"
#include "runtime/llvm/llvm_offline_cache.h"
#include "runtime/llvm/llvm_runtime_executor.h"

namespace llvm {
    class Module;
}

namespace quint::lang {
    class Program;

    class LLVMProgramImpl : public ProgramImpl {
    public:
        LLVMProgramImpl(CompileConfig &config, KernelProfilerBase *profiler);

        void prepare_runtime_context(RuntimeContext *ctx) override;

        ParallelExecutor compilation_workers;
    private:
        std::size_t num_snode_trees_processed_{0};
        std::unique_ptr<LLVMRuntimeExecutor> runtime_exec_;
        std::unique_ptr<LLVMOfflineCache> cache_data_;
        std::unique_ptr<LLVMOfflineCacheFileReader> cache_reader_;
    };

}

#endif //QUINT_LLVM_PROGRAM_H
