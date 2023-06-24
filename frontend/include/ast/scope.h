//
// Created by BY210033 on 2023/6/21.
//

#ifndef QUINT_SCOPE_H
#define QUINT_SCOPE_H

#include "ast/stmt.h"
#include "parser/lexer.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"

#include <vector>
#include <memory>

namespace quint::ast {

    class Scope {

    };

    class BlockScope : public Scope {
        std::vector<std::shared_ptr<Stmt>> stmts;
        Location location;
    public:
        BlockScope(Location loc, std::vector<std::shared_ptr<Stmt>> stmts);

        const Location& loc() { return location; }

        llvm::ArrayRef<std::shared_ptr<Stmt>> getStmts() { return stmts; }
    };

    class FunctionScope : public Scope {
        Location location;
        std::shared_ptr<IdentifierExpr> name;
        std::vector<std::shared_ptr<Param>> args;
        std::shared_ptr<Type> ret;
        std::vector<std::shared_ptr<Stmt>> body;
    public:
        FunctionScope(const Location &location, std::shared_ptr<IdentifierExpr> name,
                      std::vector<std::shared_ptr<Param>> args, std::vector<std::shared_ptr<Stmt>> body,
                      std::shared_ptr<Type> ret);

        const Location loc() { return location; }

        std::string getName() const { return name->getName(); }

        llvm::ArrayRef<std::shared_ptr<Param>> getArgs() { return args; }

        llvm::ArrayRef<std::shared_ptr<Stmt>> getBody() { return body; }
    };

    class ModuleScope : public Scope {
        std::vector<std::shared_ptr<FunctionScope>> functions;
    public:
        ModuleScope(std::vector<std::shared_ptr<FunctionScope>> functions)
            : functions(std::move(functions)) {}

        auto begin() -> decltype(functions.begin())
        {
            return functions.begin();
        }

        auto end() -> decltype(functions.end())
        {
            return functions.end();
        }
    };


}

#endif //QUINT_SCOPE_H
