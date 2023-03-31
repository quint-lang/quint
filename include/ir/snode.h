//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_SNODE_H
#define QUINT_SNODE_H

#include "ir/expr.h"
#include "ir/type.h"

namespace quint::lang {

    class SNode {
    public:

        // This class decouples SNode from the frontend expression.
        class GradInfoProvider {

        };

        std::string name;

        std::string get_node_type_name_hinted() const;
    };

}

#endif //QUINT_SNODE_H
