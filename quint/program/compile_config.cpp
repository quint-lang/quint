//
// Created by BY210033 on 2023/3/15.
//
#include "quint/program/compile_config.h"

#include <thread>

namespace quint::lang {

    CompileConfig::CompileConfig() {
        debug = false;
        cfg_optimization = true;
        check_out_of_bound = false;
        opt_level = 1;
        use_llvm = true;
        kernel_profiler = false;
        verbose = true;
        fast_math = true;
        gpu_max_reg = 0;
        packed = true;
        print_ir = false;
        print_preprocessed_ir = false;
        print_accessor_ir = false;
        print_evaluator_ir = false;
        advanced_optimization = true;
        constant_folding = true;
        external_optimization_level = 3;
        demote_dense_struct_for = true;
        detect_read_only = true;
        make_thread_local = true;
        real_matrix_scalarize = true;
        default_cpu_block_dim = 32;
        default_gpu_block_dim = 128;

        saturating_grid_dim = 0;
        max_block_dim = 0;
        cpu_max_num_threads = std::thread::hardware_concurrency();
        random_seed = 0;

        // LLVM backend options
        print_struct_llvm_ir = false;
        print_kernel_llvm_ir = false;
        print_kernel_nvptx = false;
        print_kernel_llvm_ir_optimized = false;
    }

}