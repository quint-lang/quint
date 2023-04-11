//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_UTILS_H
#define QUINT_UTILS_H
#include "quint/ir/statements.h"

namespace quint::lang {

    Stmt *generate_mod(VecStatement *stmts, Stmt *x, int y);
    Stmt *generate_div(VecStatement *stmts, Stmt *x, int y);

}

#endif //QUINT_UTILS_H
