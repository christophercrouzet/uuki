#ifndef UUKI_BASE_ARRAY_H
#define UUKI_BASE_ARRAY_H

#include <uuki/base/assert.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>

#define W_ARRAY_MAKE(name, type)                                               \
    struct name {                                                              \
        type *buf;                                                             \
        size_t cap;                                                            \
        size_t count;                                                          \
    };                                                                         \
                                                                               \
    static const size_t                                                        \
    name##_alignment = W_ROUND_UP_POW2(sizeof(type));                          \
                                                                               \
    static enum w_status                                                       \
    name##_create(                                                             \
        struct name *array,                                                    \
        struct w_alloc *alloc,                                                 \
        size_t cap                                                             \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(alloc != NULL);                                               \
                                                                               \
        return wp_array_create(                                                \
            (void **)&array->buf,                                              \
            &array->cap,                                                       \
            &array->count,                                                     \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment,                                                  \
            cap                                                                \
        );                                                                     \
    }                                                                          \
                                                                               \
    static void                                                                \
    name##_destroy(                                                            \
        struct name *array,                                                    \
        struct w_alloc *alloc                                                  \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(alloc != NULL);                                               \
                                                                               \
        wp_array_destroy(                                                      \
            (void *)array->buf,                                                \
            array->cap,                                                        \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment                                                   \
        );                                                                     \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    name##_extend(                                                             \
        struct name *array,                                                    \
        struct w_alloc *alloc,                                                 \
        type **slice,                                                          \
        size_t count                                                           \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(slice != NULL);                                               \
                                                                               \
        return wp_array_extend(                                                \
            (void **)&array->buf,                                              \
            &array->cap,                                                       \
            &array->count,                                                     \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment,                                                  \
            (void **)slice,                                                    \
            count                                                              \
        );                                                                     \
    }                                                                          \
                                                                               \
    W_REQUIRE_SEMICOLON

#include "private/array.h"

#endif // UUKI_BASE_ARRAY_H
