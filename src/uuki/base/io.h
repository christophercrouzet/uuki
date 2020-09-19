#ifndef UUKI_BASE_IO_H
#define UUKI_BASE_IO_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_file {
    void *handle;
    const char *path;
};

enum w_status
w_file_open(struct w_file *file,
            const char *path);

enum w_status
w_file_read(size_t *size,
            void *buf,
            struct w_file *file);

enum w_status
w_file_close(struct w_file *file);

#endif // UUKI_BASE_IO_H
