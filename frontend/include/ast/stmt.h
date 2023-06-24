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
    class SuiteStmt;


    class Stmt {
        Location location;
        using base_type = Stmt;
        bool done = false;
        int age;
    public:
        Stmt();
        Stmt(const Location& location) : location(location) {}
        Stmt(const Stmt &s) = default;

        virtual std::string toString() const { return ""; }

        void validate() const;

        virtual std::shared_ptr<Stmt> clone() const { return nullptr; }

        virtual void accept(ASTVisitor &) {  };

        friend std::ostream &operator<<(std::ostream &os, const Stmt &stmt) {
            return os << stmt.toString();
        }

        virtual DeclStmt *getDecl() { return nullptr; }
        virtual ExprStmt *getExpr() { return nullptr; }
        virtual SuiteStmt *getSuite() { return nullptr; }

        Location getLocation() { return location; }

        virtual Stmt *firstInBlock() { return this; }

        bool isDone() const { return done; }
        void setDone() { done = true; }
        virtual ~Stmt() {}
    };

    using StmtPtr = std::shared_ptr<Stmt>;

    class DeclStmt : public Stmt {
    public:
        ExprPtr lhs, rhs = nullptr;
        std::shared_ptr<Type> type = nullptr;

        DeclStmt(const Location& loc, ExprPtr lhs, ExprPtr rhs = nullptr, std::shared_ptr<Type> type = nullptr);
        DeclStmt(const DeclStmt& other);

        std::string toString() const override;

        std::shared_ptr<Stmt> clone() const override;

        void accept(ASTVisitor &visitor) override;

        DeclStmt *getDecl() override { return this; }
    };

    class ExprStmt : public Stmt {
        ExprPtr expr;

    public:
        explicit ExprStmt(const Location& loc, ExprPtr expr);
        ExprStmt(const ExprStmt &s);

        ExprPtr getExpression();

        std::string toString() const override;

        ACCEPT(ASTVisitor);

        ExprStmt *getExpr() override { return this; }
    };

    class IfStmt : public Stmt {
    public:
        ExprPtr cond;
        StmtPtr ifSuite, elseSuite;

        IfStmt(const Location& loc, ExprPtr cond, StmtPtr ifSuite, StmtPtr elseSuite = nullptr);
        IfStmt(const IfStmt& other);

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class WhileStmt : public Stmt {
    public:
        ExprPtr cond;
        StmtPtr suite;
        std::string gotoVar = "";

        WhileStmt(const Location& loc, ExprPtr cond, StmtPtr suite);
        WhileStmt(const WhileStmt& other);

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class ForStmt : public Stmt {
    public:
        ExprPtr var;
        ExprPtr iter;
        StmtPtr suite;

        bool wrapped;

        ForStmt(const Location& loc, ExprPtr var, ExprPtr iter, StmtPtr suite);
        ForStmt(const ForStmt& other);

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class TryStmt : public Stmt {
    public:
        struct Catch {
            std::string var;
            ExprPtr exc;
            StmtPtr suite;

            Catch clone() const;
        };

        StmtPtr suite;
        std::vector<Catch> catches;

        TryStmt(const Location& loc, StmtPtr suite, std::vector<Catch> catches);
        TryStmt(const TryStmt& other);

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class ReturnStmt : public Stmt {
    public:
        ExprPtr expr;
        explicit ReturnStmt(const Location& loc, ExprPtr expr = nullptr);
        ReturnStmt(const ReturnStmt& other);

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class BreakStmt : public Stmt {
    public:
        BreakStmt(const Location& loc);
        BreakStmt(const BreakStmt& other) = default;

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class ContinueStmt : public Stmt {
    public:
        ContinueStmt(const Location& loc);
        ContinueStmt(const ContinueStmt& other) = default;

        std::string toString() const override;
        ACCEPT(ASTVisitor);
    };

    class ThrowStmt : public Stmt {
    public:
        ExprPtr expr;
        bool transformed;

        explicit ThrowStmt(const Location& loc, ExprPtr expr, bool transformed = false);
        ThrowStmt(const ThrowStmt& other);

        ACCEPT(ASTVisitor);

        std::string toString() const override;
    };


    class SuiteStmt : public Stmt {
    public:
        std::vector<StmtPtr> stmts;

        explicit SuiteStmt(const Location& loc, std::vector<StmtPtr> stmts = {});
        SuiteStmt(const SuiteStmt& other);
        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        SuiteStmt *getSuite() override { return this; }

        Stmt *firstInBlock() override {
            return stmts.empty() ? nullptr : stmts[0]->firstInBlock();
        }

        StmtPtr *lastInBlock();

        static void flatten(const StmtPtr &s, std::vector<StmtPtr> &stmts);

    };

}

#endif //QUINT_STMT_H
