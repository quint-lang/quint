//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_COMPILE_CONFIG_H
#define QUINT_COMPILE_CONFIG_H

namespace quint::lang {

    struct CompileConfig {
        bool debug;
        int opt_level;
        bool use_llvm;
        bool kernel_profiler;
        bool verbose;
        int gpu_max_reg;

        int cpu_max_num_threads;
        int random_seed;

        int num_compile_threads{4};
        size_t cuda_stack_limit{8192};

        CompileConfig();
    };

    extern CompileConfig default_compile_config;

}

#endif //QUINT_COMPILE_CONFIG_H
