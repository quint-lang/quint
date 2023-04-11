//
// Created by BY210033 on 2023/2/7.
//
#include "quint/runtime/hvm/general.h"

using namespace std;

namespace quint {

    std::vector<StateInfoType> System::name_register_map;
    std::vector<int> System::temporal_registers;
    std::vector<int> System::reusable_registers;
    size_t System::max_qubit_count = 0;
    size_t System::max_register_count = 0;
    size_t System::max_system_size = 0;

    AllocRegister::AllocRegister(std::string register_name_, StateStorageType type_, size_t size_)
        : reg_name(register_name_), type(type_), size(size_) {}

    Register AllocRegister::operator()() const {
        profiler _("AllocRegister");
        int addr = System::add_register_synchronous(reg_name, type, size);
        return Register(addr, false);
    }

    AllocAuxiliaryRegister::AllocAuxiliaryRegister(std::string register_name_, StateStorageType type_, size_t size_)
        : reg_name(register_name_), type(type_), size(size_) {}

    Register AllocAuxiliaryRegister::operator()() const {
        profiler _("AllocAuxiliaryRegister");
        int addr = System::add_register_synchronous(reg_name, type, size);
        return Register(addr, true);
    }

    void QRAMLoad::operator()(vector <System> &state) const {

    }

    void Assign::operator()(vector <System> &state) const {

    }

    void AssignConstant::operator()(vector <System> &state) const {

    }

    void Add_UInt_ConstUInt::operator()(vector <System> &state) const {

    }

    void GetMid_UInt_UInt::operator()(vector <System> &state) const {

    }

    void Pop::operator()(vector <System> &state) const {

    }

    void Push::operator()(vector <System> &state) const {

    }

    void Compare_Less_UInt_UInt::operator()(vector <System> &state) const {

    }

    void Compare_Equal_UInt_UInt::operator()(vector <System> &state) const {

    }

    void Control_Negate_Bitwise::operator()(vector <System> &state) const {

    }

    void FlipBool::operator()(vector <System> &state) const {

    }

    Swap_General_General::Swap_General_General(Register &reg1, Register &reg2)
            : Function("Swap_General_General", {std::make_shared<Register>(reg1), std::make_shared<Register>(reg1)}, {}),
              reg1(std::make_shared<Register>(reg1)), reg2(std::make_shared<Register>(reg2)) {
        if (!reg1.local || !reg2.local)
            cerr << "Swap Operation can't support global register" << endl;
    }

    void Swap_General_General::operator()(Module& mod) {
        auto first = *(reg1.get());
        auto second = *(reg2.get());
        mod | Control_Negate_Bitwise(first, second)
           | Control_Negate_Bitwise(second, first)
           | Control_Negate_Bitwise(first, second);
    }

    void Compare_UInt_UInt::operator()(Module& mod) {
        auto r_reg = *(right.get());
        auto l_reg = *(left.get());
        auto eq_reg = *(eq.get());
        auto less_reg = *(less.get());
        mod | Compare_Equal_UInt_UInt(l_reg, r_reg, eq_reg)
            | Compare_Less_UInt_UInt(l_reg, r_reg, less_reg);
    }

    void ClearZero::operator()(vector <System> &system_states) const {

    }

    void Hadamard_Int::operator()(vector <System> &system_states) const {

    }

    void GetRowAddr::operator()(vector <System> &state) const {

    }

    void AddAssign_AnyInt_AnyInt::operator()(vector <System> &state) const {

    }

}

