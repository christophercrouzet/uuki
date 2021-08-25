#ifndef UUKI_BASE_VMEM_H
#define UUKI_BASE_VMEM_H

#include <uuki/base/status.h>

#include <stddef.h>

struct w_vmem {
    void *addr;
};

size_t
w_get_alloc_granularity(
    void
);

size_t
w_get_commit_granularity(
    void
);

size_t
w_round_up_alloc_size(
    size_t size
);

size_t
w_round_up_commit_size(
    size_t size
);

enum w_status
w_vmem_create(
    struct w_vmem *mem,
    void *addr,
    size_t size
);

void
w_vmem_destroy(
    struct w_vmem *mem,
    size_t size
);

enum w_status
w_vmem_commit(
    struct w_vmem *mem,
    void *addr,
    size_t size
);

enum w_status
w_vmem_uncommit(
    struct w_vmem *mem,
    void *addr,
    size_t size
);

#endif // UUKI_BASE_VMEM_H
