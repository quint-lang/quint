//
// Created by BY210033 on 2023/2/13.
//

#ifndef QUINT_QUANTUM_WALK_H
#define QUINT_QUANTUM_WALK_H

#include "general.h"

namespace quint {

    struct CondRot_General_Bool_QW : Operation {
        int j;
        int k;
        int in;
        int out;

        CondRot_General_Bool_QW(Register& j, Register& k, Register& in, Register& out,
                                size_t l, size_t r);

        void operator()(std::vector<System>& state);
    };

    struct SparseMatrixOracle1 : Function {
        QRAMCircuit* qram;
        int reg_offset;
        int reg_row;
        int reg_col_id;
        int reg_output;
        size_t row_size;

        SparseMatrixOracle1(QRAMCircuit* qram,
                            Register& reg_offset,
                            Register& reg_row,
                            Register& reg_col_id,
                            Register& reg_output,
                            size_t row_size);

        void operator()(Module &mod) override;
    };

    struct SparseMatrixOracle2 : Function {
        QRAMCircuit* qram;
        std::shared_ptr<Register> sparse_offset;
        std::shared_ptr<Register> row;
        std::shared_ptr<Register> col;
        std::shared_ptr<Register> search_res;
        size_t row_size;

        SparseMatrixOracle2(QRAMCircuit* qram,
                            Register& sparse_offset,
                            Register& row,
                            Register& col,
                            Register& search_res,
                            size_t row_size);

        void operator()(Module &mod) override;
    };

    struct T : Function {
        QRAMCircuit* qram;
        int reg_data_offset;
        int reg_spares_offset;
        int reg_j;
        int reg_b1;
        int reg_k;
        int reg_b2;
        int reg_search_res;
        size_t nnz_col;
        size_t data_size;
        walk_angle_function_t func;
        walk_angle_function_t func_inv;

        T(QRAMCircuit* qram, Register& reg_data_offset_,
          Register& reg_sparse_offset_, Register& reg_j_, Register& reg_b1_,
          Register& reg_k_, Register& reg_b2_, Register& reg_search_res_,
          size_t nnz_col_, size_t data_size_,
          walk_angle_function_t func_,
          walk_angle_function_t func_inv_);

        void operator()(Module &mod) override;
    };
}

#endif //QUINT_QUANTUM_WALK_H
