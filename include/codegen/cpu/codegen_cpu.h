//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_CODEGEN_CPU_H
#define QUINT_CODEGEN_CPU_H

#include "codegen/codegen.h"
#include "codegen/llvm/codegen_llvm.h"

#include <memory>

namespace quint::lang {

    class KernelCodeGenCPU : public KernelCodeGen {
    public:
        explicit KernelCodeGenCPU(Kernel *kernel) : KernelCodeGen(kernel) {
        }

#ifdef QUINT_WITH_LLVM
        static std::unique_ptr<TaskCodegenLLVM> make_codegen_llvm(Kernel *kernel,
                                                                  IRNode *ir);

        bool supports_offline_cache() const override {
            return true;
        }

        LLVMCompiledTask compile_task(std::unique_ptr<llvm::Module> &&module, OffloadedStmt *stmt) override;

#endif

        FunctionType compile_to_function() override;
    };

#ifdef QUINT_WITH_LLVM

    class CPUModuleToFunctionConverter : public ModuleToFunctionConverter {
    public:
        explicit CPUModuleToFunctionConverter(QuintLLVMContext *qlctx,
                                              LLVMRuntimeExecutor *executor)
            : ModuleToFunctionConverter(qlctx, executor) {
        }

        using ModuleToFunctionConverter::convert;

        FunctionType convert(const std::string &kernel_name,
                             const std::vector<LLVMLaunchArgInfo> &args,
                             LLVMCompiledKernel data) const override;
    };

#endif

}

#endif //QUINT_CODEGEN_CPU_H
