//
// Created by BY210033 on 2023/3/29.
//

#ifndef QUINT_CODEGEN_H
#define QUINT_CODEGEN_H

#include "ir/ir.h"
#include "program/program.h"
#include "ir/statements.h"
#ifdef QUINT_WITH_LLVM
#include "codegen/llvm/codegen_llvm.h"
#include "llvm/IR/Module.h"
#include "codegen/llvm/llvm_compiled_data.h"
#include "runtime/llvm/llvm_runtime_executor.h"
#include "runtime/llvm/launch_arg_info.h"
#endif

namespace quint::lang {

    class KernelCodeGen {
    protected:
        Program *prog;
        Kernel *kernel;
        IRNode *ir;

    public:
        explicit KernelCodeGen(Kernel *kernel);

        virtual ~KernelCodeGen() = default;

        static std::unique_ptr<KernelCodeGen> create(Kernel *kernel);

        virtual FunctionType compile_to_function() = 0;
        virtual bool supports_offline_cache() const {
            return false;
        }

#ifdef QUINT_WITH_LLVM
        virtual LLVMCompiledKernel compile_kernel_to_module();

        virtual LLVMCompiledTask compile_task(
            std::unique_ptr<llvm::Module> &&module = nullptr,
            OffloadedStmt *stmt = nullptr) {
            QUINT_NOT_IMPLEMENTED
        }

        std::optional<LLVMCompiledKernel> maybe_read_compilation_from_cache(const std::string &kernel_key);

        void cache_kernel(const std::string &kernel_key, const LLVMCompiledKernel &data);

#endif
    };

#ifdef QUINT_WITH_LLVM

    class ModuleToFunctionConverter {
    public:
        explicit ModuleToFunctionConverter(QuintLLVMContext *qlctx,
                                           LLVMRuntimeExecutor *program);

        virtual ~ModuleToFunctionConverter() = default;

        virtual FunctionType convert(const std::string &kernel_name,
                                     const std::vector<LLVMLaunchArgInfo> &args,
                                     LLVMCompiledKernel data) const = 0;

        virtual FunctionType convert(const Kernel *kernel,
                                     LLVMCompiledKernel data) const;

    protected:
        QuintLLVMContext *qlctx{nullptr};
        LLVMRuntimeExecutor *executor_{nullptr};
    };

#endif

}

#endif //QUINT_CODEGEN_H
