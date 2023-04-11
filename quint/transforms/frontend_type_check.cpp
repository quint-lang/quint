//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/frontend_ir.h"
#include "quint/ir/statements.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class FrontendTypeCheck : public IRVisitor {
        void check_out_type(const Expr &cond, std::string stmt_name) {
            if (!cond->ret_type->is<PrimitiveType>() || !is_integral(cond->ret_type))
                throw QuintTypeError(fmt::format(
                        "`{0}` conditions must be an integer; found {1}. Consider using "
                        "`{0} x != 0` instead of `{0} x` for float values.",
                        stmt_name, cond->ret_type.to_string()));
        }

    public:
        explicit FrontendTypeCheck() {
            allow_undefined_visitor = true;
        }

        void visit(Stmt *stmt) override {

        }

        void visit(Block *stmt) override {

        }

    };

    namespace irpass {
        void fronted_type_check(IRNode *root) {
            QUINT_AUTO_PROF
            FrontendTypeCheck checker;
            root->accept(&checker);
        }
    }

}