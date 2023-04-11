//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_VISITORS_H
#define QUINT_VISITORS_H

#include "quint/ir/ir.h"

namespace quint::lang {

    class BasicStmtVisitor : public IRVisitor {
    public:
        BasicStmtVisitor();

        virtual void preprocess_container_stmt(Stmt *stmt) {
        }

        void visit(Stmt *stmt) override;

        void visit(Block *stmt) override;

    };

}

#endif //QUINT_VISITORS_H
