//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_PLATFORM_MACROS_H
#define QUINT_PLATFORM_MACROS_H

// Windows
#if defined(_WIN64)
#define QUINT_PLATFORM_WINDOWS
#endif

#if defined(_WIN32) && !defined(_WIN64)
static_assert(false, "32-bit Windows systems are not supported")
#endif

// Linux
#if defined(__linux__)
#if defined(ANDROID)
#define QUINT_PLATFORM_ANDROID
#else
#define QUINT_PLATFORM_LINUX
#endif
#endif

// OSX
#if defined(__APPLE__)
#define QUINT_PLATFORM_OSX
#endif

#if (defined(QUINT_PLATFORM_LINUX) || defined(QUINT_PLATFORM_OSX) || \
     defined(__unix__))
#define QUINT_PLATFORM_UNIX
#endif

#endif //QUINT_PLATFORM_MACROS_H
