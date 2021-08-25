#include "log.h"

#include <uuki/base/assert.h>
#include <uuki/base/debug.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define WP_LOG_GET_HANDLE_IDX(x)                                               \
    ((x) & 0xFFFF)

#define WP_LOG_GET_HANDLE_ID(x)                                                \
    (((x) >> 16) & 0xFFFF)

#define WP_LOG_SET_HANDLE(idx, id)                                             \
    (uint32_t)(idx | (id << 16))

#define WP_LOG_LVL_STYLE_NONE                                                  \
    ""

#define WP_LOG_LVL_STYLE_FATAL                                                 \
    "\x1b[1;35m"

#define WP_LOG_LVL_STYLE_ERROR                                                 \
    "\x1b[1;31m"

#define WP_LOG_LVL_STYLE_WARNING                                               \
    "\x1b[1;33m"

#define WP_LOG_LVL_STYLE_INFO                                                  \
    "\x1b[1;36m"

#define WP_LOG_LVL_STYLE_DEBUG                                                 \
    "\x1b[1;32m"

#define WP_LOG_LVL_COUNT                                                       \
    (WP_LOG_LVL_LAST - WP_LOG_LVL_FIRST + 1)

#if W_DEBUGGING(ENABLED)
    #define WP_LOG_DEFAULT_LEVEL                                               \
        W_LOG_LVL_DEBUG
#else
    #define WP_LOG_DEFAULT_LEVEL                                               \
        W_LOG_LVL_WARNING
#endif

struct wp_log_logger {
    w_stream stream;
    enum w_log_lvl lvl;
    enum w_log_fmt fmt;
    uint16_t id;
    int valid;
};

static const char *
wp_log_lvl_name[WP_LOG_LVL_COUNT] = {
    [W_LOG_LVL_NONE]    = "none",
    [W_LOG_LVL_FATAL]   = "fatal",
    [W_LOG_LVL_ERROR]   = "error",
    [W_LOG_LVL_WARNING] = "warning",
    [W_LOG_LVL_INFO]    = "info",
    [W_LOG_LVL_DEBUG]   = "debug",
};

static const char *
wp_log_lvl_style_begin[2][WP_LOG_LVL_COUNT] = {
    [0] = {
        [W_LOG_LVL_NONE]    = "",
        [W_LOG_LVL_FATAL]   = "",
        [W_LOG_LVL_ERROR]   = "",
        [W_LOG_LVL_WARNING] = "",
        [W_LOG_LVL_INFO]    = "",
        [W_LOG_LVL_DEBUG]   = "",
    },
    [1] = {
        [W_LOG_LVL_NONE]    = WP_LOG_LVL_STYLE_NONE,
        [W_LOG_LVL_FATAL]   = WP_LOG_LVL_STYLE_FATAL,
        [W_LOG_LVL_ERROR]   = WP_LOG_LVL_STYLE_ERROR,
        [W_LOG_LVL_WARNING] = WP_LOG_LVL_STYLE_WARNING,
        [W_LOG_LVL_INFO]    = WP_LOG_LVL_STYLE_INFO,
        [W_LOG_LVL_DEBUG]   = WP_LOG_LVL_STYLE_DEBUG,
    },
};

static const char *
wp_log_lvl_style_end[2] = {
    [0] = "",
    [1] = "\x1b[0m",
};

static struct wp_log_logger
wp_log_logger_pool[8] = { 0 };

static int
wp_log_is_std_err_console;

W_MTX_INTIALIZE_STATIC(wp_log_mtx);

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_log_logger_find(
    struct wp_log_logger **logger,
    uint32_t handle
)
{
    W_ASSERT(logger != NULL);
    W_ASSERT(
        WP_LOG_GET_HANDLE_IDX(handle) < W_GET_ARRAY_LEN(wp_log_logger_pool)
    );

    *logger = &wp_log_logger_pool[WP_LOG_GET_HANDLE_IDX(handle)];
    if (!(*logger)->valid || (*logger)->id != WP_LOG_GET_HANDLE_ID(handle))
    {
        return W_ERROR_NOT_FOUND;
    }

    return W_SUCCESS;
}

W_PRINTF_CHECK(6, 0)
static enum w_status
wp_log_fmt_plain(
    w_stream stream,
    enum w_log_lvl lvl,
    const char *file,
    int line,
    int stylized,
    const char *msg,
    va_list args
)
{
    enum w_status status;
    va_list copied_args;
    char buf[1024];
    size_t len;

    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(msg != NULL);

    // Format the header.
    if (
        w_format_msg(
            buf,
            W_GET_ARRAY_LEN(buf),
            "%s:%d: %s%s%s: ",
            file,
            line,
            wp_log_lvl_style_begin[stylized][lvl],
            wp_log_lvl_name[lvl],
            wp_log_lvl_style_end[stylized]
        )
        != 0)
    {
        return W_ERROR_STR_FORMATTING_FAILED;
    }

    w_get_str_len(&len, buf);

    // Format the body.
    va_copy(copied_args, args);
    if (
        w_format_msg_va(
            &buf[len], W_GET_ARRAY_LEN(buf) - len, msg, copied_args
        )
        != 0)
    {
        status = W_ERROR_STR_FORMATTING_FAILED;
        goto exit;
    }

    // Print the result.
    if (w_print(stream, buf) != 0)
    {
        status = W_ERROR_IO_FAILED;
        goto exit;
    }

    status = W_SUCCESS;

exit:
    va_end(copied_args);
    return status;
}

