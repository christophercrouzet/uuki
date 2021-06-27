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
            8, 7, 6, 5, 4, 3, 2, 1, 0,                                         \
        )                                                                      \
    )

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

#endif // UUKI_BASE_PRIVATE_MACROS_H
