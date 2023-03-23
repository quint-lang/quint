//
// Created by BY210033 on 2023/3/22.
//
#include "util/offline_cache.h"

namespace quint::lang::offline_cache {

    void disable_offline_cache_if_needed(CompileConfig *config) {
        QUINT_ASSERT(config);
        if (config->offline_cache) {
            if (config->print_preprocessed_ir || config->print_ir || config->print_accessor_ir) {
                config->offline_cache = false;
                QUINT_WARN("Disable offline_cache because print_preprocessed_ir or print_or or"
                           "print_accessor_ir is enabled");
            }
        }
    }

    std::string get_cache_path(const std::string &base_path) {
        std::string subdir = kLLVNMCacheSubPath;
        return join_path(base_path, subdir);
    }

}