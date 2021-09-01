#include <uuki/base/math.h>

#include <uuki/base/assert.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>

#include <stddef.h>
#include <stdint.h>

uint32_t
w_round_up_pow2_32(
    uint32_t x
)
{
    W_ASSERT(!W_IS_ROUND_UP_POW2_WRAPPING(UINT32_MAX, x));

    return x == 1
        ? 1
        : (uint32_t)1 << (32 - w_count_leading_zero_bits_32(x - 1));
}

uint64_t
w_round_up_pow2_64(
    uint64_t x
)
{
    W_ASSERT(!W_IS_ROUND_UP_POW2_WRAPPING(UINT64_MAX, x));

    return x == 1
        ? 1
        : (uint64_t)1 << (64 - w_count_leading_zero_bits_64(x - 1));
}

size_t
w_size_round_up_pow2(
    size_t x
)
{
    W_ASSERT(!W_IS_ROUND_UP_POW2_WRAPPING(SIZE_MAX, x));

#if W_SIZE_T_SIZE == 8
    return w_round_up_pow2_64(x);
#elif W_SIZE_T_SIZE == 4
    return w_round_up_pow2_32(x);
#else
    #error "unexpected size_t size"
#endif
}
