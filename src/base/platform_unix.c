#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int
wp_platform_enforce_error(
    int error
)
{
    return error == 0 ? -1 : error;
}

static void
wp_platform_copy_str(
    char *dst,
    size_t len,
    const char *src
)
{
    WP_PLATFORM_ASSERT(dst != NULL);
    WP_PLATFORM_ASSERT(src != NULL);

    strncpy(dst, src, len);
}

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

void
w_abort(
    int status
)
{
    _exit(status);
}

int
w_mtx_lock(
    pthread_mutex_t *mtx
)
{
    int error;

    WP_PLATFORM_ASSERT(mtx != NULL);

    error = pthread_mutex_lock(mtx);
    if (error != 0)
    {
        return wp_platform_enforce_error(error);
    }

    return 0;
}

int
w_mtx_unlock(
    pthread_mutex_t *mtx
)
{
    int error;

    WP_PLATFORM_ASSERT(mtx != NULL);

    error = pthread_mutex_unlock(mtx);
    if (error != 0)
    {
        return wp_platform_enforce_error(error);
    }

    return 0;
}

int
w_is_console(
    FILE *stream
)
{
    WP_PLATFORM_ASSERT(stream != NULL);

    return isatty(fileno(stream));
}

int
w_print(
    FILE *stream,
    const char *msg
)
{
    WP_PLATFORM_ASSERT(stream != NULL);
    WP_PLATFORM_ASSERT(msg != NULL);

    errno = 0;
    if (fputs(msg, stream) == EOF)
    {
        return wp_platform_enforce_error(errno);
    }

    return 0;
}

int
w_format_msg_va(
    char *msg,
    size_t msg_len,
    const char *fmt,
    va_list args
)
{
    int result;

    WP_PLATFORM_ASSERT(msg != NULL);
    WP_PLATFORM_ASSERT(fmt != NULL);

    errno = 0;
    result = vsnprintf(msg, msg_len, fmt, args);
    if (result < 0)
    {
        return wp_platform_enforce_error(errno);
    }
    else if ((size_t)result >= msg_len)
    {
        wp_platform_truncate(msg, msg_len);
    }

    return 0;
}

int
w_format_system_error(
    char *msg,
    size_t msg_len,
    int error
)
{
    WP_PLATFORM_ASSERT(msg != NULL);

#if (                                                                          \
        (                                                                      \
               (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L)        \
            || (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600)                \
        )                                                                      \
        && !defined(_GNU_SOURCE)                                               \
    )
    // XSI-compliant implementation of ‘strerror_r()’.
    errno = 0;
    if (strerror_r(error, msg, msg_len) != 0)
    {
        return wp_platform_enforce_error(errno);
    }
#else
    {
        char *err;

        // GNU-specific implementation of ‘strerror_r()’.
        errno = 0;
        err = strerror_r(error, msg, msg_len);
        if (err == NULL)
        {
            return wp_platform_enforce_error(errno);
        }

        // The GNU implementation may or may not use the buffer that we passed
        // so we may need to copy the message returned over to our buffer.
        if (err != msg)
        {
            strncpy(msg, err, msg_len);
        }
    }
#endif

    return 0;
}
