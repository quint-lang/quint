//
// Created by BY210033 on 2023/6/21.
//
#include "ast/scope.h"

namespace quint::ast {

    FunctionScope::FunctionScope(const quint::Location &location, const std::string &name,
                                 std::vector<std::unique_ptr<Param>> args, std::vector<std::unique_ptr<Stmt>> body)
                                 : location(location), name(name), args(std::move(args)), body(std::move(body)) {}

}
