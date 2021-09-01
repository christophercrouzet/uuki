#ifndef UUKI_BASE_PRIVATE_ARRAY_H
#define UUKI_BASE_PRIVATE_ARRAY_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_alloc;

enum w_status
wp_array_create(
    void **array_buf,
    size_t *array_capacity,
    size_t *array_len,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size,
    size_t capacity
);

void
wp_array_destroy(
    void *array_buf,
    size_t array_capacity,
    struct w_alloc *alloc,
    size_t alignment,
    size_t element_size
);

enum w_status
wp_array_extend(
    void **array_buf,
    size_t *array_capacity,
    size_t *array_len,
    struct w_alloc *alloc,
    void **slice,
    size_t alignment,
    size_t element_size,
    size_t len
);

#endif // UUKI_BASE_PRIVATE_ARRAY_H
