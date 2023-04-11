//
// Created by BY210033 on 2023/3/22.
//
#include "quint/jit/jit_session.h"
#include "quint/jit/jit_module.h"
#include "quint/util/lang_util.h"
#include "quint/program/program.h"
#include "quint/system/profiler.h"
#include "quint/util/file_sequence_writer.h"

#ifdef QUINT_WITH_LLVM
#include "quint/runtime/llvm/llvm_context.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Error.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Support/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Host.h"
#endif

namespace quint::lang {

#ifdef QUINT_WITH_LLVM
    using namespace llvm;
    using namespace llvm::orc;
#endif

    std::pair<JITTargetMachineBuilder, llvm::DataLayout> get_host_target_info() {
        auto expected_jit = JITTargetMachineBuilder::detectHost();
        if (!expected_jit)
            QUINT_ERROR("LLVM TargetMachineBuilder has failed.")
        auto jit = *expected_jit;
        auto expected_data_layout = jit.getDefaultDataLayoutForTarget();
        if (!expected_data_layout)
            QUINT_ERROR("LLVM TargetMachineBuilder has failed when getting data layout.")
        auto data_layout = *expected_data_layout;
        return std::make_pair(jit, data_layout);
    }

    class JITSessionCPU;

    class JITModuleCPU : public JITModule {
    private:
        JITSessionCPU *session_;
        JITDylib *dylib_;

    public:
        JITModuleCPU(JITSessionCPU *session, JITDylib *dylib)
            : session_(session), dylib_(dylib) {
        }

        void *lookup_function(const std::string &name) override;

        bool direct_dispatch() const override {
            return true;
        }

    };

    class JITSessionCPU : public JITSession {
    private:
        ExecutionSession es_;
        RTDyldObjectLinkingLayer object_layer_;
        IRCompileLayer compile_layer_;
        DataLayout dl_;
        MangleAndInterner mangle_;
        std::mutex mut_;
        std::vector<JITDylib *> all_libs_;
        int module_counter_;
        SectionMemoryManager *memory_manager_;

    public:
        JITSessionCPU(QuintLLVMContext *ctx,
                      std::unique_ptr<ExecutorProcessControl> EPC,
                      CompileConfig *config,
                      JITTargetMachineBuilder JITMB,
                      DataLayout DL)
                : JITSession(ctx, config),
                  es_(std::move(EPC)),
                  object_layer_(es_,
                                [&]() {
                                    auto smgr = std::make_unique<SectionMemoryManager>();
                                    memory_manager_ = smgr.get();
                                    return smgr;
                                }),
                  compile_layer_(es_,
                                 object_layer_,
                                 std::make_unique<ConcurrentIRCompiler>(JITMB)),
                  dl_(DL),
                  mangle_(es_, dl_),
                  module_counter_(0),
                  memory_manager_(nullptr){
            if (JITMB.getTargetTriple().isOSBinFormatCOFF()) {
                object_layer_.setOverrideObjectFlagsWithResponsibilityFlags(true);
                object_layer_.setAutoClaimResponsibilityForObjectSymbols(true);
            }
        }

        ~JITSessionCPU() override {
            std::lock_guard<std::mutex> _(mut_);
            if (memory_manager_)
                memory_manager_->deregisterEHFrames();
            if (auto Err = es_.endSession())
                es_.reportError(std::move(Err));
        }

