//
// Created by BY210033 on 2023/3/16.
//
#include "quint/util/lang_util.h"
#include "quint/program/program.h"
#include "quint/program/compile_config.h"

namespace quint::lang {
    CompileConfig default_compile_config;
    std::string compiled_lib_dir;

    std::string runtime_lib_dir() {
        std::string folder;
        if (!compiled_lib_dir.empty()) {
            folder = compiled_lib_dir;
        } else {
            auto quint_lib_dir = getenv("QUINT_LIB_DIR");
            QUINT_ERROR_IF(!quint_lib_dir,
                           "If you are running the quint_cpp_tests please set $QUINT_LIB_DIR "
                           "to $QUINT_INSTALL_DIR/_lib/runtime. $QUINT_INSTALL_DIR can be "
                           "retrieved from quint.__path__[0] in python. You can also use this "
                           "script to find out $TI_LIB_DIR:\n\n"
                           "python -c \"import os; import quint as qu; p = "
                           "os.path.join(qu.__path__[0], '_lib', 'runtime'); print(p)\"")
            folder = std::string(quint_lib_dir);
        }
        return folder;
    }

    void set_lib_dir(const std::string &dir) {
        compiled_lib_dir = dir;
    }

}