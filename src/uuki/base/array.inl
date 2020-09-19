#ifndef UUKI_BASE_ARRAY_INL
#define UUKI_BASE_ARRAY_INL

#include <uuki/base/assert.h>
#include <uuki/base/logging.h>
#include <uuki/base/macros.inl>
#include <uuki/base/math.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

#define W_ARRAY_MAKE(name, type)                                               \
    struct w__##name {                                                         \
        type *buf;                                                             \
        size_t cap;                                                            \
        size_t count;                                                          \
    };                                                                         \
                                                                               \
    static const size_t                                                        \
    w__##name##_max_cap = (SIZE_MAX / sizeof(type));                           \
                                                                               \
    static const size_t                                                        \
    w__##name##_alignment = W_CEIL_POW2(sizeof(type), type);                   \
                                                                               \
    static enum w_status                                                       \
    w__##name##_realloc(struct w_alloc *alloc,                                 \
                        void **buf,                                            \
                        size_t *cap,                                           \
                        size_t req)                                            \
    {                                                                          \
        enum w_status status;                                                  \
        size_t new_cap;                                                        \
                                                                               \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(buf != NULL);                                                 \
        W_ASSERT(cap != NULL);                                                 \
        W_ASSERT(req <= w__##name##_max_cap);                                  \
        W_ASSERT(*buf == NULL || req > *cap);                                  \
                                                                               \
        new_cap = *cap;                                                        \
        w_grow_cap(&new_cap, req, sizeof(type));                               \
        W_ASSERT(new_cap > *cap);                                              \
                                                                               \
        status = W_REALLOCATE_ALIGNED(                                         \
            alloc,                                                             \
            buf,                                                               \
            sizeof(type) * (*cap),                                             \
            sizeof(type) * new_cap,                                            \
            w__##name##_alignment);                                            \
        if (status != W_SUCCESS) {                                             \
            W_LOG_DEBUG("failed to reallocate the array\n");                   \
            return status;                                                     \
        }                                                                      \
                                                                               \
        *cap = new_cap;                                                        \
        return status;                                                         \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    w__##name##_create(struct w_alloc *alloc,                                  \
                       struct w__##name *array,                                \
                       size_t count)                                           \
    {                                                                          \
        enum w_status status;                                                  \
        void *buf;                                                             \
        size_t cap;                                                            \
                                                                               \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
                                                                               \
        if (count > w__##name##_max_cap) {                                     \
            W_LOG_ERROR("the requested capacity is too large\n");              \
            return W_ERROR_MAX_SIZE_EXCEEDED;                                  \
        }                                                                      \
                                                                               \
        buf = NULL;                                                            \
        cap = 0;                                                               \
        status = w__##name##_realloc(alloc, &buf, &cap, count);                \
        if (status != W_SUCCESS) {                                             \
            W_LOG_DEBUG("failed to create the array\n");                       \
            return status;                                                     \
        }                                                                      \
                                                                               \
        array->buf = buf;                                                      \
        array->cap = cap;                                                      \
        array->count = 0;                                                      \
        return status;                                                         \
    }                                                                          \
                                                                               \
    static void                                                                \
    w__##name##_destroy(struct w_alloc *alloc,                                 \
                        struct w__##name *array)                               \
    {                                                                          \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
                                                                               \
        W_FREE_ALIGNED(                                                        \
            alloc,                                                             \
            array->buf,                                                        \
            sizeof(type) * array->cap,                                         \
            w__##name##_alignment);                                            \
    }                                                                          \
                                                                               \
    static enum w_status                                                       \
    w__##name##_extend(struct w_alloc *alloc,                                  \
                       type **slice,                                           \
                       struct w__##name *array,                                \
                       size_t count)                                           \
    {                                                                          \
        enum w_status status;                                                  \
        size_t cap;                                                            \
        void *buf;                                                             \
                                                                               \
        W_ASSERT(alloc != NULL);                                               \
        W_ASSERT(slice != NULL);                                               \
        W_ASSERT(array != NULL);                                               \
        W_ASSERT(array->buf != NULL);                                          \
                                                                               \
        status = W_SUCCESS;                                                    \
                                                                               \
        if (count > w__##name##_max_cap                                        \
            || array->count > w__##name##_max_cap - count)                     \
        {                                                                      \
            W_LOG_ERROR("the requested capacity is too large\n");              \
            return W_ERROR_MAX_SIZE_EXCEEDED;                                  \
        }                                                                      \
                                                                               \
        count += array->count;                                                 \
        if (array->cap >= count) {                                             \
            goto exit;                                                         \
        }                                                                      \
                                                                               \
        buf = array->buf;                                                      \
        cap = array->cap;                                                      \
        status = w__##name##_realloc(alloc, &buf, &cap, count);                \
        if (status != W_SUCCESS) {                                             \
            W_LOG_DEBUG("failed to extend the array\n");                       \
            return status;                                                     \
        }                                                                      \
                                                                               \
        array->buf = buf;                                                      \
        array->cap = cap;                                                      \
                                                                               \
    exit:                                                                      \
        *slice = &array->buf[array->count];                                    \
        array->count = count;                                                  \
        return status;                                                         \
    }                                                                          \
                                                                               \
    W__REQUIRE_SEMICOLON

#endif // UUKI_BASE_ARRAY_INL
