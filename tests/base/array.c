#include <rexo.h>

#include <uuki/base/array.h>
#include <uuki/base/macros.h>
#include <uuki/base/mem.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

W_PACKED_STRUCT(data) {
    int8_t a;
    int32_t b;
};

W_ARRAY_DECLARE(wp_data_array, struct data);

RX_TEST_CASE(array, main)
{
    size_t i;
    int status;
    struct w_alloc alloc;
    struct w_linear_alloc linear_alloc;
    struct wp_data_array array;
    struct data *slice;

    w_linear_alloc_create(&linear_alloc, 8);
    w_linear_alloc_get_universal_alloc(&alloc, &linear_alloc);

    // Creation.
    status = wp_data_array_create(&array, &alloc, 4);

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(array.buf != NULL);
    RX_UINT_REQUIRE_EQUAL(array.len, 0);
    RX_UINT_REQUIRE_EQUAL(array.capacity, 4);

    // Extension without reallocation.
    status = wp_data_array_extend(&array, &alloc, &slice, 3);
    for (i = 0; i < 3; ++i)
    {
        slice[i] = (struct data) {
            (int8_t)i,
            (int32_t)(i * i),
        };
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(array.buf != NULL);
    RX_UINT_REQUIRE_EQUAL(array.len, 3);
    RX_UINT_REQUIRE_EQUAL(array.capacity, 4);

    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[0], &(struct data){0, 0}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[1], &(struct data){1, 1}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[2], &(struct data){2, 4}, sizeof(struct data)), 0);

    // Extension with reallocation.
    status = wp_data_array_extend(&array, &alloc, &slice, 2);
    for (i = 0; i < 2; ++i)
    {
        slice[i] = (struct data) {
            (int8_t)i,
            (int32_t)((i + 3) * (i + 3) * (i + 3)),
        };
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(array.buf != NULL);
    RX_UINT_REQUIRE_EQUAL(array.len, 5);
    RX_UINT_REQUIRE_EQUAL(array.capacity, 8);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[0], &(struct data){0, 0}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[1], &(struct data){1, 1}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[2], &(struct data){2, 4}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[3], &(struct data){0, 27}, sizeof(struct data)), 0);
    RX_INT_REQUIRE_EQUAL(
        memcmp(&array.buf[4], &(struct data){1, 64}, sizeof(struct data)), 0);

    // Destruction.
    wp_data_array_destroy(&array, &alloc);
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
