#include <uuki/base/assert.h>

#include <stddef.h>
#include <string.h>

static void
wp_log_get_str_len(
    size_t *len,
    const char *str
)
{
    W_ASSERT(len != NULL);
    W_ASSERT(str != NULL);

    *len = strlen(str);
}
