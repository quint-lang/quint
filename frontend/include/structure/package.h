//
// Created by BY210033 on 2023/6/14.
//

#ifndef QUINT_PACKAGE_H
#define QUINT_PACKAGE_H

#include <cmath>

namespace quint {

    template<bool sign = true>
    class BitInt {
    public:
        size_t nbits;
        int64_t val;
        BitInt(size_t nbits, int64_t val): nbits(nbits), val(val) {
            wrap();
        }

        void wrap() {
            val &= (size_t(1) << nbits) - 1;
            if constexpr (sign) {
                if (nbits && val & (size_t(1) << (nbits - 1)))
                    val -= (size_t(1) << nbits);
            }
        }

        bool isUint() {
            return sign;
        }

        void check_out(BitInt &r) {
            assert(sign == r.isUint() && nbits == r.nbits);
        }

        BitInt operator+(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val + r.val);
        }

        BitInt operator-(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val - r.val);
        }

        BitInt operator*(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val * r.val);
        }

        BitInt operator/(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val / r.val);
        }

        BitInt operator%(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val % r.val);
        }

        BitInt operator^(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val ^ r.val);
        }

        BitInt operator|(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val | r.val);
        }

        BitInt operator&(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, val & r.val);
        }

        BitInt operator~() {
            return BitInt<sign>(nbits, ~val);
        }

        void operator++() {
            val++;
            wrap();
        }

        void operator--() {
            val--;
            wrap();
        }

        BitInt operator<<(BitInt<sign> &r) {
            return BitInt<sign>(nbits, val << r.val);
        }

        BitInt operator>>(BitInt<sign> &r) {
            return BitInt<sign>(nbits, val >> r.val);
        }

        void operator+=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val += r.val;
            wrap();
        }

        void operator-=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val += r.val;
            wrap();
        }

        void operator*=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val *= r.val;
            wrap();
        }

        void operator/=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val /= r.val;
            wrap();
        }

        void operator%=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val %= r.val;
            wrap();
        }

        void operator&=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val &= r.val;
            wrap();
        }

        void operator^=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val ^= r.val;
            wrap();
        }

        void operator|=(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val |= r.val;
            wrap();
        }

        void operator<<=(BitInt<sign> &r) {
            val <<= r.val;
            wrap();
        }

        void operator>>=(BitInt<sign> &r) {
            val >>= r.val;
            wrap();
        }

        BitInt pow(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            return BitInt<sign>(nbits, std::pow(val, r.val));
        }

        BitInt powAssign(BitInt<sign> &r) {
            assert(nbits == r.nbits);
            val = std::pow(val, r.val);
            wrap();
        }

        BitInt operator-() {
            assert(sign);
            return BitInt<sign>(nbits, -val);
        }

        BitInt operator!() {
            return BitInt<sign>(nbits, !val);
        }

        bool operator==(BitInt<sign> &rhs) {
            return val == rhs.val;
        }

        bool operator>(BitInt<sign> &rhs) {
            return val > rhs.val;
        }

        bool operator<(BitInt<sign> &rhs) {
            return val < rhs.val;
        }

        bool operator!=(BitInt<sign> &rhs) {
            return val != rhs.val;
        }

        bool operator>=(BitInt<sign> &rhs) {
            return val >= rhs.val;
        }

        bool operator<=(BitInt<sign> &rhs) {
            return val <= rhs.val;
        }

        std::string toString() {
            return std::to_string(val);
        }

    };

}

namespace std {
    template<>
    struct hash<quint::BitInt<false>> {
        std::size_t operator()(const quint::BitInt<false> &t) const {
            size_t hash_value = 0x9e3779c9;
            hash_value ^= std::hash<size_t>()(t.nbits) + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<int64_t>()(t.val) + (hash_value << 6) + (hash_value >> 2);
            return hash_value;
        }
    };

    template<>
    struct hash<quint::BitInt<true>> {
        std::size_t operator()(const quint::BitInt<true> &t) const {
            size_t hash_value = 0x9e3779cb;
            hash_value ^= std::hash<size_t>()(t.nbits) + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<int64_t>()(t.val) + (hash_value << 6) + (hash_value >> 2);
            return hash_value;
        }
    };
}

#endif //QUINT_PACKAGE_H
