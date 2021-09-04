#ifndef UUKI_BASE_FS_H
#define UUKI_BASE_FS_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_file {
    void *handle;
};

enum w_status
w_file_open(
    struct w_file *file,
    const char *path
);

enum w_status
w_file_read(
    struct w_file *file,
    void *buf,
    size_t *size
);

enum w_status
w_file_close(
    struct w_file *file
);

#endif // UUKI_BASE_FS_H