// Private API
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
wp_log(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    ...
)
{
    enum w_status status;
    va_list args;

    W_ASSERT(file != NULL);
    W_ASSERT(msg != NULL);

    va_start(args, msg);
    status = wp_log_va(lvl, file, line, msg, args);
    va_end(args);
    return status;
}

enum w_status
wp_log_va(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    va_list args
)
{
    static int32_t initialized = 0;

    enum w_status status;
    uint16_t i;
    int has_registered_logger;

    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(msg != NULL);

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    if (!initialized)
    {
        wp_log_is_std_err_console = w_is_console(W_STD_ERR);
        initialized = 1;
    }

    has_registered_logger = 0;
    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        struct wp_log_logger *logger;

        logger = &wp_log_logger_pool[i];
        if (!logger->valid || (logger->lvl < lvl))
        {
            continue;
        }

        has_registered_logger = 1;
        switch (logger->fmt)
        {
            case W_LOG_FMT_PLAIN:
                status = wp_log_fmt_plain(
                    logger->stream, lvl, file, line, 0, msg, args
                );
                break;
            case W_LOG_FMT_PLAIN_STYLIZED:
                status = wp_log_fmt_plain(
                    logger->stream, lvl, file, line, 1, msg, args
                );
                break;
            default:
                W_ASSERT(0);
        }

        if (status != W_SUCCESS)
        {
            goto exit;
        }
    }

    // Fallback to a default logger if none was explicitly registered.
    if (!has_registered_logger)
    {
        status = wp_log_fmt_plain(
            W_STD_ERR,
            lvl,
            file,
            line,
            wp_log_is_std_err_console,
            msg,
            args
        );

        if (status != W_SUCCESS)
        {
            goto exit;
        }
    }

exit:
    if (w_mtx_unlock(&wp_log_mtx) != 0)
    {
        status = W_ERROR_UNLOCK_FAILED;
    }

    return status;
}

enum w_status
wp_log_system_error(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    int error
)
{
    char msg[512];

    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(file != NULL);

    if (w_format_system_error(msg, W_GET_ARRAY_LEN(msg), error) != 0)
    {
        return W_ERROR;
    }

    return wp_log(lvl, file, line, "system error %d: %s\n", error, msg);
}

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
w_logger_register(
    struct w_logger *logger_handle,
    w_stream stream,
    enum w_log_lvl lvl,
    enum w_log_fmt fmt
)
{
    enum w_status status;
    uint16_t i;

    W_ASSERT(logger_handle != NULL);
    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(fmt >= WP_LOG_FMT_FIRST && fmt <= WP_LOG_FMT_LAST);

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        struct wp_log_logger *logger;

        logger = &wp_log_logger_pool[i];

        // Find the first slot available with no valid logger assigned to it.
        if (
            !logger->valid
            && !W_UINT_IS_ADD_WRAPPING(UINT16_MAX, logger->id, 1)
        )
        {
            logger->id += 1;
            logger->stream = stream;
            logger->lvl = lvl;
            logger->fmt = fmt;
            logger->valid = 1;

            logger_handle->handle = WP_LOG_SET_HANDLE(i, logger->id);
            status = W_SUCCESS;
            goto exit;
        }
    }

    status = W_ERROR_BUF_FULL;

exit:
    if (w_mtx_unlock(&wp_log_mtx) != 0)
    {
        status = W_ERROR_UNLOCK_FAILED;
    }

    return status;
}

enum w_status
w_logger_deregister(
    struct w_logger logger_handle
)
{
    enum w_status status;
    struct wp_log_logger *logger;

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    status = wp_log_logger_find(&logger, logger_handle.handle);
    if (status == W_SUCCESS)
    {
        logger->valid = 0;
    }

    if (w_mtx_unlock(&wp_log_mtx) != 0)
    {
        status = W_ERROR_UNLOCK_FAILED;
    }

    return status;
}

enum w_status
w_logger_deregister_all(
    void
)
{
    uint16_t i;

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        wp_log_logger_pool[i].valid = 0;
    }

    if (w_mtx_unlock(&wp_log_mtx) != 0)
    {
        return W_ERROR_UNLOCK_FAILED;
    }

    return W_SUCCESS;
}
