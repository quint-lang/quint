//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/control_flow_graph.h"
#include "quint/system/profiler.h"

namespace quint::lang::irpass {

    bool cfg_optimization(IRNode *node,
                          bool after_lower_access,
                          bool autodiff_enabled,
                          bool real_matrix_enabled,
                          const std::optional<ControlFlowGraph::LiveVarAnalysisConfig>
                                  &lva_config_opt) {

        QUINT_AUTO_PROF
        auto cfg = analysis::build_cfg(node);
        bool modified = false;
        if (!real_matrix_enabled) {
            cfg->simplify_graph();
            if (cfg->store_to_load_forwarding(after_lower_access, autodiff_enabled))
                modified = true;
            if (cfg->dead_store_elimination(after_lower_access, lva_config_opt))
                modified = true;
        }
        die(node);  // remove unused allocas
        return modified;
    }

}
