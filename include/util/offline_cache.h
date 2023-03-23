//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_OFFLINE_CACHE_H
#define QUINT_OFFLINE_CACHE_H

#include <ctime>
#include <cstdint>
#include <queue>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "common/core.h"
#include "util/io.h"
#include "program/compile_config.h"

namespace quint::lang {
    namespace offline_cache {

        constexpr char kLLVMCacheFilenameLLExt[] = "ll";
        constexpr char kLLVMCacheFilenameBCExt[] = "bc";
        constexpr char kLLVNMCacheSubPath[] = "llvm";

        using Version = std::uint16_t[3]; // {MAJOR, MINOR, PATCH}

        enum CleanCacheFlags {
            NotClean = 0b000,
            CleanOldVersion = 0b001,
            CleanOldUsed = 0b010,
            CleanOldCreated = 0b100
        };


        void disable_offline_cache_if_needed(CompileConfig *config);
        std::string get_cache_path(const std::string &base_path);
    }
}

#endif //QUINT_OFFLINE_CACHE_H
