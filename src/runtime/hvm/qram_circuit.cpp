//
// Created by BY210033 on 2023/2/7.
//
#include "runtime/hvm/qram_circuit.h"
#include "util/basic.h"
#include "util/util.h"

namespace quint {

    QRAMCircuit::QRAMCircuit(size_t address_sz, size_t data_sz)
        : address_size(address_sz), data_size(data_sz){
        memory.resize(pow2(address_size));
    }

    QRAMCircuit::QRAMCircuit(size_t address_sz, size_t data_sz, const memory_t &memory_)
            : address_size(address_sz), data_size(data_sz) {
        set_memory(memory_);
    }

    QRAMCircuit::QRAMCircuit(size_t address_sz, size_t data_sz, memory_t &&memory_) {
        memory_.resize(pow2(address_size));
        set_memory(std::move(memory_));
    }

    void QRAMCircuit::set_memory_random() {
        random_memory(memory, data_size);
    }

    void QRAMCircuit::set_memory(const memory_t &new_memory) {
        if (new_memory.size() != pow2(address_size))
            throw_invalid_input();

        memory = new_memory;
    }

    void QRAMCircuit::set_memory(memory_t &&new_memory) {
        if (new_memory.size() != pow2(address_size))
            throw_invalid_input();

        memory = std::move(new_memory);
    }

}