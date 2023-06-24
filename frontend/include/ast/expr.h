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

#include "parser/lexer.h"
#include "ast/type.h"

namespace quint {
    enum aug_assign {
        assign, // =
        plus, // +=
        minus, // -=
        star, // *=
        slash, // /=
        percent, // %=
        ampersand, // ^=
        vbar, // |=
        caret, // ^=
        lshift, // <<=
        rshift, // >>=
    };

    enum binary_op {
        add, // +
        sub, // -
        multiply, // *
        div, // /
        remainder, // %
        AND, // &
        OR, // |
        XOR, // ^
        ls, // <<
        rs, // >>
        pow, // **
        lt, // <
        eq, // ==
        gt, // >
        le, // <=
        ge, // >=
        notequal, // !=
        land, // &&
        lor, // ||
    };

    enum unary_op {
        min, // -
        bang, // !
        tilde, // ~
    };
}

namespace quint::ast {

#define ACCEPT(X) \
    ExprPtr clone() const override; \
    void accept(X &visitor) override;

    class ASTVisitor;
    class Expr;
    class Param;
    class IdentifierExpr;
    class AssignExpr;
    class BinaryExpr;
    class CallExpr;
    class TrailerExpr;
    class ConstantExpr;
    class IndexExpr;
    class ListExpr;
    class TupleExpr;
    class UnaryExpr;
    class StringExpr;
    class RecordExpr;
    class Stmt;

    class Expr {
        Location location;
        using base_type = Expr;

        bool done;

        int attributes;

        std::shared_ptr<Expr> origExpr;

    public:
        Expr();

        Expr(const Location &location): location(location) {}

        Expr(const Expr &e) = default;

        Location getLoc() { return location; }

        virtual std::string toString() const { return ""; }

        void validate() const;

        virtual std::shared_ptr<Expr> clone() const { return nullptr; }

        virtual void accept(ASTVisitor &) {}

        friend std::ostream &operator<<(std::ostream &os, const Expr &expr) {
            return os << expr.toString();
        }

        virtual bool isId(const std::string &val) const { return false; }
        virtual IdentifierExpr* getId() { return nullptr; }
        virtual AssignExpr* getAssign() { return nullptr; }
        virtual BinaryExpr* getBinary() { return nullptr; }
        virtual CallExpr* getCall() { return nullptr; }
        virtual TrailerExpr* getTrailer() { return nullptr; }
        virtual ConstantExpr* getConstant() { return nullptr; }
        virtual IndexExpr* getIndex() { return nullptr; }
        virtual ListExpr* getList() { return nullptr; }
        virtual TupleExpr* getTuple() { return nullptr; }
        virtual UnaryExpr* getUnary() { return nullptr; }
        virtual StringExpr* getString() { return nullptr; }
        virtual RecordExpr* getRecord() { return nullptr; }

        bool hasAttr(int attr) const;
        void setAttr(int attr);

        bool isDone() const { return done; }
        void setDone() { done = true; }

        std::string getTypeName();

    protected:
        std::string wrapType(const std::string &s) const;

    };

    using ExprPtr = std::shared_ptr<Expr>;

    class AssignExpr : public Expr {
        ExprPtr left;
        ExprPtr right;
        aug_assign op;

    public:
        AssignExpr(const Location &loc, ExprPtr left, aug_assign op, ExprPtr right);
        AssignExpr(const AssignExpr &expr);

        ExprPtr getLeft();

        ExprPtr getRight();

        aug_assign getOp();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        AssignExpr *getAssign() override { return this; }
    };

    class BinaryExpr : public Expr {
        ExprPtr left, right;
        binary_op op;
    public:
        BinaryExpr(const Location &loc, ExprPtr left, binary_op op, ExprPtr right);
        BinaryExpr(const BinaryExpr &expr);

        ExprPtr getLeft();

        ExprPtr getRight();

        binary_op getOp();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        BinaryExpr *getBinary() override { return this; }
    };

    class IdentifierExpr : public Expr {
        std::string value;
    public:
        explicit IdentifierExpr(const Location &loc, const std::string& value);
        IdentifierExpr(const IdentifierExpr &expr) = default;

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        bool isId(const std::string &val) const override;
        std::string getName() { return value; }

        IdentifierExpr *getId() override { return this; }
    };

