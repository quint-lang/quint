//
// Created by BY210033 on 2023/2/13.
//
#include "runtime/hvm/quantum_walk.h"
#include "runtime/hvm/qbs.h"

using namespace std;

namespace quint {

    void SparseMatrixOracle1::operator()(quint::Module &mod) {
        Function::operator()(mod);
    }

    void T::operator()(Module &mod) {
        Function::operator()(mod);
    }

    SparseMatrixOracle2::SparseMatrixOracle2(QRAMCircuit *qram, Register &sparse_offset, Register &row, Register &col,
                                             Register &search_res, size_t row_size)
        : Function("SparseMatrixOracle2", {make_shared<Register>(col), make_shared<Register>(search_res)},
                   {make_shared<Register>(row), make_shared<Register>(sparse_offset)}),
          sparse_offset(make_shared<Register>(sparse_offset)), row(make_shared<Register>(row)),
          col(make_shared<Register>(col)), search_res(make_shared<Register>(search_res)), row_size(row_size) {}

    void SparseMatrixOracle2::operator()(Module &mod) {
        // get all function argument
        auto reg_sparse_offset = *(sparse_offset.get());
        auto reg_row = *(row.get());
        auto reg_col = *(col.get());
        auto reg_search_result = *(search_res.get());

        // alloc Local register
        auto row_addr = AllocAuxiliaryRegister("row_addr", UnsignedInteger, qram->address_size)();
        mod << row_addr;

        mod | GetRowAddr(reg_sparse_offset, reg_row, row_addr, row_size)
            | QuantumBinarySearch(qram, row_addr, row_size, reg_col, reg_search_result)
            | QRAMLoad(qram, reg_search_result, reg_col)
            | Swap_General_General(reg_col, reg_search_result)
            | AddAssign_AnyInt_AnyInt(reg_col, row_addr);
     }
}
