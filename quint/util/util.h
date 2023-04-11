//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_UTIL_H
#define QUINT_UTIL_H

#include "quint/common/typedefs.h"
#include "quint/common/err_handler.h"
#include "quint/util/basic.h"
#include "spdlog/fmt/fmt.h"
#include "quint/util/random_engine.h"

#include <omp.h>

#ifdef QRAM_Release
#define QRAM_NOEXCEPT noexcept
#else
#define QRAM_NOEXCEPT
#endif

namespace quint {
    /* Helper class array_length */
    template<typename Ty = void>
    struct array_length
    {};

    template<typename DTy, int sz>
    struct array_length<std::array<DTy, sz>>
    {
        static constexpr int value = sz;
    };

    template<typename DTy, int sz>
    struct array_length<DTy[sz]>
    {
        static constexpr int value = sz;
    };

    constexpr std::pair<size_t, size_t> get_layer_range(size_t layerid) QRAM_NOEXCEPT
    {
        size_t lower = pow2(layerid + 1) - 2;
        size_t upper = pow2(layerid + 2) - 3;
        return { lower, upper };
    }

    constexpr std::pair<size_t, size_t> get_layer_node_range(size_t layerid) QRAM_NOEXCEPT
    {
        size_t lower = pow2(layerid) - 1;
        size_t upper = pow2(layerid + 1) - 2;
        return { lower, upper };
    }

    template<typename T>
    struct _remove_cvref {
        using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };

    template<typename T>
    using _remove_cvref_t = typename _remove_cvref<T>::type;

    template<typename Ty>
    void* to_voidptr(Ty ptr) QRAM_NOEXCEPT {
        using T_ptr_t = _remove_cvref_t<Ty>;
        using T = _remove_cvref_t<std::remove_pointer_t<T_ptr_t>>;
        using clear_pointer_type = T * ;
        return reinterpret_cast<void*>(const_cast<clear_pointer_type>(ptr));
    }

    template<typename KeyTy, typename ValTy>
    void map2vec(std::vector<std::pair<void*, void*>>& vec, const std::map<KeyTy, ValTy>& map1) {
        vec.clear();
        vec.reserve(map1.size());
        for (const auto& item : map1) {
            void* keyptr = to_voidptr(&(item.first));
            void* valptr = to_voidptr(&(item.second));
            vec.push_back({ keyptr, valptr });
        }
    }

    template<typename EngineType, typename MemoryContainer>
    void random_memory(MemoryContainer& memory, size_t memory_size, EngineType &engine) {
        size_t size = memory.size();
        std::uniform_int_distribution<memory_entry_t> ud(0, pow2(memory_size) - 1);
        /*for (size_t i = 0; i < memory.size(); ++i) {
            memory[i] = ud(engine);
        }*/
        for (auto iter = std::begin(memory); iter != std::end(memory); ++iter)
        {
            *iter = ud(engine);
        }
    }

    inline void random_memory(std::vector<size_t>& memory, size_t memory_size) {
        random_memory(memory, memory_size, random_engine::get_engine());
    }

    template <typename FwdIt, typename Pred, typename Func>
    FwdIt unique_and_merge(FwdIt first, FwdIt last, Pred pred, Func fn)
    {
        if (first == last) return last;

        FwdIt result = first;
        while (++first != last)
        {
            if (!pred(*result, *first))
                *(++result) = *first;
            else
                fn(*result, *first);
        }
        return ++result;
    }

    template<typename ContainerTy, typename PredLt, typename PredEq, typename MergeFn, typename EraseFn>
    void sort_merge_unique_erase(ContainerTy& vec, PredLt lt, PredEq eqn, MergeFn fn, EraseFn erase)
    {
        std::sort(vec.begin(), vec.end(), lt);
        auto iter = unique_and_merge(vec.begin(), vec.end(), eqn, fn);
        iter = std::remove_if(vec.begin(), iter, erase);
        vec.erase(iter, vec.end());
    }

