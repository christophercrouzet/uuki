#include "mem.h"

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>
#include <uuki/base/trace.h>
#include <uuki/base/vmem.h>

#include <stddef.h>
#include <stdint.h>

#define WP_MEM_GROW_FACTOR                                                     \
    2

#define WP_MEM_ALIGN_UP_IS_WRAPPING(ptr, alignment)                            \
    W_UINT_IS_ADD_WRAPPING(                                                    \
        UINTPTR_MAX, (uintptr_t)(ptr), (uintptr_t)(alignment) - 1)

#define WP_MEM_ALIGN_UP(ptr, alignment)                                        \
    (                                                                          \
        ((uintptr_t)(ptr) + ((uintptr_t)(alignment) - 1))                      \
        & -(uintptr_t)(alignment)                                              \
    )

// Any power of two alignment that is greater or equal to this value
// is guaranteed to be a multiple of `sizeof(void *)`, thus conforming to
// the requirement of `posix_memalign()`.
#define WP_MEM_MIN_ALIGNMENT                                                   \
    sizeof(void *)

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static size_t
wp_mem_max(
    size_t a,
    size_t b
)
{
    return a > b ? a : b;
}

static size_t
wp_mem_grow_reserved_size(
    size_t reserved,
    size_t requested
)
{
    W_ASSERT(!W_UINT_IS_MUL_WRAPPING(SIZE_MAX, reserved, WP_MEM_GROW_FACTOR));
    return wp_mem_max(reserved * WP_MEM_GROW_FACTOR, requested);
}

static enum w_status
wp_mem_realloc(
    struct w_vmem *mem,
    size_t old_reserved,
    size_t new_reserved,
    size_t old_used,
    size_t new_committed
)
{
    enum w_status status;
    struct w_vmem mem_buf;

    status = w_vmem_create(&mem_buf, NULL, new_reserved);
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to reserve more space\n");
        return status;
    }

    status = w_vmem_commit(&mem_buf, mem_buf.addr, new_committed);
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to commit more space\n");
        goto undo_creation;
    }

    w_copy_mem(mem_buf.addr, mem->addr, old_used);
    w_vmem_destroy(mem, old_reserved);
    *mem = mem_buf;

    W_ASSERT(status == W_SUCCESS);
    return status;

undo_creation:
    w_vmem_destroy(&mem_buf, new_reserved);
    return status;
}

// Helpers: Linear Allocator
// ---------------------------------------------------------------- //   O-(''Q)

#if W_TRACING(ENABLED)
    struct wp_mem_linear_alloc_ptr_data {
        size_t size;
        size_t alignment;
    };

    W_TRACE_POOL_DECLARE(
        wp_mem_linear_alloc_ptr_pool,
        struct wp_mem_linear_alloc_ptr_data,
        128
    );

    struct wp_mem_linear_alloc_trace_data {
        struct wp_mem_linear_alloc_ptr_pool ptr_pool;
    };

    W_TRACER_INTIALIZE(
        wp_mem_linear_alloc_tracer,
        struct wp_mem_linear_alloc_trace_data,
        64
    );
#endif

enum w_status
wp_mem_linear_alloc_ensure_has_enough_space(
    struct w_linear_alloc *alloc,
    size_t used
)
{
    if (used > alloc->reserved)
    {
        enum w_status status;
        size_t reserved;
        size_t committed;

        reserved = w_round_up_alloc_size(
            wp_mem_grow_reserved_size(alloc->reserved, used)
        );
        committed = w_round_up_commit_size(used);
        status = wp_mem_realloc(
            &alloc->mem,
            alloc->reserved,
            reserved,
            alloc->used,
            committed
        );
        if (status != W_SUCCESS)
        {
            W_LOG_DEBUG("failed to reallocate the virtual memory\n");
            return status;
        }

        alloc->reserved = reserved;
        alloc->committed = committed;

        W_LOG_WARNING("consider increasing the initial reserved size\n");
    }
    else if (used > alloc->committed)
    {
        enum w_status status;
        size_t committed;

        committed = w_round_up_commit_size(used);
        status = w_vmem_commit(&alloc->mem, alloc->mem.addr, committed);
        if (status != W_SUCCESS)
        {
            W_LOG_DEBUG("failed to commit more space\n");
            return status;
        }

        alloc->committed = committed;
    }

    return W_SUCCESS;
}

