//
// Created by BY210033 on 2023/3/10.
//
#include "ir/frontend_ir.h"
#include "common/exceptions.h"

#include <numeric>

namespace quint::lang {

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

}
