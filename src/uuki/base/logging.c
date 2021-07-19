#include "logging.h"

#include <uuki/base/assert.h>
#include <uuki/base/debugging.h>

#include <stdarg.h>
#include <stdio.h>

#define WP_LOGGING_LVL_COUNT                                                   \
    WP_LOGGING_LOG_LVL_LAST - WP_LOGGING_LOG_LVL_FIRST + 1

#if W_DEBUGGING(ENABLED)
    static enum w_log_lvl
    wp_logging_lvl = W_LOG_LVL_DEBUG;
#else
    static enum w_log_lvl
    wp_logging_lvl = W_LOG_LVL_WARNING;
#endif

static int
wp_logging_styling = 0;

static const char *
wp_logging_lvl_name[WP_LOGGING_LVL_COUNT] = {
    "none",
    "fatal",
    "error",
    "warning",
    "info",
    "debug",
};

static const char *
wp_logging_lvl_style_begin[2][WP_LOGGING_LVL_COUNT] = {
    // styling == 0
    {
        "",
        "",
        "",
        "",
        "",
        "",
    },
    // styling == 1
    {
        "",
        "\x1b[1;35m",
        "\x1b[1;31m",
        "\x1b[1;33m",
        "\x1b[1;36m",
        "\x1b[1;32m",
    },
};

static const char *
wp_logging_lvl_style_end[2] = {
    "",         // styling == 0
    "\x1b[0m",  // styling == 1
};

void
w_vlog(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *fmt,
    va_list args
)
{
    W_ASSERT(wp_logging_styling == 0 || wp_logging_styling == 1);
    W_ASSERT(lvl >= WP_LOGGING_LOG_LVL_FIRST && lvl <= WP_LOGGING_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(fmt != NULL);

    fprintf(
        stderr,
        "%s:%d: %s%s%s: ",
        file,
        line,
        wp_logging_lvl_style_begin[wp_logging_styling][lvl],
        wp_logging_lvl_name[lvl],
        wp_logging_lvl_style_end[wp_logging_styling]
    );
    vfprintf(stderr, fmt, args);
}

void
w_log(
    enum w_log_lvl lvl,
    const char *file,
    int line,
    const char *fmt,
    ...
)
{
    va_list args;

    W_ASSERT(lvl >= WP_LOGGING_LOG_LVL_FIRST && lvl <= WP_LOGGING_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(fmt != NULL);

    va_start(args, fmt);
    w_vlog(lvl, file, line, fmt, args);
    va_end(args);
}

enum w_log_lvl
w_get_log_lvl(
    void
)
{
    return wp_logging_lvl;
}

void
w_set_log_lvl(
    enum w_log_lvl lvl
)
{
    if (lvl < WP_LOGGING_LOG_LVL_FIRST || lvl > WP_LOGGING_LOG_LVL_LAST)
    {
        W_LOG_ERROR("level out of range\n");
        return;
    }

    wp_logging_lvl = lvl;
}

int
w_get_log_styling(
    void
)
{
    return wp_logging_styling;
}

void
w_set_log_styling(
    int styling
)
{
    wp_logging_styling = styling != 0;
}
