#include <intrin.h>

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

uint32_t
w_count_leading_zero_bits_32(
    uint32_t x
)
{
    unsigned long idx;

    _BitScanReverse(&idx, x);
    return (uint32_t)idx ^ 31;
}

uint64_t
w_count_leading_zero_bits_64(
    uint64_t x
)
{
    unsigned long idx;

    _BitScanReverse64(&idx, x);
    return (uint64_t)idx ^ 63;
}
