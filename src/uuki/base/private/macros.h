#ifndef UUKI_BASE_PRIVATE_MACROS_H
#define UUKI_BASE_PRIVATE_MACROS_H

#include <uuki/base/platform.h>

#if W_SIZE_WIDTH >= 128
    #error "Unsupported size width."
#else
    #define W__MACROS_GET_ALIGNMENT_6(x)                                       \
        ((x) | ((x) >> 32))
#endif

#if W_SIZE_WIDTH >= 64
    #define W__MACROS_GET_ALIGNMENT_5(x)                                       \
        W__MACROS_GET_ALIGNMENT_6((x) | ((x) >> 16))
#else
    #define W__MACROS_GET_ALIGNMENT_5(x)                                       \
        ((x) | ((x) >> 16))
#endif

#if W_SIZE_WIDTH >= 32
    #define W__MACROS_GET_ALIGNMENT_4(x)                                       \
        W__MACROS_GET_ALIGNMENT_5((x) | ((x) >> 8))
#else
    #define W__MACROS_GET_ALIGNMENT_4(x)                                       \
        ((x) | ((x) >> 8))
#endif

#if W_SIZE_WIDTH >= 16
    #define W__MACROS_GET_ALIGNMENT_3(x)                                       \
        W__MACROS_GET_ALIGNMENT_4((x) | ((x) >> 4))
#else
    #define W__MACROS_GET_ALIGNMENT_3(x)                                       \
        ((x) | ((x) >> 4))
#endif

#if W_SIZE_WIDTH >= 8
    #define W__MACROS_GET_ALIGNMENT_2(x)                                       \
        W__MACROS_GET_ALIGNMENT_3((x) | ((x) >> 2))
#else
    #define W__MACROS_GET_ALIGNMENT_2(x)                                       \
        ((x) | ((x) >> 2))
#endif

#define W__MACROS_GET_ALIGNMENT_1(x)                                           \
    W__MACROS_GET_ALIGNMENT_2((x) | ((x) >> 1))

#endif // UUKI_BASE_PRIVATE_MACROS_H
