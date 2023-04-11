//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_CODEGEN_LLVM_H
#define QUINT_CODEGEN_LLVM_H

#include <set>
#include <unordered_map>

#ifdef QUINT_WITH_LLVM
#include "quint/ir/ir.h"
#include "quint/codegen/llvm/llvm_compiled_data.h"
#include "quint/codegen/llvm/llvm_codegen_utils.h"
#include "quint/program/program.h"

namespace quint::lang {

    class TaskCodegenLLVM : public IRVisitor, public LLVMModuleBuilder {
    public:
        Kernel *kernel;
        IRNode *ir;
        Program *prog;
        std::string kernel_name;
        std::vector<llvm::Value *> kernel_args;
        llvm::Type *context_ty;
        llvm::Type *physical_coordinate_ty;
        llvm::Value *current_coordinates;
        llvm::Value *parent_coordinates{nullptr};
        llvm::Value *block_corner_coordinates{nullptr};
        llvm::GlobalVariable *bls_buffer{nullptr};
        // Mainly for supporting continue stmt
        llvm::BasicBlock *current_loop_reentry;
        // Mainly for supporting break stmt
        llvm::BasicBlock *current_while_after_loop;
        llvm::FunctionType *task_function_type;
        std::unordered_map<Stmt *, llvm::Value *> llvm_val;
        llvm::Function *func;
//        OffloadedStmt *current_offload{nullptr};
        std::unique_ptr<OffloadedTask> current_task;
        std::vector<OffloadedTask> offloaded_tasks;
        llvm::BasicBlock *func_body_bb;
        llvm::BasicBlock *final_block;
        std::set<std::string> linked_modules;
        bool returned{false};
        std::unordered_set<int> used_tree_ids;
        std::unordered_set<int> struct_for_tls_sizes;
//        Function *now_real_func{nullptr};

        std::unordered_map<const Stmt *, std::vector<llvm::Value *>> loop_vars_llvm;

//        std::unordered_map<Function *, llvm::Function *> func_map;

        using IRVisitor::visit;
        using LLVMModuleBuilder::call;

        explicit TaskCodegenLLVM(Kernel *kernel,
                                 IRNode *ir = nullptr,
                                 std::unique_ptr<llvm::Module> &&module = nullptr);

        void initialize_context();

        virtual void emit_to_module();

        void eliminate_unused_functions();

        virtual LLVMCompiledTask run_compilation();
    };

}

#endif

#endif //QUINT_CODEGEN_LLVM_H
