#ifndef UUKI_BASE_PRIVATE_ARRAY_H
#define UUKI_BASE_PRIVATE_ARRAY_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_alloc;

enum w_status
w__array_create(struct w_alloc *alloc,
                size_t element_size,
                size_t alignment,
                void **array_buf,
                size_t *array_cap,
                size_t *array_count,
                size_t cap);

void
w__array_destroy(struct w_alloc *alloc,
                 size_t element_size,
                 size_t alignment,
                 void *array_buf,
                 size_t array_cap);

enum w_status
w__array_extend(struct w_alloc *alloc,
                size_t element_size,
                size_t alignment,
                void **slice,
                void **array_buf,
                size_t *array_cap,
                size_t *array_count,
                size_t count);

#endif // UUKI_BASE_PRIVATE_ARRAY_H