    template< class Key, class T, class Compare, class Alloc, class Pred >
    void erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred) {
        for (auto i = c.begin(), last = c.end(); i != last; ) {
            if (pred(*i)) {
                i = c.erase(i);
            }
            else {
                ++i;
            }
        }
    }

    template<typename Rng>
    void choice_from(std::set<size_t>& samples, int size, size_t n_samples, Rng& g)
    {
        samples.clear();
        std::uniform_int_distribution<size_t> ud(0, 1ull << size);
        while (n_samples > 0) {
            if (samples.insert(ud(g)).second) { n_samples--; };
        }
    }

    inline std::vector<double> linspace(double min, double max, size_t points) {
        double delta = (max - min) / (points - 1);
        std::vector<double> ret;
        ret.reserve(points);
        for (size_t i = 0; i < points; ++i) {
            ret.push_back(min + delta * i);
        }
        return ret;
    }

    inline std::pair<double, double> mean_std(const std::vector<double> &m) {
        auto sq = [](double m, double y) {
            return m + y * y;
        };

        double sum = std::accumulate(m.begin(), m.end(), 0.0);
        double sumsq = std::accumulate(m.begin(), m.end(), 0.0, sq);
        double mean = sum / m.size();
        double meansq = sumsq / m.size();
        return { mean, sqrt(meansq - mean * mean) };

    }

    constexpr const char* bool2char(bool x) QRAM_NOEXCEPT
    {
        return x ? "1" : "0";
    }

    constexpr const char* bool2str(bool x) QRAM_NOEXCEPT
    {
        return x ? "True" : "False";
    }

    constexpr int bool2int(bool x) QRAM_NOEXCEPT
    {
        return x ? 1 : 0;
    }

    constexpr const char* bool2char_pmbasis(bool x) QRAM_NOEXCEPT
    {
        return x ? "+" : "-";
    }

    constexpr char addr2str(int addr) QRAM_NOEXCEPT
    {
        if (addr == W) { return 'W'; }
        if (addr == L) { return 'L'; }
        if (addr == R) { return 'R'; }
        else { return '?'; }
    }

    constexpr const char* arch2str(int arch) QRAM_NOEXCEPT
    {
        // if (arch == arch_qubit) return "qubit";
        if (arch == arch_qutrit) return "qutrit";
        return "unknown_arch";
    }

    inline std::string complex2str(const std::complex<double>& x)
    {
        if (x.imag() > 0)
            return fmt::format("{}+{}j", x.real(), x.imag());
        else
            return fmt::format("{}-{}j", x.real(), -x.imag());
    }

    inline std::string complex2str(const std::vector<complex_t>& vec)
    {
        std::string ret = "[";
        for (auto &x : vec)
        {
            if (x.imag() > 0)
                fmt::format_to(std::back_inserter(ret), " {}+{}j", x.real(), x.imag());
            else
                fmt::format_to(std::back_inserter(ret), " {}-{}j", x.real(), -x.imag());
        }
        ret += "]";
        return ret;
    }

    inline std::string noise2str(const noise_t &noises)
    {
        std::string ret = "[";
        for (auto& noise : noises)
        {
            fmt::format_to(std::back_inserter(ret),
                           "{}={},", type2str(noise.first), noise.second
            );
        }
        if (noises.size() > 0)
            ret.back() = ']';
        else
            ret += "]";
        return ret;
    }

    constexpr int bitcount(size_t n) QRAM_NOEXCEPT
    {
        int count = 0;
        while (n) {
            count++;
            n &= (n - 1);
        }
        return count;
    }

    inline double get_fidelity(const std::vector<complex_t> &state,
                               const std::vector<complex_t> &target)
    {
        if (state.size() == 0)
            return 0;
        if (state.size() != target.size())
            throw_general_runtime_error();

        complex_t sum = 0;
        for (size_t i = 0; i < state.size(); i++)
        {
            sum += state[i] * std::conj(target[i]);
        }

        return abs_sqr(sum);
    }

    template<typename ContainerTy>
    bool check_unique_sort(const ContainerTy& cont) QRAM_NOEXCEPT
    {
        if (std::size(cont) <= 1)
            return true;

        auto iter = std::next(std::begin(cont));
        for (; iter != std::end(cont); ++iter)
        {
            if (*iter < *(std::prev(iter)))
                return false;
            if (*iter == *(std::prev(iter)))
                return false;
        }
        return true;
    }

    template<typename Iter>
    bool check_unique_sort(Iter beg, Iter end) QRAM_NOEXCEPT
    {
        if (std::distance(beg, end) <= 1)
            return true;

        auto iter = std::next(beg);
        for (; iter != end; ++iter)
        {
            if (*iter < *(std::prev(iter)))
                return false;
            if (*iter == *(std::prev(iter)))
                return false;
        }
        return true;
    }

} // namespace qram_simulator


template <> struct fmt::formatter<std::complex<double>> {
    // Presentation format: 'f' - fixed, 'e' - exponential.
    char presentation = 'f';

    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const std::complex<double>& p, FormatContext& ctx) -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        if (p.imag() >= 0)
            return presentation == 'f'
                   ? format_to(ctx.out(), "{:f}+{:f}i", p.real(), p.imag())
                   : format_to(ctx.out(), "{:e}+{:e}i", p.real(), p.imag());
        else
            return presentation == 'f'
                   ? format_to(ctx.out(), "{:f}{:f}i", p.real(), p.imag())
                   : format_to(ctx.out(), "{:e}{:e}i", p.real(), p.imag());
    }
};

namespace std {

    template<size_t sz>
    inline bool operator<(const std::bitset<sz>& lhs, const std::bitset<sz>& rhs)
    {
        return lhs.to_ullong() < rhs.to_ullong();
    }

    template<typename Ty>
    std::vector<Ty> operator+(const std::vector<Ty>& lhs, const std::vector<Ty>& rhs)
    {
        std::vector<Ty> ret;
        if (lhs.size() == rhs.size())
        {
            ret.resize(lhs.size());
            for (size_t i = 0; i < lhs.size(); ++i)
            {
                ret[i] = lhs[i] + rhs[i];
            }
        }
        return lhs;
    }

    template<typename Ty>
    std::vector<Ty>& operator+=(std::vector<Ty>& lhs, const std::vector<Ty>& rhs)
    {
        if (lhs.size() == rhs.size())
            for (size_t i = 0; i < lhs.size(); ++i)
            {
                lhs[i] += rhs[i];
            }
        return lhs;
    }

}

#endif //QUINT_UTIL_H
