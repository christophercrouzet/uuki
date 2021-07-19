#ifndef UUKI_BASE_PRIVATE_MACROS_H
#define UUKI_BASE_PRIVATE_MACROS_H

#include <uuki/base/platform.h>

#define WP_EXPAND(x)                                                           \
    x

#define WP_CONCAT_(x1, x2)                                                     \
    x1 ## x2

#define WP_CONCAT(x1, x2)                                                      \
    WP_CONCAT_(x1, x2)

#define WP_ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...)                        \
    _8

#define WP_COUNT_ARGS(...)                                                     \
    WP_EXPAND(                                                                 \
        WP_ARG(                                                                \
            __VA_ARGS__,                                                       \
            8, 7, 6, 5, 4, 3, 2, 1, 0,                                         \
        )                                                                      \
    )

#define WP_UINT_IS_ADD_WRAPPING_1(max, x1, x2)                                 \
    ((x1) > (max) - (x2))

#define WP_UINT_IS_ADD_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           WP_UINT_IS_ADD_WRAPPING_1(max, x1, x2)                              \
        || WP_UINT_IS_ADD_WRAPPING_1(max, x1 + x2, x3)                         \
    )

#define WP_UINT_IS_ADD_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           WP_UINT_IS_ADD_WRAPPING_2(max, x1, x2, x3)                          \
        || WP_UINT_IS_ADD_WRAPPING_1(max, x1 + x2 + x3, x4)                    \
    )

#define WP_UINT_IS_SUB_WRAPPING_1(max, x1, x2)                                 \
    ((x1) < (x2))

#define WP_UINT_IS_SUB_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           WP_UINT_IS_SUB_WRAPPING_1(max, x1, x2)                              \
        || WP_UINT_IS_SUB_WRAPPING_1(max, x1 - x2, x3)                         \
    )

#define WP_UINT_IS_SUB_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           WP_UINT_IS_SUB_WRAPPING_2(max, x1, x2, x3)                          \
        || WP_UINT_IS_SUB_WRAPPING_1(max, x1 - x2 - x3, x4)                    \
    )

#define WP_UINT_IS_MUL_WRAPPING_1(max, x1, x2)                                 \
    ((x1) > (max) / (x2))

#define WP_UINT_IS_MUL_WRAPPING_2(max, x1, x2, x3)                             \
    (                                                                          \
           WP_UINT_IS_MUL_WRAPPING_1(max, x1, x2)                              \
        || WP_UINT_IS_MUL_WRAPPING_1(max, x1 * x2, x3)                         \
    )

#define WP_UINT_IS_MUL_WRAPPING_3(max, x1, x2, x3, x4)                         \
    (                                                                          \
           WP_UINT_IS_MUL_WRAPPING_2(max, x1, x2, x3)                          \
        || WP_UINT_IS_MUL_WRAPPING_1(max, x1 * x2 * x3, x4)                    \
    )

#endif // UUKI_BASE_PRIVATE_MACROS_H
