//
// Created by BY210033 on 2023/3/10.
//
#include "quint/program/program.h"
#include "quint/util/offline_cache.h"
#include "quint/system/timeline.h"
#include "quint/runtime/program_impls/llvm/llvm_program.h"
#include "quint/system/profiler.h"
#include "quint/math/arithmetic.h"

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
        int qunit = 1;
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

    FunctionType Program::compile(Kernel &kernel) {
        auto start_t = Time::get_time();
        QUINT_AUTO_PROF
        auto ret = program_impl_->compile(&kernel);
        QUINT_ASSERT(ret)
        total_compilation_time_ += Time::get_time() - start_t;
        return ret;
    }

    Kernel &Program::get_snode_reader(SNode *snode) {
        QUINT_ASSERT(snode->type == SNodeType::place)
        auto kernel_name = fmt::format("snode_reader_{}", snode->id);
        auto &ker = kernel([snode, this](Kernel *kernel) {
            ExprGroup indices;
            for (int i = 0; i < snode->num_active_indices; i++) {
                auto argload_expr = Expr::make<ArgLoadExpression>(i, PrimitiveType::i32);
                argload_expr.type_check(&this->this_thread_config());
                indices.push_back(argload_expr);
            }
            ASTBuilder &builder = kernel->context->builder();
            auto ret = Stmt::make<FrontendReturnStmt>(ExprGroup(
                    builder.expr_subscript(Expr(snode_to_fields_.at(snode)), indices)));
            builder.insert(std::move(ret));
        });
        ker.name = kernel_name;
        ker.is_accessor = true;
        for (int i = 0; i < snode->num_active_indices; ++i) {
            ker.insert_scalar_arg(PrimitiveType::i32);
        }
        ker.insert_ret(snode->dt);
        return ker;
    }

    Kernel &Program::get_snode_writer(SNode *snode) {
        QUINT_ASSERT(snode->type == SNodeType::place)
        auto kernel_name = fmt::format("snode_writer_{}", snode->id);
        auto &ker = kernel([snode, this](Kernel *kernel) {
            ExprGroup indices;
            for (int i = 0; i < snode->num_active_indices; i++) {
                auto argload_expr = Expr::make<ArgLoadExpression>(i, PrimitiveType::i32);
                argload_expr.type_check(&this->this_thread_config());
                indices.push_back(argload_expr);
            }
            ASTBuilder &builder = kernel->context->builder();
            auto expr = builder.expr_subscript(Expr(snode_to_fields_.at(snode)), indices);
            builder.insert_assignment(expr,
                                      Expr::make<ArgLoadExpression>(snode->num_active_indices,
                                                                    snode->dt->get_compute_type()),
                                      expr->tb);
        });
        ker.name = kernel_name;
        ker.is_accessor = true;
        for (int i = 0; i < snode->num_active_indices; ++i) {
            ker.insert_scalar_arg(PrimitiveType::i32);
        }
        ker.insert_scalar_arg(snode->dt);
        return ker;
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

    int Program::default_block_dim(const CompileConfig &config) {
        return config.default_cpu_block_dim;
    }

    void Program::synchronize() {

    }

    void Program::prepare_runtime_context(RuntimeContext *ctx) {
        ctx->result_buffer = result_buffer;
        program_impl_->prepare_runtime_context(ctx);
    }

    SNode *Program::get_snode_root(int tree_id) {
        return snode_trees_[tree_id]->root();
    }

}