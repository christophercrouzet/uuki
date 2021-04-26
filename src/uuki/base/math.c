#include "math.h"

#include <uuki/base/assert.h>
#include <uuki/base/macros.h>

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

size_t
w_size_round_up_pow2(size_t x)
{
    size_t i;

    W_ASSERT(!W_IS_ROUND_UP_POW2_WRAPPING(SIZE_MAX, x));

    --x;
    for (i = 1; i < sizeof(size_t) * CHAR_BIT; i *= 2) {
        x |= x >> i;
    }

    return ++x;
}