// Public API: Linear Allocator
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
w_linear_alloc_create(
    struct w_linear_alloc *alloc,
    size_t size
)
{
    enum w_status status;

    W_ASSERT(alloc != NULL);
    W_ASSERT(size > 0);

    size = w_round_up_alloc_size(size);
    status = w_vmem_create(&alloc->mem, NULL, size);
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to create the linear allocator\n");
        return status;
    }

    alloc->reserved = size;
    alloc->committed = 0;
    alloc->used = 0;

#if W_TRACING(ENABLED)
    {
        W_TRACER_ATTACH_DATA(
            wp_mem_linear_alloc_tracer,
            alloc,
            (
                &(struct wp_mem_linear_alloc_trace_data) {
                    .ptr_pool = { 0 },
                }
            )
        );
    }
#endif

    return status;
}

void
w_linear_alloc_destroy(
    struct w_linear_alloc *alloc
)
{
    W_ASSERT(alloc != NULL);

    w_vmem_destroy(&alloc->mem, alloc->reserved);

#if W_TRACING(ENABLED)
    {
        W_TRACER_DETACH_DATA(wp_mem_linear_alloc_tracer, alloc);
    }
#endif
}

// Public API: Linear Allocator (Universal Allocator API)
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
w_linear_alloc_allocate(
    void *inst,
    void **ptr,
    size_t size,
    size_t alignment
)
{
    enum w_status status;
    struct w_linear_alloc *alloc;
    size_t end;
    size_t padding;
    size_t used;

    W_ASSERT(inst != NULL);
    W_ASSERT(ptr != NULL);
    W_ASSERT(W_IS_POW2(alignment));
    W_ASSERT(alignment >= WP_MEM_MIN_ALIGNMENT);

    alloc = (struct w_linear_alloc *)inst;

    if (size == 0)
    {
        *ptr = NULL;
        return W_SUCCESS;
    }

    // The new block is to be appended at the end of the used space.
    end = (uintptr_t)alloc->mem.addr + alloc->used;

    // Compute the padding required to fulfill the requested alignment.
    W_ASSERT(!WP_MEM_ALIGN_UP_IS_WRAPPING(end, alignment));
    padding = (uintptr_t)WP_MEM_ALIGN_UP(end, alignment) - end;

    // Compute the total size about to be used.
    W_ASSERT(!W_UINT_IS_ADD_WRAPPING(SIZE_MAX, alloc->used, padding, size));
    used = alloc->used + padding + size;

    status = wp_mem_linear_alloc_ensure_has_enough_space(alloc, used);
    if (status != W_SUCCESS)
    {
        return status;
    }

    alloc->used = used;
    *ptr = (void *)((uintptr_t)alloc->mem.addr + used - size);

#if W_TRACING(ENABLED)
    {
        struct wp_mem_linear_alloc_trace_data *trace_data;

        W_TRACER_FIND_DATA(wp_mem_linear_alloc_tracer, &trace_data, inst);
        W_TRACE_POOL_INSERT_DATA(
            trace_data->ptr_pool,
            *ptr,
            (
                &(struct wp_mem_linear_alloc_ptr_data) {
                    .size = size,
                    .alignment = alignment,
                }
            )
        );
    }
#endif

    return W_SUCCESS;
}

void
w_linear_alloc_free(
    void *inst,
    const void *ptr,
    size_t size,
    size_t alignment
)
{
    W_DISMISS_ARG(inst);
    W_DISMISS_ARG(ptr);
    W_DISMISS_ARG(size);
    W_DISMISS_ARG(alignment);

    W_ASSERT(inst != NULL);
    W_ASSERT(W_IS_POW2(alignment));
    W_ASSERT(alignment >= WP_MEM_MIN_ALIGNMENT);

#if W_TRACING(ENABLED)
    {
        struct wp_mem_linear_alloc_trace_data *trace_data;

        W_TRACER_FIND_DATA(wp_mem_linear_alloc_tracer, &trace_data, inst);
        W_TRACE_POOL_REMOVE_DATA(trace_data->ptr_pool, ptr);
    }
#endif
}

