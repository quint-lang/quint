//
// Created by BY210033 on 2023/5/26.
//
#include "ast/stmt.h"

namespace quint::ast {

    const int INDENT_SIZE = 2;

    Stmt::Stmt() {

    }

    void Stmt::validate() const {

    }

    StmtPtr ExprStmt::clone() const {
        return quint::ast::StmtPtr();
    }

    SuiteStmt::SuiteStmt(const Location &loc, std::vector<StmtPtr> stmts)
        : stmts(std::move(stmts)), Stmt(loc) {}

    SuiteStmt::SuiteStmt(const SuiteStmt &other) {

    }

    std::string SuiteStmt::toString() const {
//        std::string pad = indent >= 0 ? ("\n" + std::string(indent + INDENT_SIZE, ' ')) : "";
//        std::string s;

        return std::string();
    }

    void SuiteStmt::accept(ASTVisitor &visitor) {

    }

    ExprStmt::ExprStmt(const Location &loc, ExprPtr expr)
        : expr(std::move(expr)), Stmt(loc) {}

    ExprStmt::ExprStmt(const ExprStmt &s) {

    }

    ExprPtr ExprStmt::getExpression() {
        return expr;
    }

    std::string ExprStmt::toString() const {
        return std::string();
    }



    ForStmt::ForStmt(const Location &loc, ExprPtr var, ExprPtr iter, StmtPtr suite)
        : var(std::move(var)), iter(std::move(iter)), suite(std::move(suite)), Stmt(loc) {}

    ForStmt::ForStmt(const ForStmt &other) {

    }

    std::string ForStmt::toString() const {
        return std::string();
    }

    void ForStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr ForStmt::clone() const {
        return Stmt::clone();
    }

    IfStmt::IfStmt(const Location &loc, ExprPtr cond, StmtPtr ifSuite, StmtPtr elseSuite)
        : cond(std::move(cond)), ifSuite(std::move(ifSuite)), elseSuite(std::move(elseSuite)), Stmt(loc) {}

    IfStmt::IfStmt(const IfStmt &other) {

    }

    std::string IfStmt::toString() const {
        return std::string();
    }

    void IfStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr IfStmt::clone() const {
        return Stmt::clone();
    }

    WhileStmt::WhileStmt(const Location &loc, ExprPtr cond, StmtPtr suite)
        : cond(std::move(cond)), suite(std::move(suite)), Stmt(loc) {}

    WhileStmt::WhileStmt(const WhileStmt &other) {

    }

    void WhileStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr WhileStmt::clone() const {
        return Stmt::clone();
    }

    std::string WhileStmt::toString() const {
        return std::string();
    }

    TryStmt::Catch TryStmt::Catch::clone() const {
        return TryStmt::Catch();
    }

    TryStmt::TryStmt(const Location &loc, StmtPtr suite, std::vector<Catch> catches)
        : suite(std::move(suite)), catches(std::move(catches)) {}

    TryStmt::TryStmt(const TryStmt &other) {

    }

    std::string TryStmt::toString() const {
        return std::string();
    }

    void TryStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr TryStmt::clone() const {
        return Stmt::clone();
    }

    ReturnStmt::ReturnStmt(const Location &loc, ExprPtr expr)
        : expr(std::move(expr)), Stmt(loc) {}

    ReturnStmt::ReturnStmt(const ReturnStmt &other) {

    }

    std::string ReturnStmt::toString() const {
        return std::string();
    }

    void ReturnStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr ReturnStmt::clone() const {
        return Stmt::clone();
    }

    BreakStmt::BreakStmt(const Location &loc) : Stmt(loc) {

    }

    std::string BreakStmt::toString() const {
        return std::string();
    }

    void BreakStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr BreakStmt::clone() const {
        return Stmt::clone();
    }

    ContinueStmt::ContinueStmt(const Location &loc) : Stmt(loc) {

    }

    std::string ContinueStmt::toString() const {
        return std::string();
    }

    void ContinueStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr ContinueStmt::clone() const {
        return Stmt::clone();
    }

    ThrowStmt::ThrowStmt(const Location &loc, ExprPtr expr, bool transformed)
        : expr(std::move(expr)), transformed(transformed), Stmt(loc) {}

    ThrowStmt::ThrowStmt(const ThrowStmt &other) {

    }

    void ThrowStmt::accept(ASTVisitor &visitor) {
        Stmt::accept(visitor);
    }

    StmtPtr ThrowStmt::clone() const {
        return Stmt::clone();
    }

    std::string ThrowStmt::toString() const {
        return std::string();
    }

    StmtPtr *SuiteStmt::lastInBlock() {
        return nullptr;
    }

    void SuiteStmt::flatten(const StmtPtr &s, std::vector<StmtPtr> &stmts) {

    }

    void ExprStmt::accept(ASTVisitor &visitor) {

    }

    bool isInt(ast::Type &ty) {
        return true;
    }

    bool isUint(ast::Type &ty) {
        return true;
    }

    DeclStmt::DeclStmt(const Location &loc, ExprPtr lhs, ExprPtr rhs, std::shared_ptr<Type> type)
        : lhs(std::move(lhs)), rhs(std::move(rhs)), type(std::move(type)), Stmt(loc) {}

    DeclStmt::DeclStmt(const DeclStmt &other) {

    }

    std::string DeclStmt::toString() const {
        return std::string();
    }

    std::shared_ptr<Stmt> DeclStmt::clone() const {
        return std::shared_ptr<Stmt>();
    }

    void DeclStmt::accept(ASTVisitor &visitor) {

    }

}
