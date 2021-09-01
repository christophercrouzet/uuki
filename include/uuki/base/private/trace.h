#ifndef UUKI_BASE_PRIVATE_TRACE_H
#define UUKI_BASE_PRIVATE_TRACE_H

#include <uuki/base/platform.h>

#include <stddef.h>

struct wp_trace_idx {
    const void *key;
    int valid;
};

void
wp_trace_pool_insert_data(
    struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_capacity,
    size_t data_size,
    const void *key,
    const void *data
);

void
wp_trace_pool_remove_data(
    struct wp_trace_idx *idx_pool,
    size_t pool_capacity,
    const void *key
);

void
wp_trace_pool_find_data(
    void **data,
    const struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_capacity,
    size_t data_size,
    const void *key
);

#endif // UUKI_BASE_PRIVATE_TRACE_H
