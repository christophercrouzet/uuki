#ifndef UUKI_BASE_TRACE_H
#define UUKI_BASE_TRACE_H

#include <uuki/base/macros.h>
#include <uuki/base/platform.h>

#define W_TRACING(x)                                                           \
    WP_TRACE_TRACING_##x()

#if defined(W_ENABLE_TRACING) && !defined(W_DISABLE_TRACING)
    #define WP_TRACE_TRACING_ENABLED()  1
    #define WP_TRACE_TRACING_DISABLED() 0
#else
    #define WP_TRACE_TRACING_ENABLED()  0
    #define WP_TRACE_TRACING_DISABLED() 1
#endif

struct wp_trace_idx {
    const void *obj;
    int valid;
};

#define W_TRACER_INTIALIZE(name, data_type, pool_cap)                          \
    struct name {                                                              \
        struct wp_trace_idx idx_pool[pool_cap];                                \
        data_type data_pool[pool_cap];                                         \
    };                                                                         \
                                                                               \
    static struct name                                                         \
    name##_inst = { 0 };                                                       \
                                                                               \
    W_MTX_INTIALIZE_STATIC(name##_mtx)

#define W_TRACER_ATTACH_DATA(name, obj, data)                                  \
    do                                                                         \
    {                                                                          \
        wp_trace_attach_data(                                                  \
            name##_inst.idx_pool,                                              \
            name##_inst.data_pool,                                             \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            sizeof *data,                                                      \
            obj,                                                               \
            data,                                                              \
            &name##_mtx                                                        \
        );                                                                     \
    }                                                                          \
    while (0)

#define W_TRACER_DETACH_DATA(name, obj)                                        \
    do                                                                         \
    {                                                                          \
        wp_trace_detach_data(                                                  \
            name##_inst.idx_pool,                                              \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            obj,                                                               \
            &name##_mtx                                                        \
        );                                                                     \
    }                                                                          \
    while (0)

#define W_TRACER_GET_DATA_PTR(name, data, obj)                                 \
    do                                                                         \
    {                                                                          \
        wp_trace_get_data_ptr(                                                 \
            (void **)data,                                                     \
            name##_inst.idx_pool,                                              \
            name##_inst.data_pool,                                             \
            W_GET_ARRAY_LEN(name##_inst.idx_pool),                             \
            sizeof **data,                                                     \
            obj,                                                               \
            &name##_mtx                                                        \
        );                                                                     \
    }                                                                          \
    while (0)

#include "private/trace.h"

#endif // UUKI_BASE_TRACE_H
