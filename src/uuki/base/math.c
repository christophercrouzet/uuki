#include "math.h"

#include <uuki/base/assert.h>
#include <uuki/base/macros.h>

#include <limits.h>
#include <stddef.h>

int
w_size_is_pow2(size_t x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

size_t
w_size_ceil_pow2(size_t x)
{
    size_t i;

    W_ASSERT(!W_IS_CEIL_POW2_WRAPPING(size_t, x));

    --x;
    for (i = 1; i < sizeof(size_t) * CHAR_BIT; i *= 2) {
        x |= x >> i;
    }

    return ++x;
}
