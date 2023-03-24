//
// Created by BY210033 on 2023/3/24.
//

#ifndef QUINT_EXPRESSION_H
#define QUINT_EXPRESSION_H

#include "program/compile_config.h"
#include "ir/ir.h"
#include "ir/expr.h"

namespace quint::lang {

    class ExpressionVisitor;

    class Expression {
    protected:
        Stmt *stmt;

    public:
        std::string tb;
        std::map<std::string, std::string> attributes;
        DataType ret_type;

        struct FlattenContext {
            VecStatement stmts;
            Block *current_block = nullptr;

            inline Stmt *push_pack(pStmt &&stmt) {
                return stmts.push_back(std::move(stmt));
            }

            template<typename T, typename... Args>
            T *push_back(Args &&...args) {
                return stmts.push_back<T>(std::forward<Args>(args)...);
            }

            Stmt *back_stmt() {
                return stmts.back().get();
            }
        };


        Expression() {
            stmt = nullptr;
        }

        virtual void type_check(CompileConfig *config) = 0;

        virtual void accept(ExpressionVisitor *visitor) = 0;

        virtual void flatten(FlattenContext *ctx) {
            QUINT_NOT_IMPLEMENTED
        }

        virtual bool is_value() const {
            return false;
        }

        virtual ~Expression() {
        }

        Stmt *get_flattened_stmt() const {
            return stmt;
        }
     };


    class ExprGroup {
    public:
        std::vector<Expr> expressions;

        ExprGroup() {
        }

        explicit ExprGroup(const Expr &a) {
            expressions.emplace_back(a);
        }

        ExprGroup(const Expr &a, const Expr &b) {
            expressions.emplace_back(a);
            expressions.emplace_back(b);
        }

        ExprGroup(const ExprGroup &a, const Expr &b) {
            expressions.resize(a.size() + 1);

            for (int i = 0; i < a.size(); ++i) {
                expressions[i].set(a.expressions[i]);
            }
            expressions.back().set(b);
        }

        ExprGroup(const Expr &a, const ExprGroup &b) {
            expressions.resize(b.size() + 1);
            expressions.front().set(a);
            for (int i = 0; i < b.size(); ++i) {
                expressions[i + 1].set(b.expressions[i]);
            }
        }

        void push_back(const Expr &expr) {
            expressions.emplace_back(expr);
        }

        std::size_t size() const {
            return expressions.size();
        }

        const Expr &operator[](int i) const {
            return expressions[i];
        }

        Expr &operator[](int i) {
            return expressions[i];
        }
    };

#define PER_EXPRESSION(x) class x;
#include "inc/expressions.inc.h"
#undef PER_EXPRESSION

    class ExpressionVisitor {
    public:
        explicit ExpressionVisitor(bool allow_undefined_visitor = false,
                                   bool invoke_default_visitor = false)
            : allow_undefined_visitor_(allow_undefined_visitor),
              invoke_default_visitor_(invoke_default_visitor) {
        }

        virtual ~ExpressionVisitor() = default;

        virtual void visit(ExprGroup &exprGroup) = 0;

        void visit(Expr &expr) {
            expr.expr->accept(this);
        }

        virtual void visit(Expression *expr) {
            if (!allow_undefined_visitor_) {
                QUINT_ERROR("missing visitor function")
            }
        }

#define DEFINE_VISIT(T)             \
  virtual void visit(T *expr) {     \
    if (allow_undefined_visitor_) { \
      if (invoke_default_visitor_)  \
        visit((Expression *)expr);  \
    } else                          \
      QUINT_NOT_IMPLEMENTED         \
  }

#define PER_EXPRESSION(x) DEFINE_VISIT(x)
#include "inc/expressions.inc.h"
#undef PER_EXPRESSION
#undef DEFINE_VISIT

    private:
        bool allow_undefined_visitor_{false};
        bool invoke_default_visitor_{false};
    };

#define QUINT_DEFINE_ACCEPT_FOR_EXPRESSION \
  void accept(ExpressionVisitor *visitor) override { visitor->visit(this); }

}

#endif //QUINT_EXPRESSION_H
