#include <uuki/base/assert.h>

#include <strsafe.h>

static void
wp_log_get_str_len(
    size_t *len,
    const char *str
)
{
    W_ASSERT(len != NULL);
    W_ASSERT(str != NULL);

    StringCchLengthA(str, STRSAFE_MAX_CCH, len);
}