        JITModule *add_module(std::unique_ptr<llvm::Module> M, int max_reg) override {
            QUINT_ASSERT(max_reg == 0)
            QUINT_ASSERT(M)
            global_optimize_module_cpu(M.get());
            std::lock_guard<std::mutex> _(mut_);
            auto dylib_expect = es_.createJITDylib(fmt::format("{}", module_counter_));
            QUINT_ASSERT(dylib_expect)
            auto &dylib = dylib_expect.get();
            dylib.addGenerator(
                    cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
                            dl_.getGlobalPrefix())));
            auto *thread_safe_context =
                    this->ctx_->get_this_thread_safe_context();
            cantFail(compile_layer_.add(
                    dylib,
                    llvm::orc::ThreadSafeModule(std::move(M), *thread_safe_context)));
            all_libs_.push_back(&dylib);
            auto new_module = std::make_unique<JITModuleCPU>(this, &dylib);
            auto new_module_raw_ptr = new_module.get();
            modules.push_back(std::move(new_module));
            module_counter_++;
            return new_module_raw_ptr;
        }

        void *lookup(const std::string name) override {
            std::lock_guard<std::mutex> _(mut_);
#ifdef __APPLE__
#else
            auto symbol = es_.lookup(all_libs_, es_.intern(name));
#endif
            if (!symbol)
                QUINT_ERROR("Function \"{}\" not found", name)
            return (void *)(symbol->getAddress());
        }

        DataLayout get_data_layout() override {
            return dl_;
        }

        void global_optimize_module(llvm::Module *module) override {
            global_optimize_module_cpu(module);
        }

        void *lookup_in_module(JITDylib *lib, const std::string name) {
            std::lock_guard<std::mutex> _(mut_);
#ifdef __APPLE__
            auto symbol = es_.lookup({lib}, mangle_(Name));
#else
            auto symbol = es_.lookup({lib}, es_.intern(name));
#endif
            if (!symbol)
                QUINT_ERROR("Function \"{}\" not found", name);
            return (void *)(symbol->getAddress());
        }

    private:
        void global_optimize_module_cpu(llvm::Module *module);
    };

    void *JITModuleCPU::lookup_function(const std::string &name) {
        return session_->lookup_in_module(dylib_, name);
    }

    void JITSessionCPU::global_optimize_module_cpu(llvm::Module *module) {
        QUINT_AUTO_PROF
        if (llvm::verifyModule(*module, &llvm::errs())) {
            module->print(llvm::errs(), nullptr);
            QUINT_ERROR("Module broken")
        }

        auto triple = get_host_target_info().first.getTargetTriple();

        std::string err_str;
        const llvm::Target *target = TargetRegistry::lookupTarget(triple.str(), err_str);
        QUINT_ERROR_UNLESS(target, err_str)

        TargetOptions options;
        if (this->config_->fast_math) {
            options.AllowFPOpFusion = FPOpFusion::Fast;
            options.UnsafeFPMath = 1;
            options.NoInfsFPMath = 1;
            options.NoNaNsFPMath = 1;
        } else {
            options.AllowFPOpFusion = FPOpFusion::Strict;
            options.UnsafeFPMath = 0;
            options.NoInfsFPMath = 0;
            options.NoNaNsFPMath = 0;
        }
        options.HonorSignDependentRoundingFPMathOption = false;
        options.NoZerosInBSS = false;
        options.GuaranteedTailCallOpt = false;

        legacy::FunctionPassManager function_pass_manager(module);
        legacy::PassManager module_pass_manager;

        llvm::StringRef mcpu = llvm::sys::getHostCPUName();
        std::unique_ptr<TargetMachine> target_machine(target->createTargetMachine(
                triple.str(), mcpu.str(), "", options, llvm::Reloc::PIC_,
                llvm::CodeModel::Small, CodeGenOpt::Aggressive));

        QUINT_ERROR_UNLESS(target_machine.get(), "Could not allocate target machine!")

        module->setDataLayout(target_machine->createDataLayout());

        module_pass_manager.add(createTargetTransformInfoWrapperPass(
                target_machine->getTargetIRAnalysis()));
        function_pass_manager.add(createTargetTransformInfoWrapperPass(
                target_machine->getTargetIRAnalysis()));

        PassManagerBuilder b;
        b.OptLevel = 3;
        b.Inliner = createFunctionInliningPass(b.OptLevel, 0, false);
        b.LoopVectorize = true;
        b.SLPVectorize = true;

        target_machine->adjustPassManager(b);

        b.populateFunctionPassManager(function_pass_manager);
        b.populateModulePassManager(module_pass_manager);

        {
            QUINT_PROFILER("llvm_function_pass")
            function_pass_manager.doInitialization();
            for (llvm::Module::iterator i = module->begin(); i != module->end(); i++)
                function_pass_manager.run(*i);

            function_pass_manager.doFinalization();
        }

        module_pass_manager.add(llvm::createLoopStrengthReducePass());
        module_pass_manager.add(llvm::createIndVarSimplifyPass());
        module_pass_manager.add(llvm::createSeparateConstOffsetFromGEPPass(false));
        module_pass_manager.add(llvm::createEarlyCSEPass(true));

        {
            QUINT_PROFILER("llvm_module_pass")
            module_pass_manager.run(*module);
        }

        if (this->config_->print_kernel_llvm_ir_optimized) {
            if (false) {
                QUINT_INFO("Functions with > 100 instructions in optimized LLVM IR:");
                QuintLLVMContext::print_huge_functions(module);
            }

            static FileSequenceWriter writer(
                    "taichi_kernel_cpu_llvm_ir_optimized_{:04d}.ll",
                    "optimized LLVM IR (CPU)"
                    );
            writer.write(module);

        }
    }

    std::unique_ptr<JITSession> create_llvm_jit_session_cpu(QuintLLVMContext *ctx, CompileConfig *config) {
        auto target_info = get_host_target_info();
        auto EPC = SelfExecutorProcessControl::Create();
        QUINT_ASSERT(EPC)
        return std::make_unique<JITSessionCPU>(ctx, std::move(*EPC), config, target_info.first,
                                               target_info.second);
    }

}