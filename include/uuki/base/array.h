#ifndef UUKI_BASE_ARRAY_H
#define UUKI_BASE_ARRAY_H

#include <uuki/base/assert.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>

#define W_ARRAY_DECLARE(name, type)                                            \
    struct name {                                                              \
        type *buf;                                                             \
        size_t capacity;                                                       \
        size_t len;                                                            \
    };                                                                         \
                                                                               \
    static const size_t                                                        \
    name##_alignment = W_ROUND_UP_POW2(sizeof(type));                          \
                                                                               \
    static enum w_status                                                       \
    name##_create(                                                             \
        struct name *array,                                                    \
        struct w_alloc *alloc,                                                 \
        size_t capacity                                                        \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(alloc != NULL);                                               \
                                                                               \
        return wp_array_create(                                                \
            (void **)&array->buf,                                              \
            &array->capacity,                                                  \
            &array->len,                                                       \
            alloc,                                                             \
            name##_alignment,                                                  \
            sizeof(type),                                                      \
            capacity                                                           \
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
            array->capacity,                                                   \
            alloc,                                                             \
            name##_alignment,                                                  \
            sizeof(type)                                                       \
        );                                                                     \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    name##_extend(                                                             \
        struct name *array,                                                    \
        struct w_alloc *alloc,                                                 \
        type **slice,                                                          \
        size_t len                                                             \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(slice != NULL);                                               \
                                                                               \
        return wp_array_extend(                                                \
            (void **)&array->buf,                                              \
            &array->capacity,                                                  \
            &array->len,                                                       \
            alloc,                                                             \
            (void **)slice,                                                    \
            name##_alignment,                                                  \
            sizeof(type),                                                      \
            len                                                                \
        );                                                                     \
    }                                                                          \
                                                                               \
    W_REQUIRE_SEMICOLON

#include "private/array.h"

#endif // UUKI_BASE_ARRAY_H
