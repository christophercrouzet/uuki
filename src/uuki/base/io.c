#include "io.h"

#include <uuki/base/assert.h>
#include <uuki/base/logging.h>
#include <uuki/base/status.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

static void
w__io_debug_error(
    int err_num
)
{
    // The function `strerror()` is not thread-safe and its safe alternative
    // `strerror_s()` is not supported by most compilers, so don't bother
    // printing out the error's description.
    W_LOG_DEBUG("errno ‘%d’\n", err_num);
}

enum w_status
w_file_open(
    struct w_file *file,
    const char *path
)
{
    W_ASSERT(file != NULL);
    W_ASSERT(path != NULL);

    errno = 0;
    file->handle = fopen(path, "rb");
    if (file->handle != NULL)
    {
        file->path = path;
        return W_SUCCESS;
    }

    if (errno != 0)
    {
        w__io_debug_error(errno);
    }

    W_LOG_ERROR("failed to open the file ‘%s’\n", path);
    return W_ERROR_STREAM_IO_FAILED;
}

enum w_status
w_file_read(
    size_t *size,
    void *buf,
    struct w_file *file
)
{
    W_ASSERT(size != NULL);
    W_ASSERT(file != NULL);
    W_ASSERT(file->handle != NULL);

    if (buf == NULL)
    {
        long pos;

        errno = 0;
        if (fseek((FILE *)file->handle, 0, SEEK_END) != 0)
        {
            if (errno != 0)
            {
                w__io_debug_error(errno);
            }

            W_LOG_ERROR(
                "failed to reach the end of the file ‘%s’\n", file->path);
            return W_ERROR_STREAM_IO_FAILED;
        }

        errno = 0;
        pos = ftell((FILE *)file->handle);
        if (pos == -1)
        {
            if (errno != 0)
            {
                w__io_debug_error(errno);
            }

            W_LOG_ERROR(
                "failed to retrieve the size of the file ‘%s’\n", file->path);
            return W_ERROR_STREAM_IO_FAILED;
        }

        *size = (size_t)pos;
        return W_SUCCESS;
    }

    errno = 0;
    if (fseek((FILE *)file->handle, 0, SEEK_SET) != 0)
    {
        if (errno != 0)
        {
            w__io_debug_error(errno);
        }

        W_LOG_ERROR("failed to rewind the file ‘%s’\n", file->path);
        return W_ERROR_STREAM_IO_FAILED;
    }

    errno = 0;
    if (fread(buf, 1, *size, (FILE *)file->handle) != *size)
    {
        if (errno != 0)
        {
            w__io_debug_error(errno);
        }

        W_LOG_ERROR("failed to read the file ‘%s’\n", file->path);
        return W_ERROR_STREAM_IO_FAILED;
    }

    return W_SUCCESS;
}

enum w_status
w_file_close(
    struct w_file *file
)
{
    W_ASSERT(file != NULL);
    W_ASSERT(file->handle != NULL);

    errno = 0;
    if (fclose((FILE *)file->handle) == EOF)
    {
        if (errno != 0)
        {
            w__io_debug_error(errno);
        }

        W_LOG_ERROR("failed to close the file ‘%s’\n", file->path);
        return W_ERROR_STREAM_IO_FAILED;
    }

    return W_SUCCESS;
}
