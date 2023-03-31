//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_LLVM_CONTEXT_H
#define QUINT_LLVM_CONTEXT_H

#include <mutex>
#include <functional>
#include <thread>

#include "ir/snode.h"
#include "codegen/llvm/llvm_compiled_data.h"
#include "runtime/llvm/llvm_fwd.h"
#include "jit/jit_session.h"
#include "util/lang_util.h"

namespace quint::lang {

    class JITSessionCPU;
    class LLVMProgramImpl;

    class QuintLLVMContext {
    private:
        struct ThreadLocalData {
            std::unique_ptr<llvm::orc::ThreadSafeContext> thread_safe_llvm_context {nullptr};
            llvm::LLVMContext *llvm_context{nullptr};
            std::unique_ptr<llvm::Module> runtime_module{nullptr};
            std::unordered_map<int, std::unique_ptr<llvm::Module>> struct_modules;
            explicit ThreadLocalData(std::unique_ptr<llvm::orc::ThreadSafeContext> ctx);
            ~ThreadLocalData();
        };
        CompileConfig *config_;

    public:
        std::unique_ptr<JITSession> jit{nullptr};

        JITModule *runtime_jit_module{nullptr};

        std::unique_ptr<ThreadLocalData> linking_context_data{nullptr};

        QuintLLVMContext(CompileConfig *config);

        virtual ~QuintLLVMContext();

        llvm::LLVMContext *get_this_thread_context();

        llvm::orc::ThreadSafeContext *get_this_thread_safe_context();

        static void print_huge_functions(llvm::Module *module);

        void init_runtime_jit_module();

        JITModule *create_jit_module(std::unique_ptr<llvm::Module> module);

        void add_struct_module(std::unique_ptr<llvm::Module> module, int tree_id);

        static void eliminate_unused_functions(
                llvm::Module *module,
                std::function<bool(const std::string &)> export_indicator);

        llvm::Function *get_runtime_function(const std::string &name);
        llvm::Type *get_runtime_type(const std::string &name);

        std::unique_ptr<llvm::Module> new_module(std::string name, llvm::LLVMContext *context = nullptr);

        llvm::Type *get_data_type(DataType dt);

        llvm::Module *get_this_thread_runtime_module();

    private:
        std::unique_ptr<llvm::Module> clone_module_to_context(llvm::Module *module, llvm::LLVMContext *target_context);

        ThreadLocalData *get_this_thread_data();

        std::thread::id main_thread_id_;
        ThreadLocalData *main_thread_data_{nullptr};
        std::mutex mut_;
        std::mutex thread_map_mut_;

        std::unordered_map<int, std::vector<std::string>> snode_tree_funcs_;
    };

}

#endif //QUINT_LLVM_CONTEXT_H
