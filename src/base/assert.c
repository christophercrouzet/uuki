#include <uuki/base/assert.h>

#include <uuki/base/macros.h>
#include <uuki/base/platform.h>

#define WP_ASSERT_ASSERTION_FAILURE_FMT                                        \
    "---------------------------\n"                                            \
    " /!\\ ASSERTION FAILURE /!\\ \n"                                          \
    "---------------------------\n"                                            \
    "expression : %s\n"                                                        \
    "  function : %s\n"                                                        \
    "      file : %s\n"                                                        \
    "      line : %d\n"

// Public API
// ---------------------------------------------------------------- //   O-(''Q)

void
w_handle_failed_assertion(
    const char *expr,
    const char *fn,
    const char *file,
    int line
)
{
    char msg[1024];

    if (
        w_format_msg(
            msg,
            W_GET_ARRAY_LEN(msg),
            WP_ASSERT_ASSERTION_FAILURE_FMT,
            expr,
            fn,
            file,
            line
        )
        == 0
    )
    {
        w_print(W_STD_ERR, msg);
    }

    w_abort(-1);
}
