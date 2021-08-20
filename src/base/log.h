#ifndef UUKI_BASE_LOG_H
#define UUKI_BASE_LOG_H

#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stdarg.h>
#include <stdint.h>

#define W_LOG(lvl, ...)                                                        \
    do                                                                         \
    {                                                                          \
        w_log((lvl), __FILE__, __LINE__, __VA_ARGS__);                         \
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
    W_LOG_LVL_NONE    = 0,
    W_LOG_LVL_FATAL   = 1,
    W_LOG_LVL_ERROR   = 2,
    W_LOG_LVL_WARNING = 3,
    W_LOG_LVL_INFO    = 4,
    W_LOG_LVL_DEBUG   = 5,
    W_LOG_LVL_ALL     = W_LOG_LVL_DEBUG,

    WP_LOG_LVL_FIRST  = W_LOG_LVL_NONE,
    WP_LOG_LVL_LAST   = W_LOG_LVL_ALL,
};

enum w_log_fmt {
    W_LOG_FMT_PLAIN          = 0,
    W_LOG_FMT_PLAIN_STYLIZED = 1,

    WP_LOG_FMT_FIRST         = W_LOG_FMT_PLAIN,
    WP_LOG_FMT_LAST          = W_LOG_FMT_PLAIN_STYLIZED,
};

struct w_logger {
    uint32_t handle;
};

void
w_get_default_logger(
    struct w_logger *logger
);

enum w_status
w_logger_register(
    struct w_logger *logger,
    w_stream stream,
    enum w_log_lvl lvl,
    enum w_log_fmt fmt
);

enum w_status
w_logger_deregister(
    struct w_logger logger
);

enum w_status
w_logger_deregister_all(
    void
);

enum w_status
w_log(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    ...
)
W_PRINTF_CHECK(4, 5);

enum w_status
w_log_va(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    va_list args
)
W_PRINTF_CHECK(4, 0);

#endif // UUKI_BASE_LOG_H
