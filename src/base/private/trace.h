#ifndef UUKI_BASE_PRIVATE_TRACE_H
#define UUKI_BASE_PRIVATE_TRACE_H

#include <uuki/base/platform.h>

#include <stddef.h>

void
wp_trace_attach_data(
    struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *obj,
    const void *data,
    w_mtx *mtx
);

void
wp_trace_detach_data(
    struct wp_trace_idx *idx_pool,
    size_t pool_cap,
    const void *obj,
    w_mtx *mtx
);

void
wp_trace_get_data_ptr(
    void **data,
    const struct wp_trace_idx *idx_pool,
    void *data_pool,
    size_t pool_cap,
    size_t data_size,
    const void *obj,
    w_mtx *mtx
);

#endif // UUKI_BASE_PRIVATE_TRACE_H
