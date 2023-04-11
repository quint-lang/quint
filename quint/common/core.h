//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_CORE_H
#define QUINT_CORE_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <type_traits>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <string>
#include <functional>

#include "quint/common/platform_macros.h"

// Avoid dependency on glibc 2.27
#if defined(TI_PLATFORM_LINUX) && defined(TI_ARCH_x64)
// objdump -T libtaichi_python.so| grep  GLIBC_2.27
__asm__(".symver logf,logf@GLIBC_2.2.5");
__asm__(".symver powf,powf@GLIBC_2.2.5");
__asm__(".symver expf,expf@GLIBC_2.2.5");
#endif

// Compilers

// MSVC
#if defined(_MSC_VER)
#define QUINT_COMPILER_MSVC
#endif

// MINGW
#if defined(__MINGW64__)
#define QUINT_COMPILER_MINGW
#endif

// gcc
#if defined(__GNUC__)
#define QUINT_COMPILER__GCC
#endif

// clang
#if defined(__clang__)
#define QUINT_COMPILER_CLANG
#endif

#if defined(QUINT_COMPILER_MSVC)
#define QUINT_ALIGNED(x) __declspec(align(x))
#else
#define QUINT_ALIGNED(x) __attribute__((aligned(x)))
#endif

#if __cplusplus >= 201703L
#define QUINT_CPP17
#else
#if defined(QUINT_COMPILER_CLANG)
static_assert(false, "For clang compilers, use -std=c++17");
#endif
static_assert(__cplusplus >= 201402L, "C++14 required.");
#define QUINT_CPP14
#endif

// Do not disable assert...
#ifdef NDEBUG
#undef NDEBUG
#endif

#ifdef _WIN64
#pragma warning(push)
#pragma warning(disable : 4005)
#include "quint/platform/windows/windows.h"
#pragma warning(pop)
#include <intrin.h>
#endif  // _WIN64

#define TI_STATIC_ASSERT(x) static_assert((x), #x)

namespace quint {

//******************************************************************************
//                                 System State
//******************************************************************************
    class CoreState {
    public:
        bool python_imported = false;
        bool trigger_gdb_when_crash = false;

        static CoreState &get_instance();

        static void set_python_imported(bool val) {
            get_instance().python_imported = val;
        }

        static void set_trigger_gdb_when_crash(bool val) {
            get_instance().trigger_gdb_when_crash = val;
        }
    };

//******************************************************************************
//                                 Types
//******************************************************************************

    using uchar = unsigned char;

    using int8 = int8_t;
    using uint8 = uint8_t;

    using int16 = int16_t;
    using uint16 = uint16_t;

    using int32 = int32_t;
    using uint32 = uint32_t;
    using uint = unsigned int;

    using int64 = int64_t;
    using uint64 = uint64_t;

    using float32 = float;
    using float64 = double;

    using real = float32;

    real constexpr operator"" _f(long double v) {
        return real(v);
    }
    real constexpr operator"" _f(unsigned long long v) {
        return real(v);
    }

    float64 constexpr operator"" _f64(long double v) {
        return float64(v);
    }

#ifdef QUINR_USE_DOUBLE
    using real = float64;
#else
    using real = float32;
#endif

#ifdef _WIN64
#define QUINT_FORCE_INLINE __forceinline
#else
#define QUINT_FORCE_INLINE inline __attribute__((always_inline))
#endif


} // namespace quint

//******************************************************************************
//                               String Utils
//******************************************************************************

#include "quint/common/logger.h"

namespace quint {

    namespace zip {
        void write(std::string fn, const uint8 *data, std::size_t len);
        void write(const std::string &fn, const std::string &data);
        std::vector<uint8> read(const std::string fn, bool verbose = false);
    }

    inline bool ends_with(std::string const &str, std::string const &ending) {
        if (ending.size() > str.size())
            return false;
        else
            return std::equal(ending.begin(), ending.end(), str.end() - ending.size());
    }
}

#include "quint/common/serialization.h"

//******************************************************************************
//                                   Misc.
//******************************************************************************

namespace quint {

    extern int __trash__;

    template<typename T>
    void trash(T &&t) {
        static_assert(!std::is_same<T, void>::value, "");
        __trash__ = *reinterpret_cast<uint8 *>(&t);
    }

    std::string get_repo_dir();

    std::string get_python_package_dir();

    void set_python_package_dir(const std::string &dir);

}

#endif //QUINT_CORE_H
