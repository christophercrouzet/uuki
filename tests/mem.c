#include <rexo.h>

#include <uuki/base/mem.h>
#include <uuki/base/status.h>

RX_TEST_CASE(mem, linear_alloc)
{
    size_t i;
    int status;
    struct w_alloc alloc;
    struct w_linear_alloc linear_alloc;
    void *ptr;
    void *it;

    // Create an allocator with a capacity of 4 bytes and a 8192-byte alignment.
    status = w_linear_alloc_create(&linear_alloc, NULL, 4, 8192);

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 4);

    // Allocate 3 bytes with a 4096-byte alignment.
    status = w_linear_alloc_allocate(&linear_alloc, &ptr, 3, 4096);
    for (i = 0; i < 3; ++i) {
        ((unsigned char *)ptr)[i] = (unsigned char)i;
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 3);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 4);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)ptr % 4096, 0);

    it = linear_alloc.buf;
    RX_REQUIRE(it == ptr);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);

    // Allocate 2 bytes with a 128-byte alignment.
    status = w_linear_alloc_allocate(&linear_alloc, &ptr, 2, 128);
    for (i = 0; i < 2; ++i) {
        ((unsigned char *)ptr)[i] = (unsigned char)((i + 3) * 2);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 130);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 256);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)ptr % 128, 0);

    it = linear_alloc.buf;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);

    it = (void *)((uintptr_t)linear_alloc.buf + 128);
    RX_REQUIRE(it == ptr);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 8);

    // Reallocate the last block.
    ptr = (void *)((uintptr_t)linear_alloc.buf + 128);
    status = w_linear_alloc_reallocate(&linear_alloc, &ptr, 2, 5, 128);
    for (i = 2; i < 5; ++i) {
        ((unsigned char *)ptr)[i] = (unsigned char)((i + 3) * 2);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 133);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 256);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)ptr % 128, 0);

    it = linear_alloc.buf;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);

    it = (void *)((uintptr_t)linear_alloc.buf + 128);
    RX_REQUIRE(it == ptr);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 10);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 12);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 14);

    // Reallocate the first block.
    ptr = linear_alloc.buf;
    status = w_linear_alloc_reallocate(&linear_alloc, &ptr, 3, 6, 4096);
    for (i = 3; i < 6; ++i) {
        ((unsigned char *)ptr)[i] = (unsigned char)i;
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 4102);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 8192);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)ptr % 4096, 0);

    it = linear_alloc.buf;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);

    it = (void *)((uintptr_t)linear_alloc.buf + 128);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 10);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 12);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 14);

    it = (void *)((uintptr_t)linear_alloc.buf + 4096);
    RX_REQUIRE(it == ptr);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[5], 5);

    // Allocate 5 bytes with a 64-byte alignment using
    // the universal allocator interface.
    w_linear_alloc_get_universal_alloc(&alloc, &linear_alloc);
    status = alloc.allocate(alloc.inst, &ptr, 5, 64);
    for (i = 0; i < 5; ++i) {
        ((unsigned char *)ptr)[i] = (unsigned char)((i + 5) * 3);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.buf != NULL);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)linear_alloc.buf % 8192, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 4165);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.cap, 8192);
    RX_UINT_REQUIRE_EQUAL((uintptr_t)ptr % 64, 0);

    it = linear_alloc.buf;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);

    it = (void *)((uintptr_t)linear_alloc.buf + 128);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 10);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 12);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 14);

    it = (void *)((uintptr_t)linear_alloc.buf + 4096);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[5], 5);

    it = (void *)((uintptr_t)linear_alloc.buf + 4160);
    RX_REQUIRE(it == ptr);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 15);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 18);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 21);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 24);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 27);

    w_linear_alloc_destroy(&linear_alloc);
}

int
main(int argc,
     const char **argv)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
