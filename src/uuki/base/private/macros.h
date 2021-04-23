#ifndef UUKI_BASE_PRIVATE_MACROS_H
#define UUKI_BASE_PRIVATE_MACROS_H

#include <uuki/base/platform.h>

#define W__EXPAND(x)                                                           \
    x

#define W__CONCAT_(x1, x2)                                                     \
    x1 ## x2

#define W__CONCAT(x1, x2)                                                      \
    W__CONCAT_(x1, x2)

#define W__ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...)                        \
    _8

#define W__COUNT_ARGS(...)                                                     \
    W__EXPAND(                                                                 \
        W__ARG(                                                                \
            __VA_ARGS__,                                                       \
            8, 7, 6, 5, 4, 3, 2, 1, 0,))

#define W__UINT_IS_ADD_WRAPPING_1(max, x1, x2)                                 \
    ((x1) > (max) - (x2))

#define W__UINT_IS_ADD_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           W__UINT_IS_ADD_WRAPPING_1(max, x1, x2)                              \
        || W__UINT_IS_ADD_WRAPPING_1(max, x1 + x2, x3)                         \
    )

#define W__UINT_IS_ADD_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           W__UINT_IS_ADD_WRAPPING_2(max, x1, x2, x3)                          \
        || W__UINT_IS_ADD_WRAPPING_1(max, x1 + x2 + x3, x4)                    \
    )

#define W__UINT_IS_SUB_WRAPPING_1(max, x1, x2)                                 \
    ((x1) < (x2))

#define W__UINT_IS_SUB_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           W__UINT_IS_SUB_WRAPPING_1(max, x1, x2)                              \
        || W__UINT_IS_SUB_WRAPPING_1(max, x1 - x2, x3)                         \
    )

#define W__UINT_IS_SUB_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           W__UINT_IS_SUB_WRAPPING_2(max, x1, x2, x3)                          \
        || W__UINT_IS_SUB_WRAPPING_1(max, x1 - x2 - x3, x4)                    \
    )

#define W__UINT_IS_MUL_WRAPPING_1(max, x1, x2)                                 \
    ((x1) > (max) / (x2))

#define W__UINT_IS_MUL_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           W__UINT_IS_MUL_WRAPPING_1(max, x1, x2)                              \
        || W__UINT_IS_MUL_WRAPPING_1(max, x1 * x2, x3)                         \
    )

#define W__UINT_IS_MUL_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           W__UINT_IS_MUL_WRAPPING_2(max, x1, x2, x3)                          \
        || W__UINT_IS_MUL_WRAPPING_1(max, x1 * x2 * x3, x4)                    \
    )

#if W_SIZE_T_SIZE >= 16
    #error "unexpected size_t width"
#else
    #define W__MACROS_GET_ALIGNMENT_6(x)                                       \
        ((x) | ((x) >> 32))
#endif

#if W_SIZE_T_SIZE >= 8
    #define W__MACROS_GET_ALIGNMENT_5(x)                                       \
        W__MACROS_GET_ALIGNMENT_6((x) | ((x) >> 16))
#else
    #define W__MACROS_GET_ALIGNMENT_5(x)                                       \
        ((x) | ((x) >> 16))
#endif

#if W_SIZE_T_SIZE >= 4
    #define W__MACROS_GET_ALIGNMENT_4(x)                                       \
        W__MACROS_GET_ALIGNMENT_5((x) | ((x) >> 8))
#else
    #define W__MACROS_GET_ALIGNMENT_4(x)                                       \
        ((x) | ((x) >> 8))
#endif

#if W_SIZE_T_SIZE >= 2
    #define W__MACROS_GET_ALIGNMENT_3(x)                                       \
        W__MACROS_GET_ALIGNMENT_4((x) | ((x) >> 4))
#else
    #define W__MACROS_GET_ALIGNMENT_3(x)                                       \
        ((x) | ((x) >> 4))
#endif

#if W_SIZE_T_SIZE >= 1
    #define W__MACROS_GET_ALIGNMENT_2(x)                                       \
        W__MACROS_GET_ALIGNMENT_3((x) | ((x) >> 2))
#else
    #define W__MACROS_GET_ALIGNMENT_2(x)                                       \
        ((x) | ((x) >> 2))
#endif

#define W__MACROS_GET_ALIGNMENT_1(x)                                           \
    W__MACROS_GET_ALIGNMENT_2((x) | ((x) >> 1))

#endif // UUKI_BASE_PRIVATE_MACROS_H
