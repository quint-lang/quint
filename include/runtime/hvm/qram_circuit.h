//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_QRAM_CIRCUIT_H
#define QUINT_QRAM_CIRCUIT_H

#include "common/typedefs.h"

namespace quint {

    struct QRAMCircuit {
        size_t address_size;
        size_t data_size;
        memory_t memory;

        QRAMCircuit(size_t address_sz, size_t data_sz);
        QRAMCircuit(size_t address_sz, size_t data_sz, const memory_t& memory_);
        QRAMCircuit(size_t address_sz, size_t data_sz, memory_t&& memory_);

        void set_memory_random();
        void set_memory(const memory_t& new_memory);
        void set_memory(memory_t&& new_memory);
        inline auto& get_memory() { return memory; }
        inline auto& get_memory() const { return memory; }
    };

}

#endif //QUINT_QRAM_CIRCUIT_H
