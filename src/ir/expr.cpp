//
// Created by BY210033 on 2023/3/24.
//
#include "ir/expr.h"
#include "ir/frontend_ir.h"

namespace quint::lang {

    Expr::Expr(int16 x): Expr() {
        expr = std::make_shared<ConstExpression>(PrimitiveType::i16, x);
    }

    Expr::Expr(int32 x): Expr() {
        expr = std::make_shared<ConstExpression>(PrimitiveType::i32, x);
    }

    Expr::Expr(int64 x): Expr() {
        expr = std::make_shared<ConstExpression>(PrimitiveType::i64, x);
    }

    Expr::Expr(float32 x): Expr() {
        expr = std::make_shared<ConstExpression>(PrimitiveType::f32, x);
    }

    Expr::Expr(float64 x): Expr() {
        expr = std::make_shared<ConstExpression>(PrimitiveType::f64, x);
    }

    Expr::Expr(const Identifier &id): Expr() {
        expr = std::make_shared<IdExpression>(id);
    }

    Expr &Expr::operator=(const Expr &other) {
        set(other);
        return *this;
    }

    SNode *Expr::snode() const {
        return nullptr;
    }

    void Expr::set_tb(const std::string &tb) {

    }

    void Expr::set_adjoint(const Expr &other) {

    }

    void Expr::set_dual(const Expr &other) {

    }

    DataType Expr::get_ret_type() const {
        return DataType();
    }

    void Expr::type_check(CompileConfig *config) {
        expr->type_check(config);
    }

    Expr lang::cast(const Expr &input, DataType dt) {
        return Expr::make<UnaryOpExpression>(UnaryOpType::cast_value, input, dt);
    }

    Expr lang::bit_cast(const Expr &input, DataType dt) {
        return Expr::make<UnaryOpExpression>(UnaryOpType::cast_bits, input, dt);
    }

}