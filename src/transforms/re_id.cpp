//
// Created by BY210033 on 2023/3/31.
//
#include "ir/transforms.h"
#include "ir/visitors.h"

namespace quint::lang {

    class ReId : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;

        int id_counter;

        ReId() : id_counter(0) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void re_id(Stmt *stmt) {
            stmt->id = id_counter++;
        }

        void preprocess_container_stmt(Stmt *stmt) override {
            re_id(stmt);
        }

        void visit(Stmt *stmt) override {
            re_id(stmt);
        }

        static void run(IRNode *node) {
            ReId instance;
            node->accept(&instance);
        }
    };

    namespace irpass {
        void re_id(IRNode *root) {
            ReId::run(root);
        }
    }
}