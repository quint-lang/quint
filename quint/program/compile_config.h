//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_COMPILE_CONFIG_H
#define QUINT_COMPILE_CONFIG_H

#include "quint/common/core.h"

namespace quint::lang {

    struct CompileConfig {
        bool debug;
        bool cfg_optimization;
        bool check_out_of_bound;
        int opt_level;
        bool use_llvm;
        int external_optimization_level;
        bool kernel_profiler;
        bool verbose;
        bool fast_math;
        int gpu_max_reg;
        bool packed;
        bool print_preprocessed_ir;
        bool print_ir;
        bool print_accessor_ir;
        bool print_evaluator_ir;
        bool advanced_optimization;
        bool constant_folding;
        bool demote_dense_struct_for;
        bool detect_read_only;
        bool make_thread_local;
        bool real_matrix_scalarize;
        int default_cpu_block_dim;
        int default_gpu_block_dim;

        int saturating_grid_dim;
        int max_block_dim;
        int cpu_max_num_threads;
        int random_seed;

        bool cache_loop_invariant_global_vars{true};

        bool timeline{false};
        int num_compile_threads{4};
        int default_ad_stack_size{32};
        size_t cuda_stack_limit{8192};

        // LLVM backend options
        bool print_struct_llvm_ir;
        bool print_kernel_llvm_ir;
        bool print_kernel_llvm_ir_optimized;
        bool print_kernel_nvptx;

        // Offline cache options
        bool offline_cache{false};
        std::string offline_cache_file_path{get_repo_dir() + "qucache"};
        std::string offline_cache_cleaning_policy {"lru"}; // "never"|"version"|"lru"|"info"
        int offline_cache_max_size_of_files{100 * 1024 * 1024};
        double offline_cache_cleaning_factor{0.25};

        CompileConfig();
    };

    extern CompileConfig default_compile_config;

}

#endif //QUINT_COMPILE_CONFIG_H
