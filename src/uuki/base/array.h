#ifndef UUKI_BASE_ARRAY_H
#define UUKI_BASE_ARRAY_H

#include <uuki/base/assert.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>
#include <uuki/base/private/array.h>

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
    name##_create(struct w_alloc *alloc,                                       \
                  struct name *array,                                          \
                  size_t cap)                                                  \
    {                                                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
                                                                               \
        return w__array_create(                                                \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment,                                                  \
            (void **)&array->buf,                                              \
            &array->cap,                                                       \
            &array->count,                                                     \
            cap);                                                              \
    }                                                                          \
                                                                               \
    static void                                                                \
    name##_destroy(struct w_alloc *alloc,                                      \
                   struct name *array)                                         \
    {                                                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
                                                                               \
        w__array_destroy(                                                      \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment,                                                  \
            (void *)array->buf,                                                \
            array->cap);                                                       \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    name##_extend(struct w_alloc *alloc,                                       \
                  type **slice,                                                \
                  struct name *array,                                          \
                  size_t count)                                                \
    {                                                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(slice != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
                                                                               \
        return w__array_extend(                                                \
            alloc,                                                             \
            sizeof(type),                                                      \
            name##_alignment,                                                  \
            (void **)slice,                                                    \
            (void **)&array->buf,                                              \
            &array->cap,                                                       \
            &array->count,                                                     \
            count);                                                            \
    }                                                                          \
                                                                               \
    W_REQUIRE_SEMICOLON

#endif // UUKI_BASE_ARRAY_H
