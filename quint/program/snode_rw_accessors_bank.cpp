//
// Created by BY210033 on 2023/4/11.
//
#include "quint/program/snode_rw_accessors_bank.h"
#include "quint/program/program.h"

namespace quint::lang {

    SNodeRWAccessorsBank::Accessors::Accessors(const SNode *snode, const SNodeRWAccessorsBank::RwKernels &kernels,
                                               Program *prog)
        : snode_(snode), prog_(prog), reader_(kernels.reader), writer_(kernels.writer) {
        QUINT_ASSERT(reader_ != nullptr)
        QUINT_ASSERT(writer_ != nullptr)
    }

    SNodeRWAccessorsBank::Accessors SNodeRWAccessorsBank::get(SNode *snode) {
        auto &kernels = snode_to_kernels_[snode];
        if (kernels.reader == nullptr) {
            kernels.reader = &(program_->get_snode_reader(snode));
        }
        if (kernels.writer == nullptr) {
            kernels.writer = &(program_->get_snode_writer(snode));
        }
        return Accessors(snode, kernels, program_);
    }

}
