#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <strsafe.h>
#include <windows.h>

static HANDLE
wp_platform_print_mtx = NULL;

static int
wp_platform_enforce_error(
    DWORD error
)
{
    return error == 0 ? -1 : (int)error;
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

    StringCchCopyA(dst, len, src);
}


// String Breaking
// ---------------------------------------------------------------- //   O-(''Q)

enum wp_platform_break_state {
    WP_BREAK_STATE_DEFAULT              = 0,
    WP_BREAK_STATE_ESC                  = 1,
    WP_BREAK_STATE_ESC_CSI_INITIAL      = 2,
    WP_BREAK_STATE_ESC_CSI_INTERMEDIATE = 3,
    WP_BREAK_STATE_ESC_CSI_FINAL        = 4,
};

#define WP_PLATFORM_IS_UNICODE_CONTINUATION_BYTE(x)                            \
    ((x & 0xc0) == 0x80)

#define WP_PLATFORM_IS_ESC_BYTE(x)                                             \
    ((x) == '\x1b')

#define WP_PLATFORM_IS_ESC_CSI_BYTE(x)                                         \
    ((x) == '[')

#define WP_PLATFORM_IS_ESC_CSI_INITIAL_BYTE(x)                                 \
    ((x) >= 0x30 && (x) < 0x40)

#define WP_PLATFORM_IS_ESC_CSI_INITIAL_BYTE(x)                                 \
    ((x) >= 0x30 && (x) < 0x40)

#define WP_PLATFORM_IS_ESC_CSI_INTERMEDIATE_BYTE(x)                            \
    ((x) >= 0x20 && (x) < 0x30)

#define WP_PLATFORM_IS_ESC_CSI_FINAL_BYTE(x)                                   \
    ((x) >= 0x40 && (x) < 0x80)

static size_t
wp_platform_break_str(
    const char *str,
    size_t max_len
)
{
    size_t i;
    size_t out;
    enum wp_platform_break_state state;

    WP_PLATFORM_ASSERT(str != NULL);

    state = WP_BREAK_STATE_DEFAULT;
    out = 0;
    i = 0;
    while (i < max_len)
    {
        unsigned char c;

        c = (unsigned char)str[i];

        switch (state)
        {
            case WP_BREAK_STATE_ESC:
                if (WP_PLATFORM_IS_ESC_CSI_BYTE(c))
                {
                    state = WP_BREAK_STATE_ESC_CSI_INITIAL;
                }

                break;
            case WP_BREAK_STATE_ESC_CSI_INITIAL:
                if (WP_PLATFORM_IS_ESC_CSI_INTERMEDIATE_BYTE(c))
                {
                    state = WP_BREAK_STATE_ESC_CSI_INTERMEDIATE;
                }
                else if (
                        WP_PLATFORM_IS_ESC_CSI_FINAL_BYTE(c)
                    || !WP_PLATFORM_IS_ESC_CSI_INITIAL_BYTE(c)
                )
                {
                    out = i;
                    state = WP_BREAK_STATE_DEFAULT;
                }

                break;

            case WP_BREAK_STATE_ESC_CSI_INTERMEDIATE:
                if (
                        WP_PLATFORM_IS_ESC_CSI_FINAL_BYTE(c)
                    || !WP_PLATFORM_IS_ESC_CSI_INTERMEDIATE_BYTE(c)
                )
                {
                    out = i;
                    state = WP_BREAK_STATE_DEFAULT;
                }

                break;
            default:
                if (c == '\0')
                {
                    out = i;
                    goto exit;
                }
                else if (WP_PLATFORM_IS_ESC_BYTE(c))
                {
                    out = i;
                    state = WP_BREAK_STATE_ESC;
                }
                else if (!WP_PLATFORM_IS_UNICODE_CONTINUATION_BYTE(c))
                {
                    out = i;
                }
        }

        ++i;
    }

    if (
        state == WP_BREAK_STATE_DEFAULT
        && !WP_PLATFORM_IS_UNICODE_CONTINUATION_BYTE(str[i])
    )
    {
        out = i;
    }

exit:
    return out;
}

