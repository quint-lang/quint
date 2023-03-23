//
// Created by BY210033 on 2023/3/15.
//
#include "program/compile_config.h"

#include <thread>

namespace quint::lang {

    CompileConfig::CompileConfig() {
        debug = false;
        check_out_of_bound = false;
        opt_level = 1;
        use_llvm = true;
        kernel_profiler = false;
        verbose = true;
        gpu_max_reg = 0;
        packed = true;
        print_ir = false;
        print_preprocessed_ir = false;
        print_accessor_ir = false;
        print_evaluator_ir = false;

        cpu_max_num_threads = std::thread::hardware_concurrency();
        random_seed = 0;
    }

}