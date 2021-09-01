#include <uuki/base/trace.h>

#include <uuki/base/assert.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

// Private API
// ---------------------------------------------------------------- //   O-(''Q)

void
wp_trace_pool_insert_data(
    struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *key,
    const void *data
)
{
    size_t i;

    W_ASSERT(idx_pool != NULL);
    W_ASSERT(data_pool != NULL);
    W_ASSERT(key != NULL);
    W_ASSERT(data != NULL);

    for (i = 0; i < pool_cap; ++i)
    {
        struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (!idx->valid)
        {
            void *data_it;

            data_it = (void *)((uintptr_t)data_pool + data_size * i);

            idx->key = key;
            idx->valid = 1;
            w_copy_mem(data_it, data, data_size);
            return;
        }
    }

    W_ASSERT(0);
}

void
wp_trace_pool_remove_data(
    struct wp_trace_idx *idx_pool,
    size_t pool_cap,
    const void *key
)
{
    size_t i;

    W_ASSERT(idx_pool != NULL);
    W_ASSERT(key != NULL);

    for (i = 0; i < pool_cap; ++i)
    {
        struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (idx->valid && idx->key == key)
        {
            idx->valid = 0;
            return;
        }
    }

    W_ASSERT(0);
}

void
wp_trace_pool_find_data(
    void **data,
    const struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *key
)
{
    size_t i;

    W_ASSERT(data != NULL);
    W_ASSERT(idx_pool != NULL);
    W_ASSERT(data_pool != NULL);
    W_ASSERT(key != NULL);

    for (i = 0; i < pool_cap; ++i)
    {
        const struct wp_trace_idx *idx;

        idx = &idx_pool[i];
        if (idx->valid && idx->key == key)
        {
            *data = (void *)((uintptr_t)data_pool + data_size * i);
            return;
        }
    }

    *data = NULL;
}
