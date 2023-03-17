//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_BASIC_H
#define QUINT_BASIC_H

#include "common/typedefs.h"
#include "common/err_handler.h"

namespace quint {
    template<typename Ty>
    auto abs_sqr(const std::complex<Ty>& c) -> Ty {
        return c.real() * c.real() + c.imag() * c.imag();
    }

    constexpr bool get_digit(size_t n, size_t digit)
    {
        return (n >> digit) & 1;
    }

    constexpr bool get_digit_reverse(size_t n, size_t digit, size_t maxdigit) { return (n >> (maxdigit - digit - 1)) % 2; }

    constexpr size_t pow2(size_t n) { return (1ull) << (n); }

    constexpr size_t log2(size_t n) {
        size_t ret = 0;
        while (n > 1) {
            ret++;
            n /= 2;
        }
        return ret;
    }

    template<typename Ty>
    Ty amp_sum(const std::vector<std::complex<Ty>>& amps)
    {
        Ty val = 0;
        for (auto& a : amps) { val += abs_sqr(a); }
        return val;
    }

    constexpr bool ignorable(const double v) {
        if (v > -epsilon && v < epsilon) return true;
        else return false;
    }

    constexpr bool ignorable(const double v, double eps) {
        if (v > -eps && v < eps) return true;
        else return false;
    }

    template<typename Ty>
    constexpr bool ignorable(const std::complex<Ty>& v) {
        Ty value = abs_sqr(v);
        if (ignorable(value)) return true;
        else return false;
    }

    template<typename Ty>
    void check_normalization(const std::vector<std::complex<Ty>>& amps)
    {
        double A = amp_sum(amps);
        if (!ignorable(A - 1.0))
            throw_bad_result();
    }

    constexpr inline bool digit1(size_t i, size_t digit)
    {
        return (i >> digit) & 1;
    }

    constexpr inline bool digit0(size_t i, size_t digit)
    {
        return !digit1(i, digit);
    }

    constexpr size_t flip_digit(size_t i, size_t digit)
    {
        auto m = (1 << digit);
        return digit1(i, digit) ? (i -= m) : (i += m);
    }

    constexpr size_t make_complement(int64_t data, size_t data_sz)
    {
        if (data_sz == 64 || data >= 0) {
            return data;
        }
        return pow2(data_sz) + data;
    }

    constexpr size_t get_rational(double data, size_t data_sz)
    {
        // profiler _("Common");
        if (data >= 1 || data < 0) return 0;
        size_t ret = 0;
        for (size_t i = 0; i < data_sz; ++i)
        {
            ret <<= 1;
            data *= 2;
            if (data >= 1)
            {
                ret += 1;
                data -= 1;
            }
        }
        return ret;
    }

    bool operator<(const complex_t& lhs, const complex_t& rhs);
    std::string dec2bin(size_t n, size_t size);
    int64_t get_complement(size_t data, size_t data_sz);
    size_t get_rational_IEEE754(double data, size_t data_sz);
    std::vector<bool> calc_pos(int pos, int layer);
    std::vector<size_t> get_nodes_in_layer(int layer);
    std::string pos2str(int pos, int layer);
    std::string type2str(OperationType type);
}

#endif //QUINT_BASIC_H
