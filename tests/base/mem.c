#include <rexo.h>

#include <uuki/base/mem.h>
#include <uuki/base/status.h>
#include <uuki/base/vmem.h>

#include <stddef.h>
#include <stdint.h>

#define ALIGN_UP(ptr, alignment)                                               \
    (                                                                          \
        ((uintptr_t)(ptr) + ((uintptr_t)(alignment) - 1))                      \
        & -(uintptr_t)(alignment)                                              \
    )

RX_TEST_CASE(mem, linear_alloc)
{
    size_t i;
    int status;
    size_t alloc_granularity;
    struct w_linear_alloc linear_alloc;
    void *ptr;
    void *it;

    alloc_granularity = w_get_alloc_granularity();

    // Create an allocator.
    status = w_linear_alloc_create(&linear_alloc, alloc_granularity);

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(linear_alloc.mem.addr != NULL);
    RX_PTR_REQUIRE_ALIGNED(linear_alloc.mem.addr, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, 0);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 0);

    // Allocate 5 bytes with a 8-byte alignment.
    status = w_linear_alloc_allocate(&linear_alloc, &ptr, 5, 8);
    for (i = 0; i < 5; ++i)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)i;
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, w_round_up_commit_size(5));
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 5);
    RX_PTR_REQUIRE_EQUAL(ptr, linear_alloc.mem.addr);
    RX_PTR_REQUIRE_ALIGNED(ptr, 8);

    it = linear_alloc.mem.addr;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);

    // Allocate 4 bytes with a 8-byte alignment.
    status = w_linear_alloc_allocate(&linear_alloc, &ptr, 4, 8);
    for (i = 0; i < 4; ++i)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)(i * 2);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, w_round_up_commit_size(12));
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 12);
    RX_PTR_REQUIRE_EQUAL(ptr, (void *)((uintptr_t)linear_alloc.mem.addr + 8));
    RX_PTR_REQUIRE_ALIGNED(ptr, 8);

    it = linear_alloc.mem.addr;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 6);

    // Allocate 3 bytes with a 128-byte alignment.
    status = w_linear_alloc_allocate(&linear_alloc, &ptr, 3, 128);
    for (i = 0; i < 3; ++i)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)(i * 3);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, w_round_up_commit_size(131));
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 131);
    RX_PTR_REQUIRE_EQUAL(ptr, (void *)((uintptr_t)linear_alloc.mem.addr + 128));
    RX_PTR_REQUIRE_ALIGNED(ptr, 128);

    it = linear_alloc.mem.addr;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 6);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 128);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 6);

    // Grow the last block.
    ptr = (void *)((uintptr_t)linear_alloc.mem.addr + 128);
    status = w_linear_alloc_reallocate(&linear_alloc, &ptr, 3, 5, 128);
    for (i = 3; i < 5; ++i)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)(i * 4);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, w_round_up_commit_size(133));
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 133);
    RX_PTR_REQUIRE_EQUAL(ptr, (void *)((uintptr_t)linear_alloc.mem.addr + 128));
    RX_PTR_REQUIRE_ALIGNED(ptr, 128);

    it = linear_alloc.mem.addr;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 6);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 128);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 12);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 16);

    // Grow the first block.
    ptr = linear_alloc.mem.addr;
    status = w_linear_alloc_reallocate(&linear_alloc, &ptr, 5, 7, 8);
    for (i = 5; i < 7; ++i)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)(i * 5);
    }

    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.committed, w_round_up_commit_size(143));
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, 143);
    RX_PTR_REQUIRE_EQUAL(ptr, (void *)((uintptr_t)linear_alloc.mem.addr + 136));
    RX_PTR_REQUIRE_ALIGNED(ptr, 8);

    it = linear_alloc.mem.addr;
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 8);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 6);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 128);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 6);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 12);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 16);

    it = (void *)((uintptr_t)linear_alloc.mem.addr + 136);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[0], 0);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[1], 1);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[2], 2);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[3], 3);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[4], 4);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[5], 25);
    RX_INT_REQUIRE_EQUAL(((unsigned char *)it)[6], 30);

    w_linear_alloc_destroy(&linear_alloc);
}

RX_TEST_CASE(mem, linear_realloc)
{
    int status;
    size_t alloc_granularity;
    struct w_linear_alloc linear_alloc;
    void *ptr;

    alloc_granularity = w_get_alloc_granularity();

    // Create an allocator.
    status = w_linear_alloc_create(&linear_alloc, alloc_granularity);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Allocate a first block.
    status = w_linear_alloc_allocate(
        &linear_alloc, &ptr, 16, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Allocate a second block that takes most of the remaining reserved space.
    status = w_linear_alloc_allocate(
        &linear_alloc, &ptr, alloc_granularity - 48, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Grow the first block such that it causes a new page to be reserved.
    ptr = linear_alloc.mem.addr;
    status = w_linear_alloc_reallocate(
        &linear_alloc, &ptr, 16, 64, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_PTR_REQUIRE_ALIGNED(linear_alloc.mem.addr, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity * 2);
    RX_UINT_REQUIRE_EQUAL(
        linear_alloc.committed, w_round_up_commit_size(alloc_granularity + 32)
    );
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, alloc_granularity + 32);
    RX_PTR_REQUIRE_EQUAL(
        ptr,
        (void *)((uintptr_t)linear_alloc.mem.addr + alloc_granularity - 32)
    );
    RX_PTR_REQUIRE_ALIGNED(ptr, 8);
}

RX_TEST_CASE(mem, linear_realloc_in_place)
{
    int status;
    size_t alloc_granularity;
    struct w_linear_alloc linear_alloc;
    void *ptr;

    alloc_granularity = w_get_alloc_granularity();

    // Create an allocator.
    status = w_linear_alloc_create(&linear_alloc, alloc_granularity);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Allocate a first block that takes most of the reserved space.
    status = w_linear_alloc_allocate(
        &linear_alloc, &ptr, alloc_granularity - 32, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Allocate a second block that fits in the same page.
    status = w_linear_alloc_allocate(
        &linear_alloc, &ptr, 16, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    // Grow the last block such that it causes a new page to be reserved.
    ptr = (void *)((uintptr_t)linear_alloc.mem.addr + alloc_granularity - 32);
    status = w_linear_alloc_reallocate(
        &linear_alloc, &ptr, 16, 64, 8
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_PTR_REQUIRE_ALIGNED(linear_alloc.mem.addr, alloc_granularity);
    RX_UINT_REQUIRE_EQUAL(linear_alloc.reserved, alloc_granularity * 2);
    RX_UINT_REQUIRE_EQUAL(
        linear_alloc.committed, w_round_up_commit_size(alloc_granularity + 32)
    );
    RX_UINT_REQUIRE_EQUAL(linear_alloc.used, alloc_granularity + 32);
    RX_PTR_REQUIRE_EQUAL(
        ptr,
        (void *)((uintptr_t)linear_alloc.mem.addr + alloc_granularity - 32)
    );
    RX_PTR_REQUIRE_ALIGNED(ptr, 8);
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
