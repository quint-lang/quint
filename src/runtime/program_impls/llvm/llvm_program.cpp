//
// Created by BY210033 on 2023/3/22.
//
#include "runtime/program_impls/llvm/llvm_program.h"

namespace quint::lang {

    LLVMProgramImpl::LLVMProgramImpl(CompileConfig &config, KernelProfilerBase *profiler)
        : ProgramImpl(config), compilation_workers("compile", config.num_compile_threads) {
        runtime_exec_ = std::make_unique<LLVMRuntimeExecutor>(config, profiler);
        cache_data_ = std::make_unique<LLVMOfflineCache>();
        if (config.offline_cache) {
            cache_reader_ =
                    LLVMOfflineCacheFileReader::make(offline_cache::get_cache_path(config.offline_cache_file_path));

        }
    }

    void LLVMProgramImpl::prepare_runtime_context(RuntimeContext *ctx) {
        ProgramImpl::prepare_runtime_context(ctx);
    }

}