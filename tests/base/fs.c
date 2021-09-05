#include <rexo.h>

#include <uuki/base/fs.h>
#include <uuki/base/status.h>

#include <stddef.h>

#define SIZE 13
#define BUF "hello, world!\0"
#define FILE_PATH "tests/base/data/fs.txt"

RX_TEST_CASE(fs, main)
{
    enum w_status status;
    struct w_file file;
    size_t size;
    char buf[SIZE + 1];

    buf[SIZE] = '\0';

    status = w_file_open(&file, FILE_PATH);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);

    status = w_file_read(&file, NULL, &size);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_UINT_REQUIRE_EQUAL(size, SIZE);

    status = w_file_read(&file, buf, &size);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
    RX_STR_REQUIRE_EQUAL(buf, BUF);

    status = w_file_close(&file);
    RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
