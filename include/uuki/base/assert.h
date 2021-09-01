#ifndef UUKI_BASE_ASSERT_H
#define UUKI_BASE_ASSERT_H

#include <uuki/base/debug.h>

#if W_DEBUGGING(ENABLED)
    #define W_ASSERT(expr)                                                     \
        (                                                                      \
            !!(expr)                                                           \
                ? (void)0                                                      \
                : (void)w_handle_failed_assertion(                             \
                    #expr, __func__, __FILE__, __LINE__                        \
                )                                                              \
        )
#else
    #define W_ASSERT(expr)                                                     \
        ((void)0)
#endif

void
w_handle_failed_assertion(
    const char *expr,
    const char *fn,
    const char *file,
    int line
);

#endif // UUKI_BASE_ASSERT_H
