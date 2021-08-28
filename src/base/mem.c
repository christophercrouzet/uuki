#include "mem.h"

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
static const size_t
wp_mem_min_alignment = sizeof(void *);

static size_t
wp_mem_min(
    size_t a,
    size_t b
)
{
    return a < b ? a : b;
}

// System Allocator: Definitions
// ---------------------------------------------------------------- //   O-(''Q)

#if W_OS(WINDOWS)
    #include <malloc.h>
#else
    #include <stdlib.h>
#endif

static enum w_status
wp_mem_sys_alloc_allocate(
    void *inst,
    void **ptr,
    size_t size,
    size_t alignment
)
{
    void *buf;

    W_DISMISS_ARG(inst);

    W_ASSERT(ptr != NULL);
    W_ASSERT(size == 0 || W_IS_POW2(alignment));
    W_ASSERT(alignment >= wp_mem_min_alignment);

    if (size == 0)
    {
        *ptr = NULL;
        return W_SUCCESS;
    }

#if W_OS(WINDOWS)
    {
        buf = _aligned_malloc(size, alignment);
        if (buf == NULL)
        {
            goto alloc_error;
        }
    }
#else
    {
        if (posix_memalign(&buf, alignment, size) != 0)
        {
            goto alloc_error;
        }
    }
#endif

    *ptr = buf;
    return W_SUCCESS;

alloc_error:
    W_LOG_ERROR(
        "failed to allocate %zu bytes with a %zu-byte alignment\n",
        size,
        alignment
    );
    return W_ERROR_ALLOC_FAILED;
}

static void
wp_mem_sys_alloc_free(
    void *inst,
    const void *ptr,
    size_t size,
    size_t alignment
)
{
    W_DISMISS_ARG(inst);
    W_DISMISS_ARG(size);
    W_DISMISS_ARG(alignment);

    W_ASSERT(size == 0 || W_IS_POW2(alignment));
    W_ASSERT(alignment >= wp_mem_min_alignment);

#if W_OS(WINDOWS)
    _aligned_free((void *)ptr);
#else
    #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-qual"
    #endif
    free((void *)ptr);
    #if defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
#endif
}

static enum w_status
wp_mem_sys_alloc_reallocate(
    void *inst,
    void **ptr,
    size_t prev_size,
    size_t size,
    size_t alignment
)
{
    void *buf;

    W_DISMISS_ARG(inst);

    W_ASSERT(ptr != NULL);
    W_ASSERT(size == 0 || W_IS_POW2(alignment));
    W_ASSERT(alignment >= wp_mem_min_alignment);

    if (*ptr == NULL)
    {
        return wp_mem_sys_alloc_allocate(inst, ptr, size, alignment);
    }

    if (size == 0)
    {
        wp_mem_sys_alloc_free(inst, ptr, size, alignment);
        return W_SUCCESS;
    }

    if (size <= prev_size)
    {
        return W_SUCCESS;
    }

#if W_OS(WINDOWS)
    {
        buf = _aligned_realloc(*ptr, size, alignment);
        if (buf == NULL)
        {
            goto alloc_error;
        }
    }
#else
    {
        if (posix_memalign(&buf, alignment, size) != 0)
        {
            goto alloc_error;
        }

        memcpy(buf, *ptr, prev_size);
    }
#endif

    *ptr = buf;
    return W_SUCCESS;

alloc_error:
    W_LOG_ERROR(
        "failed to allocate %zu bytes with a %zu-byte alignment\n",
        size,
        alignment
    );
    return W_ERROR_ALLOC_FAILED;
}

static struct w_alloc
wp_mem_sys_alloc = {
    wp_mem_sys_alloc_allocate,
    wp_mem_sys_alloc_free,
    wp_mem_sys_alloc_reallocate,
    NULL,
};

