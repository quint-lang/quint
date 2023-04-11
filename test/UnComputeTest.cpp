//
// Created by BY210033 on 2023/2/7.
//
#include "../quint/runtime/hvm/data_structure.h"
#include "../quint/runtime/hvm/qbs.h"

using namespace quint;

void TestQBS(std::vector<System>& states)
{
    Module mod;
    auto address = AllocRegister("address", UnsignedInteger, 4)();
    auto target = AllocRegister("target", UnsignedInteger, 32)();
    auto result = AllocRegister("result", UnsignedInteger, 32)();

    QRAMCircuit qram(4, 32);
    qram.set_memory({ 0, 3, 6, 12, 20, 180, 320, 700, 0, 3, 6, 12, 20, 180, 320, 700 });

    mod << address << target << result
        | QuantumBinarySearch(&qram, address, 8, target, result);

//    mod.execute_dump(states);
    mod.execute(states);
}

void logicSort(std::vector<System>& states)
{
    Module mod;
    auto result = AllocRegister("result", UnsignedInteger, 32)();
    auto left = AllocAuxiliaryRegister("left", UnsignedInteger, 32)();
    auto right = AllocAuxiliaryRegister("right", UnsignedInteger, 32)();
    auto flag = AllocAuxiliaryRegister("flag", Boolean, 1)();
    auto compare_less = AllocAuxiliaryRegister("compare_less", Boolean, 1)();
    auto compare_equal = AllocAuxiliaryRegister("compare_equal", Boolean, 1)();
    auto target = AllocAuxiliaryRegister("target", UnsignedInteger, 32)();

    mod << result << left << right << flag << compare_less << compare_equal << target;

    mod | FlipBool(flag, 0)
        | AssignConstant(left, 7)
        | AssignConstant(right, 8);

    for (int i = 0; i < 2; ++i) {
        mod | AssignConstant(target, 3).controlled_by(flag)
            | Compare_UInt_UInt(left, right, compare_less, compare_equal).controlled_by(flag)
            | Assign(target, result).controlled_by(compare_equal).controlled_by(flag);

        if (i != 1) {
            mod | Assign(compare_equal, flag)
                | FlipBool(compare_less, 0)
                | Push(target, fmt::format("{}-{}", "target", i))
                | Push(compare_less, fmt::format("{}-{}", "compare_less", i))
                | Push(compare_equal, fmt::format("{}-{}", "compare_equal", i));
        }
    }

    mod.execute(states);
}

int main()
{
    std::vector<System> system_states;
    system_states.emplace_back();
    TestQBS(system_states);
//    logicSort(system_states);
}