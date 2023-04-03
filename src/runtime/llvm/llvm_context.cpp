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
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Linker/Linker.h"


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
        ThreadLocalData *data = get_this_thread_data();
        QUINT_ASSERT(data->llvm_context)
        return data->llvm_context;
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

    void QuintLLVMContext::fetch_this_thread_struct_module() {
        ThreadLocalData *data = get_this_thread_data();
        if (data->struct_modules.empty()) {
            for (auto &[id, mod] : main_thread_data_->struct_modules) {
                data->struct_modules[id] = clone_module_to_this_thread_context(mod.get());
            }
        }
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

    std::unique_ptr<llvm::Module> QuintLLVMContext::clone_module_to_this_thread_context(llvm::Module *module) {
        QUINT_TRACE("Cloning struct module");
        QUINT_ASSERT(module)
        auto this_context = get_this_thread_context();
        return clone_module_to_context(module, this_context);
    }

    void QuintLLVMContext::add_struct_for_func(llvm::Module *module, int tls_size) {
        auto func_name = get_struct_for_func_name(tls_size);
        if (module->getFunction(func_name)) {
            return;
        }
        auto struct_for_func = module->getFunction("parallel_struct_for");
        auto &llvm_context = module->getContext();
        auto value_map = llvm::ValueToValueMapTy();
        auto patched_struct_for_func =
                llvm::CloneFunction(struct_for_func, value_map);
        patched_struct_for_func->setName(func_name);

        int num_found_alloca = 0;
        llvm::AllocaInst *alloca = nullptr;

        auto char_type = llvm::Type::getInt8Ty(llvm_context);

        for (auto &bb : *patched_struct_for_func) {
            for (llvm::Instruction &inst : bb) {
                auto now_alloca = llvm::dyn_cast<llvm::AllocaInst>(&inst);
                if (!now_alloca || now_alloca->getAlign().value() != 8)
                    continue;
                auto alloca_type = now_alloca->getAllocatedType();
                if (alloca_type->isArrayTy() && alloca_type->getArrayNumElements() == 1 &&
                    alloca_type->getArrayElementType() == char_type) {
                    alloca = now_alloca;
                    num_found_alloca++;
                }
            }
        }

        QUINT_ASSERT(num_found_alloca == 1 && alloca)
        auto new_type = llvm::ArrayType::get(char_type, tls_size);
        {
            llvm::IRBuilder<> builder(alloca);
            auto *new_alloca = builder.CreateAlloca(new_type);
            new_alloca->setAlignment(llvm::Align(8));
            QUINT_ASSERT(alloca->hasOneUse())
            auto *gep = llvm::cast<llvm::GetElementPtrInst>(alloca->user_back());
            QUINT_ASSERT(gep->getPointerOperand() == alloca)
            std::vector<llvm::Value *> indices(gep->idx_begin(), gep->idx_end());
            builder.SetInsertPoint(gep);
            auto *new_gep = builder.CreateInBoundsGEP(new_type, new_alloca, indices);
            gep->replaceAllUsesWith(new_gep);
            gep->eraseFromParent();
            alloca->eraseFromParent();
        }
    }

    std::string QuintLLVMContext::get_struct_for_func_name(int tls_size) {
        return "parallel_struct_for_" + std::to_string(tls_size);
    }

    LLVMCompiledKernel QuintLLVMContext::link_compiled_tasks(std::vector<std::unique_ptr<LLVMCompiledTask>> data_list) {
        LLVMCompiledKernel linked;
        std::unordered_set<int> used_tree_ids;
        std::unordered_set<int> tls_sizes;
        std::unordered_set<std::string> offloaded_names;
        auto mod = new_module("kernel", linking_context_data->llvm_context);
        llvm::Linker linker(*mod);
        for (auto &datum : data_list) {
            for (auto tree_id : datum->used_tree_ids) {
                used_tree_ids.insert(tree_id);
            }
            for (auto tls_size : datum->struct_for_tls_sizes) {
                tls_sizes.insert(tls_size);
            }
            for (auto &task : datum->tasks) {
                offloaded_names.insert(task.name);
                linked.tasks.push_back(std::move(task));
            }
            linker.linkInModule(clone_module_to_context(
                    datum->module.get(), linking_context_data->llvm_context));
        }
        for (auto tree_id : used_tree_ids) {
            linker.linkInModule(
                    llvm::CloneModule(*linking_context_data->struct_modules[tree_id]),
                    llvm::Linker::LinkOnlyNeeded | llvm::Linker::OverrideFromSrc);
        }
        auto runtime_module =
                llvm::CloneModule(*linking_context_data->runtime_module);
        for (auto tls_size : tls_sizes) {
            add_struct_for_func(runtime_module.get(), tls_size);
        }
        linker.linkInModule(
                std::move(runtime_module),
                llvm::Linker::LinkOnlyNeeded | llvm::Linker::OverrideFromSrc);
        eliminate_unused_functions(mod.get(), [&](std::string func_name) -> bool {
            return offloaded_names.count(func_name);
        });
        linked.module = std::move(mod);
        return linked;
    }

    std::unique_ptr<llvm::Module>
    QuintLLVMContext::clone_module_to_context(llvm::Module *module, llvm::LLVMContext *target_context) {
        std::string bitcode;
        {
            std::lock_guard<std::mutex> _(mut_);
            llvm::raw_string_ostream sos(bitcode);
            llvm::WriteBitcodeToFile(*module, sos);
        }

        auto cloned = llvm::parseBitcodeFile(
                llvm::MemoryBufferRef(bitcode, "runtime_bitcode"), *target_context);
        if (!cloned) {
            auto error = cloned.takeError();
            QUINT_ERROR("Bitcode cloned failed")
        }
        return std::move(cloned.get());
    }

    QuintLLVMContext::ThreadLocalData *QuintLLVMContext::get_this_thread_data() {
        std::lock_guard<std::mutex> _(thread_map_mut_);
        auto tid = std::this_thread::get_id();
        if (per_thread_data_.find(tid) == per_thread_data_.end()) {
            std::stringstream ss;
            ss << tid;
            QUINT_TRACE("Creating thread local data for thread {}", ss.str());
            per_thread_data_[tid] = std::make_unique<ThreadLocalData>(
                    std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>()));
        }
        return per_thread_data_[tid].get();
    }

}