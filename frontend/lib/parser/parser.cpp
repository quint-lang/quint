//
// Created by BY210033 on 2023/4/18.
//
#include "parser/parser.h"
#include "parser/lexer.h"
#include "ast/scope.h"

namespace quint {

    std::shared_ptr<ast::ModuleScope> Parser::parse() {
        lexer.getNextToken();
        std::vector<std::shared_ptr<ast::FunctionScope>> functions;

        while (auto c = parseFunction()) {
            functions.push_back(c);
            if (lexer.getCurToken() == tok_eof)
                break;
        }

        if (lexer.getCurToken() != tok_eof)
            return parseError<ast::ModuleScope>("nothing", "at end of file");

        return std::make_shared<ast::ModuleScope>(functions);
    }

    std::shared_ptr<ast::FunctionScope> Parser::parseFunction() {
        std::vector<std::shared_ptr<ast::Param>> args;
        std::shared_ptr<ast::Type> ret;
        std::vector<std::shared_ptr<ast::Stmt>> stmts;
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
        auto functionName = std::make_shared<ast::IdentifierExpr>(lexer.getLocation(), name);
        lexer.consume(tok_name);
        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::FunctionScope>("(", "after function name");
        lexer.consume(tok_lparen);

        if (lexer.getCurToken() != tok_rparen) {
            args = parseParameters();
        }
        if (lexer.getCurToken() != tok_rparen)
            return parseError<ast::FunctionScope>(")", "to end function parameters");
        lexer.consume(tok_rparen);

        if (lexer.getCurToken() == tok_arrow) {
            lexer.getNextToken();
            ret = parseType();
        }

        if (lexer.getCurToken() != '{')
            return parseError<ast::FunctionScope>("{", "in function declaration");
        lexer.consume(tok_lbrace);

        while (lexer.getCurToken() != '}' && lexer.getCurToken() != tok_eof)
        {
            auto expr = parseStmt();
            if (!expr)
            {
                return nullptr;
            }
            stmts.push_back(expr);
        }
        lexer.consume(tok_rbrace);
        return std::make_shared<ast::FunctionScope>(std::move(loc), functionName,
                                                    args, stmts, ret);
    }


    std::vector<std::shared_ptr<ast::Param>> Parser::parseParameters() {
        std::vector<std::shared_ptr<ast::Param>> params;
        do {
            auto param = parseParam();
            params.push_back(param);
            if (lexer.getCurToken() != tok_comma)
                break;
            lexer.consume(tok_comma);
            if (lexer.getCurToken() != tok_name)
                return parseVectorError<std::shared_ptr<ast::Param>>("variable", "in parameter list");
        } while (true);
        return params;
    }

    std::shared_ptr<ast::Param> Parser::parseParam() {
        std::string name(lexer.getIdentifier());
        auto loc = lexer.getLastLocation();
        lexer.consume(tok_name);
        if (lexer.getCurToken() != tok_colon)
            return parseError<ast::Param>(":" "after parameter name");
        lexer.getNextToken();
        auto ty = parseType();
        return std::make_shared<ast::Param>(ty, loc, name);
    }

