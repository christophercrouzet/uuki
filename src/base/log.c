#include "log.h"

#include <uuki/base/assert.h>
#include <uuki/base/debug.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>

#include <stddef.h>
#include <stdint.h>

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

#define WP_LOG_DEFAULT_LOGGER_ID                                               \
    0

struct wp_log_logger {
    uint32_t id;
    w_stream stream;
    enum w_log_lvl lvl;
    enum w_log_fmt fmt;
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

static uint32_t
wp_log_logger_last_id = WP_LOG_DEFAULT_LOGGER_ID;

static struct wp_log_logger
wp_log_logger_pool[8] = {
    {
        .id = WP_LOG_DEFAULT_LOGGER_ID,
        .valid = 1
    }
};

W_MTX_INTIALIZE(wp_log_mtx);

// Forward Declarations
// ---------------------------------------------------------------- //   O-(''Q)

static void
wp_log_get_str_len(
    size_t *len,
    const char *str
);

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_log_logger_find(
    struct wp_log_logger **logger,
    uint32_t id
)
{
    size_t i;

    W_ASSERT(logger != NULL);

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        *logger = &wp_log_logger_pool[i];
        if ((*logger)->valid && (*logger)->id == id)
        {
            return W_SUCCESS;
        }
    }

    return W_ERROR_NOT_FOUND;
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

    wp_log_get_str_len(&len, buf);

    // Format the body.
    if (
        w_format_msg_va(
            &buf[len], W_GET_ARRAY_LEN(buf) - len, msg, args
        )
        != 0)
    {
        return W_ERROR_STR_FORMATTING_FAILED;
    }

    // Print the result.
    if (w_print(stream, buf) != 0)
    {
        return W_ERROR_IO_FAILED;
    }

    return W_SUCCESS;
}

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

void
w_get_default_logger(
    struct w_logger *logger_handle
)
{
    W_ASSERT(logger_handle != NULL);

    logger_handle->id = WP_LOG_DEFAULT_LOGGER_ID;
}

enum w_status
w_logger_register(
    struct w_logger *logger_handle,
    w_stream stream,
    enum w_log_lvl lvl,
    enum w_log_fmt fmt
)
{
    enum w_status status;
    size_t i;

    W_ASSERT(logger_handle != NULL);
    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(fmt >= WP_LOG_FMT_FIRST && fmt <= WP_LOG_FMT_LAST);

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    if (W_UINT_IS_ADD_WRAPPING(UINT32_MAX, wp_log_logger_last_id, 1))
    {
        status = W_ERROR_MAX_ID_EXCEEDED;
        goto exit;
    }

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        struct wp_log_logger *logger;

        logger = &wp_log_logger_pool[i];

        // Replace the default logger, which should be on the first slot,
        // or find the first slot available with no valid logger assigned to it.
        if (!logger->valid || logger->id == WP_LOG_DEFAULT_LOGGER_ID)
        {
            ++wp_log_logger_last_id;

            logger->id = wp_log_logger_last_id;
            logger->stream = stream;
            logger->lvl = lvl;
            logger->fmt = fmt;
            logger->valid = 1;

            logger_handle->id = wp_log_logger_last_id;
            status = W_SUCCESS;
            goto exit;
        }
    }

    status = W_ERROR_BUF_SIZE_EXCEEDED;

exit:
    w_mtx_unlock(&wp_log_mtx);
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

    status = wp_log_logger_find(&logger, logger_handle.id);
    if (status == W_SUCCESS)
    {
        logger->valid = 0;
    }

    w_mtx_unlock(&wp_log_mtx);
    return status;
}

enum w_status
w_logger_deregister_all(
    void
)
{
    size_t i;

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        wp_log_logger_pool[i].valid = 0;
    }

    w_mtx_unlock(&wp_log_mtx);
    return W_SUCCESS;
}

enum w_status
w_log(
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
    status = w_log_va(lvl, file, line, msg, args);
    va_end(args);
    return status;
}

enum w_status
w_log_va(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *msg,
    va_list args
)
{
    static int32_t default_logger_initialized = 0;

    enum w_status status;
    size_t i;

    W_ASSERT(lvl >= WP_LOG_LVL_FIRST && lvl <= WP_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(msg != NULL);

    if (w_mtx_lock(&wp_log_mtx) != 0)
    {
        return W_ERROR_LOCK_FAILED;
    }

    if (!default_logger_initialized)
    {
        struct wp_log_logger *logger;

        status = wp_log_logger_find(&logger, WP_LOG_DEFAULT_LOGGER_ID);
        if (status == W_SUCCESS)
        {
            logger->stream = W_STD_ERR;
            logger->lvl = WP_LOG_DEFAULT_LEVEL;
            logger->fmt = w_is_console(W_STD_ERR)
                ? W_LOG_FMT_PLAIN_STYLIZED
                : W_LOG_FMT_PLAIN;
        }

        default_logger_initialized = 1;
    }

    for (i = 0; i < W_GET_ARRAY_LEN(wp_log_logger_pool); ++i)
    {
        struct wp_log_logger *logger;

        logger = &wp_log_logger_pool[i];
        if (!logger->valid || (logger->lvl < lvl))
        {
            continue;
        }

        status = W_SUCCESS;
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
    }

    w_mtx_unlock(&wp_log_mtx);
    return status;
}

#if W_OS(UNIX)
    #include "log_unix.c"
#elif W_OS(WINDOWS)
    #include "log_windows.c"
#else
    #error "not implemented"
#endif
