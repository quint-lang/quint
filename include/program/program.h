//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_PROGRAM_H
#define QUINT_PROGRAM_H

#include <functional>
#include <optional>
#include <atomic>
#include <stack>
#include <shared_mutex>

#define QUINT_RUNTIME_HOST
#include "program/callable.h"
#include "program/kernel.h"
#include "program/compile_config.h"
#include "program/kernel_profiler.h"
#include "program/program_impl.h"
#include "ir/frontend_ir.h"
#include "program/context.h"
#include "ir/ir.h"

namespace quint::lang {

    class Program {
    public:
        using Kernel = quint::lang::Kernel;
        Callable *current_callable{nullptr};

        std::unordered_map<std::thread::id, CompileConfig> configs;
        std::thread::id main_thread_id_;
        bool sync{false};

        uint64 *result_buffer{nullptr}; // Note result_buffer is used by all backends

        std::vector<std::unique_ptr<Kernel>> kernels;

        std::unique_ptr<KernelProfilerBase> profiler{nullptr};

        Program();

        ~Program();

        CompileConfig &this_thread_config() {
            std::shared_lock<std::shared_mutex> read_lock(config_map_mut);
            auto thread_id = std::this_thread::get_id();
            if (!configs.count(thread_id)) {
                read_lock.unlock();
                std::unique_lock<std::shared_mutex> write_lock(config_map_mut);
                configs[thread_id] = configs[main_thread_id_];
                return configs[thread_id];
            }
            return configs[thread_id];
        }

        const CompileConfig &config() {
            return configs[main_thread_id_];
        }

        void materialize_runtime();

        int get_snode_tree_size();

        void visualize_layout(const std::string &fn);

        Kernel &kernel(const std::function<void(Kernel *)> &body,
                       const std::string &name = "") {
            // Expr::set_allow_store(true);
            auto func = std::make_unique<Kernel>(*this, body, name);
            kernels.emplace_back(std::move(func));
            return *kernels.back();
        }

        FunctionType compile(Kernel *kernel);

        void finalize();

        void synchronize();

        void prepare_runtime_context(RuntimeContext *ctx);

        SNode *get_snode_root(int tree_id);

        ASTBuilder *current_ast_builder() {
            return current_callable ? &current_callable->context->builder() : nullptr;
        }

    private:
        int global_id_counter_{0};

        std::stack<int> free_snode_tree_ids_;

        std::unique_ptr<ProgramImpl> program_impl_;
        float64 total_compilation_time_{0.0};
        static std::atomic<int> num_instances_;
        bool finalized_{false};

        std::shared_mutex config_map_mut;
    };
}

#endif //QUINT_PROGRAM_H
