//
// Created by BY210033 on 2023/3/29.
//
#include "codegen/codegen.h"

#if defined(QUINT_WITH_LLVM)
#include "codegen/cpu/codegen_cpu.h"
#include "runtime/program_impls/llvm/llvm_program.h"
#include "analysis/offline_cache_util.h"
#include "analysis/analysis.h"
#include "ir/transforms.h"
#endif


namespace quint::lang {

    KernelCodeGen::KernelCodeGen(Kernel *kernel)
        : prog(kernel->program), kernel(kernel) {
        this->ir = kernel->ir.get();
    }

    std::unique_ptr<KernelCodeGen> KernelCodeGen::create(Kernel *kernel) {
        // todo handler multiply backend
#ifdef QUINT_WITH_LLVM
        return std::make_unique<KernelCodeGenCPU>(kernel);
#else
        QUINT_NOT_IMPLEMENTED
#endif
    }

    LLVMCompiledKernel KernelCodeGen::compile_kernel_to_module() {
        auto *llvm_prog = get_llvm_program(prog);
        auto *qlctx = llvm_prog->get_llvm_context();
        auto &config = prog->this_thread_config();
        std::string kernel_key = get_hashed_offline_cache_key(&config, kernel);
        kernel->set_kernel_key_for_cache(kernel_key);
        if (config.offline_cache && this->supports_offline_cache() &&
            !kernel->is_evaluator) {
            auto res = maybe_read_compilation_from_cache(kernel_key);
            if (res) {
                QUINT_DEBUG("Create kernel '{}' from cache (key = '{}')", kernel->get_name(),
                            kernel_key);
                cache_kernel(kernel_key, *res);
                return std::move(*res);
            }
        }
        if (!kernel->lowered()) {
            kernel->lower(false);
        }

        auto block = dynamic_cast<Block *>(kernel->ir.get());
        auto &worker = get_llvm_program(kernel->program)->compilation_workers;
        QUINT_ASSERT(block)

        auto &offloads = block->statements;
        std::vector<std::unique_ptr<LLVMCompiledTask>> data(offloads.size());
        using TaskFunc = int32 (*)(void *);
        std::vector<TaskFunc > task_funcs(offloads.size());
        for (int i = 0; i < offloads.size(); ++i) {
            auto compile_func = [&, i] {
                qlctx->fetch_this_thread_struct_module();
                auto offload = irpass::analysis::clone(offloads[i].get());
                irpass::re_id(offload.get());
                auto new_data = this->compile_task(nullptr, offload->as<OffloadedStmt>());
                data[i] = std::make_unique<LLVMCompiledTask>(std::move(new_data));
            };
            if (kernel->is_evaluator) {
                compile_func();
            } else {
                worker.enqueue(compile_func);
            }
        }
        if (!kernel->is_evaluator) {
            worker.flush();
        }

        auto linked = qlctx->link_compiled_tasks(std::move(data));

        if (!kernel->is_evaluator) {
            QUINT_DEBUG("Cache kernel '{}' (key='{}')", kernel->get_name(), kernel_key);
            cache_kernel(kernel_key, linked);
        }
        return linked;
    }

    std::optional<LLVMCompiledKernel> KernelCodeGen::maybe_read_compilation_from_cache(const std::string &kernel_key) {
        return std::optional<LLVMCompiledKernel>();
    }

    void KernelCodeGen::cache_kernel(const std::string &kernel_key, const LLVMCompiledKernel &data) {
        get_llvm_program(prog)->cache_kernel(kernel_key, data, infer_launch_args(kernel));
    }

    ModuleToFunctionConverter::ModuleToFunctionConverter(QuintLLVMContext *qlctx, LLVMRuntimeExecutor *program)
        : qlctx(qlctx), executor_(program){
    }

    FunctionType ModuleToFunctionConverter::convert(const Kernel *kernel, LLVMCompiledKernel data) const {
        return convert(kernel->name, infer_launch_args(kernel), std::move(data));
    }

}