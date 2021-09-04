#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/status.h>

#include <windows.h>

enum w_status
w_file_open(
    struct w_file *file,
    const char *path
)
{
    wchar_t buf[512];
    HANDLE handle;

    W_ASSERT(file != NULL);
    W_ASSERT(path != NULL);

    // Widen the string path.
    if (
        MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            path,
            -1,
            buf,
            W_GET_ARRAY_LEN(buf)
        )
        == 0
    )
    {
        W_LOG_ERROR("failed to widen the string path\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, GetLastError());
        return W_ERROR_FILE_OPEN_FAILED;
    }

    handle = CreateFileW(
        buf,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (handle == INVALID_HANDLE_VALUE)
    {
        W_LOG_ERROR("failed to open the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, GetLastError());
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
    DWORD read;

    W_ASSERT(file != NULL);
    W_ASSERT(file->handle != NULL);
    W_ASSERT(size != NULL);

    if (buf == NULL)
    {
        *size = GetFileSize(file->handle, NULL);
        return W_SUCCESS;
    }

    read = 0;
    while (read < *size)
    {
        DWORD count;

        if (
            ReadFile(
                file->handle,
                (void *)((uintptr_t)buf + read),
                (DWORD)*size - read,
                &count,
                NULL
            )
            == 0)
        {
            W_LOG_ERROR("failed to read the file\n");
            W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, GetLastError());
            return W_ERROR_FILE_READ_FAILED;
        }

        read += count;
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

    if (CloseHandle(file->handle) == 0)
    {
        W_LOG_ERROR("failed to close the file\n");
        W_LOG_SYSTEM_ERROR(W_LOG_LVL_DEBUG, GetLastError());
        return W_ERROR_FILE_CLOSE_FAILED;
    }

    return W_SUCCESS;
}
