//
// Created by BY210033 on 2023/3/29.
//
#include "codegen/codegen.h"

#if defined(QUINT_WITH_LLVM)
#include "codegen/cpu/codegen_cpu.h"
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
        return LLVMCompiledKernel();
    }

    std::optional<LLVMCompiledKernel> KernelCodeGen::maybe_read_compilation_from_cache(const std::string &kernel_key) {
        return std::optional<LLVMCompiledKernel>();
    }

    void KernelCodeGen::cache_kernel(const std::string &kernel_key, const LLVMCompiledKernel &data) {

    }

    ModuleToFunctionConverter::ModuleToFunctionConverter(QuintLLVMContext *qlctx, LLVMRuntimeExecutor *program)
        : qlctx(qlctx), executor_(program){
    }

    FunctionType ModuleToFunctionConverter::convert(const Kernel *kernel, LLVMCompiledKernel data) const {
        return convert(kernel->name, infer_launch_args(kernel), std::move(data));
    }

}