//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_LLVM_COMPILED_DATA_H
#define QUINT_LLVM_COMPILED_DATA_H

#include <memory>
#include <unordered_set>

#include "llvm/IR/Module.h"
#include "common/serialization.h"

namespace quint::lang {

    class OffloadedTask {
    public:
        std::string name;
        int block_dim{0};
        int grid_dim{0};

        explicit OffloadedTask(const std::string &name = "",
                               int block_dim = 0,
                               int grid_dim = 0)
            : name(name), block_dim(block_dim), grid_dim(grid_dim) {
        };

//        QUINT_IO_DEF(name, block_dim, grid_dim);
    };

    struct LLVMCompiledTask {
        std::vector<OffloadedTask> tasks;
        std::unique_ptr<llvm::Module> module{nullptr};
        std::unordered_set<int> used_tree_ids;
        std::unordered_set<int> struct_for_tls_sizes;
        LLVMCompiledTask() = default;
        LLVMCompiledTask(LLVMCompiledTask &&) = default;
        LLVMCompiledTask &operator=(LLVMCompiledTask &&) = default;
        LLVMCompiledTask(std::vector<OffloadedTask> tasks,
                         std::unique_ptr<llvm::Module> module,
                         std::unordered_set<int> used_tree_ids,
                         std::unordered_set<int> struct_for_tls_sizes)
                : tasks(std::move(tasks)),
                  module(std::move(module)),
                  used_tree_ids(std::move(used_tree_ids)),
                  struct_for_tls_sizes(std::move(struct_for_tls_sizes)) {
        }
        LLVMCompiledTask clone() const;
    };

    struct LLVMCompiledKernel {
        std::vector<OffloadedTask> tasks;
        std::unique_ptr<llvm::Module> module{nullptr};
        LLVMCompiledKernel() = default;
        LLVMCompiledKernel(LLVMCompiledKernel &&) = default;
        LLVMCompiledKernel &operator=(LLVMCompiledKernel &&) = default;
        LLVMCompiledKernel(std::vector<OffloadedTask> tasks,
                           std::unique_ptr<llvm::Module> module)
                : tasks(std::move(tasks)), module(std::move(module)) {
        }
        LLVMCompiledKernel clone() const;
    };
}

#endif //QUINT_LLVM_COMPILED_DATA_H