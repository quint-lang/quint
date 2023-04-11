//
// Created by BY210033 on 2023/3/31.
//
#include "quint/ir/visitors.h"

namespace quint::lang {

    BasicStmtVisitor::BasicStmtVisitor() {
        allow_undefined_visitor = true;
    }

    void BasicStmtVisitor::visit(Stmt *stmt) {

    }

    void BasicStmtVisitor::visit(Block *stmt) {

    }


}