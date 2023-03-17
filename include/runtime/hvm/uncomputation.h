//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_UNCOMPUTATION_H
#define QUINT_UNCOMPUTATION_H

#include "dag_graph.h"

namespace quint {

    class UnComputation
    {
    public:
        UnComputation() {}

        bool unComputeListNode(DAG& dag, std::vector<std::shared_ptr<Node>> nodes);

        bool unComputeNode(DAG& dag, std::shared_ptr<Node> node);
    };

}

#endif //QUINT_UNCOMPUTATION_H
