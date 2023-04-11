//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_SNODE_RW_ACCESSORS_BANK_H
#define QUINT_SNODE_RW_ACCESSORS_BANK_H

#include <unordered_map>

#include "quint/ir/snode.h"
#include "quint/program/kernel.h"

namespace quint::lang {

    class Program;

    class SNodeRWAccessorsBank {
    private:
        struct RwKernels {
            Kernel *reader{nullptr};
            Kernel *writer{nullptr};
        };

    public:
        class Accessors {
        private:
            const SNode *snode_;
            Program *prog_;
            Kernel *reader_;
            Kernel *writer_;

        public:
            explicit Accessors(const SNode *snode,
                               const RwKernels &kernels,
                               Program *prog);

        };

        explicit SNodeRWAccessorsBank(Program *program) : program_(program) {
        }

        Accessors get(SNode *snode);

    private:
        Program *const program_;
        std::unordered_map<const SNode *, RwKernels> snode_to_kernels_;
    };

}


#endif //QUINT_SNODE_RW_ACCESSORS_BANK_H
