//
// Created by BY210033 on 2023/4/11.
//
#include "quint/struct/snode_tree.h"

namespace quint::lang {

    SNodeTree::SNodeTree(int id, std::unique_ptr<SNode> root)
        : id_(id), root_(std::move(root)) {
        check_tree_validity(*root_);
    }

    void SNodeTree::check_tree_validity(SNode &node) {

    }

}