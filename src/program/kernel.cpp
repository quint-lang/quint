//
// Created by BY210033 on 2023/3/10.
//
#include "program/kernel.h"
#include "program/program.h"

#ifdef QUINT_WITH_LLVM

#endif

namespace quint::lang {
    Kernel::LaunchContextBuilder::LaunchContextBuilder(Kernel *kernel, RuntimeContext *ctx) {

    }

    Kernel::LaunchContextBuilder::LaunchContextBuilder(Kernel *kernel) {

    }

    void Kernel::LaunchContextBuilder::set_arg_float(int arg_id, float64 d) {

    }

    void Kernel::LaunchContextBuilder::set_arg_int(int arg_id, int64 d) {

    }

    void Kernel::LaunchContextBuilder::set_arg_uint(int arg_id, uint64 d) {

    }

    void Kernel::LaunchContextBuilder::set_extra_arg_int(int i, int j, int32 d) {

    }

    void Kernel::LaunchContextBuilder::set_arg_external_array_with_shape(int arg_id, uintptr_t ptr, uint64 size,
                                                                         const std::vector<int64> &shape) {

    }

    void Kernel::LaunchContextBuilder::set_arg_raw(int arg_id, uint64 d) {

    }

    RuntimeContext &Kernel::LaunchContextBuilder::get_context() {
        kernel_->program->prepare_runtime_context(ctx_);
        return *ctx_;
    }

    Kernel::Kernel(Program &program, const std::function<void()> &func, const std::string &name) {
        this->init(program, func, name);
    }

    Kernel::Kernel(Program &program, const std::function<void(Kernel *)> &func, const std::string &name) {
        this->init(program,
                   [&] { return func(this); }, name);
    }

    Kernel::Kernel(Program &program, std::unique_ptr<IRNode> &&ir, const std::string &name) {
        this->ir = std::move(ir);
        this->program = &program;
        is_accessor = false;
        is_evaluator = false;
        compiled_ = nullptr;
        ir_is_ast_ = false;
    }

    void Kernel::compile() {

    }

    void Kernel::lower(bool to_executable) {

    }

    void Kernel::operator()(Kernel::LaunchContextBuilder &ctx_builder) {

    }

    Kernel::LaunchContextBuilder Kernel::make_launch_context() {
        return Kernel::LaunchContextBuilder(nullptr, nullptr);
    }

    template<typename T>
    T Kernel::fetch_ret(DataType dt, int i) {
        return nullptr;
    }

    float64 Kernel::get_ret_float(int i) {
        return 0;
    }

    int64 Kernel::get_ret_int(int i) {
        return 0;
    }

    uint64 Kernel::get_ret_uint(int i) {
        return 0;
    }

    std::vector<int64> Kernel::get_ret_int_tensor(int i) {
        return std::vector<int64>();
    }

    std::vector<uint64> Kernel::get_ret_uint_tensor(int i) {
        return std::vector<uint64>();
    }

    std::vector<float64> Kernel::get_ret_float_tensor(int i) {
        return std::vector<float64>();
    }

    std::string Kernel::get_name() const {
        return std::string();
    }

    void Kernel::offload_to_executable(IRNode *stmt) {

    }

    void Kernel::init(Program &program, const std::function<void()> &func, const std::string &name) {
        this->lowered_ = false;
        this->program = &program;

        is_accessor = false;
        is_evaluator = false;
        compiled_ = nullptr;
        context = std::make_unique<FrontendContext>();
        ir = context->get_root();
        ir_is_ast_ = true;

        {
            CurrentCallableGuard _(this->program, this);
            func();
        }

    }

}