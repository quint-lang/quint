//
// Created by BY210033 on 2023/4/11.
//
#include "quint/program/program.h"
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/type_factory.h"
#include "quint/ir/visitors.h"
#include "quint/ir/statements.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class BitLoopVectorize : public IRVisitor {
    public:
        bool is_bit_vectorized;
        bool is_struct_for_loop;
        StructForStmt *loop_stmt;
        PrimitiveType *quant_array_physical_type;
        std::unordered_map<Stmt *, std::vector<Stmt *>> transformed_atomics;

        BitLoopVectorize() {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
            is_bit_vectorized = false;
            is_struct_for_loop = false;
            loop_stmt = nullptr;
            quant_array_physical_type = nullptr;
        }

        void visit(Block *stmt) override {

        }

        static void run(IRNode *root) {
            BitLoopVectorize bit_loop_vectorize;
            root->accept(&bit_loop_vectorize);
        }
    };

    namespace irpass {
        void bit_loop_vectorize(IRNode *root) {
            QUINT_AUTO_PROF
            BitLoopVectorize::run(root);
            die(root);
        }
    }

}