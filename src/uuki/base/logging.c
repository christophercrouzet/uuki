#include "logging.h"

#include <uuki/base/assert.h>

#include <stdarg.h>
#include <stdio.h>

#define W__LOGGING_LVL_COUNT                                                   \
    W__LOGGING_LOG_LVL_LAST - W__LOGGING_LOG_LVL_FIRST + 1

static enum w_log_lvl
w__logging_lvl = W_LOG_LVL_WARNING;

static int
w__logging_styling = 0;

static const char *
w__logging_lvl_name[W__LOGGING_LVL_COUNT] = {
    "none",
    "fatal",
    "error",
    "warning",
    "info",
    "debug",
};

static const char *
w__logging_lvl_style_begin[2][W__LOGGING_LVL_COUNT] = {
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
w__logging_lvl_style_end[2] = {
    "",         // styling == 0
    "\x1b[0m",  // styling == 1
};

void
w_vlog(enum w_log_lvl lvl,
       const char *file,
       int line,
       const char *fmt,
       va_list args)
{
    W_ASSERT(w__logging_styling == 0 || w__logging_styling == 1);
    W_ASSERT(lvl >= W__LOGGING_LOG_LVL_FIRST && lvl <= W__LOGGING_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(fmt != NULL);

    fprintf(stderr,
            "%s:%d: %s%s%s: ",
            file,
            line,
            w__logging_lvl_style_begin[w__logging_styling][lvl],
            w__logging_lvl_name[lvl],
            w__logging_lvl_style_end[w__logging_styling]);
    vfprintf(stderr, fmt, args);
}

void
w_log(enum w_log_lvl lvl,
      const char *file,
      int line,
      const char *fmt,
      ...)
{
    va_list args;

    W_ASSERT(lvl >= W__LOGGING_LOG_LVL_FIRST && lvl <= W__LOGGING_LOG_LVL_LAST);
    W_ASSERT(file != NULL);
    W_ASSERT(fmt != NULL);

    va_start(args, fmt);
    w_vlog(lvl, file, line, fmt, args);
    va_end(args);
}

enum w_log_lvl
w_get_log_lvl()
{
    return w__logging_lvl;
}

void
w_set_log_lvl(enum w_log_lvl lvl)
{
    if (lvl < W__LOGGING_LOG_LVL_FIRST || lvl > W__LOGGING_LOG_LVL_LAST) {
        W_LOG_ERROR("level out of range\n");
        return;
    }

    w__logging_lvl = lvl;
}

void
w_set_log_styling(int styling)
{
    w__logging_styling = styling != 0;
}
