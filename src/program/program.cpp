//
// Created by BY210033 on 2023/3/10.
//
#include "program/program.h"
#include "util/offline_cache.h"
#include "system/timeline.h"
#include "runtime/program_impls/llvm/llvm_program.h"

namespace quint::lang {

    std::atomic<int> Program::num_instances_;

    Program::Program() {
        QUINT_TRACE("Program initializing...");

#if defined(_M_X64) || defined(__x86_64)
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
#if defined(__arm64__) || defined(__aarch64__)
        std::uint64_t fpcr;
        __asm__ __volatile__("");
        __asm__ __volatile__("MRS %0, FPCR" : "=r"(fpcr));
        __asm__ __volatile__("");
        __asm__ __volatile__("MSR FPCR, %0"
                             :
                             : "ri"(fpcr | (1 << 24)));
        __asm__ __volatile__("");
#endif
        main_thread_id_ = std::this_thread::get_id();
        configs.rehash(default_compile_config.num_compile_threads +1);
        configs[main_thread_id_] = default_compile_config;
        auto &config = this_thread_config();
        if (config.debug)
            config.check_out_of_bound = true;
        offline_cache::disable_offline_cache_if_needed(&config);

        profiler = make_profiler(config.kernel_profiler);

        if (config.use_llvm) {
#ifdef QUINT_WITH_LLVM
            program_impl_ = std::make_unique<LLVMProgramImpl>(config, profiler.get());
#else
            QUINT_ERRPR("This quint is not compiled with LLVM");
#endif
        } else {
            QUINT_NOT_IMPLEMENTED
        }

        QUINT_ASSERT(program_impl_)

        QUINT_ASSERT_INFO(num_instances_ == 0, "Only one instance at a time")
        total_compilation_time_ = 0;
        num_instances_ += 1;

        result_buffer = nullptr;
        current_callable = nullptr;
        sync = true;
        finalized_ = false;

        Timelines::get_instance().set_enabled(config.timeline);
    }

    Program::~Program() {
//        finalize();
    }

    void Program::materialize_runtime() {

    }

    int Program::get_snode_tree_size() {
        return 1;
    }

    void Program::visualize_layout(const std::string &fn) {

    }

    FunctionType Program::compile(Kernel *kernel) {
        return quint::lang::FunctionType();
    }

    void Program::finalize() {
        if (finalized_)
            return;
        synchronize();
        QUINT_ASSERT(std::this_thread::get_id() == main_thread_id_)
        QUINT_TRACE("Program finalizing...");

        synchronize();
        // todo handler memory_pool
    }

    void Program::synchronize() {

    }

    void Program::prepare_runtime_context(RuntimeContext *ctx) {
        ctx->result_buffer = result_buffer;
        program_impl_->prepare_runtime_context(ctx);
    }

    SNode *Program::get_snode_root(int tree_id) {
        return nullptr;
    }

}