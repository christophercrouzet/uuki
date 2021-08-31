#include <uuki/base/assert.h>
#include <uuki/base/fs.h>
#include <uuki/base/log.h>
#include <uuki/base/mem.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdlib.h>

static enum w_status
w_run(
    struct w_alloc *alloc,
    const char *path
)
{
    enum w_status status;
    struct w_file file;
    size_t size;
    void *buf;

    W_ASSERT(alloc != NULL);
    W_ASSERT(path != NULL);

    status = w_file_open(&file, path);
    if (status != W_SUCCESS)
    {
        goto exit;
    }

    status = w_file_read(&file, NULL, &size);
    if (status != W_SUCCESS)
    {
        goto exit;
    }

    status = W_ALLOCATE(alloc, &buf, size);
    if (status != W_SUCCESS)
    {
        goto exit;
    }

    status = w_file_read(&file, buf, &size);
    if (status != W_SUCCESS)
    {
        goto buf_cleanup;
    }

    status = w_file_close(&file);
    if (status != W_SUCCESS)
    {
        goto buf_cleanup;
    }

buf_cleanup:
    W_FREE(alloc, buf, size);

exit:
    return status;
}

int
main(
    int argc,
    const char **argv
)
{
    enum w_status status;
    struct w_alloc alloc;
    struct w_linear_alloc linear_alloc;

    if (argc < 2)
    {
        W_LOG_WARNING("no input files\n");
        return EXIT_SUCCESS;
    }

    status = w_linear_alloc_create(&linear_alloc, 4096);
    if (status != W_SUCCESS)
    {
        goto exit;
    }

    w_linear_alloc_get_universal_alloc(&alloc, &linear_alloc);

    status = w_run(&alloc, argv[1]);
    if (status != W_SUCCESS)
    {
        goto linear_alloc_cleanup;
    }

linear_alloc_cleanup:
    w_linear_alloc_destroy(&linear_alloc);

exit:
    return status == W_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
