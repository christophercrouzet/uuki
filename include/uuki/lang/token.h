#ifndef UUKI_LANG_TOKEN_H
#define UUKI_LANG_TOKEN_H

#include <uuki/base/macros.h>
#include <uuki/base/status.h>

#include <stddef.h>
#include <stdint.h>

// Token ID
// ---------------------------------------------------------------- //   O-(''Q)

#define WP_TOKEN_ID_ENUM(x)                                                    \
    x(W_TOKEN_ID, INVALID),                                                    \
    x(W_TOKEN_ID, EOF),                                                        \
    x(W_TOKEN_ID, FLT_DECIMAL),                                                \
    x(W_TOKEN_ID, FLT_HEXADECIMAL),                                            \
    x(W_TOKEN_ID, INT_BINARY),                                                 \
    x(W_TOKEN_ID, INT_DECIMAL),                                                \
    x(W_TOKEN_ID, INT_HEXADECIMAL),                                            \
    x(W_TOKEN_ID, INT_OCTAL),                                                  \
    x(W_TOKEN_ID, NEWLINE),                                                    \
    x(W_TOKEN_ID, OP_ADD),                                                     \
    x(W_TOKEN_ID, OP_DIV),                                                     \
    x(W_TOKEN_ID, OP_MUL),                                                     \
    x(W_TOKEN_ID, OP_SUB)

enum w_token_id {
    WP_TOKEN_ID_ENUM(W_DEFINE_ENUM_MEMBER_VALUE),

    WP_TOKEN_ID_END,
};

void
w_get_token_id_as_str(
    const char **str,
    enum w_token_id id
);

// Token
// ---------------------------------------------------------------- //   O-(''Q)

#define W_TOKEN_FIELD_ID_BIT_LEN                                               \
    8
#define W_TOKEN_FIELD_LEN_BIT_LEN                                              \
    12
#define W_TOKEN_FIELD_POS_BIT_LEN                                              \
    30

struct w_token {
    uint32_t pos : W_TOKEN_FIELD_POS_BIT_LEN;
    uint32_t len : W_TOKEN_FIELD_LEN_BIT_LEN;
    enum w_token_id id : W_TOKEN_FIELD_ID_BIT_LEN;
};

// Tokenizer
// ---------------------------------------------------------------- //   O-(''Q)

struct w_token_error {
    char loc[128];
    char msg[128];
};

struct w_tokenizer {
    const char *buf;
    const char *it;
    const char *limit;
    const char *token_begin;
    uint32_t line;
    uint32_t column;
    size_t error_count;
    struct w_token_error errors[32];
};

enum w_status
w_tokenizer_create(
    struct w_tokenizer *tokenizer,
    const char *buf
);

void
w_tokenizer_destroy(
    struct w_tokenizer *tokenizer
);

enum w_status
w_tokenizer_advance(
    struct w_tokenizer *tokenizer,
    struct w_token *token
);

#endif // UUKI_LANG_TOKEN_H
