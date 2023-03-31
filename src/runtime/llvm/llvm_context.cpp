//
// Created by BY210033 on 2023/3/15.
//
#include "runtime/llvm/llvm_context.h"
#include "system/profiler.h"

#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsNVPTX.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/IPO/Internalize.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"


namespace quint::lang {

    QuintLLVMContext::ThreadLocalData::ThreadLocalData(std::unique_ptr<llvm::orc::ThreadSafeContext> ctx)
        : thread_safe_llvm_context(std::move(ctx)), llvm_context(thread_safe_llvm_context->getContext()){
    }

    QuintLLVMContext::ThreadLocalData::~ThreadLocalData() {
        runtime_module.reset();
        struct_modules.clear();
        thread_safe_llvm_context.reset();
    }

    QuintLLVMContext::QuintLLVMContext(CompileConfig *config) : config_(config) {
//        QUINT_TRACE("Creating Quint llvm context for arch: {}",)
        main_thread_id_ = std::this_thread::get_id();
        main_thread_data_ = get_this_thread_data();
        llvm::remove_fatal_error_handler();
        // todo llvm install error_handler and init
//        llvm::install_fatal_error_handler(
//                [](void *user_data, const char *reason, bool gen_crash_diag) {
//                    QUINT_ERROR("LLVM Fatal Error: {}", reason);
//                },
//                nullptr);

//        llvm::Initi
        jit = JITSession::create(this, config);

        linking_context_data = std::make_unique<ThreadLocalData>(
                std::make_unique<llvm::orc::ThreadSafeContext>(
                        std::make_unique<llvm::LLVMContext>()));
        linking_context_data->runtime_module = clone_module_to_context(
                get_this_thread_runtime_module(), linking_context_data->llvm_context);
    }

    QuintLLVMContext::~QuintLLVMContext() {

    }

    llvm::LLVMContext *QuintLLVMContext::get_this_thread_context() {
        return nullptr;
    }

    llvm::orc::ThreadSafeContext *QuintLLVMContext::get_this_thread_safe_context() {
        return nullptr;
    }

    void QuintLLVMContext::print_huge_functions(llvm::Module *module) {

    }

    void QuintLLVMContext::init_runtime_jit_module() {

    }

    JITModule *QuintLLVMContext::create_jit_module(std::unique_ptr<llvm::Module> module) {
        return jit->add_module(std::move(module));
    }

    void QuintLLVMContext::add_struct_module(std::unique_ptr<llvm::Module> module, int tree_id) {

    }

    void QuintLLVMContext::eliminate_unused_functions(llvm::Module *module,
                                                      std::function<bool(const std::string &)> export_indicator) {
        QUINT_AUTO_PROF
        using namespace llvm;
        QUINT_ASSERT(module)
        llvm::ModulePassManager manager;
        llvm::ModuleAnalysisManager  ana;
        llvm::PassBuilder pb;
        pb.registerModuleAnalyses(ana);
        manager.addPass(llvm::InternalizePass([&](const GlobalValue &val) -> bool {
            return export_indicator(val.getName().str());
        }));
        manager.addPass(GlobalDCEPass());
        manager.run(*module, ana);
    }

    llvm::Function *QuintLLVMContext::get_runtime_function(const std::string &name) {
        return nullptr;
    }

    llvm::Type *QuintLLVMContext::get_runtime_type(const std::string &name) {
        return nullptr;
    }

    std::unique_ptr<llvm::Module> QuintLLVMContext::new_module(std::string name, llvm::LLVMContext *context) {
        auto new_mod = std::make_unique<llvm::Module>(
                name, context ? *context : *get_this_thread_context());
        new_mod->setDataLayout(get_this_thread_runtime_module()->getDataLayout());
        return new_mod;
    }

    llvm::Type *QuintLLVMContext::get_data_type(DataType dt) {
        return nullptr;
    }

    llvm::Module *QuintLLVMContext::get_this_thread_runtime_module() {
        return nullptr;
    }

    std::unique_ptr<llvm::Module>
    QuintLLVMContext::clone_module_to_context(llvm::Module *module, llvm::LLVMContext *target_context) {
        return std::unique_ptr<llvm::Module>();
    }

    QuintLLVMContext::ThreadLocalData *QuintLLVMContext::get_this_thread_data() {
        return nullptr;
    }

}