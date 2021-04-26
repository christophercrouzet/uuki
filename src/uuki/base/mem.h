#ifndef UUKI_BASE_MEM_H
#define UUKI_BASE_MEM_H

#include <uuki/base/status.h>

#include <stddef.h>

#define W_DEFAULT_ALIGNMENT sizeof(void *)

#define W_ALLOCATE(alloc, ptr, size)                                           \
    ((alloc)->allocate(                                                        \
        (alloc)->inst, (ptr), (size), W_DEFAULT_ALIGNMENT))

#define W_ALLOCATE_ALIGNED(alloc, ptr, size, alignment)                        \
    ((alloc)->allocate(                                                        \
        (alloc)->inst, (ptr), (size), (alignment)))

#define W_FREE(alloc, ptr, size)                                               \
    ((alloc)->free(                                                            \
        (alloc)->inst, (ptr), (size), W_DEFAULT_ALIGNMENT))

#define W_FREE_ALIGNED(alloc, ptr, size, alignment)                            \
    ((alloc)->free(                                                            \
        (alloc)->inst, (ptr), (size), (alignment)))

#define W_REALLOCATE(alloc, ptr, prev_size, size)                              \
    ((alloc)->reallocate(                                                      \
        (alloc)->inst, (ptr), (prev_size), (size), W_DEFAULT_ALIGNMENT))

#define W_REALLOCATE_ALIGNED(alloc, ptr, prev_size, size, alignment)           \
    ((alloc)->reallocate(                                                      \
        (alloc)->inst, (ptr), (prev_size), (size), (alignment)))

typedef enum w_status
(*w_allocate_fn)(void *inst,
                 void **ptr,
                 size_t size,
                 size_t alignment);

typedef void
(*w_free_fn)(void *inst,
             const void *ptr,
             size_t size,
             size_t alignment);

typedef enum w_status
(*w_reallocate_fn)(void *inst,
                   void **ptr,
                   size_t prev_size,
                   size_t size,
                   size_t alignment);

struct w_alloc {
    w_allocate_fn allocate;
    w_free_fn free;
    w_reallocate_fn reallocate;
    void *inst;
};

struct w_linear_alloc {
    struct w_alloc *parent;
    void *buf;
    size_t cap;
    size_t used;
    size_t alignment;
};

enum w_status
w_linear_alloc_create(struct w_linear_alloc *alloc,
                      struct w_alloc *parent,
                      size_t size,
                      size_t alignment);

void
w_linear_alloc_destroy(struct w_linear_alloc *alloc);

enum w_status
w_linear_alloc_allocate(void *inst,
                        void **ptr,
                        size_t size,
                        size_t alignment);

void
w_linear_alloc_free(void *inst,
                    const void *ptr,
                    size_t size,
                    size_t alignment);

enum w_status
w_linear_alloc_reallocate(void *inst,
                          void **ptr,
                          size_t prev_size,
                          size_t size,
                          size_t alignment);

void
w_linear_alloc_get_universal_alloc(struct w_alloc *alloc,
                                   struct w_linear_alloc *linear_alloc);

#endif // UUKI_BASE_MEM_H
