#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define W_COMPILER(x)                                                          \
    WP_PLATFORM_COMPILER_##x()

#if defined(_MSC_VER)
    #define WP_PLATFORM_COMPILER_MSVC()  1
    #define WP_PLATFORM_COMPILER_CLANG() 0
    #define WP_PLATFORM_COMPILER_INTEL() 0
    #define WP_PLATFORM_COMPILER_GCC()   0
#elif defined(__clang__)
    #define WP_PLATFORM_COMPILER_MSVC()  0
    #define WP_PLATFORM_COMPILER_CLANG() 1
    #define WP_PLATFORM_COMPILER_INTEL() 0
    #define WP_PLATFORM_COMPILER_GCC()   0
#elif defined(__INTEL_COMPILER)
    #define WP_PLATFORM_COMPILER_MSVC()  0
    #define WP_PLATFORM_COMPILER_CLANG() 0
    #define WP_PLATFORM_COMPILER_INTEL() 1
    #define WP_PLATFORM_COMPILER_GCC()   0
#elif defined(__GNUC__)
    #define WP_PLATFORM_COMPILER_MSVC()  0
    #define WP_PLATFORM_COMPILER_CLANG() 0
    #define WP_PLATFORM_COMPILER_INTEL() 0
    #define WP_PLATFORM_COMPILER_GCC()   1
#else
    #define WP_PLATFORM_COMPILER_MSVC()  0
    #define WP_PLATFORM_COMPILER_CLANG() 0
    #define WP_PLATFORM_COMPILER_INTEL() 0
    #define WP_PLATFORM_COMPILER_GCC()   0
#endif

#if defined(__GNUC__)
    #define WP_PLATFORM_COMPILER_GNUC_COMPLIANT() 1
#else
    #define WP_PLATFORM_COMPILER_GNUC_COMPLIANT() 0
#endif

#define W_OS(x)                                                                \
    WP_PLATFORM_OS_##x()

#if defined(_WIN32)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           0
    #define WP_PLATFORM_OS_WINDOWS()        1
#elif defined(__linux__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          1
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__APPLE__)
    #define WP_PLATFORM_OS_DARWIN()         1
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__DragonFly__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 1
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__FreeBSD__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       1
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__NetBSD__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        1
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__OpenBSD__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       1
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__QNX__) || defined(__QNXNTO__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            1
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#elif defined(__unix__)
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           1
    #define WP_PLATFORM_OS_WINDOWS()        0
#else
    #define WP_PLATFORM_OS_DARWIN()         0
    #define WP_PLATFORM_OS_DRAGON_FLY_BSD() 0
    #define WP_PLATFORM_OS_FREE_BSD()       0
    #define WP_PLATFORM_OS_LINUX()          0
    #define WP_PLATFORM_OS_NET_BSD()        0
    #define WP_PLATFORM_OS_OPEN_BSD()       0
    #define WP_PLATFORM_OS_QNX()            0
    #define WP_PLATFORM_OS_UNIX()           0
    #define WP_PLATFORM_OS_WINDOWS()        0
#endif

// Platform-specific headers.
#if W_OS(UNIX)
    #include <pthread.h>
    #include <stdio.h>
#elif W_OS(WINDOWS)
    #include <windows.h>
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
    #if UINTPTR_MAX == UINT64_MAX
        #define W_PTR_SIZE 8
    #elif UINTPTR_MAX == UINT32_MAX
        #define W_PTR_SIZE 4
    #else
        #error "could not determine the pointer size"
    #endif
#endif

typedef char
wp_invalid_ptr_size[W_PTR_SIZE == sizeof(void *) ? 1 : -1];

// Assume that the size of the ‘size_t’ and pointer types are the same.
// Storing this size as a constant is useful in preprocessor conditions where
// the ‘sizeof’ operator is not yet available.
#define W_SIZE_T_SIZE W_PTR_SIZE

typedef char
wp_invalid_size_t_size[W_SIZE_T_SIZE == sizeof(size_t) ? 1 : -1];

// Struct padding removal.
#if W_COMPILER(GNUC_COMPLIANT)
    #define W_PACKED_STRUCT(name)                                              \
        struct __attribute__((packed)) name
#elif W_COMPILER(MSVC)
    #define W_PACKED_STRUCT(name)                                              \
        __pragma(pack(push, 1))                                                \
        struct name                                                            \
        __pragma(pack(pop))
#else
    #error "not implemented"
#endif

// Checks for printf-like functions.
#if W_COMPILER(GNUC_COMPLIANT)
    #define W_PRINTF_CHECK(fmt, args)                                          \
        __attribute__((format (printf, (fmt), (args))))
#else
    #define W_PRINTF_CHECK(fmt, args)
#endif

// I/O streams.
#if W_OS(UNIX)
    #define W_STD_IN                                                           \
        (stdin)
    #define W_STD_OUT                                                          \
        (stdout)
    #define W_STD_ERR                                                          \
        (stderr)

    typedef FILE * w_stream;
#elif W_OS(WINDOWS)
    #define W_STD_IN                                                           \
        (GetStdHandle(STD_INPUT_HANDLE))
    #define W_STD_OUT                                                          \
        (GetStdHandle(STD_OUTPUT_HANDLE))
    #define W_STD_ERR                                                          \
        (GetStdHandle(STD_ERROR_HANDLE))

    typedef HANDLE w_stream;
#else
    #error "not implemented"
#endif

// Mutexes.
#if W_OS(UNIX)
    #define W_MTX_INTIALIZE(mtx)                                               \
        static pthread_mutex_t                                                 \
        mtx = PTHREAD_MUTEX_INITIALIZER

    typedef pthread_mutex_t w_mtx;
#elif W_OS(WINDOWS)
    #define W_MTX_INTIALIZE(mtx)                                               \
        static HANDLE                                                          \
        mtx = NULL

    typedef HANDLE w_mtx;
#else
    #error "not implemented"
#endif

void
w_abort(
    int status
);

int
w_mtx_lock(
    w_mtx *mtx
);

int
w_mtx_unlock(
    w_mtx *mtx
);

int
w_is_console(
    w_stream stream
);

int
w_print(
    w_stream stream,
    const char *msg
);

int
w_format_msg(
    char *msg,
    size_t msg_len,
    const char *fmt,
    ...
)
W_PRINTF_CHECK(3, 4);

int
w_format_msg_va(
    char *msg,
    size_t msg_len,
    const char *fmt,
    va_list args
)
W_PRINTF_CHECK(3, 0);

int
w_format_system_error(
    char *msg,
    size_t msg_len,
    int error
);

uint32_t
w_count_leading_zero_bits_32(
    uint32_t x
);

uint64_t
w_count_leading_zero_bits_64(
    uint64_t x
);

#endif // UUKI_BASE_PLATFORM_H
