//
// Created by BY210033 on 2023/3/16.
//

#ifndef QUINT_EXPORT_H
#define QUINT_EXPORT_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif


#include "pybind11/pybind11.h"
#include "pybind11/operators.h"
#include "pybind11/stl.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "common/core.h"

namespace quint {

    namespace py = pybind11;

    void export_lang(py::module &m);

    void export_misc(py::module &m);

}

#endif //QUINT_EXPORT_H
