#include <rexo.h>

#include <uuki/base/logging.h>

RX_TEST_CASE(logging, styling_off)
{
    w_set_log_styling(0);
    W_LOG_FATAL("my unstyled fatal message\n");
    W_LOG_ERROR("my unstyled error message\n");
    W_LOG_WARNING("my unstyled warning message\n");
    W_LOG_INFO("my unstyled info message\n");
    W_LOG_DEBUG("my unstyled debug message\n");
}

RX_TEST_CASE(logging, styling_on)
{
    w_set_log_styling(1);
    W_LOG_FATAL("my styled fatal message\n");
    W_LOG_ERROR("my styled error message\n");
    W_LOG_WARNING("my styled warning message\n");
    W_LOG_INFO("my unstyled info message\n");
    W_LOG_DEBUG("my styled debug message\n");
}

int
main(int argc,
     const char **argv)
{
    w_set_log_lvl(W_LOG_LVL_ALL);
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
