#ifndef UUKI_BASE_MACROS_INL
#define UUKI_BASE_MACROS_INL

#define W__UNUSED_PARAM(x)                                                     \
    (void)(x)

#define W__REQUIRE_SEMICOLON                                                   \
    void w__dummy(void)

#if defined(_MSC_VER)
    #define W__PACKED_STRUCT(name)                                             \
        __pragma(pack(push, 1))                                                \
        struct name                                                            \
        __pragma(pack(pop))
#else
    #define W__PACKED_STRUCT(name)                                             \
        struct __attribute__((packed)) name
#endif


// Integer Overflow Checks
// ---------------------------------------------------------------- //   O-(''Q)

#define W__UINT_IS_ADD_WRAPPING(a, b, max)                                     \
    ((a) > (max) - (b))

#define W__UINT_IS_SUB_WRAPPING(a, b)                                          \
    ((a) < (b))

#define W__UINT_IS_MUL_WRAPPING(a, b, max)                                     \
    ((a) > (max) / (b))

#endif // UUKI_BASE_MACROS_INL
