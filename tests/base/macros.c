#include <rexo.h>

#include <uuki/base/macros.h>

#include <stddef.h>

RX_TEST_CASE(macros, round_up_pow2)
{
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(0ULL),
        0);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(1ULL),
        1);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(2ULL),
        2);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(3ULL),
        4);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(4ULL),
        4);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(61029ULL),
        1ULL << 16);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(1221700ULL),
        1ULL << 21);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(137438953472ULL),
        1ULL << 37);
    RX_UINT_REQUIRE_EQUAL(
        W_ROUND_UP_POW2(2100107785318157799ULL),
        1ULL << 61);
}

int
main(int argc,
     const char **argv)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
