//
// Created by BY210033 on 2023/3/24.
//

#ifndef QUINT_EXPR_H
#define QUINT_EXPR_H

#include "quint/util/lang_util.h"
#include "quint/ir/type_utils.h"

namespace quint::lang {

    struct CompileConfig;
    class Identifier;
    class Expression;
    class SNode;
//    class ASTBulder;

    class Expr {
    public:
        std::shared_ptr<Expression> expr;
        bool const_value;
        bool atomic;

        Expr() {
            const_value = false;
            atomic = false;
        }

        explicit Expr(int16 x);

        explicit Expr(int32 x);

        explicit Expr(int64 x);

        explicit Expr(float32 x);

        explicit Expr(float64 x);

        explicit Expr(std::shared_ptr<Expression> expr) : Expr() {
            this->expr = expr;
        }

        Expr(const Expr &other): Expr() {
            set(other);
            const_value = other.const_value;
        }

        explicit Expr(const Identifier &id);

        void set(const Expr &other) {
            expr = other.expr;
        }

        operator bool() const {
            return expr.get() != nullptr;
        }

        Expression const *operator->() const {
            return expr.get();
        }

        template<typename T>
        std::shared_ptr<T> cast() const {
            QUINT_ASSERT(expr != nullptr)
            return std::dynamic_pointer_cast<T>(expr);
        }

        template<typename T>
        bool is() const {
            return cast<T>() != nullptr;
        }

        Expr &operator=(const Expr &other);

        template<typename T, typename... Args>
        static Expr make(Args &&...args) {
            return Expr(std::make_shared<T>(std::forward<Args>(args)...));
        }

        SNode *snode() const;

        void set_tb(const std::string &tb);

        void set_adjoint(const Expr &other);

        void set_dual(const Expr &other);

        DataType get_ret_type() const;

        void type_check(CompileConfig *config);

    };

    Expr cast(const Expr &input, DataType dt);

    template<typename T>
    Expr cast(const Expr &input) {
        return cast(input, get_data_type<T>());
    }

    Expr bit_cast(const Expr &input, DataType dt);

    template<typename T>
    Expr bit_cast(const Expr &input) {
        return bit_cast(input, get_data_type<T>());
    }

}

#endif //QUINT_EXPR_H