    class CallExpr : public Expr {
        ExprPtr expr;
        std::vector<ExprPtr> args;
        bool ordered;
    public:
        CallExpr(const Location& loc, ExprPtr expr, std::vector<ExprPtr> args);
        CallExpr(const CallExpr &expr);

        ExprPtr getExpr();

        std::vector<ExprPtr> getArgs();

        bool isOrdered();

        void validate() const;

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        CallExpr *getCall() override { return this; }
    };

    class TrailerExpr : public Expr {
        ExprPtr expr;
        std::string member;
    public:
        TrailerExpr(const Location& loc, ExprPtr expr, const std::string& member);
        TrailerExpr(const TrailerExpr& other);

        ExprPtr getExpr();

        std::string getMember();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        TrailerExpr *getTrailer() override { return this; }
    };

    class ConstantExpr : public Expr {
        bool boolean, isBool = false, isInt = false, isDouble = false;
        double value;
        int num;
    public:
        ConstantExpr(const Location &loc, double value);
        ConstantExpr(const Location &loc, int value);
        ConstantExpr(const Location &loc, bool value);
        ConstantExpr(const ConstantExpr& other);

        bool isBoolean();

        double getValue();

        int getNum();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        ConstantExpr *getConstant() override { return this; }
    };

    class IndexExpr : public Expr {
        ExprPtr expr, index;
    public:
        IndexExpr(const Location& loc, ExprPtr expr, ExprPtr index);
        IndexExpr(const IndexExpr& other);

        ExprPtr getExpr();

        ExprPtr getIndex1();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        IndexExpr *getIndex() override { return this; }
    };

    class ListExpr : public Expr {
        std::vector<ExprPtr> items;
    public:
        ListExpr(const Location& loc, std::vector<ExprPtr> items);
        ListExpr(const ListExpr& other);

        std::vector<ExprPtr> getItems();

    private:
        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        ListExpr *getList() override { return this; }
    };

    class TupleExpr : public Expr {
        std::vector<ExprPtr> items;
    public:
        TupleExpr(const Location& loc, std::vector<ExprPtr> items);
        TupleExpr(const ListExpr& other);

        std::vector<ExprPtr> getItems();

    private:
        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        TupleExpr *getTuple() override { return this; }
    };

    class UnaryExpr : public Expr {
        ExprPtr expr;
        unary_op op;
    public:
        UnaryExpr(const Location& loc, unary_op op, ExprPtr expr);
        UnaryExpr(const UnaryExpr& other);

        ExprPtr getExpr();

        unary_op getOp();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        UnaryExpr *getUnary() override { return this; }
    };

    class StringExpr : public Expr {
        std::string value;
    public:
        StringExpr(const Location& loc, const std::string& value);
        StringExpr(const StringExpr& other);

        std::string getValue();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        StringExpr *getString() override { return this; }
    };

    class RecordExpr : public Expr {
        std::vector<ExprPtr> items;
    public:
        RecordExpr(const Location& loc, std::vector<ExprPtr> items);
        RecordExpr(const ListExpr& other);

        std::vector<ExprPtr> getItems();

    private:
        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;

        RecordExpr *getRecord() override { return this; }
    };

    class SliceExpr : public Expr {
        ExprPtr start, stop, step;
    public:
        SliceExpr(const Location& loc, ExprPtr start, ExprPtr stop, ExprPtr step);
        SliceExpr(const SliceExpr& other);

        ExprPtr getStart();

        ExprPtr getStop();

        ExprPtr getStep();

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;
    };

    class LambdaExpr : public Expr {
    public:
        std::shared_ptr<Stmt> suite;
        std::vector<std::shared_ptr<Param>> args;
        std::shared_ptr<Type> ret;

        LambdaExpr(const Location& loc, std::vector<std::shared_ptr<Param>> args, std::shared_ptr<Type> ret,
                   std::shared_ptr<Stmt> suite);
        LambdaExpr(const LambdaExpr& other);

        std::string toString() const override;

        void accept(ASTVisitor &visitor) override;
    };

    class Param {
        std::string name;
        std::shared_ptr<Type> type;
        ExprPtr defaultValue;
        Location location;
        enum {
            Normal,
            Generic,
            HiddenGeneric,
        } status;
    public:
        explicit Param(std::shared_ptr<Type> type, Location location, std::string name = "",
                       ExprPtr defaultValue = nullptr, int generic = 0);

        std::string toString() const;
        Param *clone();

        virtual ~Param() {}
    };
}

#endif //QUINT_EXPR_H
