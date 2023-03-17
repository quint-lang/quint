//
// Created by BY210033 on 2023/3/16.
//
#include <optional>
#include <string>

#if QUINT_WITH_LLVM
#include "llvm/Config/llvm-config.h"
#endif

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/eigen.h"
#include "pybind11/numpy.h"

#include "ir/frontend_ir.h"
#include "python/export.h"
#include "program/program.h"

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
        py::class_<ASTBuilder>(m, "ASTBuilder");

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
                py::return_value_policy::reference);

        py::class_<Kernel>(m, "Kernel")
            .def("get_ret_int", &Kernel::get_ret_int)
            .def("get_ret_uint", &Kernel::get_ret_uint)
            .def("get_ret_float", &Kernel::get_ret_float)
            .def("make_launch_context", &Kernel::make_launch_context)
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

        py::class_<SNode>(m, "SNode")
            .def(py::init<>());

        py::class_<Type>(m, "Type").def("to_string", &Type::to_string);

        py::class_<Stmt>(m, "Stmt");
    }

}