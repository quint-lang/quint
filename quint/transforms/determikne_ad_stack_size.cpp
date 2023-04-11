//
// Created by BY210033 on 2023/4/11.
//
#include "quint/analysis/analysis.h"
#include "quint/ir/control_flow_graph.h"
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"

namespace quint::lang::irpass {

    bool determine_ad_stack_size(IRNode *root, const CompileConfig &config) {
        if (irpass::analysis::gather_statements(root, [&](Stmt *s) {
            if (auto ad_stack = s->cast<AdStackAllocaStmt>()) {
                return ad_stack->max_size == 0;  // adaptive
            }
            return false;
        }).empty()) {
            return false;  // no AD-stacks with adaptive size
        }
        auto cfg = analysis::build_cfg(root);
        cfg->simplify_graph();
        cfg->determine_ad_stack_size(config.default_ad_stack_size);
        return true;
    }

}
