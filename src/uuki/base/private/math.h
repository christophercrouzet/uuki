#ifndef UUKI_BASE_PRIVATE_MATH_H
#define UUKI_BASE_PRIVATE_MATH_H

#include <limits.h>
#include <stddef.h>

#define W__MATH_CEIL_POW2_8(x, type)                                           \
    ((x) | ((x) >> 128))

#define W__MATH_CEIL_POW2_7(x, type)                                           \
    sizeof(type) >= 16                                                         \
        ? W__MATH_CEIL_POW2_8((x) | ((x) >> 64), type)                         \
        : (x)

#define W__MATH_CEIL_POW2_6(x, type)                                           \
    sizeof(type) >= 8                                                          \
        ? W__MATH_CEIL_POW2_7((x) | ((x) >> 32), type)                         \
        : (x)

#define W__MATH_CEIL_POW2_5(x, type)                                           \
    sizeof(type) >= 4                                                          \
        ? W__MATH_CEIL_POW2_6((x) | ((x) >> 16), type)                         \
        : (x)

#define W__MATH_CEIL_POW2_4(x, type)                                           \
    sizeof(type) >= 2                                                          \
        ? W__MATH_CEIL_POW2_5((x) | ((x) >> 8), type)                          \
        : (x)

#define W__MATH_CEIL_POW2_3(x, type)                                           \
    W__MATH_CEIL_POW2_4((x) | ((x) >> 4), type)

#define W__MATH_CEIL_POW2_2(x, type)                                           \
    W__MATH_CEIL_POW2_3((x) | ((x) >> 2), type)

#define W__MATH_CEIL_POW2_1(x, type)                                           \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wshift-count-overflow\"")               \
    W__MATH_CEIL_POW2_2((x) | ((x) >> 1), type)                                \
    _Pragma("GCC diagnostic pop")

#endif // UUKI_BASE_PRIVATE_MATH_H
