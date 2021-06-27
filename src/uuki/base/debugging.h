#ifndef UUKI_BASE_DEBUGGING_H
#define UUKI_BASE_DEBUGGING_H

#define W_DEBUGGING(x)                                                         \
    W__DEBUGGING_DEBUGGING_##x()

#if (                                                                          \
        defined(W_ENABLE_DEBUGGING)                                            \
        || (                                                                   \
            !defined(W_DISABLE_DEBUGGING)                                      \
            && (defined(DEBUG) || !defined(NDEBUG))                            \
        )                                                                      \
    )
    #define W__DEBUGGING_DEBUGGING_ENABLED()  1
    #define W__DEBUGGING_DEBUGGING_DISABLED() 0
#else
    #define W__DEBUGGING_DEBUGGING_ENABLED()  0
    #define W__DEBUGGING_DEBUGGING_DISABLED() 1
#endif

#endif // UUKI_BASE_DEBUGGING_H
