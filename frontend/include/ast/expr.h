//
// Created by BY210033 on 2023/5/25.
//

#ifndef QUINT_EXPR_H
#define QUINT_EXPR_H

#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include "ast/type.h"

namespace quint::ast {

#define ACCEPT(X) \
    ExprPtr clone() const override; \
    void accept(X &visitor) override;

    class ASTVisitor;
    class Expr;
    class Param;

    enum class TypeKind {
        Int,
        Double,
        Bool,
        String,
        Void,
        Unit,
        Map,
        Array,
        Tuple,
        Function,
        Complex,
        Circuit,
        Qubit
    };


    class Expr {
        using base_type = Expr;

        bool done;

        int attributes;

        std::shared_ptr<Expr> origExpr;

    public:
        Expr();

        Expr(const Expr &e) = default;

        virtual std::string toString() const { return ""; }

        void validate() const;

        virtual std::shared_ptr<Expr> clone() const { return nullptr; }

        virtual void accept(ASTVisitor &) {}

        friend std::ostream &operator<<(std::ostream &os, const Expr &expr) {
            return os << expr.toString();
        }

        virtual bool isId(const std::string &val) const { return false; }

        bool hasAttr(int attr) const;
        void setAttr(int attr);

        bool isDone() const { return done; }
        void setDone() { done = true; }

        std::string getTypeName();

    protected:
        std::string wrapType(const std::string &s) const;

    };

    using ExprPtr = std::shared_ptr<Expr>;

    class Param {
        std::string name;
        std::shared_ptr<Type> type;
        ExprPtr defaultValue;
        enum {
            Normal,
            Generic,
            HiddenGeneric,
        } status;
    public:
        explicit Param(std::shared_ptr<Type> type, std::string name = "",
                       ExprPtr defaultValue = nullptr, int generic = 0);

        std::string toString() const;
        Param clone() const;

        virtual ~Param() {}
    };
}

#endif //QUINT_EXPR_H
