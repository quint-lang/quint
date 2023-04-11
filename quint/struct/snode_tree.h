//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_SNODE_TREE_H
#define QUINT_SNODE_TREE_H

#include <memory>
#include <unordered_map>

#include "quint/ir/snode.h"

namespace quint::lang {
    class SNodeTree {
    public:
        constexpr static int kFirstID = 0;

        explicit SNodeTree(int id, std::unique_ptr<SNode> root);

        int id() const {
            return id_;
        }

        const SNode *root() const {
            return root_.get();
        }

        SNode *root() {
            return root_.get();
        }

    private:
        int id_{0};
        std::unique_ptr<SNode> root_{nullptr};

        void check_tree_validity(SNode &node);
    };
}

#endif //QUINT_SNODE_TREE_H
