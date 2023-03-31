//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_LLVM_CODEGEN_UTILS_H
#define QUINT_LLVM_CODEGEN_UTILS_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "runtime/llvm/llvm_context.h"

namespace quint::lang {

    inline constexpr char kLLVMPhysicalCoordinatesName[] = "PhysicalCoordinates";

    void check_func_call_signature(llvm::FunctionType *func_type,
                                   llvm::StringRef func_name,
                                   std::vector<llvm::Value *> &arglist,
                                   llvm::IRBuilder<> *builder);

    class LLVMModuleBuilder {
    public:
        std::unique_ptr<llvm::Module> module{nullptr};
        llvm::BasicBlock *entry_block{nullptr};
        std::unique_ptr<llvm::IRBuilder<>> builder{nullptr};
        QuintLLVMContext *qlctx{nullptr};
        llvm::LLVMContext *llvm_context{nullptr};


        LLVMModuleBuilder(std::unique_ptr<llvm::Module> &&module,
                          QuintLLVMContext *ql_ctx)
            : module(std::move(module)), qlctx(ql_ctx) {
            QUINT_ASSERT(this->module != nullptr)
            QUINT_ASSERT(&this->module->getContext() == qlctx->get_this_thread_context())
        }

        llvm::Value *create_entry_block_alloca(llvm::Type *type,
                                              std::size_t alignment = 0,
                                              llvm::Value *array_size = nullptr) {
            llvm::IRBuilderBase::InsertPointGuard guard(*builder);
            builder->SetInsertPoint(entry_block);
            auto alloca = builder->CreateAlloca(type, (unsigned)0, array_size);
            if (alignment != 0) {
                alloca->setAlignment(llvm::Align(alignment));
            }
            return alloca;
        }

        llvm::Value *create_entry_block_alloca(DataType dt, bool is_pointer = false) {
            auto type = qlctx->get_data_type(dt);
            if (is_pointer)
                type = llvm::PointerType::get(type, 0);
            return create_entry_block_alloca(type);
        }

        llvm::Type *get_runtime_type(const std::string &name) {
            return qlctx ->get_runtime_type(name);
        }

        llvm::Function *get_runtime_function(const std::string &name) {
            auto f = qlctx->get_runtime_function(name);
            if (!f) {
                QUINT_ERROR("LLVMRuntime function {} not found.", name)
            }
            f = llvm::cast<llvm::Function>(
                    module->getOrInsertFunction(name, f->getFunctionType(), f->getAttributes()).getCallee());
            return f;
        }

        llvm::Value *call(llvm::IRBuilder<> *builder,
                          llvm::Value *func,
                          llvm::FunctionType *func_ty,
                          std::vector<llvm::Value *> args) {
            check_func_call_signature(func_ty, func->getName(), args, builder);
            return builder->CreateCall(func_ty, func, std::move(args));
        }

        llvm::Value *call(llvm::Value *func,
                          llvm::FunctionType *func_ty,
                          std::vector<llvm::Value *> args) {
            return call(builder.get(), func, func_ty, std::move(args));
        }

        llvm::Value *call(llvm::IRBuilder<> *builder,
                          llvm::Function *func,
                          std::vector<llvm::Value *> args) {
            return call(builder, func, func->getFunctionType(), std::move(args));
        }

        llvm::Value *call(llvm::Function *func, std::vector<llvm::Value *> args) {
            return call(builder.get(), func, std::move(args));
        }

        llvm::Value *call(llvm::IRBuilder<> *builder,
                          const std::string &func_name,
                          std::vector<llvm::Value *> args) {
            auto func = get_runtime_function(func_name);
            return call(builder, func, std::move(args));
        }

        llvm::Value *call(const std::string &func_name,
                          std::vector<llvm::Value *> args) {
            return call(builder.get(), func_name, std::move(args));
        }

        template <typename... Args>
        llvm::Value *call(llvm::IRBuilder<> *builder,
                          llvm::Function *func,
                          Args *...args) {
            return call(builder, func, {args...});
        }

        template <typename... Args>
        llvm::Value *call(llvm::Function *func, Args &&...args) {
            return call(builder.get(), func, std::forward<Args>(args)...);
        }

        template <typename... Args>
        llvm::Value *call(llvm::IRBuilder<> *builder,
                          const std::string &func_name,
                          Args *...args) {
            return call(builder, func_name, {args...});
        }

        template <typename... Args>
        llvm::Value *call(const std::string &func_name, Args &&...args) {
            return call(builder.get(), func_name, std::forward<Args>(args)...);
        }

    };

}

#endif //QUINT_LLVM_CODEGEN_UTILS_H
