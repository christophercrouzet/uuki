#ifndef UUKI_BASE_LOG_H
#define UUKI_BASE_LOG_H

#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stdint.h>

#define W_LOG(lvl, ...)                                                        \
    do                                                                         \
    {                                                                          \
        wp_log((lvl), __FILE__, __LINE__, __VA_ARGS__);                        \
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

#define W_LOG_SYSTEM_ERROR(lvl, error)                                         \
    do                                                                         \
    {                                                                          \
        wp_log_system_error((lvl), __FILE__, __LINE__, error);                 \
    }                                                                          \
    while (0)

enum w_log_lvl {
    W_LOG_LVL_NONE,
    W_LOG_LVL_FATAL,
    W_LOG_LVL_ERROR,
    W_LOG_LVL_WARNING,
    W_LOG_LVL_INFO,
    W_LOG_LVL_DEBUG,
    W_LOG_LVL_ALL     = W_LOG_LVL_DEBUG,

    WP_LOG_LVL_END,
};

enum w_log_fmt {
    W_LOG_FMT_PLAIN,
    W_LOG_FMT_PLAIN_STYLIZED,

    WP_LOG_FMT_END,
};

struct w_logger {
    uint32_t handle;
};

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

#include "private/log.h"

#endif // UUKI_BASE_LOG_H
