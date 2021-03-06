#ifndef UUKI_BASE_DEBUG_H
#define UUKI_BASE_DEBUG_H

#define W_DEBUGGING(x)                                                         \
    WP_DEBUG_DEBUGGING_##x()

#if (                                                                          \
    defined(W_ENABLE_DEBUGGING)                                                \
    || (                                                                       \
        !defined(W_DISABLE_DEBUGGING)                                          \
        && (defined(DEBUG) || !defined(NDEBUG))                                \
    )                                                                          \
)
    #define WP_DEBUG_DEBUGGING_ENABLED()  1
    #define WP_DEBUG_DEBUGGING_DISABLED() 0
#else
    #define WP_DEBUG_DEBUGGING_ENABLED()  0
    #define WP_DEBUG_DEBUGGING_DISABLED() 1
#endif

#endif // UUKI_BASE_DEBUG_H
