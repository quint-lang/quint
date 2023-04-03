//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_ANALYSIS_H
#define QUINT_ANALYSIS_H

#include "ir/ir.h"
#include "ir/mesh.h"

namespace quint::lang {

    class DiffRange {

    };

    namespace irpass::analysis {
        std::unique_ptr<IRNode> clone(IRNode *root);
    }

}

#endif //QUINT_ANALYSIS_H
