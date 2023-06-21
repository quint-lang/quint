//
// Created by BY210033 on 2023/5/26.
//

#ifndef QUINT_STMT_H
#define QUINT_STMT_H

#include <string>
#include <memory>
#include <iostream>

#include "ast/expr.h"
#include "ast/type.h"

namespace quint::ast {

#define ACCEPT(X)                   \
    using Stmt::toString;           \
    StmtPtr clone() const override; \
    void accept(X &visitor) override;

    class ASTVisitor;
    class DeclStmt;
    class ExprStmt;
    class IfStmt;
    class WhileStmt;
    class ForStmt;


    class Stmt {

        using base_type = Stmt;

        bool done = false;
        int age;
    public:
        Stmt();
        Stmt(const Stmt &s) = default;

        std::string toString() const;
        virtual std::string toString(int indent) const = 0;

        void validate() const;

        virtual std::shared_ptr<Stmt> clone() const = 0;

        virtual void accept(ASTVisitor &) = 0;

        friend std::ostream &operator<<(std::ostream &os, const Stmt &stmt) {
            return os << stmt.toString();
        }

        virtual DeclStmt *getDecl() { return nullptr; }
        virtual ExprStmt *getExpr() { return nullptr; }

        virtual Stmt *firstInBlock() { return this; }

        bool isDone() const { return done; }
        void setDone() { done = true; }
        virtual ~Stmt() {}
    };

    using StmtPtr = std::shared_ptr<Stmt>;

    class DeclStmt : public Stmt {

    };

    class ExprStmt : public Stmt {
        ExprPtr expr;

        explicit ExprStmt(ExprPtr expr);
        ExprStmt(const ExprStmt &s);

    public:
        std::string toString(int indent) const override;

        ACCEPT(ASTVisitor);

        ExprStmt *getExpr() override { return this; }
    };

    class IfStmt : public Stmt {

    };

    class WhileStmt : public Stmt {

    };

    class ForStmt : public Stmt {

    };



    class SuiteStmt : public Stmt {
    public:
        std::string toString(int indent) const override;
    };

}

#endif //QUINT_STMT_H
