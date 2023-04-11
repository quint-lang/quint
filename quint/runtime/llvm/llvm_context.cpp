//
// Created by BY210033 on 2023/3/15.
//
#include "quint/runtime/llvm/llvm_context.h"
#include "quint/system/profiler.h"

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
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO/Internalize.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/Bitcode/BitcodeWriter.h"


#include "quint/util/lang_util.h"
#include "quint/jit/jit_session.h"
#include "quint/runtime/program_impls/llvm/llvm_program.h"

#ifdef _WIN32
// Travis CI seems doesn't support <filesystem>...
#include <filesystem>
#else
#include <unistd.h>
#endif


namespace quint::lang {

    using namespace llvm;

    std::string get_runtime_fn(std::string name) {
        return fmt::format("runtime_{}.bc", name);
    }

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
        llvm::install_fatal_error_handler(
                [](void *user_data, const char *reason, bool gen_crash_diag) {
                    QUINT_ERROR("LLVM Fatal Error: {}", reason)
                },
                nullptr);

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        jit = JITSession::create(this, config);

        linking_context_data = std::make_unique<ThreadLocalData>(
                std::make_unique<llvm::orc::ThreadSafeContext>(
                        std::make_unique<llvm::LLVMContext>()));
        linking_context_data->runtime_module = clone_module_to_context(
                get_this_thread_runtime_module(), linking_context_data->llvm_context);

        QUINT_TRACE("Quint llvm context created.");
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
        int total_inst = 0;
        int total_big_inst = 0;

        for (auto &f : *module) {
            int c = num_instructions(&f);
            if (c > 100) {
                total_big_inst += c;
                QUINT_INFO("{}: {} inst.", std::string(f.getName()), c);
            }
            total_inst += c;
        }
        QUINT_P(total_inst)
        QUINT_P(total_big_inst)
    }

    void QuintLLVMContext::init_runtime_jit_module() {

    }

    JITModule *QuintLLVMContext::create_jit_module(std::unique_ptr<llvm::Module> module) {
        return jit->add_module(std::move(module));
    }

    void QuintLLVMContext::add_struct_module(std::unique_ptr<llvm::Module> module, int tree_id) {

    }

    std::unique_ptr<llvm::Module> QuintLLVMContext::module_from_file(const std::string &file) {
        auto ctx = get_this_thread_context();
        std::unique_ptr<llvm::Module> module = module_from_bitcode_file(
                fmt::format("{}/{}", runtime_lib_dir(), file), ctx);

//        auto patch_atomic_add = [&](std::string name,
//                                    llvm::AtomicRMWInst::BinOp op) {
//            auto func = module->getFunction(name);
//            if (!func) {
//                return ;
//            }
//            func->deleteBody();
//            auto bb = llvm::BasicBlock::Create(*ctx, "entry", func);
//            llvm::IRBuilder<> builder(*ctx);
//            builder.SetInsertPoint(bb);
//            std::vector<llvm::Value *> args;
//            for (auto &arg : func->args()) {
//                args.push_back(&arg);
//            }
//            builder.CreateRet(builder.CreateAtomicRMW(
//                    op, args[0], args[1], llvm::MaybeAlign(0),
//                    llvm::AtomicOrdering::SequentiallyConsistent));
//            QuintLLVMContext::mark_inline(func);
//        };
//
//        patch_atomic_add("atomic_add_i32", llvm::AtomicRMWInst::Add);
//        patch_atomic_add("atomic_add_i64", llvm::AtomicRMWInst::Add);
//        patch_atomic_add("atomic_add_f64", llvm::AtomicRMWInst::FAdd);
//        patch_atomic_add("atomic_add_f32", llvm::AtomicRMWInst::FAdd);

        return module;
    }

    void QuintLLVMContext::mark_inline(llvm::Function *func) {
        for (auto &B : *func) {
            for (auto &I : B) {
                if (auto *call = llvm::dyn_cast<llvm::CallInst>(&I)) {
                    if (auto func = call->getCalledFunction();
                        func && func->getName() == "mark_force_no_inline") {
                        return;
                    }
                }
            }
        }
        func->removeFnAttr(llvm::Attribute::OptimizeNone);
        func->removeFnAttr(llvm::Attribute::NoInline);
        func->addFnAttr(llvm::Attribute::AlwaysInline);
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
        return get_this_thread_runtime_module()->getFunction(name);
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
        QUINT_AUTO_PROF
        auto data = get_this_thread_data();
        if (!data->runtime_module) {
            data->runtime_module = module_from_file(get_runtime_fn("x64"));
        }
        return data->runtime_module.get();
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

    int QuintLLVMContext::num_instructions(llvm::Function *func) {
        int counter = 0;
        for (llvm::BasicBlock &bb : *func) {
            counter += std::distance(bb.begin(), bb.end());
        }
        return counter;
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

    std::unique_ptr<llvm::Module> LLVMModuleBitCodeLoader::load(llvm::LLVMContext *ctx) const {
        QUINT_AUTO_PROF
        std::ifstream ifs(bitcode_path_, std::ios::binary);
        QUINT_ERROR_IF(!ifs, "Bitcode file ({}) not found.", bitcode_path_)
        std::string bitcode(std::istreambuf_iterator<char>(ifs),
                            (std::istreambuf_iterator<char>()));
        auto runtime =
                parseBitcodeFile(llvm::MemoryBufferRef(bitcode, buffer_id_), *ctx);
        if (!runtime) {
            auto error = runtime.takeError();
            QUINT_WARN("Bitcode loading error message:");
            llvm::errs() << error << "\n";
            QUINT_ERROR("Failed to load bitcode={}", bitcode_path_)
            return nullptr;
        }

        if (inline_funcs_) {
            for (auto &f : *(runtime.get())) {
                QuintLLVMContext::mark_inline(&f);
            }
        }

        const bool module_broken = llvm::verifyModule(*runtime.get(), &llvm::errs());
        if (module_broken) {
            QUINT_ERROR("Broken bitcode={}", bitcode_path_);
            return nullptr;
        }
        std::unique_ptr<Module> &ptr = runtime.get();
        return std::move(runtime.get());
    }

    std::unique_ptr<llvm::Module> module_from_bitcode_file(const std::string &bitcode_path, llvm::LLVMContext *ctx) {
        LLVMModuleBitCodeLoader loader;
        return loader.set_bitcode_path(bitcode_path)
            .set_buffer_id("runtime_bitcode")
            .set_inline_funcs(true)
            .load(ctx);
    }

}