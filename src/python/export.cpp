//
// Created by BY210033 on 2023/3/16.
//
#include "python/export.h"

namespace quint {
    PYBIND11_MODULE(quint_python, m) {
        m.doc() = "quint_python";

//        for (auto &kv : ) {
//
//        }
        export_lang(m);
        export_misc(m);
    }
}
