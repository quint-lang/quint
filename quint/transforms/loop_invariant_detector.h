//
// Created by BY210033 on 2023/4/10.
//

#ifndef QUINT_LOOP_INVARIANT_DETECTOR_H
#define QUINT_LOOP_INVARIANT_DETECTOR_H

#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/system/profiler.h"
#include "quint/ir/visitors.h"

#include <stack>

namespace quint::lang {

    class LoopInvariantDetector : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        std::stack<Block *> loop_blocks;

        const CompileConfig &config;

        explicit LoopInvariantDetector(const CompileConfig &config) : config(config) {
            allow_undefined_visitor = true;
        }

    };

}

#endif //QUINT_LOOP_INVARIANT_DETECTOR_H
