//
// Created by BY210033 on 2023/2/7.
//
#include "util/basic.h"
#include "common/err_handler.h"

namespace quint {
    bool operator<(const complex_t& lhs, const complex_t& rhs)
    {
        return (abs_sqr(lhs) < abs_sqr(rhs));
    }

    std::string dec2bin(size_t n, size_t size)
    {
        if (n >= pow2(size))
            throw_invalid_input();

        std::string binstr = "";
        for (size_t i = 0; i < size; ++i) {
            binstr = (char)((n & 1) + '0') + binstr;
            n >>= 1;
        }
        return binstr;
    }

    size_t get_rational_IEEE754(double data, size_t data_sz)
    {
        // profiler _("IEEE");
        if (data >= 1 || data < 0) return 0;
        size_t idata = *reinterpret_cast<size_t*>(&data);
        size_t bias = 1022 - (idata >> 52);
        data_sz -= 1;
        if (bias > data_sz) return 0;
        if (bias == data_sz) return 1;
        data_sz -= bias;
        return ((idata << 12) >> (64 - data_sz)) + pow2(data_sz);
    }

    int64_t get_complement(size_t data, size_t data_sz)
    {
        if (data_sz == 64)
        {
            return *(reinterpret_cast<int64_t*>(&data));
        }
        if (data_sz == 0)
        {
            return 0;
        }

        if (data < pow2(data_sz - 1)) return data;
        return -1 * (pow2(data_sz) - data);
    }

    std::vector<bool> calc_pos(int pos, int layer) {
        std::vector<bool> posv;
        posv.reserve(layer);

        for (int i = 0; i < layer; i++) {
            posv.push_back(get_digit(pos, i));
        }
        return posv;
    }

    std::vector<size_t> get_nodes_in_layer(int layer) {
        // layer -= 1;
        size_t begin = pow2(layer) - 1;
        size_t end = begin + pow2(layer);
        std::vector<size_t> ret;
        ret.resize(end - begin);
        for (size_t i = begin; i < end; ++i) {
            ret[i - begin] = i;
        }
        return ret;
    }

    std::string pos2str(int pos, int layer) {
        std::vector<bool> posv = calc_pos(pos, layer);
        std::string ret;
        for (bool i : posv) {
            char x = i ? '1' : '0';
            ret = x + ret;
        }
        return ret;
    }

    std::string type2str(OperationType type) {
        const static std::map<OperationType, std::string> _TypeNameMap = {
                {OperationType::ControlSwap, "cSwap"},
                {OperationType::HadamardData, "Hadamard"},
                {OperationType::SwapInternal, "Swap"},
                {OperationType::FirstCopy, "ACopy"},
                {OperationType::FetchData, "FetchData"},
                {OperationType::CopyIn, "CopyIn"},
                {OperationType::CopyOut, "CopyOut"},
                {OperationType::SetZero, "SetZero"},
                {OperationType::Damping, "Damping"},
                {OperationType::Damp_Full, "Damp_Full"},
                {OperationType::Damp_Common, "Damp_Common"},
                {OperationType::BitFlip, "BitFlip"},
                {OperationType::PhaseFlip, "PhaseFlip"},
                {OperationType::BitPhaseFlip, "BitPhaseFlip"},
                {OperationType::Depolarizing, "Depolarizing"},
        };
        if (type > OperationType::Begin && type < OperationType::End)
            return _TypeNameMap.at(type);
        else
            return "Unknown";
    }
}