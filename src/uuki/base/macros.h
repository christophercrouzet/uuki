#ifndef UUKI_BASE_MACROS_H
#define UUKI_BASE_MACROS_H

#include <uuki/base/private/macros.h>

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

// Math
// ---------------------------------------------------------------- //   O-(''Q)

#define W_ILOG2(x)                                                             \
    (                                                                          \
        (x) < 2 ? 0 :                                                          \
        (x) & (1ULL << 63) ? 63 :                                              \
        (x) & (1ULL << 63) ? 63 :                                              \
        (x) & (1ULL << 62) ? 62 :                                              \
        (x) & (1ULL << 61) ? 61 :                                              \
        (x) & (1ULL << 60) ? 60 :                                              \
        (x) & (1ULL << 59) ? 59 :                                              \
        (x) & (1ULL << 58) ? 58 :                                              \
        (x) & (1ULL << 57) ? 57 :                                              \
        (x) & (1ULL << 56) ? 56 :                                              \
        (x) & (1ULL << 55) ? 55 :                                              \
        (x) & (1ULL << 54) ? 54 :                                              \
        (x) & (1ULL << 53) ? 53 :                                              \
        (x) & (1ULL << 52) ? 52 :                                              \
        (x) & (1ULL << 51) ? 51 :                                              \
        (x) & (1ULL << 50) ? 50 :                                              \
        (x) & (1ULL << 49) ? 49 :                                              \
        (x) & (1ULL << 48) ? 48 :                                              \
        (x) & (1ULL << 47) ? 47 :                                              \
        (x) & (1ULL << 46) ? 46 :                                              \
        (x) & (1ULL << 45) ? 45 :                                              \
        (x) & (1ULL << 44) ? 44 :                                              \
        (x) & (1ULL << 43) ? 43 :                                              \
        (x) & (1ULL << 42) ? 42 :                                              \
        (x) & (1ULL << 41) ? 41 :                                              \
        (x) & (1ULL << 40) ? 40 :                                              \
        (x) & (1ULL << 39) ? 39 :                                              \
        (x) & (1ULL << 38) ? 38 :                                              \
        (x) & (1ULL << 37) ? 37 :                                              \
        (x) & (1ULL << 36) ? 36 :                                              \
        (x) & (1ULL << 35) ? 35 :                                              \
        (x) & (1ULL << 34) ? 34 :                                              \
        (x) & (1ULL << 33) ? 33 :                                              \
        (x) & (1ULL << 32) ? 32 :                                              \
        (x) & (1ULL << 31) ? 31 :                                              \
        (x) & (1ULL << 30) ? 30 :                                              \
        (x) & (1ULL << 29) ? 29 :                                              \
        (x) & (1ULL << 28) ? 28 :                                              \
        (x) & (1ULL << 27) ? 27 :                                              \
        (x) & (1ULL << 26) ? 26 :                                              \
        (x) & (1ULL << 25) ? 25 :                                              \
        (x) & (1ULL << 24) ? 24 :                                              \
        (x) & (1ULL << 23) ? 23 :                                              \
        (x) & (1ULL << 22) ? 22 :                                              \
        (x) & (1ULL << 21) ? 21 :                                              \
        (x) & (1ULL << 20) ? 20 :                                              \
        (x) & (1ULL << 19) ? 19 :                                              \
        (x) & (1ULL << 18) ? 18 :                                              \
        (x) & (1ULL << 17) ? 17 :                                              \
        (x) & (1ULL << 16) ? 16 :                                              \
        (x) & (1ULL << 15) ? 15 :                                              \
        (x) & (1ULL << 14) ? 14 :                                              \
        (x) & (1ULL << 13) ? 13 :                                              \
        (x) & (1ULL << 12) ? 12 :                                              \
        (x) & (1ULL << 11) ? 11 :                                              \
        (x) & (1ULL << 10) ? 10 :                                              \
        (x) & (1ULL <<  9) ?  9 :                                              \
        (x) & (1ULL <<  8) ?  8 :                                              \
        (x) & (1ULL <<  7) ?  7 :                                              \
        (x) & (1ULL <<  6) ?  6 :                                              \
        (x) & (1ULL <<  5) ?  5 :                                              \
        (x) & (1ULL <<  4) ?  4 :                                              \
        (x) & (1ULL <<  3) ?  3 :                                              \
        (x) & (1ULL <<  2) ?  2 :                                              \
        1                                                                      \
    )

#define W_IS_POW2(x)                                                           \
    (((x) != 0) && (((x) & ((x) - 1)) == 0))

#define W_ROUND_DOWN_POW2(x)                                                   \
    (1ULL << W_ILOG2(x))

#define W_ROUND_UP_POW2(x)                                                     \
    (                                                                          \
        (x) == 0 ? 0 :                                                         \
        (x) == 1 ? 1 :                                                         \
        (1ULL << (1 + W_ILOG2((x) - 1)))                                       \
    )

// Overflow Checks
// ---------------------------------------------------------------- //   O-(''Q)

#define W_UINT_IS_ADD_WRAPPING(max, x, ...)                                    \
    W__EXPAND(                                                                 \
        W__CONCAT(                                                             \
            W__UINT_IS_ADD_WRAPPING_,                                          \
            W__COUNT_ARGS(__VA_ARGS__)                                         \
        )(max, x, __VA_ARGS__)                                                 \
    )

#define W_UINT_IS_SUB_WRAPPING(max, x, ...)                                    \
    W__EXPAND(                                                                 \
        W__CONCAT(                                                             \
            W__UINT_IS_SUB_WRAPPING_,                                          \
            W__COUNT_ARGS(__VA_ARGS__)                                         \
        )(max, x, __VA_ARGS__)                                                 \
    )

#define W_UINT_IS_MUL_WRAPPING(max, x, ...)                                    \
    W__EXPAND(                                                                 \
        W__CONCAT(                                                             \
            W__UINT_IS_MUL_WRAPPING_,                                          \
            W__COUNT_ARGS(__VA_ARGS__)                                         \
        )(max, x, __VA_ARGS__)                                                 \
    )

#define W_IS_ROUND_UP_POW2_WRAPPING(max, x)                                    \
    (!W_IS_POW2(x) && W_ROUND_DOWN_POW2(x) > (max) / 2)

// Memory
// ---------------------------------------------------------------- //   O-(''Q)

#define W_ARE_BUFS_OVERLAPPING(a, b, size)                                     \
    (((uintptr_t)(a) <= (uintptr_t)(b)                                         \
         && (uintptr_t)(a) + (size) > (uintptr_t)(b))                          \
     || ((uintptr_t)(b) <= (uintptr_t)(a)                                      \
         && (uintptr_t)(b) + (size) > (uintptr_t)(a)))

#endif // UUKI_BASE_MACROS_H
