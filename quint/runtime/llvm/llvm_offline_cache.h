//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_OFFLINE_CACHE_H
#define QUINT_LLVM_OFFLINE_CACHE_H

#include <memory>

#ifdef QUINT_WITH_LLVM
#include "llvm/IR/Module.h"

#include "quint/common/core.h"
#include "quint/program/kernel.h"
#include "quint/util/offline_cache.h"
#include "quint/runtime/llvm/launch_arg_info.h"
#include "quint/codegen/llvm/llvm_compiled_data.h"

namespace quint::lang {

    struct LLVMOfflineCache {
        using Version = uint16[3];

        enum Format {
            LL = 0x01,
            BC = 0X10,
        };

        struct KernelCacheData {
            std::string kernel_key;
            std::vector<LLVMLaunchArgInfo> args;
            LLVMCompiledKernel compiled_data;

            // For cache cleaning
            std::size_t size{0};          // byte
            std::time_t created_at{0};    // millsec
            std::time_t last_used_at{0};  // millsec

            KernelCacheData() = default;
            KernelCacheData(KernelCacheData &&) = default;
            KernelCacheData &operator=(KernelCacheData &&) = default;
            ~KernelCacheData() = default;

            KernelCacheData clone() const;
        };

        std::unordered_map<std::string, KernelCacheData> kernels;
    };

    class LLVMOfflineCacheFileReader {
    public:
        static std::unique_ptr<LLVMOfflineCacheFileReader> make(
                const std::string &path,
                LLVMOfflineCache::Format format = LLVMOfflineCache::Format::LL);

    private:
        LLVMOfflineCacheFileReader(const std::string &path,
                                   LLVMOfflineCache &&data,
                                   LLVMOfflineCache::Format format);

        std::string path_;
        LLVMOfflineCache data_;
        LLVMOfflineCache::Format format_;
    };

}

#endif

#endif //QUINT_LLVM_OFFLINE_CACHE_H
