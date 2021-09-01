#ifndef UUKI_BASE_TRACE_H
#define UUKI_BASE_TRACE_H

#include <uuki/base/debug.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#define W_TRACING(x)                                                           \
    WP_TRACE_TRACING_##x()

#if (                                                                          \
    (defined(W_ENABLE_TRACING) || W_DEBUGGING(ENABLED))                        \
    && !defined(W_DISABLE_TRACING)                                             \
)
    #define WP_TRACE_TRACING_ENABLED()  1
    #define WP_TRACE_TRACING_DISABLED() 0
#else
    #define WP_TRACE_TRACING_ENABLED()  0
    #define WP_TRACE_TRACING_DISABLED() 1
#endif

// Pool Handling
// ---------------------------------------------------------------- //   O-(''Q)

#define W_TRACE_POOL_DECLARE(name, data_type, pool_cap)                        \
    struct name {                                                              \
        struct wp_trace_idx idx_pool[pool_cap];                                \
        data_type data_pool[pool_cap];                                         \
    }

#define W_TRACE_POOL_INSERT_DATA(name, key, data)                              \
    do                                                                         \
    {                                                                          \
        wp_trace_pool_insert_data(                                             \
            name.idx_pool,                                                     \
            name.data_pool,                                                    \
            W_GET_ARRAY_LEN(name.idx_pool),                                    \
            sizeof *data,                                                      \
            key,                                                               \
            data                                                               \
        );                                                                     \
    }                                                                          \
    while (0)

#define W_TRACE_POOL_REMOVE_DATA(name, key)                                    \
    do                                                                         \
    {                                                                          \
        wp_trace_pool_remove_data(                                             \
            name.idx_pool,                                                     \
            W_GET_ARRAY_LEN(name.idx_pool),                                    \
            key                                                                \
        );                                                                     \
    }                                                                          \
    while (0)

#define W_TRACE_POOL_FIND_DATA(name, data, key)                                \
    do                                                                         \
    {                                                                          \
        wp_trace_pool_find_data(                                               \
            (void **)data,                                                     \
            name.idx_pool,                                                     \
            name.data_pool,                                                    \
            W_GET_ARRAY_LEN(name.idx_pool),                                    \
            sizeof **data,                                                     \
            key                                                                \
        );                                                                     \
        W_ASSERT(*data != NULL);                                               \
    }                                                                          \
    while (0)

// Tracer
// ---------------------------------------------------------------- //   O-(''Q)

#define W_TRACER_INTIALIZE(name, data_type, pool_cap)                          \
    W_TRACE_POOL_DECLARE(name, data_type, pool_cap);                           \
                                                                               \
    static struct name                                                         \
    name##_inst = { 0 };                                                       \
                                                                               \
    W_MTX_INTIALIZE_STATIC(name##_mtx)

#define W_TRACER_ATTACH_DATA(name, obj, data)                                  \
    do                                                                         \
    {                                                                          \
        if (w_mtx_lock(&name##_mtx) != 0)                                      \
        {                                                                      \
            w_abort(W_ERROR_LOCK_FAILED);                                      \
        }                                                                      \
                                                                               \
        wp_trace_pool_insert_data(                                             \
            name##_inst.idx_pool,                                              \
            name##_inst.data_pool,                                             \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            sizeof *data,                                                      \
            obj,                                                               \
            data                                                               \
        );                                                                     \
                                                                               \
        if (w_mtx_unlock(&name##_mtx) != 0)                                    \
        {                                                                      \
            w_abort(W_ERROR_UNLOCK_FAILED);                                    \
        }                                                                      \
    }                                                                          \
    while (0)

#define W_TRACER_DETACH_DATA(name, obj)                                        \
    do                                                                         \
    {                                                                          \
        if (w_mtx_lock(&name##_mtx) != 0)                                      \
        {                                                                      \
            w_abort(W_ERROR_LOCK_FAILED);                                      \
        }                                                                      \
                                                                               \
        wp_trace_pool_remove_data(                                             \
            name##_inst.idx_pool,                                              \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            obj                                                                \
        );                                                                     \
                                                                               \
        if (w_mtx_unlock(&name##_mtx) != 0)                                    \
        {                                                                      \
            w_abort(W_ERROR_UNLOCK_FAILED);                                    \
        }                                                                      \
    }                                                                          \
    while (0)

#define W_TRACER_FIND_DATA(name, data, obj)                                    \
    do                                                                         \
    {                                                                          \
        if (w_mtx_lock(&name##_mtx) != 0)                                      \
        {                                                                      \
            w_abort(W_ERROR_LOCK_FAILED);                                      \
        }                                                                      \
                                                                               \
        wp_trace_pool_find_data(                                               \
            (void **)data,                                                     \
            name##_inst.idx_pool,                                              \
            name##_inst.data_pool,                                             \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            sizeof **data,                                                     \
            obj                                                                \
        );                                                                     \
                                                                               \
        if (w_mtx_unlock(&name##_mtx) != 0)                                    \
        {                                                                      \
            w_abort(W_ERROR_UNLOCK_FAILED);                                    \
        }                                                                      \
    }                                                                          \
    while (0)

#include "private/trace.h"

#endif // UUKI_BASE_TRACE_H