// Writing
// ---------------------------------------------------------------- //   O-(''Q)

typedef int
(*wp_write_fn)(
    HANDLE,
    const wchar_t *,
    DWORD
);

static int
wp_platform_write_console(
    HANDLE stream,
    const wchar_t *msg,
    DWORD msg_len
)
{
    DWORD written;

    WP_PLATFORM_ASSERT(msg != NULL);

    written = 0;
    while (written < msg_len)
    {
        DWORD count;

        if (
            WriteConsoleW(
                stream,
                &msg[written],
                msg_len - written,
                &count,
                NULL
            )
            == 0)
        {
            return 0;
        }

        written += count;
    }

    return -1;
}

static int
wp_platform_write_file(
    HANDLE stream,
    const wchar_t *msg,
    DWORD msg_len
)
{
    DWORD written;

    WP_PLATFORM_ASSERT(msg != NULL);

    msg_len *= sizeof *msg;

    written = 0;
    while (written < msg_len)
    {
        DWORD count;

        if (
            WriteFile(
                stream,
                &msg[written],
                msg_len - written,
                &count,
                NULL
            )
            == 0)
        {
            return 0;
        }

        written += count;
    }

    return -1;
}

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

void
w_abort(
    int status
)
{
    TerminateProcess(GetCurrentProcess(), (UINT)status);
}

int
w_mtx_lock(
    HANDLE *mtx
)
{
    WP_PLATFORM_ASSERT(mtx != NULL);

    if (*mtx == NULL)
    {
        HANDLE handle;

        handle = CreateMutex(NULL, 0, NULL);
        if (handle == NULL)
        {
            return wp_platform_enforce_error(GetLastError());
        }

        if (
            InterlockedCompareExchangePointer(
                (void **)mtx, (void *)handle, NULL
            )
            != NULL
        )
        {
            // Another thread already initialized the mutex.
            if (CloseHandle(handle) != 0)
            {
                return wp_platform_enforce_error(GetLastError());
            }
        }
    }

    if (WaitForSingleObject(*mtx, INFINITE) == WAIT_FAILED)
    {
        return wp_platform_enforce_error(GetLastError());
    }

    return 0;
}

int
w_mtx_unlock(
    HANDLE *mtx
)
{
    WP_PLATFORM_ASSERT(mtx != NULL);

    if (ReleaseMutex(*mtx) == 0)
    {
        return wp_platform_enforce_error(GetLastError());
    }

    return 0;
}

