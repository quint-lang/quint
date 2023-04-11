//
// Created by BY210033 on 2023/4/11.
//
#include "quint/transforms/make_block_local.h"
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/statements.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    void make_block_local_offload(OffloadedStmt *offload,
                                  const CompileConfig &config,
                                  const std::string &kernel_name) {

    }

    const PassID MakeBlockLocalPass::id = "MakeBlockLocalPass";

    namespace irpass {
        void make_block_local(IRNode *root, const CompileConfig &config, const MakeBlockLocalPass::Args &args) {
            QUINT_AUTO_PROF
            if (auto root_block = root->cast<Block>()) {
                for (auto &offload : root_block->statements) {
                    make_block_local_offload(offload->cast<OffloadedStmt>(), config, args.kernel_name);
                }
            } else {
                make_block_local_offload(root->cast<OffloadedStmt>(), config, args.kernel_name);
            }
            type_check(root, config);
        }
    }

}