#include "array.h"

#include <uuki/base/assert.h>
#include <uuki/base/logging.h>
#include <uuki/base/macros.h>
#include <uuki/base/math.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

#define W__ARRAY_GET_MAX_CAP(element_size)                                     \
    (SIZE_MAX / (element_size))

static size_t
w__array_max(size_t a,
             size_t b)
{
    return a > b ? a : b;
}

static void
w__array_grow_cap(size_t *cap,
                  size_t req,
                  size_t element_size)
{
    req = w__array_max(*cap * 2, req);

    W_ASSERT(!W_UINT_IS_MUL_WRAPPING(SIZE_MAX, req, element_size));
    if (W_IS_ROUND_UP_POW2_WRAPPING(SIZE_MAX, req * element_size)) {
        *cap = SIZE_MAX / element_size;
        return;
    }

    *cap = w_size_round_up_pow2(req);
}

static enum w_status
w__array_realloc(struct w_alloc *alloc,
                 size_t element_size,
                 size_t alignment,
                 void **buf,
                 size_t *cap,
                 size_t req)
{
    enum w_status status;
    size_t new_cap;

    W_ASSERT(alloc != NULL);
    W_ASSERT(buf != NULL);
    W_ASSERT(cap != NULL);
    W_ASSERT(req <= W__ARRAY_GET_MAX_CAP(element_size));
    W_ASSERT(*buf == NULL || req > *cap);

    new_cap = *cap;
    w__array_grow_cap(&new_cap, req, element_size);
    W_ASSERT(new_cap > *cap);

    status = W_REALLOCATE_ALIGNED(
        alloc,
        buf,
        element_size * (*cap),
        element_size * new_cap,
        alignment);
    if (status != W_SUCCESS) {
        W_LOG_DEBUG("failed to reallocate the array\n");
        return status;
    }

    *cap = new_cap;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

enum w_status
w__array_create(struct w_alloc *alloc,
                size_t element_size,
                size_t alignment,
                void **array_buf,
                size_t *array_cap,
                size_t *array_count,
                size_t cap)
{
    enum w_status status;
    void *buf;
    size_t new_cap;

    W_ASSERT(alloc != NULL);
    W_ASSERT(array_buf != NULL);
    W_ASSERT(array_cap != NULL);
    W_ASSERT(array_count != NULL);

    if (cap > W__ARRAY_GET_MAX_CAP(element_size)) {
        W_LOG_ERROR("the requested capacity is too large\n");
        return W_ERROR_MAX_SIZE_EXCEEDED;
    }

    buf = NULL;
    new_cap = 0;
    status = w__array_realloc(
        alloc, element_size, alignment, &buf, &new_cap, cap);
    if (status != W_SUCCESS) {
        W_LOG_DEBUG("failed to create the array\n");
        return status;
    }

    *array_buf = buf;
    *array_cap = new_cap;
    *array_count = 0;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

void
w__array_destroy(struct w_alloc *alloc,
                 size_t element_size,
                 size_t alignment,
                 void *array_buf,
                 size_t array_cap)
{
    W_ASSERT(alloc != NULL);
    W_ASSERT(array_buf != NULL);

    W_FREE_ALIGNED(
        alloc,
        array_buf,
        element_size * array_cap,
        alignment);
}

enum w_status
w__array_extend(struct w_alloc *alloc,
                size_t element_size,
                size_t alignment,
                void **slice,
                void **array_buf,
                size_t *array_cap,
                size_t *array_count,
                size_t count)
{
    enum w_status status;
    size_t cap;
    void *buf;

    W_ASSERT(alloc != NULL);
    W_ASSERT(slice != NULL);
    W_ASSERT(array_buf != NULL);
    W_ASSERT(array_cap != NULL);
    W_ASSERT(array_count != NULL);
    W_ASSERT(*array_buf != NULL);

    status = W_SUCCESS;

    if (count > W__ARRAY_GET_MAX_CAP(element_size)
        || *array_count > W__ARRAY_GET_MAX_CAP(element_size) - count)
    {
        W_LOG_ERROR("the requested capacity is too large\n");
        return W_ERROR_MAX_SIZE_EXCEEDED;
    }

    count += *array_count;
    if (*array_cap >= count) {
        goto exit;
    }

    buf = *array_buf;
    cap = *array_cap;
    status = w__array_realloc(
        alloc, element_size, alignment, &buf, &cap, count);
    if (status != W_SUCCESS) {
        W_LOG_DEBUG("failed to extend the array\n");
        return status;
    }

    *array_buf = buf;
    *array_cap = cap;

exit:
    *slice = (void*)((uintptr_t)*array_buf + element_size * (*array_count));
    *array_count = count;

    W_ASSERT(status == W_SUCCESS);
    return status;
}
