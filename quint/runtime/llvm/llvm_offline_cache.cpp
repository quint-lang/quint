//
// Created by BY210033 on 2023/3/22.
//
#include "quint/runtime/llvm/llvm_offline_cache.h"

namespace quint::lang {

    LLVMOfflineCache::KernelCacheData LLVMOfflineCache::KernelCacheData::clone()
    const {
        LLVMOfflineCache::KernelCacheData result;
        result.kernel_key = kernel_key;
        result.args = args;
        result.compiled_data = compiled_data.clone();
        result.size = size;
        result.created_at = created_at;
        result.last_used_at = last_used_at;
        return result;
    }

    std::unique_ptr<LLVMOfflineCacheFileReader>
    LLVMOfflineCacheFileReader::make(const std::string &path, LLVMOfflineCache::Format format) {
        LLVMOfflineCache data;
        // todo load_meta_data;
        return std::unique_ptr<LLVMOfflineCacheFileReader>(new LLVMOfflineCacheFileReader(path, std::move(data), format));
    }

    LLVMOfflineCacheFileReader::LLVMOfflineCacheFileReader(const std::string &path, LLVMOfflineCache &&data,
                                                           LLVMOfflineCache::Format format)
        : path_(path), data_(std::move(data)), format_(format) {
    }

}