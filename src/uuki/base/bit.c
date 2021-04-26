#ifndef UUKI_BASE_BIT_H
#define UUKI_BASE_BIT_H

#include <uuki/base/platform.h>

#if W_COMPILER(GNUC_COMPLIANT)
    #include <uuki/base/private/bit_gnuc.inl>
#elif W_COMPILER(MSVC)
    #include <uuki/base/private/bit_msvc.inl>
#else
    #error "unsupported compiler"
#endif

#endif // UUKI_BASE_BIT_H
