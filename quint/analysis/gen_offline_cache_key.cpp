//
// Created by BY210033 on 2023/3/31.
//
#include "quint/analysis/offline_cache_util.h"

#include "quint/ir/expr.h"
#include "quint/ir/frontend_ir.h"
#include "quint/ir/ir.h"
#include "quint/ir/type.h"
#include "quint/program/program.h"

namespace quint::lang {

    class ASTSerializer : public IRVisitor, ExpressionVisitor {
    private:
        using ExpressionVisitor::visit;
        using IRVisitor::visit;

    public:
        ASTSerializer(Program *prog, std::ostream *os)
            : ExpressionVisitor(true), prog_(prog), os_(os) {
            this->allow_undefined_visitor = true;
        }

        void visit(Stmt *stmt) override {
            IRVisitor::visit(stmt);
        }

        void visit(Block *stmt) override {
            IRVisitor::visit(stmt);
        }

        void visit(ExprGroup &exprGroup) override {

        }

        void visit(Expression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(ArgLoadExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(IdExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        void visit(ConstExpression *expr) override {
            ExpressionVisitor::visit(expr);
        }

        static void run(Program *program, IRNode *ast, std::ostream *os) {
            ASTSerializer serializer(program, os);
            ast->accept(&serializer);
            serializer.emit_dependencies();
        }

    private:
        void emit_dependencies() {

        }

        Program *prog_{nullptr};
        std::ostream  *os_{nullptr};
        std::unordered_set<SNode *> snode_tree_roots_;
        std::vector<char> string_pool_;
    };

    void gen_offline_cache_key(Program *prog, IRNode *ast, std::ostream *os) {
        ASTSerializer::run(prog, ast, os);
    }

}