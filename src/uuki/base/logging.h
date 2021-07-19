#ifndef UUKI_BASE_LOGGING_H
#define UUKI_BASE_LOGGING_H

#include <stdarg.h>

#define W_LOG(lvl, ...)                                                        \
    do                                                                         \
    {                                                                          \
        if ((lvl) <= w_get_log_lvl())                                          \
        {                                                                      \
            w_log((lvl), __FILE__, __LINE__, __VA_ARGS__);                     \
        }                                                                      \
    }                                                                          \
    while (0)

#define W_LOG_FATAL(...)                                                       \
    W_LOG(W_LOG_LVL_FATAL, __VA_ARGS__)

#define W_LOG_ERROR(...)                                                       \
    W_LOG(W_LOG_LVL_ERROR, __VA_ARGS__)

#define W_LOG_WARNING(...)                                                     \
    W_LOG(W_LOG_LVL_WARNING, __VA_ARGS__)

#define W_LOG_INFO(...)                                                        \
    W_LOG(W_LOG_LVL_INFO, __VA_ARGS__)

#define W_LOG_DEBUG(...)                                                       \
    W_LOG(W_LOG_LVL_DEBUG, __VA_ARGS__)

enum w_log_lvl {
    W_LOG_LVL_NONE            = 0,
    W_LOG_LVL_FATAL           = 1,
    W_LOG_LVL_ERROR           = 2,
    W_LOG_LVL_WARNING         = 3,
    W_LOG_LVL_INFO            = 4,
    W_LOG_LVL_DEBUG           = 5,
    W_LOG_LVL_ALL             = W_LOG_LVL_DEBUG,

    WP_LOGGING_LOG_LVL_FIRST  = W_LOG_LVL_NONE,
    WP_LOGGING_LOG_LVL_LAST   = W_LOG_LVL_ALL,
};

void
w_vlog(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *fmt,
    va_list args
);

void
w_log(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *fmt,
    ...
);

enum w_log_lvl
w_get_log_lvl(
    void
);

void
w_set_log_lvl(
    enum w_log_lvl lvl
);

int
w_get_log_styling(
    void
);

void
w_set_log_styling(
    int styling
);

#endif // UUKI_BASE_LOGGING_H
