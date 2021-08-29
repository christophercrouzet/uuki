#ifndef UUKI_BASE_VMEM_WINDOWS_C
#define UUKI_BASE_VMEM_WINDOWS_C

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/status.h>

#include <windows.h>

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
        SYSTEM_INFO system_info;

        GetSystemInfo(&system_info);
        granularity = (size_t)system_info.dwAllocationGranularity;
    }

    return granularity;
}

size_t
w_get_commit_granularity(
    void
)
{
    static size_t granularity = 0;
    
    if (granularity == 0)
    {
        SYSTEM_INFO system_info;

        GetSystemInfo(&system_info);
        granularity = (size_t)system_info.dwPageSize;
    }

    return granularity;
}

enum w_status
w_vmem_create(
    struct w_vmem *mem,
    void *addr,
    size_t size
)
{
    void *alloc;

    W_ASSERT(mem != NULL);
    W_ASSERT(addr == NULL || (uintptr_t)addr % w_get_alloc_granularity() == 0);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_alloc_granularity() == 0);

    alloc = VirtualAlloc(addr, size, MEM_RESERVE, PAGE_NOACCESS);
    if (alloc == NULL)
    {
        DWORD error;

        error = GetLastError();
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

    mem->addr = alloc;

#if W_TRACING(ENABLED)
    {
        W_TRACER_ATTACH_DATA(
            wp_vmem_tracer,
            mem,
            (
                &(struct wp_vmem_trace_data) {
                    .size = size,
                }
            )
        );
    }
#endif

    return W_SUCCESS;
}

void
w_vmem_destroy(
    struct w_vmem *mem,
    size_t size
)
{
#if W_TRACING(ENABLED)
    {
        struct wp_vmem_trace_data *trace_data;

        W_TRACER_GET_DATA_PTR(wp_vmem_tracer, &trace_data, mem);
        W_ASSERT(size == trace_data->size);
    }
#endif

    W_ASSERT(mem != NULL);
    W_ASSERT(mem->addr != NULL);
    W_ASSERT(size > 0);
    W_ASSERT(size % w_get_alloc_granularity() == 0);

    if (VirtualFree(mem->addr, 0, MEM_RELEASE) == 0)
    {
        DWORD error;

        error = GetLastError();
        W_LOG_WARNING(WP_VMEM_GET_ERROR_MSG("release"), size, mem->addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
    }

#if W_TRACING(ENABLED)
    {
        W_TRACER_DETACH_DATA(wp_vmem_tracer, mem);
    }
#endif
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

    if (VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE) == NULL)
    {
        DWORD error;

        error = GetLastError();
        W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG("commit"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_ALLOC_FAILED;
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

    if (VirtualFree(addr, size, MEM_DECOMMIT) == 0)
    {
        DWORD error;

        error = GetLastError();
        W_LOG_ERROR(WP_VMEM_GET_ERROR_MSG("uncommit"), size, addr);
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_ALLOC_FAILED;
    }

    return W_SUCCESS;
}

#endif // UUKI_BASE_VMEM_WINDOWS_C