enum w_status
w_linear_alloc_reallocate(
    void *inst,
    void **ptr,
    size_t old_size,
    size_t new_size,
    size_t alignment
)
{
#if W_TRACING(ENABLED)
    if (*ptr != NULL)
    {
        struct wp_mem_linear_alloc_trace_data *trace_data;
        struct wp_mem_linear_alloc_ptr_data *ptr_data;

        W_TRACER_FIND_DATA(wp_mem_linear_alloc_tracer, &trace_data, inst);
        W_TRACE_POOL_FIND_DATA(trace_data->ptr_pool, &ptr_data, *ptr);
        W_ASSERT(old_size == ptr_data->size);
        W_ASSERT(alignment == ptr_data->alignment);
    }
#endif

    enum w_status status;
    struct w_linear_alloc *alloc;
    size_t ptr_offset;
    size_t end;
    size_t used;
    int is_last_block;
    void *old_ptr;
    void *new_ptr;

    W_ASSERT(inst != NULL);
    W_ASSERT(ptr != NULL);
    W_ASSERT(W_IS_POW2(alignment));
    W_ASSERT(alignment >= WP_MEM_MIN_ALIGNMENT);

    alloc = (struct w_linear_alloc *)inst;

    if (*ptr == NULL)
    {
        return w_linear_alloc_allocate(inst, ptr, new_size, alignment);
    }

    if (new_size == 0)
    {
        w_linear_alloc_free(inst, ptr, new_size, alignment);
        return W_SUCCESS;
    }

    ptr_offset = (uintptr_t)*ptr - (uintptr_t)alloc->mem.addr;
    end = (uintptr_t)alloc->mem.addr + alloc->used;
    is_last_block = (uintptr_t)*ptr + old_size == end;

    // Compute the total size about to be used.
    if (is_last_block)
    {
        W_ASSERT(
            !W_UINT_IS_ADD_WRAPPING(SIZE_MAX, alloc->used, new_size - old_size)
        );
        used = alloc->used + (new_size - old_size);
    }
    else
    {
        size_t padding;

        // Compute the padding required to fulfill the requested alignment.
        W_ASSERT(!WP_MEM_ALIGN_UP_IS_WRAPPING(end, alignment));
        padding = (uintptr_t)WP_MEM_ALIGN_UP(end, alignment) - end;

        W_ASSERT(!W_UINT_IS_ADD_WRAPPING(
            SIZE_MAX, alloc->used, padding, new_size)
        );
        used = alloc->used + padding + new_size;
    }

    status = wp_mem_linear_alloc_ensure_has_enough_space(alloc, used);
    if (status != W_SUCCESS)
    {
        return status;
    }

    old_ptr = (void *)((uintptr_t)alloc->mem.addr + ptr_offset);
    new_ptr = (void *)((uintptr_t)alloc->mem.addr + used - new_size);

    if (!is_last_block)
    {
        w_copy_mem(new_ptr, old_ptr, old_size);
    }

#if W_TRACING(ENABLED)
    if (*ptr != new_ptr)
    {
        struct wp_mem_linear_alloc_trace_data *trace_data;

        W_TRACER_FIND_DATA(wp_mem_linear_alloc_tracer, &trace_data, inst);
        W_TRACE_POOL_REMOVE_DATA(trace_data->ptr_pool, *ptr);
        W_TRACE_POOL_INSERT_DATA(
            trace_data->ptr_pool,
            new_ptr,
            (
                &(struct wp_mem_linear_alloc_ptr_data) {
                    .size = new_size,
                    .alignment = alignment,
                }
            )
        );
    }
#endif

    *ptr = new_ptr;
    alloc->used = used;
    return W_SUCCESS;
}

void
w_linear_alloc_get_universal_alloc(
    struct w_alloc *alloc,
    struct w_linear_alloc *linear_alloc
)
{
    W_ASSERT(alloc != NULL);
    W_ASSERT(linear_alloc != NULL);

    alloc->allocate = w_linear_alloc_allocate;
    alloc->free = w_linear_alloc_free;
    alloc->reallocate = w_linear_alloc_reallocate;
    alloc->inst = linear_alloc;
}
