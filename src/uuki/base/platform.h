#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

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
#else
    #define W__COMPILER_GNUC_COMPLIANT() 0
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
#else
    #define W__OS_UNIX() 0
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
        #error "Unsupported pointer size."
    #endif
#elif W_COMPILER(MSVC)
    #if defined(_WIN64)
        #define W_PTR_SIZE 8
    #elif defined(_WIN32)
        #define W_PTR_SIZE 4
    #else
        #error "Unsupported pointer size."
    #endif
#else
    #include <stdint.h>
    #if UINTPTR_MAX == UINT64_MAX
        #define W_PTR_SIZE 8
    #elif UINTPTR_MAX == UINT32_MAX
        #define W_PTR_SIZE 4
    #else
        #error "Unsupported pointer size."
    #endif
#endif

typedef char
w__platform_invalid_ptr_size[W_PTR_SIZE == sizeof(void *) ? 1 : -1];

#define W_SIZE_T_SIZE W_PTR_SIZE

typedef char
w__platform_invalid_size_t_size[W_SIZE_T_SIZE == sizeof(size_t) ? 1 : -1];

#endif // UUKI_BASE_PLATFORM_H
