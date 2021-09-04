#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/status.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

enum w_status
w_file_open(
    struct w_file *file,
    const char *path
)
{
    FILE *handle;

    W_ASSERT(file != NULL);
    W_ASSERT(path != NULL);

    errno = 0;
    handle = fopen(path, "rb");
    if (handle == NULL)
    {
        int error;

        error = errno;
        W_LOG_ERROR("failed to open the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_FILE_OPEN_FAILED;
    }

    file->handle = handle;
    return W_SUCCESS;
}

enum w_status
w_file_read(
    struct w_file *file,
    void *buf,
    size_t *size
)
{
    W_ASSERT(file != NULL);
    W_ASSERT(file->handle != NULL);
    W_ASSERT(size != NULL);

    if (buf == NULL)
    {
        off_t pos;

        errno = 0;
        if (fseeko((FILE *)file->handle, 0, SEEK_END) != 0)
        {
            int error;

            error = errno;
            W_LOG_ERROR("failed to reach the end of the file\n");
            W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
            return W_ERROR_FILE_READ_FAILED;
        }

        errno = 0;
        pos = ftello((FILE *)file->handle);
        if (pos == -1)
        {
            int error;

            error = errno;
            W_LOG_ERROR("failed to retrieve the size of the file\n");
            W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
            return W_ERROR_FILE_READ_FAILED;
        }

        *size = (size_t)pos;
        return W_SUCCESS;
    }

    errno = 0;
    if (fseeko((FILE *)file->handle, 0, SEEK_SET) != 0)
    {
        int error;

        error = errno;
        W_LOG_ERROR("failed to rewind the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_FILE_READ_FAILED;
    }

    errno = 0;
    if (fread(buf, 1, *size, (FILE *)file->handle) != *size)
    {
        int error;

        error = errno;
        W_LOG_ERROR("failed to read the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_FILE_READ_FAILED;
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
        int error;

        error = errno;
        W_LOG_ERROR("failed to close the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, error);
        return W_ERROR_FILE_CLOSE_FAILED;
    }

    return W_SUCCESS;
}
