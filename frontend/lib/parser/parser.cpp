//
// Created by BY210033 on 2023/4/18.
//
#include "parser/parser.h"
#include "parser/lexer.h"
#include "ast/scope.h"

namespace quint {

    std::unique_ptr<ast::ModuleScope> Parser::parse() {
        lexer.getNextToken();
        std::vector<std::unique_ptr<ast::FunctionScope>> functions;

        while (auto c = parseFunction()) {
            functions.push_back(std::move(c));
            if (lexer.getCurToken() == tok_eof)
                break;
        }

        if (lexer.getCurToken() != tok_eof)
            return parseError<ast::ModuleScope>("nothing", "at end of file");

        return std::make_unique<ast::ModuleScope>(std::move(functions));
    }

    std::unique_ptr<ast::FunctionScope> Parser::parseFunction() {
        auto loc = lexer.getLastLocation();

        if (lexer.getCurToken() != tok_func)
        {
            return parseError<ast::FunctionScope>("func", "in top level");
        }
        lexer.consume(tok_func);

        if (lexer.getCurToken() != tok_name) {
            return parseError<ast::FunctionScope>("function name", "in function declaration");
        }
        std::string name(lexer.getIdentifier());
        lexer.consume(tok_name);
        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::FunctionScope>("(", "after function name");
        lexer.consume(tok_lparen);

        std::vector<std::unique_ptr<ast::Param>> args;
        if (lexer.getCurToken() != tok_rparen) {
            do {
                std::string param_name(lexer.getIdentifier());
                auto param_loc = lexer.getLastLocation();
                lexer.consume(tok_name);
                if (lexer.getCurToken() != tok_comma)
                    break;
            } while (true);
        }

        return std::unique_ptr<ast::FunctionScope>();
    }

}