int
w_is_console(
    HANDLE stream
)
{
    DWORD file_type;
    DWORD console_mode;

    if (stream == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    file_type = GetFileType(stream);
    if (
           file_type == FILE_TYPE_UNKNOWN
        || (file_type & ~FILE_TYPE_REMOTE) != FILE_TYPE_CHAR
    )
    {
        return 0;
    }

    return !!GetConsoleMode(stream, &console_mode);
}

int
w_print(
    HANDLE stream,
    const char *msg
)
{
    static int32_t initialized = 0;

    WP_PLATFORM_ASSERT(msg != NULL);

    size_t i;
    int status;
    wp_write_fn write_fn;

    status = w_mtx_lock(&wp_platform_print_mtx);
    if (status != 0)
    {
        return status;
    }

    if (!initialized)
    {
        HANDLE std_err;
        DWORD mode;

        // Enable the processing of VT100 character sequences by the console.
        // Note that changing the console mode for the standard error stream
        // should also affect text written through the standard output stream
        // since they usually share the same console.
        std_err = GetStdHandle(STD_ERROR_HANDLE);
        if (GetConsoleMode(std_err, &mode) == 0)
        {
            return wp_platform_enforce_error(GetLastError());
        }
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (SetConsoleMode(std_err, mode) == 0)
        {
            return wp_platform_enforce_error(GetLastError());
        }

        initialized = 1;
    }

    if (w_is_console(stream))
    {
        write_fn = wp_platform_write_console;
    }
    else
    {
        write_fn = wp_platform_write_file;
    }

    // We need to use a mutex because a temporary buffer with fixed size is used
    // to hold the conversion of the input narrow string into a wide string,
    // so if that buffer is not large enough then the write operation is done
    // across multiple calls, in which case we want to avoid interleaved output
    // coming from multiple threads trying to write at the same time.
    

    i = 0;
    status = 0;
    while (msg[i] != '\0')
    {
        wchar_t buf[512] = { 0 };
        size_t narrow_len;
        size_t wide_len;

        // Since the buffer is initialized with null bytes, by reserving one
        // byte we are guaranteed to have a string terminated with a null byte
        // that can be used to know its length.
        narrow_len = wp_platform_break_str(
            &msg[i],
            WP_PLATFORM_GET_ARRAY_LEN(buf) - 1
        );

        // Widen the string.
        if (
            MultiByteToWideChar(
                CP_UTF8,
                MB_ERR_INVALID_CHARS,
                &msg[i],
                (DWORD)narrow_len,
                buf,
                WP_PLATFORM_GET_ARRAY_LEN(buf) - 1
            )
            == 0
        )
        {
            status = wp_platform_enforce_error(GetLastError());
            goto exit;
        }

        // Use our previous trick to know the wide string's length.
        // Note that narrow and wide string lengths are not expected to match.
        if (FAILED(StringCchLengthW(buf, STRSAFE_MAX_CCH, &wide_len)))
        {
            status = -1;
            goto exit;
        }

        if (write_fn(stream, buf, (DWORD)wide_len) == 0)
        {
            status = wp_platform_enforce_error(GetLastError());
            goto exit;
        }

        i += narrow_len;
    }

exit:
    w_mtx_unlock(&wp_platform_print_mtx);
    return status;
}

int
w_format_msg_va(
    char *msg,
    size_t msg_len,
    const char *fmt,
    va_list args
)
{
    HRESULT result;

    WP_PLATFORM_ASSERT(msg != NULL);
    WP_PLATFORM_ASSERT(fmt != NULL);

    result = StringCchVPrintfA(msg, msg_len, fmt, args);
    if (FAILED(result))
    {
        if (result == STRSAFE_E_INSUFFICIENT_BUFFER)
        {
            wp_platform_truncate(msg, msg_len);
        }
        else
        {
            return wp_platform_enforce_error(GetLastError());
        }
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
    wchar_t wide_msg[256];
    size_t len;

    WP_PLATFORM_ASSERT(msg != NULL);

    if (
        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            (DWORD)error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            wide_msg,
            WP_PLATFORM_GET_ARRAY_LEN(wide_msg),
            NULL
        )
        == 0
    )
    {
        return wp_platform_enforce_error(GetLastError());
    }

    // All of Windows' error messages end with a dot followed by a newline but
    // we'd like a message that can be used in-line so we strip that out.
    StringCchLengthW(wide_msg, WP_PLATFORM_GET_ARRAY_LEN(wide_msg), &len);
    if (wide_msg[len - 3] == L'.')
    {
        wide_msg[len - 3] = L'\0';
    }

    // Narrow the string.
    if (
        WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            wide_msg,
            -1,
            msg,
            (int)msg_len,
            NULL,
            NULL
        )
        == 0
    )
    {
        return wp_platform_enforce_error(GetLastError());
    }

    return 0;
}

void
w_get_str_len(
    size_t *len,
    const char *str
)
{
    WP_PLATFORM_ASSERT(len != NULL);
    WP_PLATFORM_ASSERT(str != NULL);

    StringCchLengthA(str, STRSAFE_MAX_CCH, len);
}
