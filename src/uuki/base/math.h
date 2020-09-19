#ifndef UUKI_BASE_MATH_H
#define UUKI_BASE_MATH_H

#include <uuki/base/private/math.h>

#include <limits.h>
#include <stddef.h>

#define W_IS_CEIL_POW2_WRAPPING(x, type)                                       \
    ((x) > (type)1 << (sizeof(type) * CHAR_BIT - 1))

#define W_CEIL_POW2(x, type)                                                   \
    (1 + (W__MATH_CEIL_POW2_1((x) - 1, type)))

int
w_size_is_pow2(size_t x);

size_t
w_size_ceil_pow2(size_t x);

#endif // UUKI_BASE_MATH_H
