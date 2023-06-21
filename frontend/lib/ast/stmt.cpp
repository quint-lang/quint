//
// Created by BY210033 on 2023/5/26.
//
#include "ast/stmt.h"

namespace quint::ast {

    const int INDENT_SIZE = 2;

    std::string SuiteStmt::toString(int indent) const {
        std::string pad = indent >= 0 ? ("\n" + std::string(indent + INDENT_SIZE, ' ')) : "";
        std::string s;

        return std::string();
    }

    std::string ExprStmt::toString(int indent) const {
        return std::string();
    }

    void ExprStmt::accept(ASTVisitor &visitor) {

    }

    bool isInt(ast::Type &ty) {
        return true;
    }

    bool isUint(ast::Type &ty) {
        return true;
    }
}