    std::shared_ptr<ast::Type> Parser::parseType() {
        std::shared_ptr<ast::Type> next;
        std::shared_ptr<ast::Type> ret;
        std::vector<std::shared_ptr<ast::Type>> params;
        Token tok;
        switch (lexer.getCurToken()) {
            case tok_double:
                next = std::make_shared<ast::DoubleTy>();
                tok = lexer.getNextToken();
                if (tok == tok_lbracket) {
                    lexer.consumeBracketPair();
                    return std::make_shared<ast::ArrayTy>(next);
                }
                return next;
            case tok_string:
                next = std::make_shared<ast::StringTy>(true);
                tok = lexer.getNextToken();
                if (tok == tok_lbracket) {
                    lexer.consumeBracketPair();
                    return std::make_shared<ast::ArrayTy>(next);
                }
                return next;
            case tok_void:
                lexer.getNextToken();
                return std::make_shared<ast::VoidTy>();
            case tok_complex:
                lexer.getNextToken();
                return std::make_shared<ast::ComplexTy>();
            case tok_lparen:
                params = parseTypeTuple();
                lexer.consume(tok_fatarrow);
                ret = parseType();
                return std::make_shared<ast::ClosureTy>(params, ret);
            case tok_bool:
                tok = lexer.getNextToken();
                if (tok == tok_lbracket) {
                    next = std::make_shared<ast::BoolTy>(true);
                    return std::make_shared<ast::ArrayTy>(next);
                }
                if (tok == tok_lt) {
                    next = std::make_shared<ast::BoolTy>(false);
                    lexer.consume(tok_lt);
                    assert((int )lexer.getValue() == 1 && "quantum bool type size must 1");
                    lexer.getNextToken();
                    lexer.consume(tok_gt);
                    if (lexer.getCurToken() == tok_lbracket) {
                        lexer.consumeBracketPair();
                        return std::make_shared<ast::ArrayTy>(next);
                    }
                    return next;
                }
                return std::make_shared<ast::BoolTy>(true);
            case tok_int:
                tok = lexer.getNextToken();
                if (tok == tok_lbracket) {
                    next = std::make_shared<ast::IntTy>();
                    return std::make_shared<ast::ArrayTy>(next);
                }
                if (tok == tok_lt) {
                    lexer.consume(tok_lt);
                    next = std::make_shared<ast::IntTy>(false, (int)lexer.getValue());
                    lexer.getNextToken();
                    lexer.consume(tok_gt);
                    if (lexer.getCurToken() == tok_lbracket) {
                        lexer.consumeBracketPair();
                        return std::make_shared<ast::ArrayTy>(next);
                    }
                    return next;
                }
                return std::make_shared<ast::IntTy>();
            case tok_uint:
                tok = lexer.getNextToken();
                if (tok == tok_lbracket) {
                    next = std::make_shared<ast::UintTy>();
                    return std::make_shared<ast::ArrayTy>(next);
                }
                if (tok == tok_lt) {
                    lexer.consume(tok_lt);
                    next = std::make_shared<ast::UintTy>(false, (int)lexer.getValue());
                    lexer.getNextToken();
                    lexer.consume(tok_gt);
                    if (lexer.getCurToken() == tok_lbracket) {
                        lexer.consumeBracketPair();
                        return std::make_shared<ast::ArrayTy>(next);
                    }
                    return next;
                }
                return std::make_shared<ast::UintTy>();
            default:
                return parseError<ast::Type>("type", ", quint just support double, string, array, complex, func, "
                                                     "bool, int, uint");
        }
    }

    std::vector<std::shared_ptr<ast::Type>> Parser::parseTypeTuple() {
        lexer.consume(tok_lparen);
        std::vector<std::shared_ptr<ast::Type>> tys;
        while (true) {
            auto ty = parseType();
            tys.push_back(ty);
            if (lexer.getCurToken() != tok_comma)
                break;
            lexer.consume(tok_comma);
        }
        lexer.consume(tok_rparen);
        return tys;
    }

    std::shared_ptr<ast::Stmt> Parser::parseStmt() {
        Token tok = lexer.getCurToken();
        if (tok == tok_let)
            return parseDecl();
        if (tok == tok_return)
            return parseReturn();
        if (tok == tok_break)
            return parseBreak();
        if (tok == tok_continue)
            return parseContinue();
        if (tok == tok_lbrace)
            return parseCompound();
        if (tok == tok_if)
            return parseIfStmt();
        if (tok == tok_while)
            return parseWhile();
        if (tok == tok_for)
            return parseForStmt();
        if (tok == tok_try)
            return parseTryStmt();
        if (tok == tok_panic)
            return parsePanic();
        return parseExprStmt();
    }

    std::shared_ptr<ast::DeclStmt> Parser::parseDecl() {
        auto loc = lexer.getLocation();
        std::shared_ptr<ast::Type> ty = nullptr;
        std::shared_ptr<ast::Expr> expr = nullptr;
        lexer.consume(tok_let);
        if (lexer.getCurToken() != tok_name)
            return parseError<ast::DeclStmt>("variable name", "in variable declaration");
        std::string id(lexer.getIdentifier());
        auto name = std::make_shared<ast::IdentifierExpr>(lexer.getLocation(), id);
        Token tok = lexer.getNextToken();
        if (tok == tok_colon) {
            lexer.getNextToken();
             ty = parseType();
        }
        if (tok == tok_assign) {
            lexer.getNextToken();
            expr = parseExpr();
        }
        lexer.consume(tok_semicolon);
        return std::make_shared<ast::DeclStmt>(loc, name, expr, ty);
    }

