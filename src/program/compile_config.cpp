//
// Created by BY210033 on 2023/3/15.
//
#include "program/compile_config.h"

#include <thread>

namespace quint::lang {

    CompileConfig::CompileConfig() {
        debug = false;
        opt_level = 1;
        use_llvm = true;
        kernel_profiler = false;
        verbose = true;
        gpu_max_reg = 0;

        cpu_max_num_threads = std::thread::hardware_concurrency();
        random_seed = 0;
    }

}