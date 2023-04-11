//
// Created by BY210033 on 2023/4/11.
//
#include <functional>
#include <iterator>
#include <type_traits>

#include "quint/analysis/analysis.h"
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"
#include "quint/ir/statements.h"

namespace quint::lang {

    void make_thread_local_offload(OffloadedStmt *offload) {

    }

    namespace irpass {
        void make_thread_local(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            if (auto root_block = root->cast<Block>()) {
                for (auto &offload : root_block->statements) {
                    make_thread_local_offload(offload->cast<OffloadedStmt>());
                }
            } else {
                make_thread_local_offload(root->cast<OffloadedStmt>());
            }
            type_check(root, config);
        }
    }

}
