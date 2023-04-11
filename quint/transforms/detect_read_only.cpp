//
// Created by BY210033 on 2023/4/6.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/visitors.h"
#include "quint/ir/transforms.h"

namespace quint::lang::irpass {


    void detect_read_only_in_task(OffloadedStmt *offload) {
        auto accessed = irpass::analysis::gather_snode_read_writes(offload);
        for (auto snode : accessed.first) {
            if (accessed.second.count(snode) == 0) {
                offload->mem_access_opt.add_flag(snode, SNodeAccessFlag::read_only);
            }
        }
    }

    void detect_read_only(IRNode *root) {
        if (root->is<Block>()) {
            for (auto &offload : root->as<Block>()->statements) {
                detect_read_only_in_task(offload->as<OffloadedStmt>());
            }
        } else {
            detect_read_only_in_task(root->as<OffloadedStmt>());
        }
    }

}