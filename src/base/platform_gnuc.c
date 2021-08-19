#include <stdint.h>

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

uint32_t
w_count_leading_zero_bits_32(
    uint32_t x
)
{
    return (uint32_t)__builtin_clz(x);
}

uint64_t
w_count_leading_zero_bits_64(
    uint64_t x
)
{
    return (uint64_t)__builtin_clzll(x);
}
