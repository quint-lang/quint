//
// Created by BY210033 on 2023/4/18.
//

#ifndef QUINT_PARSER_H
#define QUINT_PARSER_H

#include "parser/lexer.h"
#include "ast/scope.h"
#include "llvm/Support/raw_os_ostream.h"

namespace quint {
    class Parser {
    public:
        Parser(Lexer& lexer) :lexer(lexer) {}

        std::unique_ptr<ast::ModuleScope> parse();

    private:
        Lexer& lexer;

        std::unique_ptr<ast::FunctionScope> parseFunction();

        std::unique_ptr<ast::Type> mapToType();

        template<typename R, typename T, typename U = const char*>
        std::unique_ptr<R> parseError(T&& expected, U&& context = "")
        {
            auto curToken = lexer.getCurToken();
            llvm::errs() << "Parser error (" << lexer.getLastLocation().line << ", " << lexer.getLastLocation().col
                << "): expected '" << expected << "'" << context << " but has Token " << curToken;
            if (isprint(curToken))
                llvm::errs() << " '" << (char)curToken << "'";
            llvm::errs() << "\n";
            return nullptr;
        }
    };
}

#endif //QUINT_PARSER_H
