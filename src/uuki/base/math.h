#ifndef UUKI_BASE_MATH_H
#define UUKI_BASE_MATH_H

#include <limits.h>
#include <stddef.h>

#define W_IS_CEIL_POW2_WRAPPING(x, type)                                       \
    ((x) > (type)1 << (sizeof(type) * CHAR_BIT - 1))

int
w_size_is_pow2(size_t x);

size_t
w_size_ceil_pow2(size_t x);

#endif // UUKI_BASE_MATH_H
