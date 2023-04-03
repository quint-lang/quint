//
// Created by BY210033 on 2023/3/31.
//
#include "analysis/analysis.h"

namespace quint::lang {

    class IRCloner : public IRVisitor {
    private:
        IRNode *other_node;
        std::unordered_map<Stmt *, Stmt *> operand_map_;

    public:
        enum Phase { register_operand_map, replace_operand } phase;

        explicit IRCloner(IRNode *other_node)
                : other_node(other_node), phase(register_operand_map) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void visit(Stmt *stmt) override {
            IRVisitor::visit(stmt);
        }

        void visit(Block *stmt) override {
            IRVisitor::visit(stmt);
        }

        static std::unique_ptr<IRNode> run(IRNode *root) {
            std::unique_ptr<IRNode> new_root = root->clone();
            IRCloner cloner(new_root.get());
            cloner.phase = IRCloner::register_operand_map;
            root->accept(&cloner);
            cloner.phase = IRCloner::replace_operand;
            root->accept(&cloner);

            return new_root;
        }

    };


    namespace irpass::analysis {
        std::unique_ptr<IRNode> clone(IRNode *root) {
            return IRCloner::run(root);
        }
    }
}