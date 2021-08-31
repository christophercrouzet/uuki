#include <uuki/base/platform.h>

#include <rexo.h>

#include <errno.h>

RX_TEST_CASE(platform, printing)
{
    int status;

    if (!w_is_console(W_STD_OUT))
    {
        // Skip printing to the console if we don't have a valid one, which
        // might be the case when running this test through ctest.
        return;
    }

    status = w_print(
        W_STD_OUT,
        "\x1b[1;34mhéllo\x1b[0m … \x1b[1;31mwörld\x1b[0m!\n"
    );
    RX_INT_REQUIRE_EQUAL(status, 0);
}

RX_TEST_CASE(platform, msg_formatting)
{
    int status;
    char msg[256];

    status = w_format_msg(
        msg,
        sizeof msg / sizeof *msg,
        "%s … %s!\n",
        "\x1b[1;34mhéllo\x1b[0m",
        "\x1b[1;31mwörld\x1b[0m"
    );
    RX_INT_REQUIRE_EQUAL(status, 0);
    RX_STR_REQUIRE_EQUAL(
        msg, "\x1b[1;34mhéllo\x1b[0m … \x1b[1;31mwörld\x1b[0m!\n"
    );
}

RX_TEST_CASE(platform, msg_formatting_overflow)
{
    int status;
    char msg[128];

    status = w_format_msg(msg, 4, "abcd efgh ijkl\n");
    RX_INT_REQUIRE_EQUAL(status, 0);
    RX_STR_REQUIRE_EQUAL(msg, "abc");

    status = w_format_msg(msg, 8, "abcd efgh ijkl\n");
    RX_INT_REQUIRE_EQUAL(status, 0);
    RX_STR_REQUIRE_EQUAL(msg, "[…]\n");

    status = w_format_msg(msg, 12, "abcd efgh ijkl\n");
    RX_INT_REQUIRE_EQUAL(status, 0);
    RX_STR_REQUIRE_EQUAL(msg, "abcd […]\n");

    status = w_format_msg(msg, 16, "abcd efgh ijkl\n");
    RX_INT_REQUIRE_EQUAL(status, 0);
    RX_STR_REQUIRE_EQUAL(msg, "abcd efgh ijkl\n");
}

RX_TEST_CASE(platform, system_error_formatting)
{
    int status;
    char msg[256];

    status = w_format_system_error(msg, sizeof msg / sizeof *msg, ENOENT);
    RX_INT_REQUIRE_EQUAL(status, 0);
#if W_OS(UNIX)
    RX_STR_REQUIRE_EQUAL(msg, "No such file or directory");
#elif W_OS(WINDOWS)
    RX_STR_REQUIRE_EQUAL(msg, "The system cannot find the file specified");
#endif
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
