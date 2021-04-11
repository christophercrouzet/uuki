#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

#include <limits.h>
#include <stddef.h>

#define W_COMPILER(x)                                                          \
    W__COMPILER_##x()

#if defined(_MSC_VER)
    #define W__COMPILER_MSVC() 1
    #define W__COMPILER_CLANG() 0
    #define W__COMPILER_INTEL() 0
    #define W__COMPILER_GCC() 0
#elif defined(__clang__)
    #define W__COMPILER_MSVC() 0
    #define W__COMPILER_CLANG() 1
    #define W__COMPILER_INTEL() 0
    #define W__COMPILER_GCC() 0
#elif defined(__INTEL_COMPILER)
    #define W__COMPILER_MSVC() 0
    #define W__COMPILER_CLANG() 0
    #define W__COMPILER_INTEL() 1
    #define W__COMPILER_GCC() 0
#elif defined(__GNUC__)
    #define W__COMPILER_MSVC() 0
    #define W__COMPILER_CLANG() 0
    #define W__COMPILER_INTEL() 0
    #define W__COMPILER_GCC() 1
#else
    #define W__COMPILER_MSVC() 0
    #define W__COMPILER_CLANG() 0
    #define W__COMPILER_INTEL() 0
    #define W__COMPILER_GCC() 0
#endif

#if defined(__GNUC__)
    #define W__COMPILER_GNUC_COMPLIANT() 1
#endif

#define W_OS(x)                                                                \
    W__OS_##x()

#if defined(_WIN32)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 1
#elif defined(__linux__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 1
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__APPLE__)
    #define W__OS_DARWIN() 1
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__DragonFly__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 1
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__FreeBSD__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 1
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__NetBSD__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 1
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__OpenBSD__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 1
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#elif defined(__QNX__) || defined(__QNXNTO__)
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 1
    #define W__OS_WINDOWS() 0
#else
    #define W__OS_DARWIN() 0
    #define W__OS_DRAGON_FLY_BSD() 0
    #define W__OS_FREE_BSD() 0
    #define W__OS_LINUX() 0
    #define W__OS_NET_BSD() 0
    #define W__OS_OPEN_BSD() 0
    #define W__OS_QNX() 0
    #define W__OS_WINDOWS() 0
#endif

#if defined(__unix__)
    #define W__OS_UNIX() 1
#endif

/*
   The word width macro represents whether the code is to be generated for a
   32-bit or 64-bit target platform. Some CPUs, such as the x86-64 processors,
   allow running code in 32-bit mode if compiled using the -m32 or -mx32
   compiler switches, in which case `W_WORD_WIDTH` is set to 32.
*/
#if (!(defined(__x86_64__) || defined(_M_X64)) || defined(__ILP32__))          \
        && !(defined(__itanium__) || defined(_M_IA64))                         \
        && !(defined(__powerpc64__) || defined(__ppc64__))                     \
        && !defined(__aarch64__)
    #define W_WORD_WIDTH 32
#else
    #define W_WORD_WIDTH 64
#endif

#define W_SIZE_WIDTH W_WORD_WIDTH

typedef char
w__platform_invalid_size_width[
    W_SIZE_WIDTH == sizeof(size_t) * CHAR_BIT ? 1 : -1];

#endif // UUKI_BASE_PLATFORM_H
