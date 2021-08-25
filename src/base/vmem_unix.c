#ifndef UUKI_BASE_VMEM_UNIX_C
#define UUKI_BASE_VMEM_UNIX_C

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/status.h>
#include <uuki/base/trace.h>

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stddef.h>
#include <stdint.h>

#if !defined(MAP_ANONYMOUS)
    #if defined(MAP_ANON)
        #define MAP_ANONYMOUS MAP_ANON
    #else
        #error "anonymous pages not available"
    #endif
#endif

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

size_t
w_get_alloc_granularity(
    void
)
{
    static size_t granularity = 0;
    
    if (granularity == 0)
    {
        // For portability reasons, the manual page for getpagesize(2)
        // recommends using `sysconf(_SC_PAGESIZE)` instead of `getpagesize()`.
        granularity = (size_t)sysconf(_SC_PAGESIZE);
    }

    return granularity;
}

size_t
w_get_commit_granularity(
    void
)
{
    // Unix doesn't differentiate between allocation and commit granularities.
    return w_get_alloc_granularity();
}

enum w_status
w_vmem_create(
    struct w_vmem *mem,
    void *addr,
    size_t size
)
{
#if W_TRACING(ENABLED)
    W_TRACER_ATTACH_DATA(
        wp_vmem_tracer,
        mem,
        (
            &(struct wp_vmem_trace_data) {
                .size = size,
            }
        )
    );
#endif

    int flags;
    void *alloc;

    W_ASSERT(mem != NULL);
    W_ASSERT(addr == NULL || (uintptr_t)addr % w_get_alloc_granularity() == 0);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_alloc_granularity() == 0);

    flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (addr != NULL)
    {
        flags |= MAP_FIXED;
    }

    alloc = mmap(addr, size, PROT_NONE, flags, -1, 0);
    if (alloc == MAP_FAILED)
    {
        int error;

        error = errno;

        if (addr == NULL)
        {
            W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG_NO_ADDR("reserve"), size);
        }
        else 
        {
            W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG("reserve"), size, addr);
        }

        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_ALLOC_FAILED;
    }

    if (madvise(alloc, size, MADV_DONTNEED) != 0)
    {
        int error;

        error = errno;
        W_LOG_WARNING(WP_VMEM_GET_ERROR_MSG("advise"), size, alloc);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
    }

    mem->addr = alloc;
    return W_SUCCESS;
}

void
w_vmem_destroy(
    struct w_vmem *mem,
    size_t size
)
{
#if W_TRACING(ENABLED)
    W_TRACER_DETACH_DATA(wp_vmem_tracer, mem);
#endif

    W_ASSERT(mem != NULL);
    W_ASSERT(mem->addr != NULL);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_alloc_granularity() == 0);

    if (munmap(mem->addr, size) != 0)
    {
        int error;

        error = errno;
        W_LOG_WARNING(WP_VMEM_GET_ERROR_MSG("release"), size, mem->addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
    }
}

enum w_status
w_vmem_commit(
    struct w_vmem *mem,
    void *addr,
    size_t size
)
{
#if W_TRACING(ENABLED)
    {
        struct wp_vmem_trace_data *trace_data;

        W_TRACER_GET_DATA_PTR(wp_vmem_tracer, &trace_data, mem);
        W_ASSERT(
            (uintptr_t)addr >= (uintptr_t)mem->addr
        );
        W_ASSERT(
            (uintptr_t)addr + size <= (uintptr_t)mem->addr + trace_data->size
        );
    }
#endif

    W_ASSERT(mem != NULL);
    W_ASSERT(mem->addr != NULL);
    W_ASSERT(addr != NULL);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_commit_granularity() == 0);

    if (mprotect(addr, size, PROT_READ | PROT_WRITE) != 0)
    {
        int error;

        error = errno;
        W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG("commit"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_ALLOC_FAILED;
    }

    if (madvise(addr, size, MADV_WILLNEED) != 0)
    {
        int error;

        error = errno;
        W_LOG_WARNING(WP_VMEM_GET_ERROR_MSG("advise"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
    }

    return W_SUCCESS;
}

enum w_status
w_vmem_uncommit(
    struct w_vmem *mem,
    void *addr,
    size_t size
)
{
#if W_TRACING(ENABLED)
    {
        struct wp_vmem_trace_data *trace_data;

        W_TRACER_GET_DATA_PTR(wp_vmem_tracer, &trace_data, mem);
        W_ASSERT(
            (uintptr_t)addr >= (uintptr_t)mem->addr
        );
        W_ASSERT(
            (uintptr_t)addr + size <= (uintptr_t)mem->addr + trace_data->size
        );
    }
#endif

    W_ASSERT(mem != NULL);
    W_ASSERT(mem->addr != NULL);
    W_ASSERT(addr != NULL);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_commit_granularity() == 0);

    if (mprotect(addr, size, PROT_NONE) != 0)
    {
        int error;

        error = errno;
        W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG("uncommit"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_ALLOC_FAILED;
    }

    if (madvise(addr, size, MADV_DONTNEED) != 0)
    {
        int error;

        error = errno;
        W_LOG_WARNING(WP_VMEM_GET_ERROR_MSG("uncommit"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
    }

    return W_SUCCESS;
}

#endif // UUKI_BASE_VMEM_UNIX_C
