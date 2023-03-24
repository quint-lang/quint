//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_FRONTEND_IR_H
#define QUINT_FRONTEND_IR_H

#include <string>
#include <vector>

#include "ir/ir.h"
#include "common/exceptions.h"
#include "ir/expression.h"

namespace quint::lang {


    class ArgLoadExpression : public Expression {
    public:
        int arg_id;
        DataType dt;
        bool is_ptr;

        ArgLoadExpression(int arg_id, DataType dt, bool is_ptr = false)
            : arg_id(arg_id), dt(dt), is_ptr(is_ptr) {
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        bool is_value() const override {
            return is_ptr;
        }

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class IdExpression : public Expression {
    public:
        Identifier id;

        explicit IdExpression(const Identifier &id): id(id) {}

        void type_check(CompileConfig *config) override {
        }

        void flatten(FlattenContext *ctx) override;

        bool is_value() const override {
            return true;
        }

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class ConstExpression : public Expression {
    public:
        TypedConstant val;

        template<typename T>
        explicit ConstExpression(const T &x): val(x) {
            ret_type = val.dt;
        }

        template<typename T>
        explicit ConstExpression(const DataType &dt, const T &x): val({dt, x}) {
            ret_type = val.dt;
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

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
