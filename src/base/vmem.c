#include "vmem.h"

#include <uuki/base/assert.h>
#include <uuki/base/platform.h>
#include <uuki/base/trace.h>

#include <stddef.h>

#define WP_VMEM_GET_ERROR_MSG(op)                                              \
    "failed to " op " %zu bytes of virtual memory at %p\n"

#define WP_VMEM_GET_ERROR_MSG_NO_ADDR(op)                                      \
    "failed to " op " %zu bytes of virtual memory\n"

#if W_TRACING(ENABLED)
    struct wp_vmem_trace_data {
        size_t size;
    };

    W_TRACER_INTIALIZE(
        wp_vmem_tracer, struct w_vmem, struct wp_vmem_trace_data, 1024
    );
#endif

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

size_t
w_round_up_alloc_size(
    size_t size
)
{
    size_t out;

    out = (size + (w_get_alloc_granularity() - 1))
        & -w_get_alloc_granularity();

    W_ASSERT((size == 0 && out == 0) || (size > 0 && out > 0));
    return out;
}

size_t
w_round_up_commit_size(
    size_t size
)
{
    size_t out;

    out = (size + (w_get_commit_granularity() - 1))
        & -w_get_commit_granularity();

    W_ASSERT((size == 0 && out == 0) || (size > 0 && out > 0));
    return out;
}

#if W_OS(UNIX)
    #include "vmem_unix.c"
#elif W_OS(WINDOWS)
    #include "vmem_windows.c"
#else
    #error "not implemented"
#endif
