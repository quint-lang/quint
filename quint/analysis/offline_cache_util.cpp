//
// Created by BY210033 on 2023/3/31.
//
#include "quint/analysis/offline_cache_util.h"
#include "quint/common/serialization.h"
#include "quint/program/kernel.h"
#include "quint/program/compile_config.h"

#include "picosha2.h"

#include <vector>

namespace quint::lang {

    static std::vector<std::uint8_t> get_offline_cache_key_of_compile_config(const CompileConfig *config) {
        QUINT_ASSERT(config)
        BinaryOutputSerializer serializer;
        serializer.initialize();
        serializer(config->debug);
        serializer(config->check_out_of_bound);
        serializer(config->opt_level);
        serializer(config->kernel_profiler);
        serializer(config->fast_math);
        serializer(config->gpu_max_reg);
        serializer(config->packed);
        serializer(config->cpu_max_num_threads);
        serializer(config->random_seed);

        return serializer.data;
    }

    std::string get_hashed_offline_cache_key_of_snode(SNode *snode) {
        return std::string();
    }

    std::string get_hashed_offline_cache_key(const CompileConfig *config, Kernel *kernel) {
        std::string kernel_ast_string;
        if (kernel) {
            std::ostringstream oss;
            gen_offline_cache_key(kernel->program, kernel->ir.get(), &oss);
            kernel_ast_string = oss.str();
        }

        std::vector<std::uint8_t> compile_config_key;
        if (config) {
            compile_config_key = get_offline_cache_key_of_compile_config(config);
        }

        picosha2::hash256_one_by_one hasher;
        hasher.process(compile_config_key.begin(), compile_config_key.end());
        hasher.finish();

        return picosha2::get_hash_hex_string(hasher);
    }

}