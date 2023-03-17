//
// Created by BY210033 on 2023/2/8.
//

#ifndef QUINT_QBS_H
#define QUINT_QBS_H

#include "general.h"

namespace quint {

    struct QuantumBinarySearch : Function {
        QRAMCircuit* qram;
        size_t total_length;
        size_t max_step;

        std::shared_ptr<Register> address_offset_reg;
        std::shared_ptr<Register> target_reg;
        std::shared_ptr<Register> result_reg;
        int iteration_level;

        QuantumBinarySearch(QRAMCircuit *qram,
                            Register& address_offset_register,
                            size_t total_length_,
                            Register& target_register,
                            Register& result_register);

        void operator()(Module &mod) override;

        ClassControllable

        ControlAll

    };

}

#endif //QUINT_QBS_H
