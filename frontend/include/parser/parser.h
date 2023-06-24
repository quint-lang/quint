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

        std::shared_ptr<ast::ModuleScope> parse();

    private:
        Lexer& lexer;

        std::shared_ptr<ast::FunctionScope> parseFunction();

        std::vector<std::shared_ptr<ast::Param>> parseParameters();

        std::shared_ptr<ast::Param> parseParam();

        std::shared_ptr<ast::Type> parseType();

        std::vector<std::shared_ptr<ast::Type>> parseTypeTuple();

        std::shared_ptr<ast::Stmt> parseStmt();

        std::shared_ptr<ast::DeclStmt> parseDecl();

        std::shared_ptr<ast::BreakStmt> parseBreak();

        std::shared_ptr<ast::ContinueStmt> parseContinue();

        std::shared_ptr<ast::ReturnStmt> parseReturn();

        std::shared_ptr<ast::ExprStmt> parseExprStmt();

        std::shared_ptr<ast::TryStmt> parseTryStmt();

        std::shared_ptr<ast::SuiteStmt> parseCompound();

        std::shared_ptr<ast::IfStmt> parseIfStmt();

        std::shared_ptr<ast::IfStmt> parseElif();

        std::shared_ptr<ast::WhileStmt> parseWhile();

        std::shared_ptr<ast::ForStmt> parseForStmt();

        std::shared_ptr<ast::ThrowStmt> parsePanic();

        std::shared_ptr<ast::Expr> parseExpr();

        std::shared_ptr<ast::Expr> parseLambdaExpr();

        std::shared_ptr<ast::Expr> parseOrExpr();

        std::shared_ptr<ast::Expr> parseAndExpr();

        std::shared_ptr<ast::Expr> parseNotExpr();

        std::shared_ptr<ast::Expr> parseCompareExpr();

        std::shared_ptr<ast::Expr> parseVBarExpr();

        std::shared_ptr<ast::Expr> parseXorExpr();

        std::shared_ptr<ast::Expr> parseCaretExpr();

        std::shared_ptr<ast::Expr> parseShiftExpr();

        std::shared_ptr<ast::Expr> parseArithExpr();

        std::shared_ptr<ast::Expr> parseTermExpr();

        std::shared_ptr<ast::Expr> parseFactorExpr();

        std::shared_ptr<ast::Expr> parsePower();

        std::shared_ptr<ast::Expr> parseSlice();

        std::shared_ptr<ast::Expr> parseAtomExpr();

        std::shared_ptr<ast::Expr> parseAtom();

        std::vector<std::shared_ptr<ast::Expr>> parseExprList();


        template<typename R, typename T, typename U = const char*>
        std::shared_ptr<R> parseError(T&& expected, U&& context = "")
        {
            auto curToken = lexer.getCurToken();
            llvm::errs() << "Parser error (" << lexer.getLastLocation().line << ", " << lexer.getLastLocation().col
                << "): expected '" << expected << "'" << context << " but has Token " << curToken;
            if (isprint(curToken))
                llvm::errs() << " '" << (char)curToken << "'";
            llvm::errs() << "\n";
            return nullptr;
        }

        template <typename R, typename T, typename U = const char*>
        std::vector<R> parseVectorError(T&& expected, U&& context = "")
        {
            auto curToken = lexer.getCurToken();
            llvm::errs() << "Parse error (" << lexer.getLastLocation().line << ", " << lexer.getLastLocation().col << "): expected '" << expected << "' " << context << " but has Token " << curToken;
            if (isprint(curToken))
            {
                llvm::errs() << " '" << (char)curToken << "'";
            }
            llvm::errs() << "\n";
            return {};
        }
    };

}

#endif //QUINT_PARSER_H
