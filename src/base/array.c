#include <uuki/base/array.h>

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/math.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>

#define WP_ARRAY_GET_MAX_CAP(element_size)                                     \
    (SIZE_MAX / (element_size))

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static size_t
wp_array_max(
    size_t a,
    size_t b
)
{
    return a > b ? a : b;
}

static void
wp_array_grow_capacity(
    size_t *capacity,
    size_t requested,
    size_t element_size
)
{
    requested = wp_array_max(*capacity * 2, requested);

    W_ASSERT(!W_IS_ROUND_UP_POW2_WRAPPING(SIZE_MAX, requested));
    W_ASSERT(
        !W_UINT_IS_MUL_WRAPPING(
            SIZE_MAX, W_ROUND_UP_POW2(requested), element_size
        )
    );

    *capacity = w_size_round_up_pow2(requested);
}

static enum w_status
wp_array_realloc(
    void **buf,
    size_t *capacity,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size,
    size_t requested
)
{
    enum w_status status;
    size_t new_capacity;

    W_ASSERT(buf != NULL);
    W_ASSERT(capacity != NULL);
    W_ASSERT(alloc != NULL);
    W_ASSERT(requested <= WP_ARRAY_GET_MAX_CAP(element_size));
    W_ASSERT(*buf == NULL || requested > *capacity);

    new_capacity = *capacity;
    wp_array_grow_capacity(&new_capacity, requested, element_size);
    W_ASSERT(new_capacity > *capacity);

    status = W_REALLOCATE_ALIGNED(
        alloc,
        buf,
        element_size * (*capacity),
        element_size * new_capacity,
        alignment
    );
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to reallocate the array\n");
        return status;
    }

    *capacity = new_capacity;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

// Private API
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
wp_array_create(
    void **array_buf,
    size_t *array_capacity,
    size_t *array_len,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size,
    size_t capacity
)
{
    enum w_status status;
    void *buf;
    size_t new_capacity;

    W_ASSERT(array_buf != NULL);
    W_ASSERT(array_capacity != NULL);
    W_ASSERT(array_len != NULL);
    W_ASSERT(alloc != NULL);

    if (capacity > WP_ARRAY_GET_MAX_CAP(element_size))
    {
        W_LOG_ERROR("the requested capacity is too large\n");
        return W_ERROR_MAX_SIZE_EXCEEDED;
    }

    buf = NULL;
    new_capacity = 0;
    status = wp_array_realloc(
        &buf, &new_capacity, alloc, alignment, element_size, capacity);
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to create the array\n");
        return status;
    }

    *array_buf = buf;
    *array_capacity = new_capacity;
    *array_len = 0;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

void
wp_array_destroy(
    void *array_buf,
    size_t array_capacity,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size
)
{
    W_ASSERT(array_buf != NULL);
    W_ASSERT(alloc != NULL);

    W_FREE_ALIGNED(
        alloc,
        array_buf,
        element_size * array_capacity,
        alignment
    );
}

enum w_status
wp_array_resize(
    void **array_buf,
    size_t *array_capacity,
    size_t *array_len,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size,
    size_t len
)
{
    enum w_status status;
    void *buf;
    size_t capacity;

    W_ASSERT(array_buf != NULL);
    W_ASSERT(*array_buf != NULL);
    W_ASSERT(array_capacity != NULL);
    W_ASSERT(array_len != NULL);
    W_ASSERT(alloc != NULL);

    if (len <= *array_capacity)
    {
        *array_len = len;
        return W_SUCCESS;
    }

    if (len > WP_ARRAY_GET_MAX_CAP(element_size)
        || *array_len > WP_ARRAY_GET_MAX_CAP(element_size) - len)
    {
        W_LOG_ERROR("the requested length is too large\n");
        return W_ERROR_MAX_SIZE_EXCEEDED;
    }

    buf = *array_buf;
    capacity = *array_capacity;
    status = wp_array_realloc(
        &buf, &capacity, alloc, alignment, element_size, len
    );
    if (status != W_SUCCESS)
    {
        W_LOG_DEBUG("failed to resize the array\n");
        return status;
    }

    *array_buf = buf;
    *array_capacity = capacity;
    *array_len = len;

    W_ASSERT(status == W_SUCCESS);
    return status;
}
