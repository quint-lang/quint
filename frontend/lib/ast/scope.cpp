//
// Created by BY210033 on 2023/6/21.
//
#include "ast/scope.h"

namespace quint::ast {

    FunctionScope::FunctionScope(const quint::Location &location, std::shared_ptr<IdentifierExpr> name,
                                 std::vector<std::shared_ptr<Param>> args, std::vector<std::shared_ptr<Stmt>> body,
                                 std::shared_ptr<Type> ret)
                                 : location(location), name(std::move(name)), args(std::move(args)), body(std::move(body)),
                                   ret(std::move(ret)){}

}