    std::shared_ptr<ast::BreakStmt> Parser::parseBreak() {
        auto loc = lexer.getLocation();
        lexer.consume(tok_break);
        if (lexer.getCurToken() != tok_semicolon)
            return parseError<ast::BreakStmt>(";", "after break");
        lexer.consume(tok_semicolon);
        return std::make_shared<ast::BreakStmt>(loc);
    }

    std::shared_ptr<ast::ContinueStmt> Parser::parseContinue() {
        auto loc = lexer.getLocation();
        lexer.consume(tok_continue);
        if (lexer.getCurToken() != tok_semicolon)
            return parseError<ast::ContinueStmt>(";", "after break");
        lexer.consume(tok_semicolon);
        return std::make_shared<ast::ContinueStmt>(loc);
    }

    std::shared_ptr<ast::ReturnStmt> Parser::parseReturn() {
        auto loc = lexer.getLocation();
        lexer.consume(tok_return);
        if (lexer.getCurToken() != tok_semicolon) {
            auto expr = parseExpr();
            lexer.consume(tok_semicolon);
            return std::make_shared<ast::ReturnStmt>(loc, expr);
        }
        return std::make_shared<ast::ReturnStmt>(loc);
    }

    std::shared_ptr<ast::ExprStmt> Parser::parseExprStmt() {
        auto loc = lexer.getLocation();
        auto expr = parseExpr();
        lexer.consume(tok_semicolon);
        return std::make_shared<ast::ExprStmt>(loc, expr);
    }

    std::shared_ptr<ast::TryStmt> Parser::parseTryStmt() {
        auto loc = lexer.getLocation();
        lexer.consume(tok_try);

        auto suite = parseCompound();

        std::vector<ast::TryStmt::Catch> catches;
        while (lexer.getCurToken() != tok_catch) {
            lexer.getNextToken();
            lexer.consume(tok_lparen);
            if (lexer.getCurToken() != tok_name)
                return parseError<ast::TryStmt>("identifier", "after catch");
            std::string var(lexer.getIdentifier());
            lexer.getNextToken();
            lexer.consume(tok_rparen);
            auto catchSuite = parseCompound();
            catches.push_back({var, nullptr, catchSuite});
        }
        return std::make_shared<ast::TryStmt>(loc, suite, catches);
    }

    std::shared_ptr<ast::SuiteStmt> Parser::parseCompound() {
        auto loc = lexer.getLocation();
        std::vector<std::shared_ptr<ast::Stmt>> stmts;
        lexer.consume(tok_lbrace);

        while (lexer.getCurToken() != tok_rbrace && lexer.getCurToken() != tok_eof)
        {
            auto stmt = parseStmt();
            if (!stmt)
                return nullptr;
            stmts.push_back(stmt);
        }
        lexer.consume(tok_rbrace);
        return std::make_shared<ast::SuiteStmt>(loc, stmts);
    }

    std::shared_ptr<ast::IfStmt> Parser::parseIfStmt() {
        auto loc = lexer.getLocation();
        std::shared_ptr<ast::Stmt> falseSuite;
        lexer.consume(tok_if);

        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::IfStmt>("(", "after if");
        lexer.getNextToken();

        auto cond = parseExpr();
        lexer.consume(tok_rparen);

        auto trueSuite = parseCompound();

        if (lexer.getCurToken() == tok_elif) {
            falseSuite = parseElif();
        }

        if (lexer.getCurToken() == tok_else) {
            falseSuite = parseCompound();
        }
        return std::make_shared<ast::IfStmt>(loc, cond, trueSuite, falseSuite);
    }

    std::shared_ptr<ast::IfStmt> Parser::parseElif() {
        auto loc = lexer.getLocation();
        std::shared_ptr<ast::Stmt> elseSuite;
        lexer.consume(tok_elif);

        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::IfStmt>("(", "after if");
        lexer.getNextToken();

        auto cond = parseExpr();
        if (lexer.getCurToken() != tok_rparen)
            return parseError<ast::IfStmt>(")", "to end if boolean expression");
        lexer.getNextToken();

        auto trueSuite = parseCompound();

        if (lexer.getCurToken() == tok_elif) {
            elseSuite = parseElif();
        }

        if (lexer.getCurToken() == tok_else) {
            lexer.getNextToken();
            elseSuite = parseCompound();
        }

        return std::make_shared<ast::IfStmt>(loc, cond, trueSuite, elseSuite);
    }

