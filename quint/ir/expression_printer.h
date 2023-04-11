//
// Created by BY210033 on 2023/4/6.
//

#ifndef QUINT_EXPRESSION_PRINTER_H
#define QUINT_EXPRESSION_PRINTER_H

#include "quint/ir/expr.h"
#include "quint/ir/expression.h"
#include "quint/ir/frontend_ir.h"
#include "quint/program/program.h"
#include "quint/analysis/offline_cache_util.h"

namespace quint::lang {

    class ExpressionPrinter : public ExpressionVisitor {
    public:
        explicit ExpressionPrinter(std::ostream *os = nullptr): os_(os) {
        }

        std::ostream *get_ostream() const {
            return os_;
        }

        void set_ostream(std::ostream *os) {
            os_ = os;
        }

    private:
        std::ostream *os_{nullptr};
    };

    class ExpressionHumanFriendlyPrinter : public ExpressionPrinter {
    public:
        explicit ExpressionHumanFriendlyPrinter(std::ostream *os = nullptr)
            : ExpressionPrinter(os) {
        }

        ~ExpressionHumanFriendlyPrinter() override = default;

        void visit(ExprGroup &exprGroup) override {

        }

        void visit(Expression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(ArgLoadExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(IdExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(ConstExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }
    };

}

#endif //QUINT_EXPRESSION_PRINTER_H
