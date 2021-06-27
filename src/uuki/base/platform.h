#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

#include <assert.h>
#include <stddef.h>

#define W_COMPILER(x)                                                          \
    W__PLATFORM_COMPILER_##x()

#if defined(_MSC_VER)
    #define W__PLATFORM_COMPILER_MSVC()  1
    #define W__PLATFORM_COMPILER_CLANG() 0
    #define W__PLATFORM_COMPILER_INTEL() 0
    #define W__PLATFORM_COMPILER_GCC()   0
#elif defined(__clang__)
    #define W__PLATFORM_COMPILER_MSVC()  0
    #define W__PLATFORM_COMPILER_CLANG() 1
    #define W__PLATFORM_COMPILER_INTEL() 0
    #define W__PLATFORM_COMPILER_GCC()   0
#elif defined(__INTEL_COMPILER)
    #define W__PLATFORM_COMPILER_MSVC()  0
    #define W__PLATFORM_COMPILER_CLANG() 0
    #define W__PLATFORM_COMPILER_INTEL() 1
    #define W__PLATFORM_COMPILER_GCC()   0
#elif defined(__GNUC__)
    #define W__PLATFORM_COMPILER_MSVC()  0
    #define W__PLATFORM_COMPILER_CLANG() 0
    #define W__PLATFORM_COMPILER_INTEL() 0
    #define W__PLATFORM_COMPILER_GCC()   1
#else
    #define W__PLATFORM_COMPILER_MSVC()  0
    #define W__PLATFORM_COMPILER_CLANG() 0
    #define W__PLATFORM_COMPILER_INTEL() 0
    #define W__PLATFORM_COMPILER_GCC()   0
#endif

#if defined(__GNUC__)
    #define W__PLATFORM_COMPILER_GNUC_COMPLIANT() 1
#else
    #define W__PLATFORM_COMPILER_GNUC_COMPLIANT() 0
#endif

#define W_OS(x)                                                                \
    W__PLATFORM_OS_##x()

#if defined(_WIN32)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        1
#elif defined(__linux__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          1
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__APPLE__)
    #define W__PLATFORM_OS_DARWIN()         1
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__DragonFly__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 1
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__FreeBSD__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       1
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__NetBSD__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        1
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__OpenBSD__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       1
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#elif defined(__QNX__) || defined(__QNXNTO__)
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            1
    #define W__PLATFORM_OS_WINDOWS()        0
#else
    #define W__PLATFORM_OS_DARWIN()         0
    #define W__PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define W__PLATFORM_OS_FREE_BSD()       0
    #define W__PLATFORM_OS_LINUX()          0
    #define W__PLATFORM_OS_NET_BSD()        0
    #define W__PLATFORM_OS_OPEN_BSD()       0
    #define W__PLATFORM_OS_QNX()            0
    #define W__PLATFORM_OS_WINDOWS()        0
#endif

#if defined(__unix__)
    #define W__PLATFORM_OS_UNIX() 1
#else
    #define W__PLATFORM_OS_UNIX() 0
#endif

// Some CPUs, such as the x86-64 processors, allow running code in 32-bit mode
// if compiled using the -m32 or -mx32 compiler switches, in which case
// the macro `W_PTR_SIZE` is set to 32.
#if W_COMPILER(GNUC_COMPLIANT)
    #if __SIZEOF_POINTER__ == 8
        #define W_PTR_SIZE 8
    #elif __SIZEOF_POINTER__ == 4
        #define W_PTR_SIZE 4
    #else
        #error "could not determine the pointer size"
    #endif
#elif W_COMPILER(MSVC)
    #if defined(_WIN64)
        #define W_PTR_SIZE 8
    #elif defined(_WIN32)
        #define W_PTR_SIZE 4
    #else
        #error "could not determine the pointer size"
    #endif
#else
    #include <stdint.h>
    #if UINTPTR_MAX == UINT64_MAX
        #define W_PTR_SIZE 8
    #elif UINTPTR_MAX == UINT32_MAX
        #define W_PTR_SIZE 4
    #else
        #error "could not determine the pointer size"
    #endif
#endif

static_assert(
    W_PTR_SIZE == sizeof(void *), "could not determine the pointer size");

#define W_SIZE_T_SIZE W_PTR_SIZE

static_assert(
    W_SIZE_T_SIZE == sizeof(size_t), "could not determine the size_t size");

#endif // UUKI_BASE_PLATFORM_H
