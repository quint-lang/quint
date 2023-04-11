//
// Created by BY210033 on 2023/3/16.
//
#include <optional>
#include <string>
#include "quint/ir/snode.h"

#if QUINT_WITH_LLVM
#include "llvm/Config/llvm-config.h"
#endif

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/eigen.h"
#include "pybind11/numpy.h"

#include "quint/ir/frontend_ir.h"
#include "quint/python/export.h"
#include "quint/program/program.h"
#include "quint/ir/expression_ops.h"

namespace quint {

    void export_lang(py::module &m) {
        using namespace quint::lang;

        py::register_exception<QuintTypeError>(m, "QuintTypeError",
                                             PyExc_TypeError);
        py::register_exception<QuintSyntaxError>(m, "QuintSyntaxError",
                                               PyExc_SyntaxError);
        py::register_exception<QuintIndexError>(m, "QuintIndexError",
                                              PyExc_IndexError);
        py::register_exception<QuintRuntimeError>(m, "QuintRuntimeError",
                                                PyExc_RuntimeError);
        py::register_exception<QuintAssertionError>(m, "QuintAssertionError",
                                                  PyExc_AssertionError);

        py::class_<DataType>(m, "DataType")
            .def(py::init<Type *>())
            .def(py::self == py::self)
            .def("__hash__", &DataType::hash)
            .def("to_string", &DataType::to_string)
            .def("__str__", &DataType::to_string)
            .def("shape", &DataType::get_shape)
            .def("element_type", &DataType::get_element_type)
            .def("get_ptr", [](DataType *dtype) -> Type * { return *dtype; },
                 py::return_value_policy::reference)
            .def(py::pickle(
                [](const DataType &dt) {
                    // Note: this only works for primitive types, which is fine for now.
                    auto primitive =
                            dynamic_cast<const PrimitiveType *>((const Type *)  (dt));
                    QUINT_ASSERT(primitive)
                    return py::make_tuple((std::size_t)primitive->type);
                },
                [](py::tuple t) {
                    if (t.size() != 1)
                        throw std::runtime_error("Invalid state!");

                    DataType dt = PrimitiveType::get((PrimitiveTypeID)(t[0].cast<std::size_t>()));
                    return dt;
                }));

        py::class_<CompileConfig>(m, "CompileConfig")
            .def(py::init<>())
            .def_readwrite("debug", &CompileConfig::debug)
            .def_readwrite("opt_level", &CompileConfig::opt_level)
            .def_readwrite("use_llvm", &CompileConfig::use_llvm)
            .def_readwrite("kernel_profiler", &CompileConfig::kernel_profiler)
            .def_readwrite("verbose", &CompileConfig::verbose)
            .def_readwrite("gpu_max_reg", &CompileConfig::gpu_max_reg)
            .def_readwrite("cpu_max_num_threads", &CompileConfig::cpu_max_num_threads)
            .def_readwrite("random_seed", &CompileConfig::random_seed)
            .def_readwrite("num_compile_threads", &CompileConfig::num_compile_threads)
            .def_readwrite("cuda_stack_limit", &CompileConfig::cuda_stack_limit);

        m.def("reset_default_compile_config",
              [&]() { default_compile_config = CompileConfig(); });

        m.def("default_compile_config",
              [&]() -> CompileConfig & { return default_compile_config; },
              py::return_value_policy::reference);

        // todo finish ASTBuilder
        py::class_<ASTBuilder>(m, "ASTBuilder")
            .def("create_print", &ASTBuilder::create_print)
            .def("begin_frontend_if", &ASTBuilder::begin_frontend_if)
            .def("begin_frontend_if_true", &ASTBuilder::begin_frontend_if_true)
            .def("begin_frontend_if_false", &ASTBuilder::begin_frontend_if_false)
            .def("pop_scope", &ASTBuilder::pop_scope)
            .def("expr_var", &ASTBuilder::make_var);

        py::class_<Program>(m, "Program")
            .def(py::init<>())
            .def("config", &Program::config)
            .def("finalize", &Program::finalize)
            .def("synchronize", &Program::synchronize)
            .def("get_snode_root", &Program::get_snode_root,
                 py::return_value_policy::reference)
            .def("current_ast_builder", &Program::current_ast_builder,
                 py::return_value_policy::reference)
            .def("create_kernel",
                 [](Program *program, const std::function<void(Kernel *)> &body,
                    const std::string &name) -> Kernel * {
                    py::gil_scoped_release release;
                     return &program->kernel(body, name);
                },
                py::return_value_policy::reference)
            .def("decl_scalar_arg",
                 [&](Program *program, const DataType &dt) {
                     return program->current_callable->insert_scalar_arg(dt);
            });

        py::class_<Kernel>(m, "Kernel")
            .def("get_ret_int", &Kernel::get_ret_int)
            .def("get_ret_uint", &Kernel::get_ret_uint)
            .def("get_ret_float", &Kernel::get_ret_float)
            .def("make_launch_context", &Kernel::make_launch_context)
            .def("compile", &Kernel::compile)
            .def("ast_builder",
                 [](Kernel *self) -> ASTBuilder * {
                     return &self->context->builder();
                },
                py::return_value_policy::reference)
            .def("__call__",
                 [](Kernel *kernel, Kernel::LaunchContextBuilder &launch_ctx) {
                    py::gil_scoped_release release;
                    kernel->operator()(launch_ctx);
            });

        py::class_<Kernel::LaunchContextBuilder>(m, "KernelLaunchContext")
            .def("set_arg_int", &Kernel::LaunchContextBuilder::set_arg_int)
            .def("set_arg_uint", &Kernel::LaunchContextBuilder::set_arg_uint)
            .def("set_arg_float", &Kernel::LaunchContextBuilder::set_arg_float)
            .def("set_arg_external_array_with_shape",
                 &Kernel::LaunchContextBuilder::set_arg_external_array_with_shape)
            .def("set_extra_arg_int", &Kernel::LaunchContextBuilder::set_extra_arg_int);

        py::class_<Expr> expr(m, "Expr");
        expr.def("snode", &Expr::snode, py::return_value_policy::reference)
            .def("set_tb", &Expr::set_tb)
            .def("set_adjoint", &Expr::set_adjoint)
            .def("set_dual", &Expr::set_dual)
            .def("type_check", &Expr::type_check);

        py::class_<SNode>(m, "SNode")
            .def(py::init<>());

        py::class_<Type>(m, "Type").def("to_string", &Type::to_string);

        py::class_<Stmt>(m, "Stmt");

        m.def("value_cast", static_cast<Expr (*)(const Expr &expr, DataType)>(cast));

        m.def("make_arg_load_expr",
              Expr::make<ArgLoadExpression, int, const DataType &, bool>);

        m.def("make_const_expr_int",
              Expr::make<ConstExpression, const DataType &, int64>);

        m.def("make_const_expr_fp",
              Expr::make<ConstExpression, const DataType &, float64>);

        m.def("set_lib_dir", &set_lib_dir);

#define DEFINE_EXPRESSION_OP(x) m.def("expr_" #x, expr_##x);
        DEFINE_EXPRESSION_OP(neg)
        DEFINE_EXPRESSION_OP(sqrt)
        DEFINE_EXPRESSION_OP(round)
        DEFINE_EXPRESSION_OP(floor)
        DEFINE_EXPRESSION_OP(ceil)
        DEFINE_EXPRESSION_OP(abs)
        DEFINE_EXPRESSION_OP(sin)
        DEFINE_EXPRESSION_OP(asin)
        DEFINE_EXPRESSION_OP(cos)
        DEFINE_EXPRESSION_OP(acos)
        DEFINE_EXPRESSION_OP(tan)
        DEFINE_EXPRESSION_OP(tanh)
        DEFINE_EXPRESSION_OP(inv)
        DEFINE_EXPRESSION_OP(rcp)
        DEFINE_EXPRESSION_OP(rsqrt)
        DEFINE_EXPRESSION_OP(exp)
        DEFINE_EXPRESSION_OP(log)

        DEFINE_EXPRESSION_OP(select)
        DEFINE_EXPRESSION_OP(ifte)

        DEFINE_EXPRESSION_OP(cmp_le)
        DEFINE_EXPRESSION_OP(cmp_lt)
        DEFINE_EXPRESSION_OP(cmp_ge)
        DEFINE_EXPRESSION_OP(cmp_gt)
        DEFINE_EXPRESSION_OP(cmp_ne)
        DEFINE_EXPRESSION_OP(cmp_eq)

        DEFINE_EXPRESSION_OP(bit_and)
        DEFINE_EXPRESSION_OP(bit_or)
        DEFINE_EXPRESSION_OP(bit_xor)
        DEFINE_EXPRESSION_OP(bit_shl)
        DEFINE_EXPRESSION_OP(bit_shr)
        DEFINE_EXPRESSION_OP(bit_sar)
        DEFINE_EXPRESSION_OP(bit_not)

        DEFINE_EXPRESSION_OP(logic_not)
        DEFINE_EXPRESSION_OP(logical_and)
        DEFINE_EXPRESSION_OP(logical_or)

        DEFINE_EXPRESSION_OP(add)
        DEFINE_EXPRESSION_OP(sub)
        DEFINE_EXPRESSION_OP(mul)
        DEFINE_EXPRESSION_OP(div)
        DEFINE_EXPRESSION_OP(truediv)
        DEFINE_EXPRESSION_OP(floordiv)
        DEFINE_EXPRESSION_OP(mod)
        DEFINE_EXPRESSION_OP(max)
        DEFINE_EXPRESSION_OP(min)
        DEFINE_EXPRESSION_OP(atan2)
        DEFINE_EXPRESSION_OP(pow)

#undef DEFINE_EXPRESSION_OP


#define PER_TYPE(x)                                                        \
        m.attr(("DataType_" + data_type_name(PrimitiveType::x)).c_str()) = \
            PrimitiveType::x;
#include "quint/inc/data_type.inc.h"

#undef PER_TYPE
    }

}