    std::shared_ptr<ast::WhileStmt> Parser::parseWhile() {
        auto loc = lexer.getLocation();

        lexer.consume(tok_while);

        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::WhileStmt>("(", "after while");
        lexer.getNextToken();

        auto cond = parseExpr();

        if (lexer.getCurToken() != tok_rparen)
            return parseError<ast::WhileStmt>(")", "to end while bool expression");
        lexer.getNextToken();

        auto suite = parseCompound();
        return std::make_shared<ast::WhileStmt>(loc, cond, suite);
    }

    std::shared_ptr<ast::ForStmt> Parser::parseForStmt() {
        auto loc = lexer.getLocation();

        lexer.consume(tok_for);

        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::ForStmt>("(", "after for");
        lexer.getNextToken();

        if (lexer.getCurToken() != tok_name)
            return parseError<ast::ForStmt>("identifier", "in for statement");
        std::string name(lexer.getIdentifier());
        auto var = std::make_shared<ast::IdentifierExpr>(lexer.getLocation(), name);

        lexer.getNextToken();
        lexer.consume(tok_colon);

        auto iter = parseExpr();

        if (lexer.getCurToken() != tok_rparen)
            return parseError<ast::ForStmt>(")", "to end for bool expression");
        lexer.getNextToken();

        auto suite = parseCompound();
        return std::make_shared<ast::ForStmt>(loc, var, iter, suite);
    }

    std::shared_ptr<ast::ThrowStmt> Parser::parsePanic() {
        auto loc = lexer.getLocation();

        lexer.consume(tok_panic);

        auto expr = parseExpr();
        return std::make_shared<ast::ThrowStmt>(loc, expr);
    }

