//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_TYPEDEFS_H
#define QUINT_TYPEDEFS_H

#include <algorithm>
#include <any>
#include <array>
#include <bitset>
#include <complex>
#include <cmath>
#include <deque>
#include <execution>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace quint {

    /* typedefs */
    using complex_t = std::complex<double>;
    using memory_entry_t = size_t;
    using memory_t = std::vector<memory_entry_t>;
    using bus_t = size_t;
    using u22_t = std::array<std::complex<double>, 4>;
    using walk_angle_function_t = std::function<u22_t(size_t, size_t row, size_t col)>;

    enum StateStorageType
    {
        General,
        UnsignedInteger,
        SignedInteger,
        Boolean,
        Rational,
    };
    using StateInfoType = std::tuple<std::string, StateStorageType, size_t, bool>;

    /* math constant */
    constexpr double pi = 3.141592653589793238462643383279502884L;
    constexpr double sqrt2 = 1.41421356237309504880168872420969807856967L;
    constexpr double sqrt2inv = 1.0 / sqrt2;
    constexpr double epsilon = 1.e-18;

    constexpr int W = -1;
    constexpr int L = 0;
    constexpr int R = 1;
    static const complex_t w = { std::cos(pi * 2 / 3), std::sin(pi * 2 / 3) };
    static const complex_t w2 = w * w;

    constexpr int arch_qutrit = 0x1A1A;
    //constexpr int arch_qubit = 0x1B1B;


    enum class OperationType {
        Begin,

        ControlSwap,
        HadamardData,
        CopyIn,
        CopyOut,
        SwapInternal,
        FirstCopy,
        FetchData,

        // Noise Op
        SetZero,
        Damping,
        Damp_Common,
        Damp_Full,
        BitFlip,
        PhaseFlip,
        BitPhaseFlip,
        Depolarizing,

        // Identity
        Identity_Active,
        Identity_Inactive,

        End
    };

    using noise_t = std::map<OperationType, double>;
}


#endif //QUINT_TYPEDEFS_H
