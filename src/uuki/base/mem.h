#ifndef UUKI_BASE_MEM_H
#define UUKI_BASE_MEM_H

#include <uuki/base/status.h>
#include <uuki/base/private/mem.h>

#include <stddef.h>
#include <stdlib.h>

#define W_DEFAULT_ALIGNMENT sizeof(void *)

#define W_ARE_BUFS_OVERLAPPING(a, b, size)                                     \
    (((uintptr_t)(a) <= (uintptr_t)(b)                                         \
         && (uintptr_t)(a) + (size) > (uintptr_t)(b))                          \
     || ((uintptr_t)(b) <= (uintptr_t)(a)                                      \
         && (uintptr_t)(b) + (size) > (uintptr_t)(a)))

#define W_GET_ALIGNMENT(x)                                                     \
    (1 + (W__MEM_GET_ALIGNMENT_1((x) - 1)))

#define W_ALLOCATE(alloc, ptr, size)                                           \
    ((alloc)->allocate(                                                        \
        (alloc)->inst, (ptr), (size), W_DEFAULT_ALIGNMENT))

#define W_ALLOCATE_ALIGNED(alloc, ptr, size, alignment)                        \
    ((alloc)->allocate(                                                        \
        (alloc)->inst, (ptr), (size), (alignment)))

#define W_REALLOCATE(alloc, ptr, prev_size, size)                              \
    ((alloc)->reallocate(                                                      \
        (alloc)->inst, (ptr), (prev_size), (size), W_DEFAULT_ALIGNMENT))

#define W_REALLOCATE_ALIGNED(alloc, ptr, prev_size, size, alignment)           \
    ((alloc)->reallocate(                                                      \
        (alloc)->inst, (ptr), (prev_size), (size), (alignment)))

#define W_FREE(alloc, ptr, size)                                               \
    ((alloc)->free(                                                            \
        (alloc)->inst, (ptr), (size), W_DEFAULT_ALIGNMENT))

#define W_FREE_ALIGNED(alloc, ptr, size, alignment)                            \
    ((alloc)->free(                                                            \
        (alloc)->inst, (ptr), (size), (alignment)))

typedef enum w_status
(*w_allocate_fn)(void *inst,
                 void **ptr,
                 size_t size,
                 size_t alignment);

typedef enum w_status
(*w_reallocate_fn)(void *inst,
                   void **ptr,
                   size_t prev_size,
                   size_t size,
                   size_t alignment);

typedef void
(*w_free_fn)(void *inst,
             const void *ptr,
             size_t size,
             size_t alignment);

struct w_alloc {
    w_allocate_fn allocate;
    w_reallocate_fn reallocate;
    w_free_fn free;
    void *inst;
};

struct w_linear_alloc {
    struct w_alloc *parent;
    void *buf;
    size_t cap;
    size_t used;
    size_t alignment;
};

void
w_grow_cap_pow2(size_t *cap,
                size_t req,
                size_t element_size);

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

enum w_status
w_linear_alloc_reallocate(void *inst,
                          void **ptr,
                          size_t prev_size,
                          size_t size,
                          size_t alignment);

void
w_linear_alloc_free(void *inst,
                    const void *ptr,
                    size_t size,
                    size_t alignment);

void
w_linear_alloc_get_universal_alloc(struct w_alloc *alloc,
                                   struct w_linear_alloc *linear_alloc);

#endif // UUKI_BASE_MEM_H
