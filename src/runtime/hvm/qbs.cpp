//
// Created by BY210033 on 2023/2/8.
//
#include "runtime/hvm/qbs.h"

using namespace std;

namespace quint {

    QuantumBinarySearch::QuantumBinarySearch(QRAMCircuit *qram, Register &address_offset_register, size_t total_length_,
                                             Register &target_register, Register &result_register)
        : Function("QuantumBinarySearch",
                   {make_shared<Register>(result_register)},
                   {make_shared<Register>(address_offset_register), make_shared<Register>(target_register)}) ,
                   total_length(total_length_), qram(qram),
                   address_offset_reg(make_shared<Register>(address_offset_register)),
                   target_reg(make_shared<Register>(target_register)), result_reg(make_shared<Register>(result_register))
    {
        max_step = size_t(std::log2(total_length_)) + 1;
    }

    void QuantumBinarySearch::operator()(Module &mod)  {
        profiler _("QBS");
        iteration_level = 0;

        // get target register
        auto address_offset_id = *(address_offset_reg.get());
        auto target_id = *(target_reg.get());
        auto result_id = *(result_reg.get());

        auto flag = AllocAuxiliaryRegister("flag", Boolean, 1)();
        auto compare_less = AllocAuxiliaryRegister("compare_less", Boolean, 1)();
        auto compare_equal = AllocAuxiliaryRegister("compare_equal", Boolean, 1)();
        auto left_register = AllocAuxiliaryRegister("left_register", UnsignedInteger, qram->address_size + 1)();
        auto right_register = AllocAuxiliaryRegister("right_register", UnsignedInteger, qram->address_size + 1)();
        auto mid_register = AllocAuxiliaryRegister("mid_register", UnsignedInteger, qram->address_size + 1)();
        auto midval_register = AllocAuxiliaryRegister("midval_register", UnsignedInteger, qram->address_size)();

        // Add register into mod
        mod << flag << compare_less << compare_equal << left_register << right_register << mid_register << midval_register;

        // Add Operation into mod
        mod | FlipBool(flag, 0)
            | Assign(address_offset_id, left_register)
            | Add_UInt_ConstUInt(left_register, total_length, right_register);

        for (iteration_level = 0; iteration_level < max_step; ++iteration_level) {
            mod | GetMid_UInt_UInt(left_register, right_register, mid_register).controlled_by(flag)
                | QRAMLoad(qram, mid_register, midval_register).controlled_by(flag);
            mod | Compare_UInt_UInt(midval_register, target_id, compare_less, compare_equal).controlled_by(flag);
            mod | Assign(mid_register, result_id).controlled_by(compare_equal).controlled_by(flag);

            if (iteration_level != max_step -1) {
                mod | Assign(compare_equal, flag)
                    | Swap_General_General(left_register, mid_register)
                        .controlled_by(compare_less).controlled_by(flag)
                    | FlipBool(compare_less, 0)
                    | Swap_General_General(right_register, mid_register)
                        .controlled_by(compare_less).controlled_by(flag);

                mod | Push(mid_register, fmt::format("{}-{}", "mid_register", iteration_level))
                    | Push(midval_register, fmt::format("{}-{}", "midval_register", iteration_level))
                    | Push(compare_less, fmt::format("{}-{}", "compare_less", iteration_level))
                    | Push(compare_equal, fmt::format("{}-{}", "compare_equal", iteration_level));
            }
        }
    }


}