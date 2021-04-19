#ifndef UUKI_BASE_MACROS_H
#define UUKI_BASE_MACROS_H

#include <uuki/base/private/macros.h>

#include <limits.h>
#include <stdint.h>

#define W_UNUSED_PARAM(x)                                                      \
    (void)(x)

#define W_REQUIRE_SEMICOLON                                                    \
    void w__macros_dummy(void)

#if defined(_MSC_VER)
    #define W_PACKED_STRUCT(name)                                              \
        __pragma(pack(push, 1))                                                \
        struct name                                                            \
        __pragma(pack(pop))
#else
    #define W_PACKED_STRUCT(name)                                              \
        struct __attribute__((packed)) name
#endif

// Overflow Checks
// ---------------------------------------------------------------- //   O-(''Q)

#define W_UINT_IS_ADD_WRAPPING(a, b, max)                                      \
    ((a) > (max) - (b))

#define W_UINT_IS_SUB_WRAPPING(a, b)                                           \
    ((a) < (b))

#define W_UINT_IS_MUL_WRAPPING(a, b, max)                                      \
    ((a) > (max) / (b))

#define W_IS_CEIL_POW2_WRAPPING(x, type)                                       \
    ((x) > (type)1 << (sizeof(type) * CHAR_BIT - 1))

// Memory
// ---------------------------------------------------------------- //   O-(''Q)

#define W_ARE_BUFS_OVERLAPPING(a, b, size)                                     \
    (((uintptr_t)(a) <= (uintptr_t)(b)                                         \
         && (uintptr_t)(a) + (size) > (uintptr_t)(b))                          \
     || ((uintptr_t)(b) <= (uintptr_t)(a)                                      \
         && (uintptr_t)(b) + (size) > (uintptr_t)(a)))

#define W_GET_ALIGNMENT(x)                                                     \
    (1 + (W__MACROS_GET_ALIGNMENT_1((x) - 1)))

#endif // UUKI_BASE_MACROS_H
