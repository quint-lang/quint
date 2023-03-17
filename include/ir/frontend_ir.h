//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_FRONTEND_IR_H
#define QUINT_FRONTEND_IR_H

#include <string>
#include <vector>

#include "ir/ir.h"
#include "common/exceptions.h"

namespace quint::lang {

    class ASTBuilder {
    private:
        enum LoopState { None, Outermost, Inner };
        enum LoopType { NotLoop, For, While };


    };

    class FrontendContext {
    private:
        std::unique_ptr<ASTBuilder> current_builder_;
        std::unique_ptr<Block> root_node_;

    public:
        explicit FrontendContext() {
            root_node_ = std::make_unique<Block>();
            current_builder_ = std::make_unique<ASTBuilder>();
        }

        ASTBuilder &builder() {
            return *current_builder_;
        }

        std::unique_ptr<Block> get_root() {
            return std::move(root_node_);
        }
    };

}

#endif //QUINT_FRONTEND_IR_H
