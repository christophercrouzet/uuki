#include <rexo.h>

#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>
#include <uuki/base/vmem.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b)                                                              \
    ((a) < (b) ? (a) : (b))

struct alloc_info {
    void *addr;
    size_t size;
    int committed;
    int shared;
};

#if W_OS(UNIX)
    #include <unistd.h>

    static void
    get_alloc_info(
        struct alloc_info *info,
        void *begin_addr,
        void *end_addr
    )
    {
        int res;
        pid_t pid;
        char path[1024];
        FILE *file;
        int initialized;

        pid = getpid();

        res = sprintf(path, "/proc/%zu/maps", (size_t)pid);
        if (res < 0 || res > 1024)
        {
            W_LOG_ERROR("could not format the mapping file path\n");
            w_abort(-1);
        }

        file = fopen(path, "rbe");
        if (file == NULL)
        {
            W_LOG_ERROR("failed to open the mapping file\n");
            w_abort(-1);
        }

        initialized = 0;
        while (begin_addr < end_addr)
        {
            size_t pattern_len;
            char pattern[16];
            char line[1024];

            memset(pattern, '\0', sizeof pattern);
            res = sprintf(pattern, "%08lx", (uintptr_t)begin_addr);
            if (res < 0 || res > 16)
            {
                W_LOG_ERROR("could not format the base address\n");
                w_abort(-1);
            }

            pattern_len = (size_t)res;

            while (1)
            {
                uintptr_t range[2];
                char perms[4];
                size_t size;
                int committed;
                int shared;

                if (fgets(line, sizeof line, file) == NULL)
                {
                    W_LOG_ERROR("reached the end of the mapping file\n");
                    w_abort(-1);
                }

                if (strncmp(line, pattern, pattern_len) != 0)
                {
                    continue;
                }

                sscanf(
                    line,
                    "%lx-%lx %4s %*lx %*s %*d %*s",
                    &range[0],
                    &range[1],
                    perms
                );

                size = (size_t)(MIN(range[1], (uintptr_t)end_addr) - range[0]);
                committed =    perms[0] == 'r'
                            || perms[1] == 'w'
                            || perms[2] == 'x';
                shared = perms[3] == 's';

                if (!initialized)
                {
                    info->addr = (void *)range[0];
                    info->size = size;
                    info->committed = committed;
                    info->shared = shared;
                    initialized = 1;
                }
                else if (
                    committed != info->committed
                    || shared != info->shared
                )
                {
                    fclose(file);
                    return;
                }
                else
                {
                    info->size += size;
                }

                begin_addr = (void *)range[1];
                break;
            }
        }

        fclose(file);
    }
#elif W_OS(WINDOWS)
    #include <memoryapi.h>

    static void
    get_alloc_info(
        struct alloc_info *info,
        void *begin_addr,
        void *end_addr
    )
    {
        MEMORY_BASIC_INFORMATION mem_info;

        W_DISMISS_ARG(end_addr);

        if (VirtualQuery(begin_addr, &mem_info, sizeof mem_info) == 0)
        {
            W_LOG_ERROR("failed to query the virtual memory\n");
            w_abort(-1);
        }

        info->addr = mem_info.BaseAddress;
        info->size = mem_info.RegionSize;
        info->committed = mem_info.State == MEM_COMMIT;
        info->shared = mem_info.Type != MEM_PRIVATE;
    }
#else
    #error "unsupported OS"
#endif

static void
touch_pages
(
    void *addr,
    size_t size,
    size_t page_size
)
{
    size_t i;

    for (i = 0; i < size; i += page_size)
    {
        void *ptr;

        ptr = (void *)((uintptr_t)addr + i);
        *((uint32_t *)ptr) = 0xDECAFFFF;
    }
}
    
RX_TEST_CASE(virtual_mem, granularity)
{
    RX_UINT_REQUIRE_GREATER_OR_EQUAL(w_get_alloc_granularity(), 0x1000);
    RX_UINT_REQUIRE_GREATER_OR_EQUAL(w_get_commit_granularity(), 0x1000);
}

RX_TEST_CASE(virtual_mem, round_up)
{
    RX_UINT_REQUIRE_EQUAL(
        w_round_up_alloc_size(0x123),
        w_get_alloc_granularity()
    );
    RX_UINT_REQUIRE_EQUAL(
        w_round_up_commit_size(0x123),
        w_get_commit_granularity()
    );
}

RX_TEST_CASE(virtual_mem, main)
{
    enum w_status status;
    struct alloc_info info;
    struct w_vmem mem;
    void *begin_addr;
    void *end_addr;

    // Reserve some virtual memory.
    status = w_vmem_create(&mem, NULL, 0xA00000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    end_addr = (void *)((uintptr_t)mem.addr + 0xA00000);

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0xA00000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Commit a block at the beginning.
    begin_addr = mem.addr;
    status = w_vmem_commit(&mem, begin_addr, 0x200000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    touch_pages(begin_addr, 0x200000, w_get_commit_granularity());

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x200000);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x200000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x800000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Commit a block at the end.
    begin_addr = (void *)((uintptr_t)mem.addr + 0x900000);
    status = w_vmem_commit(&mem, begin_addr, 0x100000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    touch_pages(begin_addr, 0x100000, w_get_commit_granularity());

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x200000);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x200000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x700000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x900000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x100000);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Uncommit the block at the beginning.
    begin_addr = mem.addr;
    status = w_vmem_uncommit(&mem, begin_addr, 0x200000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x900000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x900000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Commit another block overlapping the uncommitted ones.
    begin_addr = (void *)((uintptr_t)mem.addr + 0x100000);
    status = w_vmem_commit(&mem, begin_addr, 0x700000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    touch_pages(begin_addr, 0x700000, w_get_commit_granularity());

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x100000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x100000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x700000);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x800000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x100000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    begin_addr = (void *)((uintptr_t)mem.addr + 0x900000);
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x100000);
    RX_BOOL_REQUIRE_TRUE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Release everything.
    w_vmem_destroy(&mem, 0xA00000);
}

RX_TEST_CASE(virtual_mem, fixed_addr)
{
    enum w_status status;
    struct alloc_info info;
    struct w_vmem mem;
    void *begin_addr;
    void *end_addr;

    // Reserve some virtual memory at a fixed address.
    status = w_vmem_create(&mem, (void *)0x100000, 0x200000);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_REQUIRE(mem.addr == (void *)0x100000);

    end_addr = (void *)((uintptr_t)mem.addr + 0x200000);

    begin_addr = mem.addr;
    get_alloc_info(&info, begin_addr, end_addr);
    RX_REQUIRE(info.addr == begin_addr);
    RX_UINT_REQUIRE_EQUAL(info.size, 0x200000);
    RX_BOOL_REQUIRE_FALSE(info.committed);
    RX_BOOL_REQUIRE_FALSE(info.shared);

    // Release everything.
    w_vmem_destroy(&mem, 0x200000);
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
