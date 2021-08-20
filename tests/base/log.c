#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <rexo.h>

#include <stddef.h>

RX_SET_UP(log_set_up)
{
    W_ASSERT(w_logger_deregister_all() == W_SUCCESS);
    return RX_SUCCESS;
}

RX_VOID_FIXTURE(log_fixture, .set_up = log_set_up);

RX_TEST_SUITE(log, .fixture = log_fixture);

RX_TEST_CASE(log, logger_registration)
{
    enum w_status status;
    struct w_logger default_logger;
    struct w_logger logger;

    w_get_default_logger(&default_logger);

    status = w_logger_register(
        &logger, W_STD_ERR, W_LOG_LVL_ALL, W_LOG_FMT_PLAIN
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_NOT_EQUAL(logger.handle, default_logger.handle);
}

RX_TEST_CASE(log, plain_logging)
{
    enum w_status status;
    struct w_logger logger;

    status = w_logger_register(
        &logger, W_STD_ERR, W_LOG_LVL_ALL, W_LOG_FMT_PLAIN
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    W_LOG_FATAL("fatal message\n");
    W_LOG_ERROR("error message\n");
    W_LOG_WARNING("warning message\n");
    W_LOG_INFO("info message\n");
    W_LOG_DEBUG("debug message\n");
}

RX_TEST_CASE(log, plain_stylized_logging)
{
    enum w_status status;
    struct w_logger logger;

    status = w_logger_register(
        &logger, W_STD_ERR, W_LOG_LVL_ALL, W_LOG_FMT_PLAIN_STYLIZED
    );
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    W_LOG_FATAL("fatal message\n");
    W_LOG_ERROR("error message\n");
    W_LOG_WARNING("warning message\n");
    W_LOG_INFO("info message\n");
    W_LOG_DEBUG("debug message\n");
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
