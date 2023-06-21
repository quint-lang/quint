//
// Created by BY210033 on 2023/5/25.
//
#include "ast/expr.h"

namespace quint::ast {

    Expr::Expr() {

    }

    void Expr::validate() const {

    }

    bool Expr::hasAttr(int attr) const {
        return false;
    }

    void Expr::setAttr(int attr) {

    }

    std::string Expr::getTypeName() {
        return std::string();
    }

    std::string Expr::wrapType(const std::string &s) const {
        return std::string();
    }

    Param::Param(std::shared_ptr<Type> type, std::string name, ExprPtr defaultValue, int generic) {

    }

    std::string Param::toString() const {
        return "";
    }


    Param Param::clone() const {
        return *this;
    }

}