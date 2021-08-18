#include "platform.h"

#include <uuki/base/debug.h>

#include <stdarg.h>
#include <stddef.h>

#define WP_PLATFORM_CONTINUATION_CHARS                                         \
    "[…]\n"

#define WP_PLATFORM_GET_ARRAY_LEN(x)                                           \
    (sizeof (x) / sizeof *(x))

#if W_DEBUGGING(ENABLED)
    #define WP_PLATFORM_ASSERT(expr)                                           \
        (                                                                      \
            !!(expr)                                                           \
                ? (void)0                                                      \
                : (void)w_abort(-1)                                            \
        )
#else
    #define WP_PLATFORM_ASSERT(expr)                                           \
        ((void)0)
#endif

// Forward Declarations
// ---------------------------------------------------------------- //   O-(''Q)

static void
wp_platform_copy_str(
    char *dst,
    size_t len,
    const char *src
);

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static void
wp_platform_truncate(
    char *msg,
    size_t len
)
{
    size_t pos;

    WP_PLATFORM_ASSERT(msg != NULL);

    if (len < WP_PLATFORM_GET_ARRAY_LEN(WP_PLATFORM_CONTINUATION_CHARS))
    {
        return;
    }

    pos = len - WP_PLATFORM_GET_ARRAY_LEN(WP_PLATFORM_CONTINUATION_CHARS);
    while (pos > 0)
    {
        // If the preceeding character is a space, we assume that we found
        // the beginning of a word.
        if (msg[pos - 1] == ' ')
        {
            break;
        }

        --pos;
    }
    wp_platform_copy_str(&msg[pos], len - pos, WP_PLATFORM_CONTINUATION_CHARS);
}

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

int
w_format_msg(
    char *msg,
    size_t msg_len,
    const char *fmt,
    ...
)
{
    int result;
    va_list args;

    WP_PLATFORM_ASSERT(msg != NULL);

    va_start(args, fmt);
    result = w_format_msg_va(msg, msg_len, fmt, args);
    va_end(args);
    return result;
}

#if W_OS(UNIX)
    #include "platform_unix.c"
#elif W_OS(WINDOWS)
    #include "platform_windows.c"
#else
    #error "not implemented"
#endif
