//
// Created by BY210033 on 2023/3/17.
//
#include "python/export.h"
#include "common/core.h"
#include "program/py_print_buffer.h"

namespace quint {

    void export_misc(py::module &m) {
        m.def("get_python_package_dir", get_python_package_dir);
        m.def("set_python_package_dir", set_python_package_dir);

        m.def("pop_python_print_buffer", []() { return py_cout.pop_content(); });
        m.def("toggle_python_print_buffer", [](bool opt) { py_cout.enabled = opt; });
    }

}