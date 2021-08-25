#include "trace.h"

#include <uuki/base/assert.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

// Private API
// ---------------------------------------------------------------- //   O-(''Q)

void
wp_trace_attach_data(
    struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *obj,
    const void *data,
    w_mtx *mtx
)
{
    size_t i;

    W_ASSERT(idx_pool != NULL);
    W_ASSERT(data_pool != NULL);
    W_ASSERT(obj != NULL);
    W_ASSERT(data != NULL);
    W_ASSERT(mtx != NULL);

    if (w_mtx_lock(mtx) != 0)
    {
        w_abort(W_ERROR_LOCK_FAILED);
    }

    for (i = 0; i < pool_cap; ++i)
    {
        struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (!idx->valid)
        {
            void *data_it;

            data_it = (void *)((uintptr_t)data_pool + data_size * i);

            idx->obj = obj;
            idx->valid = 1;
            w_copy_mem(data_it, data, data_size);
            goto exit;
        }
    }

    w_abort(W_ERROR_BUF_FULL);

exit:
    if (w_mtx_unlock(mtx) != 0)
    {
        w_abort(W_ERROR_UNLOCK_FAILED);
    }
}

void
wp_trace_detach_data(
    struct wp_trace_idx *idx_pool,
    size_t pool_cap,
    const void *obj,
    w_mtx *mtx
)
{
    size_t i;

    W_ASSERT(idx_pool != NULL);
    W_ASSERT(obj != NULL);
    W_ASSERT(mtx != NULL);

    if (w_mtx_lock(mtx) != 0)
    {
        w_abort(W_ERROR_LOCK_FAILED);
    }

    for (i = 0; i < pool_cap; ++i)
    {
        struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (idx->valid && idx->obj == obj)
        {
            idx->valid = 0;
            goto exit;
        }
    }

exit:
    if (w_mtx_unlock(mtx) != 0)
    {
        w_abort(W_ERROR_UNLOCK_FAILED);
    }
}

void
wp_trace_get_data_ptr(
    void **data,
    const struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *obj,
    w_mtx *mtx
)
{
    size_t i;

    W_ASSERT(data != NULL);
    W_ASSERT(idx_pool != NULL);
    W_ASSERT(data_pool != NULL);
    W_ASSERT(obj != NULL);
    W_ASSERT(mtx != NULL);

    if (w_mtx_lock(mtx) != 0)
    {
        w_abort(W_ERROR_LOCK_FAILED);
    }

    for (i = 0; i < pool_cap; ++i)
    {
        const struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (idx->valid && idx->obj == obj)
        {
            *data = (void *)((uintptr_t)data_pool + data_size * i);
            goto exit;
        }
    }

    w_abort(W_ERROR_BUF_FULL);

exit:
    if (w_mtx_unlock(mtx) != 0)
    {
        w_abort(W_ERROR_UNLOCK_FAILED);
    }
}
