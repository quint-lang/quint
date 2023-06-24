//
// Created by BY210033 on 2023/5/25.
//
#include <utility>

#include "ast/expr.h"
#include "ast/stmt.h"

namespace quint::ast {

    Expr::Expr() {

    }

    void Expr::validate() const {

    }

    bool Expr::hasAttr(int attr) const {
        return false;
    }

    void Expr::setAttr(int attr) {

    }

    std::string Expr::getTypeName() {
        return std::string();
    }

    std::string Expr::wrapType(const std::string &s) const {
        return std::string();
    }

    AssignExpr::AssignExpr(const Location &loc, ExprPtr left, aug_assign op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)), Expr(loc) {}

    AssignExpr::AssignExpr(const AssignExpr &expr) {

    }

    ExprPtr AssignExpr::getLeft() {
        return left;
    }

    ExprPtr AssignExpr::getRight() {
        return right;
    }

    aug_assign AssignExpr::getOp() {
        return op;
    }

    std::string AssignExpr::toString() const {
        return Expr::toString();
    }

    void AssignExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    IdentifierExpr::IdentifierExpr(const Location &loc, const std::string &value)
        : value(value), Expr(loc) {}

    std::string IdentifierExpr::toString() const {
        return Expr::toString();
    }

    void IdentifierExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    bool IdentifierExpr::isId(const std::string &val) const {
        return this->value == val;
    }

    CallExpr::CallExpr(const Location& loc, ExprPtr expr, std::vector<ExprPtr> args)
        : expr(std::move(expr)), args(std::move(args)), Expr(loc) {}

    CallExpr::CallExpr(const CallExpr &expr) {

    }

    void CallExpr::validate() const {
        Expr::validate();
    }

    std::string CallExpr::toString() const {
        return Expr::toString();
    }

    void CallExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    ExprPtr CallExpr::getExpr() {
        return expr;
    }

    std::vector<ExprPtr> CallExpr::getArgs() {
        return args;
    }

    bool CallExpr::isOrdered() {
        return ordered;
    }

    ConstantExpr::ConstantExpr(const Location &loc, double value)
        : value(value), isDouble(true), Expr(loc) {}

    ConstantExpr::ConstantExpr(const Location &loc, int value)
        : num(value), isInt(true), Expr(loc) {}

    ConstantExpr::ConstantExpr(const Location &loc, bool value)
        : boolean(value), isBool(true), Expr(loc) {}

    ConstantExpr::ConstantExpr(const ConstantExpr &other) {

    }

    bool ConstantExpr::isBoolean() {
        return boolean;
    }

    double ConstantExpr::getValue() {
        return value;
    }

    int ConstantExpr::getNum() {
        return num;
    }

    std::string ConstantExpr::toString() const {
        return Expr::toString();
    }

    void ConstantExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    IndexExpr::IndexExpr(const Location &loc, ExprPtr expr, ExprPtr index)
        : expr(std::move(expr)), index(std::move(index)), Expr(loc) {}

    IndexExpr::IndexExpr(const IndexExpr &other) {

    }

    ExprPtr IndexExpr::getExpr() {
        return expr;
    }

    ExprPtr IndexExpr::getIndex1() {
        return index;
    }

    std::string IndexExpr::toString() const {
        return Expr::toString();
    }

    void IndexExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    ListExpr::ListExpr(const Location &loc, std::vector<ExprPtr> items)
        : items(std::move(items)), Expr(loc) {}

    ListExpr::ListExpr(const ListExpr &other) {

    }

    std::vector<ExprPtr> ListExpr::getItems() {
        return items;
    }

    std::string ListExpr::toString() const {
        return Expr::toString();
    }

    void ListExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    TupleExpr::TupleExpr(const Location &loc, std::vector<ExprPtr> items)
        : items(std::move(items)), Expr(loc) {}

    TupleExpr::TupleExpr(const ListExpr &other) {

    }

    std::vector<ExprPtr> TupleExpr::getItems() {
        return items;
    }

    std::string TupleExpr::toString() const {
        return Expr::toString();
    }

    void TupleExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    UnaryExpr::UnaryExpr(const Location &loc, unary_op op, ExprPtr expr)
        : op(op), expr(std::move(expr)), Expr(loc) {}

    UnaryExpr::UnaryExpr(const UnaryExpr &other) {

    }

    ExprPtr UnaryExpr::getExpr() {
        return expr;
    }

    unary_op UnaryExpr::getOp() {
        return op;
    }

    std::string UnaryExpr::toString() const {
        return Expr::toString();
    }

    void UnaryExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    StringExpr::StringExpr(const Location &loc, const std::string &value)
        : value(value), Expr(loc) {}

    StringExpr::StringExpr(const StringExpr &other) {

    }

    std::string StringExpr::getValue() {
        return value;
    }

    std::string StringExpr::toString() const {
        return Expr::toString();
    }

    void StringExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    RecordExpr::RecordExpr(const Location &loc, std::vector<ExprPtr> items)
        : items(std::move(items)), Expr(loc) {}

    RecordExpr::RecordExpr(const ListExpr &other) {

    }

    std::vector<ExprPtr> RecordExpr::getItems() {
        return items;
    }

    std::string RecordExpr::toString() const {
        return Expr::toString();
    }

    void RecordExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    SliceExpr::SliceExpr(const Location &loc, ExprPtr start, ExprPtr stop, ExprPtr step)
        : start(std::move(start)), stop(std::move(stop)), step(std::move(step)), Expr(loc) {}

    SliceExpr::SliceExpr(const SliceExpr &other) {

    }

    ExprPtr SliceExpr::getStart() {
        return start;
    }

    ExprPtr SliceExpr::getStop() {
        return stop;
    }

    ExprPtr SliceExpr::getStep() {
        return step;
    }

    std::string SliceExpr::toString() const {
        return Expr::toString();
    }

    void SliceExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    Param::Param(std::shared_ptr<Type> type, Location location, std::string name, ExprPtr defaultValue, int generic)
        : type(std::move(type)), location(location), name(name), defaultValue(std::move(defaultValue)) {
    }

    std::string Param::toString() const {
        return "";
    }

    Param *Param::clone() {
        return this;
    }

    BinaryExpr::BinaryExpr(const Location &loc, ExprPtr left, binary_op op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)), Expr(loc) {}

    BinaryExpr::BinaryExpr(const BinaryExpr &expr) {

    }

    std::string BinaryExpr::toString() const {
        return Expr::toString();
    }

    void BinaryExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    ExprPtr BinaryExpr::getLeft() {
        return left;
    }

    ExprPtr BinaryExpr::getRight() {
        return right;
    }

    binary_op BinaryExpr::getOp() {
        return op;
    }

    TrailerExpr::TrailerExpr(const Location &loc, ExprPtr expr, const std::string &member)
        : expr(std::move(expr)), member(member), Expr(loc) {}

    TrailerExpr::TrailerExpr(const TrailerExpr &other) {

    }

    ExprPtr TrailerExpr::getExpr() {
        return expr;
    }

    std::string TrailerExpr::getMember() {
        return member;
    }

    std::string TrailerExpr::toString() const {
        return Expr::toString();
    }

    void TrailerExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

    LambdaExpr::LambdaExpr(const Location &loc, std::vector<std::shared_ptr<Param>> args, std::shared_ptr<Type> ret,
                           std::shared_ptr<Stmt> suite)
        : args(std::move(args)), ret(std::move(ret)), suite(std::move(suite)), Expr(loc) {}

    LambdaExpr::LambdaExpr(const LambdaExpr &other) {

    }

    std::string LambdaExpr::toString() const {
        return Expr::toString();
    }

    void LambdaExpr::accept(ASTVisitor &visitor) {
        Expr::accept(visitor);
    }

}