    std::shared_ptr<ast::Expr> Parser::parseExpr() {
        auto loc = lexer.getLocation();

        if (lexer.getCurToken() == tok_lambda) {
            return parseLambdaExpr();
        }
        auto left = parseOrExpr();
        std::shared_ptr<ast::Expr> right;

        switch (lexer.getCurToken()) {
            default:
                return left;
            case tok_assign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, assign, right);
            case tok_aassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, plus, right);
            case tok_sassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, minus, right);
            case tok_massign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, star, right);
            case tok_dassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, slash, right);
            case tok_passign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, percent, right);
            case tok_augassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, ampersand, right);
            case tok_vassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, vbar, right);
            case tok_cassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, caret, right);
            case tok_rassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, lshift, right);
            case tok_lassign:
                lexer.getNextToken();
                right = parseOrExpr();
                return std::make_shared<ast::AssignExpr>(loc, left, rshift, right);
        }
    }

    std::shared_ptr<ast::Expr> Parser::parseLambdaExpr() {
        auto loc = lexer.getLocation();

        std::vector<std::shared_ptr<ast::Param>> args;
        std::shared_ptr<ast::Type> ret;
        lexer.consume(tok_lambda);
        if (lexer.getCurToken() != tok_lparen)
            return parseError<ast::Expr>("(", "after function name");
        lexer.getNextToken();

        if (lexer.getCurToken() != tok_rparen) {
            args = parseParameters();
        }
        if (lexer.getCurToken() != tok_rparen)
            return parseError<ast::Expr>(")", "to end function parameters");
        lexer.getNextToken();

        if (lexer.getCurToken() == tok_arrow) {
            lexer.getNextToken();
            ret = parseType();
        }

        auto suite = parseCompound();
        return std::make_shared<ast::LambdaExpr>(loc, args, ret, suite);
    }

    std::shared_ptr<ast::Expr> Parser::parseOrExpr() {
        auto loc = lexer.getLocation();
        auto left = parseAndExpr();
        if (lexer.getCurToken() == tok_lor) {
            auto right = parseOrExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, lor, right);
        }

        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseAndExpr() {
        auto loc = lexer.getLocation();
        auto left = parseNotExpr();
        if (lexer.getCurToken() == tok_land) {
            auto right = parseAndExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, land, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseNotExpr() {
        if (lexer.getCurToken() == tok_not) {
            auto loc = lexer.getLocation();
            auto expr = parseNotExpr();
            return std::make_shared<ast::UnaryExpr>(loc, bang, expr);
        }
        return parseCompareExpr();
    }

    std::shared_ptr<ast::Expr> Parser::parseCompareExpr() {
        auto loc = lexer.getLocation();
        auto left = parseVBarExpr();
        std::shared_ptr<ast::Expr> right;
        switch (lexer.getCurToken()) {
            default:
                return left;
            case tok_lt:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, lt, right);
            case tok_le:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, le, right);
            case tok_eq:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, eq, right);
            case tok_gt:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, gt, right);
            case tok_ge:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, ge, right);
            case tok_notequal:
                lexer.getNextToken();
                right = parseVBarExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, notequal, right);
        }
    }

    std::shared_ptr<ast::Expr> Parser::parseVBarExpr() {
        auto loc = lexer.getLocation();
        auto left = parseXorExpr();
        if (lexer.getCurToken() == tok_vbar) {
            auto right = parseVBarExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, OR, right);
        }

        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseXorExpr() {
        auto loc = lexer.getLocation();
        auto left = parseCaretExpr();
        if (lexer.getCurToken() == tok_caret) {
            auto right = parseXorExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, XOR, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseCaretExpr() {
        auto loc = lexer.getLocation();
        auto left = parseShiftExpr();
        if (lexer.getCurToken() == tok_amp) {
            auto right = parseCaretExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, AND, right);
        }

        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseShiftExpr() {
        auto loc = lexer.getLocation();
        auto left = parseArithExpr();
        auto tok = lexer.getCurToken();
        if (tok == tok_lshift || tok == rshift) {
            auto right = parseShiftExpr();
            binary_op op;
            if (tok == tok_lshift)
                op = ls;
            else
                op = rs;
            return std::make_shared<ast::BinaryExpr>(loc, left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseArithExpr() {
        auto loc = lexer.getLocation();
        auto left = parseTermExpr();

        if (lexer.getCurToken() == tok_plus || lexer.getCurToken() == tok_minus) {
            binary_op op;
            if (lexer.getCurToken() == tok_plus)
                op = add;
            else
                op = sub;
            auto right = parseArithExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseTermExpr() {
        auto loc = lexer.getLocation();
        auto left = parseFactorExpr();
        std::shared_ptr<ast::Expr> right;
        switch (lexer.getCurToken()) {
            default:
                return left;
            case tok_star:
                lexer.getNextToken();
                right = parseTermExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, multiply, right);
            case tok_slash:
                lexer.getNextToken();
                right = parseTermExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, div, right);
            case tok_percent:
                lexer.getNextToken();
                right = parseTermExpr();
                return std::make_shared<ast::BinaryExpr>(loc, left, remainder, right);
        }
    }

    std::shared_ptr<ast::Expr> Parser::parseFactorExpr() {
        if (lexer.getCurToken() == tok_minus || lexer.getCurToken() == tok_tilde) {
            unary_op op;
            if (lexer.getCurToken() == tok_minus)
                op = min;
            else
                op = tilde;
            auto loc = lexer.getLocation();
            auto expr = parseFactorExpr();
            return std::make_shared<ast::UnaryExpr>(loc, op, expr);
        }
        return parsePower();
    }

    std::shared_ptr<ast::Expr> Parser::parsePower() {
        auto loc = lexer.getLocation();
        auto left = parseAtomExpr();

        if (lexer.getCurToken() == tok_pow) {
            lexer.getNextToken();
            auto right = parseFactorExpr();
            return std::make_shared<ast::BinaryExpr>(loc, left, pow, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expr> Parser::parseSlice() {
        auto loc = lexer.getLocation();
        if (lexer.getCurToken() == tok_colon) {
            if (lexer.getNextToken() == tok_colon) {
                if (lexer.getNextToken() == tok_rbracket)
                    return std::make_shared<ast::SliceExpr>(loc, nullptr, nullptr, nullptr);
                auto step = parseExpr();
                return std::make_shared<ast::SliceExpr>(loc, nullptr, nullptr, step);
            }
            auto stop = parseExpr();
            if (lexer.getCurToken() == tok_colon) {
                if (lexer.getNextToken() == tok_rbracket)
                    return std::make_shared<ast::SliceExpr>(loc, nullptr, stop, nullptr);
                auto step = parseExpr();
                return std::make_shared<ast::SliceExpr>(loc, nullptr, stop, step);
            }
            return std::make_shared<ast::SliceExpr>(loc, nullptr, stop, nullptr);
        }

        if (lexer.getCurToken() == tok_rbracket)
            return parseError<ast::Expr>("expression", ", slice can't be empty");

        auto start = parseExpr();

        if (lexer.getCurToken() == tok_colon) {
            if (lexer.getNextToken() == tok_colon) {
                if (lexer.getNextToken() == tok_rbracket)
                    return std::make_shared<ast::SliceExpr>(loc, start, nullptr, nullptr);
                auto step = parseExpr();
                return std::make_shared<ast::SliceExpr>(loc, start, nullptr, step);
            }
            auto stop = parseExpr();
            if (lexer.getCurToken() == tok_colon) {
                if (lexer.getNextToken() == tok_rbracket)
                    return std::make_shared<ast::SliceExpr>(loc, start, stop, nullptr);
                auto step = parseExpr();
                return std::make_shared<ast::SliceExpr>(loc, start, stop, step);
            }
            return std::make_shared<ast::SliceExpr>(loc, start, stop, nullptr);
        }
        return std::make_shared<ast::SliceExpr>(loc, start, nullptr, nullptr);
    }

    std::shared_ptr<ast::Expr> Parser::parseAtomExpr() {
        auto loc = lexer.getLocation();
        auto first = parseAtom();

        if (lexer.getCurToken() == tok_lparen) {
            lexer.getNextToken();
            std::vector<std::shared_ptr<ast::Expr>> args;
            if (lexer.getCurToken() != tok_rparen)
                args = parseExprList();
            if (lexer.getCurToken() != tok_rparen)
                return parseError<ast::Expr>(")", "to end arg list");
            lexer.consume(tok_rparen);
            return std::make_shared<ast::CallExpr>(loc, first, args);
        }

        if (lexer.getCurToken() == tok_lbracket) {
            lexer.getNextToken();
            auto slice = parseSlice();
            lexer.consume(tok_rbracket);
            return std::make_shared<ast::IndexExpr>(loc, first, slice);
        }

        if (lexer.getCurToken() == tok_dot) {
            lexer.getNextToken();
            if (lexer.getCurToken() != tok_name)
                return parseError<ast::Expr>("identifier", "in variable attribution expr");
            std::string name(lexer.getIdentifier());
            return std::make_shared<ast::TrailerExpr>(loc, first, name);
        }
        return first;
    }

    std::shared_ptr<ast::Expr> Parser::parseAtom() {
        auto loc = lexer.getLocation();
        if (lexer.getCurToken() == tok_name) {
            std::string name(lexer.getIdentifier());
            lexer.getNextToken();
            return std::make_shared<ast::IdentifierExpr>(loc, name);
        }
        if (lexer.getCurToken() == tok_number) {
            double val = lexer.getValue();
            int intPtr = static_cast<int>(val);
            if (val == static_cast<double>(intPtr)) {
                return std::make_shared<ast::ConstantExpr>(loc, intPtr);
            } else {
                return std::make_shared<ast::ConstantExpr>(loc, val);
            }
        }
        if (lexer.getCurToken() == tok_true || lexer.getCurToken() == tok_false) {
            if (lexer.getCurToken() == tok_true)
                return std::make_shared<ast::ConstantExpr>(loc, true);
            return std::make_shared<ast::ConstantExpr>(loc, false);
        }
        if (lexer.getCurToken() == tok_chars) {
            std::string value(lexer.getIdentifier());
            return std::make_shared<ast::StringExpr>(loc, value);
        }
        if (lexer.getCurToken() == tok_lparen) {
            lexer.getNextToken();
            auto items = parseExprList();
            if (lexer.getCurToken() != tok_rparen)
                return parseError<ast::Expr>(")", "to end expr list");
            lexer.consume(tok_rparen);
            return std::make_shared<ast::TupleExpr>(loc, items);
        }

        if (lexer.getCurToken() == tok_lbracket) {
            lexer.getNextToken();
            auto items = parseExprList();
            if (lexer.getCurToken() != tok_rbracket)
                return parseError<ast::Expr>("]", "to end expr list");
            lexer.consume(tok_rbracket);
            return std::make_shared<ast::ListExpr>(loc, items);
        }
        return parseError<ast::Expr>("atom", "in expression");
    }

    std::vector<std::shared_ptr<ast::Expr>> Parser::parseExprList() {
        std::vector<std::shared_ptr<ast::Expr>> expressions;
        do
        {
            auto expr = parseExpr();
            expressions.push_back(expr);
            if (lexer.getCurToken() != tok_comma)
                break;
            lexer.consume(tok_comma);
        } while (true);
        return expressions;
    }

}