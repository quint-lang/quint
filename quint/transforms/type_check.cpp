//
// Created by BY210033 on 2023/4/9.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/frontend_ir.h"
#include "quint/analysis/analysis.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    static_assert(sizeof(real) == sizeof(float32),
            "Please build the quint compiler with single precision (QUINT_USE_DOUBLE=0)");

    class TypeCheck : public IRVisitor {
    private:
        CompileConfig config_;

        Type *type_check_store(Stmt *stmt,
                               Stmt *dst,
                               Stmt *&val,
                               const std::string &stmt_name) {
            auto dst_type = dst->ret_type.ptr_removed();
            // todo handler quant
//            if ()
            if (dst_type != val->ret_type) {
                auto promoted = promoted_type(dst_type, val->ret_type);
                if (dst_type != promoted) {
                    QUINT_WARN("[{}] {} may lose precision: {} <- {}\n{}", stmt->name(),
                               stmt_name, dst_type.to_string(), val->ret_data_type_name(),
                               stmt->tb);
                }
                val = insert_type_cast_before(stmt, val, dst_type);
            }
            return dst_type;
        }

    public:
        explicit TypeCheck(const CompileConfig &config) : config_(config) {
            allow_undefined_visitor = true;
        }

        Stmt *insert_type_cast_before(Stmt *anchor,
                                      Stmt *input,
                                      DataType out_type) {
//            auto &&cast_stmt =
//                    Stmt::make_typed<>()
            return nullptr;
        }

    public:
        void visit(Stmt *stmt) override {

        }

        void visit(Block *stmt) override {

        }

    };

    namespace irpass {
        void type_check(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            analysis::check_fields_registered(root);
            TypeCheck inst(config);
            root->accept(&inst);
        }
    }

}