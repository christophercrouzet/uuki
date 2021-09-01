#ifndef UUKI_BASE_PRIVATE_ARRAY_H
#define UUKI_BASE_PRIVATE_ARRAY_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_alloc;

enum w_status
wp_array_create(
    void **array_buf,
    size_t *array_cap,
    size_t *array_count,
    struct w_alloc *alloc,
    size_t element_size,
    size_t alignment,
    size_t cap
);

void
wp_array_destroy(
    void *array_buf,
    size_t array_cap,
    struct w_alloc *alloc,
    size_t element_size,
    size_t alignment
);

enum w_status
wp_array_extend(
    void **array_buf,
    size_t *array_cap,
    size_t *array_count,
    struct w_alloc *alloc,
    size_t element_size,
    size_t alignment,
    void **slice,
    size_t count
);

#endif // UUKI_BASE_PRIVATE_ARRAY_H
