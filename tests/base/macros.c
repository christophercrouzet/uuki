#include <rexo.h>

#include <uuki/base/macros.h>

RX_TEST_CASE(macros, get_alignment)
{
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)0),
        0);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)1),
        1);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)2),
        2);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)3),
        4);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)4),
        4);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)61029),
        (size_t)1 << 16);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)1221700),
        (size_t)1 << 21);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)137438953472),
        (size_t)1 << 37);
    RX_UINT_REQUIRE_EQUAL(
        W_GET_ALIGNMENT((size_t)2100107785318157799),
        (size_t)1 << 61);
}

int
main(int argc,
     const char **argv)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
