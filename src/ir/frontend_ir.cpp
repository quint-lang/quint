//
// Created by BY210033 on 2023/3/10.
//
#include "ir/frontend_ir.h"
#include "common/exceptions.h"

#include <numeric>

namespace quint::lang {

    FrontendAssignStmt::FrontendAssignStmt(const Expr &lhs, const Expr &rhs)
        : lhs(lhs), rhs(rhs) {
        QUINT_ASSERT(lhs->is_value())
        if (lhs.is<IdExpression>() && lhs->ret_type == PrimitiveType::unknown) {
            lhs.expr->ret_type = rhs->ret_type;
        }
    }

    void ArgLoadExpression::type_check(CompileConfig *config) {

    }

    void ArgLoadExpression::flatten(Expression::FlattenContext *ctx) {
    }

    void IdExpression::flatten(Expression::FlattenContext *ctx) {
    }

    void ConstExpression::type_check(CompileConfig *config) {

    }

    void ConstExpression::flatten(Expression::FlattenContext *ctx) {
    }

    bool UnaryOpExpression::is_cast() const {
        return unary_op_is_cast(type);
    }

    void UnaryOpExpression::type_check(CompileConfig *config) {

    }

    void UnaryOpExpression::flatten(Expression::FlattenContext *ctx) {
    }

    void BinaryOpExpression::type_check(CompileConfig *config) {

    }

    void BinaryOpExpression::flatten(Expression::FlattenContext *ctx) {
    }

    void TernaryOpExpression::type_check(CompileConfig *config) {

    }

    void TernaryOpExpression::flatten(Expression::FlattenContext *ctx) {
    }

    void ASTBuilder::insert(std::unique_ptr<Stmt> &&stmt, int location) {
        QUINT_ASSERT(!stack_.empty())
        stack_.back()->insert(std::move(stmt), location);
    }

    Stmt *ASTBuilder::get_last_stmt() {
        QUINT_ASSERT(!stack_.empty())
        return stack_.back()->back();
    }

    void ASTBuilder::insert_assignment(Expr &lhs, const Expr &rhs, const std::string &tb) {
        if (lhs.expr == nullptr) {
            lhs.set(rhs);
        } else if (lhs.expr->is_value()) {
            auto stmt = std::make_unique<FrontendAssignStmt>(lhs, rhs);
            stmt->tb = tb;
            this->insert(std::move(stmt));
        } else {
            QUINT_ERROR("Cannot assign to non-lvalue: {}")
        }
    }

    Expr ASTBuilder::make_var(const Expr &x, std::string tb) {
        auto var = this->expr_alloca();
        this->insert_assignment(var, x, tb);
        return var;
    }

    void ASTBuilder::create_print(std::vector<std::variant<Expr, std::string>> contents) {
        this->insert(std::make_unique<FrontendPrintStmt>(contents));
    }

    void ASTBuilder::begin_frontend_if(const Expr &cond) {
        auto stmt_tmp = std::make_unique<FrontendIfStmt>(cond);
        this->insert(std::move(stmt_tmp));
    }

    void ASTBuilder::begin_frontend_if_true() {
        auto if_stmt = this->get_last_stmt()->as<FrontendIfStmt>();
        this->create_scope(if_stmt->true_statements);
    }

    void ASTBuilder::begin_frontend_if_false() {
        auto if_stmt = this->get_last_stmt()->as<FrontendIfStmt>();
        this->create_scope(if_stmt->false_statements);
    }

    Expr ASTBuilder::expr_alloca() {
        auto var = Expr(std::make_shared<IdExpression>(get_next_id()));
        this->insert(std::make_unique<FrontendAllocaStmt>(
                std::static_pointer_cast<IdExpression>(var.expr)->id,
                PrimitiveType::unknown));
        return var;
    }

    void ASTBuilder::create_scope(std::unique_ptr<Block> &list, ASTBuilder::LoopType tp) {
        QUINT_ASSERT(list == nullptr)
        LoopState prev = loop_state_stack_.back();
        if (tp == NotLoop) {
            loop_state_stack_.push_back(prev);
        } else if (tp == For && stack_.size() == 1) {
            loop_state_stack_.push_back(Outermost);
        } else {
            loop_state_stack_.push_back(Inner);
        }
        list = std::make_unique<Block>();
        if (!stack_.empty()) {
            list->parent_stmt = get_last_stmt();
        }
        stack_.push_back(list.get());
    }

    void ASTBuilder::pop_scope() {
        stack_.pop_back();
        loop_state_stack_.pop_back();
    }

}
