#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

#include <limits.h>
#include <stddef.h>

#if defined(_WIN32)
    #define W_OS_WINDOWS
#elif defined(__unix__) || defined(__APPLE__)
    #define W_OS_UNIX
    #if defined(__APPLE__)
        #define W_OS_DARWIN
    #elif defined(__linux__)
        #define W_OS_LINUX
    #endif
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
