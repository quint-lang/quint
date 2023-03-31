//
// Created by BY210033 on 2023/3/30.
//
#include "codegen/llvm/codegen_llvm.h"
#include "runtime/program_impls/llvm/llvm_program.h"
#include "system/profiler.h"

namespace quint::lang {

    TaskCodegenLLVM::TaskCodegenLLVM(Kernel *kernel, IRNode *ir, std::unique_ptr<llvm::Module> &&module)
        : LLVMModuleBuilder(module == nullptr ? get_llvm_program(kernel->program)
                                                    ->get_llvm_context()
                                                    ->new_module("kernel")
                                               : std::move(module),
                            get_llvm_program(kernel->program)->get_llvm_context()),
          kernel(kernel),
          ir(ir),
          prog(kernel->program) {
        if (ir == nullptr)
            this->ir = kernel->ir.get();
        initialize_context();

        context_ty = get_runtime_type("RuntimeContext");
        physical_coordinate_ty = get_runtime_type(kLLVMPhysicalCoordinatesName);

        kernel_name = kernel->name + "_kernel";
    }

    void TaskCodegenLLVM::initialize_context() {
        qlctx = get_llvm_program(prog)->get_llvm_context();
        llvm_context = qlctx->get_this_thread_context();
        builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
    }

    void TaskCodegenLLVM::emit_to_module() {
        QUINT_AUTO_PROF
        ir->accept(this);
    }

    void TaskCodegenLLVM::eliminate_unused_functions() {
        QuintLLVMContext::eliminate_unused_functions(
                module.get(), [&](std::string func_name) {
                    for (auto &task : offloaded_tasks) {
                        if (task.name == func_name)
                            return true;
                    }
                    return false;
                });
    }

    LLVMCompiledTask TaskCodegenLLVM::run_compilation() {
        auto config = kernel->program->this_thread_config();
        kernel->offload_to_executable(ir);

        emit_to_module();
        eliminate_unused_functions();

        return {std::move(offloaded_tasks), std::move(module),
                std::move(used_tree_ids), std::move(struct_for_tls_sizes)};
    }

}