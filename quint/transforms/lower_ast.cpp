//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/frontend_ir.h"
#include "quint/system/profiler.h"

#include <unordered_set>

namespace quint::lang {

    namespace {
        using FlattenContext = Expression::FlattenContext;

        template<typename Vec>
        std::vector<typename Vec::value_type::pointer> make_raw_pointer_list(
                const Vec &unique_pointers) {
            std::vector<typename Vec::value_type::pointer> raw_pointers;
            for (auto &ptr : unique_pointers)
                raw_pointers.push_back(ptr.get());
            return raw_pointers;
        }
    }

    // Lower Expr tree to a bunch of binary/unary/(bin/unary) statements
    // Goal: eliminate Expression, Identifiers, and mutable local variables. Make
    // AST SSA.
    class LowerAST : public IRVisitor {
    private:
        Stmt *capturing_loop_;
        std::unordered_set<Stmt *> detected_fors_with_break_;
        Block *current_block_;
        int current_block_depth_;

        FlattenContext make_flatten_ctx() {
            FlattenContext  fctx;
            fctx.current_block = this->current_block_;
            return fctx;
        }

    public:
        explicit LowerAST(const std::unordered_set<Stmt *> &_detected_fors_with_break)
            : detected_fors_with_break_(_detected_fors_with_break),
              current_block_(nullptr),
              current_block_depth_(0) {
            allow_undefined_visitor = true;
            capturing_loop_ = nullptr;
        }

        void visit(Stmt *stmt) override {

        }

        void visit(Block *stmt) override {

        }

        static void run(IRNode *node) {
            LowerAST inst(irpass::analysis::detect_fors_with_break(node));
            node->accept(&inst);
        }
    };

    namespace irpass {
        void lower_ast(IRNode *root) {
            QUINT_AUTO_PROF
            LowerAST::run(root);
        }
    }

}
