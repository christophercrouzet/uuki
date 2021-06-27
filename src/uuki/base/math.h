#ifndef UUKI_BASE_MATH_H
#define UUKI_BASE_MATH_H

#include <stddef.h>
#include <stdint.h>

uint32_t
w_round_up_pow2_32(
    uint32_t x
);

uint64_t
w_round_up_pow2_64(
    uint64_t x
);

size_t
w_size_round_up_pow2(
    size_t x
);

#endif // UUKI_BASE_MATH_H
