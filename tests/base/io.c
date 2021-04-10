#include <rexo.h>

#include <uuki/base/io.h>

#include <stddef.h>

#define SIZE 13
#define BUF "hello, world!\0"
#define FILE_PATH "tests/base/data/io.txt"

RX_TEST_CASE(io, main)
{
    struct w_file file;
    size_t size;
    char buf[SIZE + 1];

    buf[SIZE] = '\0';

    RX_INT_REQUIRE_EQUAL(w_file_open(&file, FILE_PATH), 0);
    RX_INT_REQUIRE_EQUAL(w_file_read(&size, NULL, &file), 0);
    RX_UINT_REQUIRE_EQUAL(size, SIZE);
    RX_INT_REQUIRE_EQUAL(w_file_read(&size, buf, &file), 0);
    RX_STR_REQUIRE_EQUAL(buf, BUF);
    RX_INT_REQUIRE_EQUAL(w_file_close(&file), 0);
}

int
main(int argc,
     const char **argv)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
