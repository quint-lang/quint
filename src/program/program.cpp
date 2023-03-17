//
// Created by BY210033 on 2023/3/10.
//
#include "program/program.h"

namespace quint::lang {

    std::atomic<int> Program::num_instances_;

    Program::~Program() {
        finalize();
    }

    void Program::materialize_runtime() {

    }

    int Program::get_snode_tree_size() {
        return 1;
    }

    void Program::visualize_layout(const std::string &fn) {

    }

    FunctionType Program::compile(Kernel *kernel) {
        return quint::lang::FunctionType();
    }

    void Program::finalize() {
        if (finalized_)
            return;
        synchronize();
        QUINT_ASSERT(std::this_thread::get_id() == main_thread_id_)
        QUINT_TRACE("Program finalizing...");

        synchronize();
        // todo handler memory_pool
    }

    void Program::synchronize() {

    }

    void Program::prepare_runtime_context(RuntimeContext *ctx) {
        ctx->result_buffer = result_buffer;
        program_impl_->prepare_runtime_context(ctx);
    }

    SNode *Program::get_snode_root(int tree_id) {
        return nullptr;
    }

}