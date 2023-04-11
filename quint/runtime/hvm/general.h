//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_GENERAL_H
#define QUINT_GENERAL_H

#include "quint/runtime/hvm/data_structure.h"
#include "quint/runtime/hvm/qram_circuit.h"

namespace quint {

    struct AllocRegister {
        std::string reg_name;
        StateStorageType type;
        size_t size;

        AllocRegister(std::string register_name_, StateStorageType type_, size_t size_);

        Register operator()() const;
    };

    struct AllocAuxiliaryRegister {
        std::string reg_name;
        StateStorageType type;
        size_t size;

        AllocAuxiliaryRegister(std::string register_name_, StateStorageType type_, size_t size_);

        Register operator()() const;
    };

    struct DeallocRegister {

    };

    struct QRAMLoad : Operation {
        QRAMCircuit* qram;
        int addr;
        int data;

        QRAMLoad(QRAMCircuit* qram, Register& reg1, Register& reg2)
            : Operation(false, "QRAMLoad", std::make_shared<Register>(reg2), {std::make_shared<Register>(reg1)}),
              qram(qram), addr(reg1.addr), data(reg2.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll

    };

    struct Assign : Operation {
        int register1;
        int register2;

        Assign(Register& reg1, Register& reg2)
            : Operation(false, "Assign", std::make_shared<Register>(reg2), {std::make_shared<Register>(reg1)}),
              register1(reg1.addr), register2(reg2.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct AssignConstant : Operation {
        int reg;
        int data;

        AssignConstant(Register& reg, int data)
            : Operation(false, "AssignConstant", std::make_shared<Register>(reg), {}), reg(reg.addr), data(data) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Add_UInt_ConstUInt : Operation {
        size_t add_int;
        int lhs;
        int res;

        Add_UInt_ConstUInt(Register& reg_in, size_t add, Register& reg_out)
            : Operation(false, "Add_UInt_ConstUInt", std::make_shared<Register>(reg_out), {std::make_shared<Register>(reg_in)}),
              add_int(add), lhs(reg_in.addr), res(reg_out.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct GetMid_UInt_UInt : Operation {
        int left;
        int right;
        int mid;

        GetMid_UInt_UInt(Register& lhs, Register& rhs, Register mhs)
            : Operation(false, "GetMid_UInt_UInt", std::make_shared<Register>(mhs),
                    {std::make_shared<Register>(lhs), std::make_shared<Register>(rhs)}),
                    left(lhs.addr), right(rhs.addr), mid(mhs.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Push : Operation {
        int reg_id;
        int garbage_id;
        std::string garbage_name;

        Push(Register& reg, const std::string& garbage)
            : Operation(false, "Push", std::make_shared<Register>(reg), {}),
            reg_id(reg.addr), garbage_name(garbage) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Pop : Operation {
        int reg_id;

        Pop(Register& reg) : Operation(false, "Pop", std::make_shared<Register>(reg), {}),
                             reg_id(reg.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Compare_Less_UInt_UInt : Operation {
        int left;
        int right;
        int res;

        Compare_Less_UInt_UInt(Register& lhs, Register& rhs, Register& res_)
            : Operation(false, "Compare_Less_UInt_UInt", std::make_shared<Register>(res_), {std::make_shared<Register>(lhs),
                    std::make_shared<Register>(rhs)}), left(lhs.addr), right(rhs.addr), res(res_.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Compare_Equal_UInt_UInt : Operation {
        int left;
        int right;
        int res;

        Compare_Equal_UInt_UInt(Register& lhs, Register& rhs, Register& res_)
        : Operation(false, "Compare_Equal_UInt_UInt", std::make_shared<Register>(res_), {std::make_shared<Register>(lhs),
                    std::make_shared<Register>(rhs)}), left(lhs.addr), right(rhs.addr), res(res_.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Control_Negate_Bitwise : Operation {
        int reg1;
        int reg2;

        Control_Negate_Bitwise(Register& reg1, Register& reg2)
            : Operation(false, "Control_Negate_Bitwise", std::make_shared<Register>(reg1), {std::make_shared<Register>(reg2)}),
              reg1(reg1.addr), reg2(reg2.addr) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct FlipBool : Operation {
        int reg;
        size_t digit;

        FlipBool(Register& reg, size_t digit)
            : Operation(false, "FlipBool", std::make_shared<Register>(reg), {}),
              reg(reg.addr), digit(digit) {}

        void operator()(std::vector<System>& state) const;

        ClassControllable

        ControlAll
    };

    struct Swap_General_General : Function {
        std::shared_ptr<Register> reg1;
        std::shared_ptr<Register> reg2;

        Swap_General_General(Register& reg1, Register& reg2);

        void operator()(Module& mod) override;

        ClassControllable

        ControlAll
    };

    struct Compare_UInt_UInt : Function {
        std::shared_ptr<Register> eq;
        std::shared_ptr<Register> less;
        std::shared_ptr<Register> left;
        std::shared_ptr<Register> right;

        Compare_UInt_UInt(Register& left, Register& right, Register& less, Register& equal)
            : Function("Compare_UInt_UInt", {std::make_shared<Register>(less), std::make_shared<Register>(equal)},
                       {std::make_shared<Register>(left), std::make_shared<Register>(right)}),
                       eq(std::make_shared<Register>(equal)), less(std::make_shared<Register>(less)),
                       right(std::make_shared<Register>(right)), left(std::make_shared<Register>(left)) {}

        void operator()(Module& mod) override;

        ClassControllable

        ControlAll
    };

    struct ClearZero : Operation {
        ClearZero() : Operation(true, "ClearZero", nullptr, {}) {}
        void operator()(std::vector<System> &system_states) const;
    };

    struct Hadamard_Int : Operation {
        int id;
        size_t n_digits;
        size_t mask;

        Hadamard_Int(Register& reg_in, size_t n_digits)
            : Operation(true, "Hadamard_Int", std::make_shared<Register>(reg_in), {}),
              id(reg_in.addr), n_digits(n_digits) {
            mask = pow2(n_digits);
            mask--;
            mask = ~mask;
        }

        void operator()(std::vector<System> &system_states) const;
    };

    struct GetRowAddr : Operation {
        int offset;
        int row;
        int row_offset;
        size_t row_sz;

        GetRowAddr(Register& offset, Register& row, Register& row_offset, size_t row_sz)
            : Operation(false, "GetRowAddr", std::make_shared<Register>(row_offset),
                        {std::make_shared<Register>(offset), std::make_shared<Register>(row)}),
              row_sz(row_sz), offset(offset.addr), row(row.addr), row_offset(row_offset.addr) {}

        void operator()(std::vector<System>& state) const;
    };

    struct AddAssign_AnyInt_AnyInt : Operation {
        int lhs;
        int rhs;
        size_t lhs_size;
        size_t rhs_size;

        AddAssign_AnyInt_AnyInt(Register& reg_lhs, Register& reg_rhs)
            : Operation(false, "AddAssign_AnyInt_AnyInt", std::make_shared<Register>(reg_lhs),
                        {std::make_shared<Register>(reg_rhs)}),
            lhs(reg_lhs.addr), lhs_size(System::size_of(reg_lhs.addr)),
            rhs(reg_rhs.addr), rhs_size(System::size_of(reg_rhs.addr)) {}

        void operator()(std::vector<System>& state) const;
    };

}

#endif //QUINT_GENERAL_H
