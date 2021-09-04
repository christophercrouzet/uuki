#ifndef UUKI_BASE_ARRAY_H
#define UUKI_BASE_ARRAY_H

#include <uuki/base/assert.h>
#include <uuki/base/macros.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

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
    name##_push(                                                               \
        struct name *array,                                                    \
        struct w_alloc *alloc,                                                 \
        type value                                                             \
    )                                                                          \
    {                                                                          \
        enum w_status status;                                                  \
                                                                               \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
        W_ASSERT(alloc != NULL);                                               \
                                                                               \
        W_ASSERT(!W_UINT_IS_ADD_WRAPPING(SIZE_MAX, array->len, 1));            \
                                                                               \
        status = wp_array_resize(                                              \
            (void **)&array->buf,                                              \
            &array->capacity,                                                  \
            &array->len,                                                       \
            alloc,                                                             \
            name##_alignment,                                                  \
            sizeof(type),                                                      \
            array->len + 1                                                     \
        );                                                                     \
        if (status != W_SUCCESS)                                               \
        {                                                                      \
            return status;                                                     \
        }                                                                      \
                                                                               \
        array->buf[array->len - 1] = value;                                    \
                                                                               \
        W_ASSERT(status == W_SUCCESS);                                         \
        return status;                                                         \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    name##_pop(                                                                \
        struct name *array,                                                    \
        type *value                                                            \
    )                                                                          \
    {                                                                          \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
        W_ASSERT(array->len > 0);                                              \
                                                                               \
        --array->len;                                                          \
        *value = array->buf[array->len];                                       \
        return W_SUCCESS;                                                      \
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
        enum w_status status;                                                  \
                                                                               \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(!W_UINT_IS_ADD_WRAPPING(SIZE_MAX, array->len, len));          \
                                                                               \
        status = wp_array_resize(                                              \
            (void **)&array->buf,                                              \
            &array->capacity,                                                  \
            &array->len,                                                       \
            alloc,                                                             \
            name##_alignment,                                                  \
            sizeof(type),                                                      \
            array->len + len                                                   \
        );                                                                     \
        if (status != W_SUCCESS)                                               \
        {                                                                      \
            return status;                                                     \
        }                                                                      \
                                                                               \
        if (slice != NULL)                                                     \
        {                                                                      \
            *slice = &array->buf[array->len - len];                            \
        }                                                                      \
                                                                               \
        W_ASSERT(status == W_SUCCESS);                                         \
        return status;                                                         \
    }                                                                          \
                                                                               \
    W_REQUIRE_SEMICOLON

#include "private/array.h"

#endif // UUKI_BASE_ARRAY_H
