#ifndef UUKI_BASE_PRIVATE_LOG_H
#define UUKI_BASE_PRIVATE_LOG_H

#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <stdarg.h>

enum w_status
wp_log(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    ...
)
W_PRINTF_CHECK(4, 5);

enum w_status
wp_log_va(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    va_list args
)
W_PRINTF_CHECK(4, 0);

enum w_status
wp_log_system_error(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    int error
);

#endif // UUKI_BASE_PRIVATE_LOG_H