// Linear Allocator
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_mem_linear_alloc_allocate(
    struct w_linear_alloc *alloc,
    size_t size
)
{
    enum w_status status;
    void *buf;
    size_t cap;

    status = W_SUCCESS;

    if (W_UINT_IS_ADD_WRAPPING(SIZE_MAX, alloc->used, size))
    {
        W_LOG_ERROR("the size and/or alignment requested are too large\n");
        return W_ERROR_MAX_SIZE_EXCEEDED;
    }

    cap = alloc->used + size;
    if (alloc->cap >= cap)
    {
        goto exit;
    }

    buf = alloc->buf;
    status = alloc->parent->reallocate(
        alloc->parent->inst, &buf, alloc->cap, cap, alloc->alignment);
    if (status != W_SUCCESS)
    {
        return status;
    }

    alloc->buf = buf;
    alloc->cap = cap;

exit:
    alloc->used = cap;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

enum w_status
w_linear_alloc_create(
    struct w_linear_alloc *alloc,
    struct w_alloc *parent,
    size_t size,
    size_t alignment
)
{
    enum w_status status;
    void *buf;

    W_ASSERT(alloc != NULL);

    if (parent == NULL)
    {
        parent = &wp_mem_sys_alloc;
    }

    if (alignment < wp_mem_min_alignment)
    {
        alignment = wp_mem_min_alignment;
    }

    buf = NULL;
    status = parent->allocate(parent->inst, &buf, size, alignment);
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to create the linear allocator\n");
        return status;
    }

    alloc->parent = parent;
    alloc->buf = buf;
    alloc->cap = size;
    alloc->used = 0;
    alloc->alignment = alignment;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

void
w_linear_alloc_destroy(
    struct w_linear_alloc *alloc
)
{
    W_ASSERT(alloc != NULL);

    alloc->parent->free(
        alloc->parent->inst, alloc->buf, alloc->cap, alloc->alignment);
}

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
    size_t padding;
    size_t end;

    W_ASSERT(inst != NULL);
    W_ASSERT(ptr != NULL);
    W_ASSERT(size == 0 || W_IS_POW2(alignment));

    status = W_SUCCESS;

    alloc = (struct w_linear_alloc *)inst;

    if (size == 0)
    {
        *ptr = NULL;
        return W_SUCCESS;
    }

    if (alignment < wp_mem_min_alignment)
    {
        alignment = wp_mem_min_alignment;
    }

    end = (uintptr_t)alloc->buf + alloc->used;

    // Compute the padding required to fulfill the requested alignment.
    W_ASSERT(!WP_MEM_ALIGN_UP_IS_WRAPPING(end, alignment));
    padding = (uintptr_t)WP_MEM_ALIGN_UP(end, alignment) - end;

    status = wp_mem_linear_alloc_allocate(alloc, size + padding);
    if (status != W_SUCCESS)
    {
        return status;
    }

    *ptr = (void *)((uintptr_t)alloc->buf + alloc->used - size);

    W_ASSERT(status == W_SUCCESS);
    return status;
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
}

enum w_status
w_linear_alloc_reallocate(
    void *inst,
    void **ptr,
    size_t prev_size,
    size_t size,
    size_t alignment
)
{
    enum w_status status;
    struct w_linear_alloc *alloc;
    size_t padding;
    size_t end;
    size_t src_offset;
    size_t dst_offset;

    W_ASSERT(inst != NULL);
    W_ASSERT(ptr != NULL);
    W_ASSERT(size == 0 || W_IS_POW2(alignment));

    status = W_SUCCESS;

    alloc = (struct w_linear_alloc *)inst;

    if (*ptr == NULL)
    {
        return w_linear_alloc_allocate(inst, ptr, size, alignment);
    }

    if (size == 0)
    {
        w_linear_alloc_free(inst, ptr, size, alignment);
        return W_SUCCESS;
    }

    if (alignment < wp_mem_min_alignment)
    {
        alignment = wp_mem_min_alignment;
    }

    end = (uintptr_t)alloc->buf + alloc->used;

    // The pointers might get invalidated after reallocating the buffer,
    // so we need to store their position relative to the beginning of
    // the buffer so that we can then recover their updated location.
    src_offset = (uintptr_t)*ptr - (uintptr_t)alloc->buf;
    dst_offset = alloc->used;

    if ((uintptr_t)*ptr + prev_size == end)
    {
        // The block to reallocate is the last one in the buffer.

        if (size <= prev_size)
        {
            // Shrink in-place.
            alloc->used -= prev_size - size;
            return W_SUCCESS;
        }

        if (W_UINT_IS_ADD_WRAPPING(SIZE_MAX, alloc->used, size - prev_size))
        {
            W_LOG_ERROR("the size requested is too large\n");
            return W_ERROR_MAX_SIZE_EXCEEDED;
        }

        if (alloc->cap >= alloc->used + size - prev_size)
        {
            // Grow in-place.
            alloc->used += size - prev_size;
            return W_SUCCESS;
        }

        size -= prev_size;
        end -= prev_size;
        dst_offset -= prev_size;
    }

    // Compute the padding required to fulfill the requested alignment.
    W_ASSERT(!WP_MEM_ALIGN_UP_IS_WRAPPING(end, alignment));
    padding = (uintptr_t)WP_MEM_ALIGN_UP(end, alignment) - end;

    dst_offset += padding;

    status = wp_mem_linear_alloc_allocate(alloc, size + padding);
    if (status != W_SUCCESS)
    {
        return status;
    }

    if (src_offset != dst_offset)
    {
        W_ASSERT(
            !W_ARE_REGIONS_OVERLAPPING(
                (void *)((uintptr_t)alloc->buf + dst_offset),
                wp_mem_min(prev_size, size),
                (void *)((uintptr_t)alloc->buf + src_offset),
                wp_mem_min(prev_size, size)
            )
        );
        memcpy(
            (void *)((uintptr_t)alloc->buf + dst_offset),
            (void *)((uintptr_t)alloc->buf + src_offset),
            wp_mem_min(prev_size, size)
        );
    }

    *ptr = (void *)((uintptr_t)alloc->buf + dst_offset);

    W_ASSERT(status == W_SUCCESS);
    return status;
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
