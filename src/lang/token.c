#include <uuki/lang/token.h>

#include <uuki/base/assert.h>
#include <uuki/base/log.h>
#include <uuki/base/macros.h>
#include <uuki/base/platform.h>
#include <uuki/base/status.h>

#include <inttypes.h>

W_ASSERT_STATIC(
    WP_TOKEN_ID_END <= (1 << W_TOKEN_FIELD_ID_BIT_LEN),
    wp_token_enum_too_large
);

#define WP_TOKEN_CASE_BINARY_DIGITS                                            \
         '0':                                                                  \
    case '1'

#define WP_TOKEN_CASE_OCTAL_DIGITS                                             \
         '0':                                                                  \
    case '1':                                                                  \
    case '2':                                                                  \
    case '3':                                                                  \
    case '4':                                                                  \
    case '5':                                                                  \
    case '6':                                                                  \
    case '7'

#define WP_TOKEN_CASE_DECIMAL_DIGITS                                           \
         '0':                                                                  \
    case '1':                                                                  \
    case '2':                                                                  \
    case '3':                                                                  \
    case '4':                                                                  \
    case '5':                                                                  \
    case '6':                                                                  \
    case '7':                                                                  \
    case '8':                                                                  \
    case '9'

#define WP_TOKEN_CASE_HEXADECIMAL_DIGITS                                       \
         '0':                                                                  \
    case '1':                                                                  \
    case '2':                                                                  \
    case '3':                                                                  \
    case '4':                                                                  \
    case '5':                                                                  \
    case '6':                                                                  \
    case '7':                                                                  \
    case '8':                                                                  \
    case 'A':                                                                  \
    case 'B':                                                                  \
    case 'C':                                                                  \
    case 'D':                                                                  \
    case 'E':                                                                  \
    case 'F'

#define WP_TOKEN_CASE_ASCII_LETTERS                                            \
         'a': case 'A':                                                        \
    case 'b': case 'B':                                                        \
    case 'c': case 'C':                                                        \
    case 'd': case 'D':                                                        \
    case 'e': case 'E':                                                        \
    case 'f': case 'F':                                                        \
    case 'g': case 'G':                                                        \
    case 'h': case 'H':                                                        \
    case 'i': case 'I':                                                        \
    case 'j': case 'J':                                                        \
    case 'k': case 'K':                                                        \
    case 'l': case 'L':                                                        \
    case 'm': case 'M':                                                        \
    case 'n': case 'N':                                                        \
    case 'o': case 'O':                                                        \
    case 'p': case 'P':                                                        \
    case 'q': case 'Q':                                                        \
    case 'r': case 'R':                                                        \
    case 's': case 'S':                                                        \
    case 't': case 'T':                                                        \
    case 'u': case 'U':                                                        \
    case 'v': case 'V':                                                        \
    case 'w': case 'W':                                                        \
    case 'x': case 'X':                                                        \
    case 'y': case 'Y':                                                        \
    case 'z': case 'Z'

#define WP_TOKEN_CASE_WHITESPACE                                               \
         ' ':                                                                  \
    case '\n'

#define WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL                        \
         WP_TOKEN_CASE_WHITESPACE:                                             \
    case '+':                                                                  \
    case '-':                                                                  \
    case '*':                                                                  \
    case '/':                                                                  \
    case '\0'

#define WP_TOKEN_GET_ERROR_MSG_UNTERMINATED(type)                              \
    "unterminated " type

#define WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(type)                              \
    "not a valid " type " character"

static uint32_t
wp_token_max_pos = 1 << W_TOKEN_FIELD_POS_BIT_LEN;

// Helpers
// ---------------------------------------------------------------- //   O-(''Q)

static void
wp_token_advance_iter(
    struct w_tokenizer *tokenizer
)
{
    ++tokenizer->it;
    ++tokenizer->column;
}

W_PRINTF_CHECK(2, 3)
static enum w_status
wp_token_add_error(
    struct w_tokenizer *tokenizer,
    const char *msg,
    ...
)
{
    enum w_status status;
    va_list args;

    W_ASSERT(tokenizer != NULL);
    W_ASSERT(msg != NULL);

    va_start(args, msg);
    status = w_format_msg_va(
        tokenizer->errors[tokenizer->error_count].msg,
        W_GET_ARRAY_LEN(tokenizer->errors[tokenizer->error_count].msg),
        msg,
        args
    );
    va_end(args);

    if (status != W_SUCCESS)
    {
        W_LOG_ERROR("failed to format the token error message\n");
        return status;
    }

    ++tokenizer->error_count;

    W_ASSERT(status == W_SUCCESS);
    return status;
}

// Character Consumption
// ---------------------------------------------------------------- //   O-(''Q)

static void
wp_token_consume_binary_digits(
    struct w_tokenizer *tokenizer
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);

    while (*tokenizer->it == '0' || *tokenizer->it == '1')
    {
        wp_token_advance_iter(tokenizer);
    }
}

static void
wp_token_consume_octal_digits(
    struct w_tokenizer *tokenizer
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);

    while (*tokenizer->it >= '0' && *tokenizer->it <= '7')
    {
        wp_token_advance_iter(tokenizer);
    }
}

static void
wp_token_consume_decimal_digits(
    struct w_tokenizer *tokenizer
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);

    while (*tokenizer->it >= '0' && *tokenizer->it <= '9')
    {
        wp_token_advance_iter(tokenizer);
    }
}

static void
wp_token_consume_hexadecimal_digits(
    struct w_tokenizer *tokenizer
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);

    while (
        (*tokenizer->it >= '0' && *tokenizer->it <= '9')
        || (*tokenizer->it >= 'A' && *tokenizer->it <= 'F')
    )
    {
        wp_token_advance_iter(tokenizer);
    }
}

// Token Scanning: Integers
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_token_scan_binary_int(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->it[-2] == '0');
    W_ASSERT(tokenizer->it[-1] == 'b');
    W_ASSERT(id != NULL);

    switch (*tokenizer->it)
    {
        case WP_TOKEN_CASE_BINARY_DIGITS:
        {
            wp_token_advance_iter(tokenizer);
            wp_token_consume_binary_digits(tokenizer);
            switch (*tokenizer->it)
            {
                case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                {
                    *id = W_TOKEN_ID_INT_BINARY;
                    return W_SUCCESS;
                }
                default:
                {
                    return wp_token_add_error(
                        tokenizer,
                        WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                            "binary integer literal"
                        )
                    );
                }
            }
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_UNTERMINATED("binary integer literal")
            );
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR("binary integer literal")
            );
        }
    }
}

static enum w_status
wp_token_scan_octal_int(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->it[-2] == '0');
    W_ASSERT(tokenizer->it[-1] == 'o');
    W_ASSERT(id != NULL);

    switch (*tokenizer->it)
    {
        case WP_TOKEN_CASE_OCTAL_DIGITS:
        {
            wp_token_advance_iter(tokenizer);
            wp_token_consume_octal_digits(tokenizer);
            switch (*tokenizer->it)
            {
                case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                {
                    *id = W_TOKEN_ID_INT_OCTAL;
                    return W_SUCCESS;
                }
                default:
                {
                    return wp_token_add_error(
                        tokenizer,
                        WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                            "octal integer literal"
                        )
                    );
                }
            }
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_UNTERMINATED("octal integer literal")
            );
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR("octal integer literal")
            );
        }
    }
}

static enum w_status
wp_token_scan_decimal_int(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->it[-2] == '0');
    W_ASSERT(tokenizer->it[-1] == 'd');
    W_ASSERT(id != NULL);

    switch (*tokenizer->it)
    {
        case WP_TOKEN_CASE_DECIMAL_DIGITS:
        {
            wp_token_advance_iter(tokenizer);
            wp_token_consume_decimal_digits(tokenizer);
            switch (*tokenizer->it)
            {
                case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                {
                    *id = W_TOKEN_ID_INT_DECIMAL;
                    return W_SUCCESS;
                }
                default:
                {
                    return wp_token_add_error(
                        tokenizer,
                        WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                            "decimal integer literal"
                        )
                    );
                }
            }
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_UNTERMINATED("decimal integer literal")
            );
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR("decimal integer literal")
            );
        }
    }
}

// Token Scanning: Floating-Points
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_token_scan_flt_exponent(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id,
    enum w_token_id id_on_success
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->it[-1] == 'e' || tokenizer->it[-1] == 'p');
    W_ASSERT(id != NULL);

    if (*tokenizer->it == '+' || *tokenizer->it == '-')
    {
        wp_token_advance_iter(tokenizer);
    }

    switch (*tokenizer->it)
    {
        case WP_TOKEN_CASE_DECIMAL_DIGITS:
        {
            wp_token_consume_decimal_digits(tokenizer);
            switch (*tokenizer->it)
            {
                case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                {
                    *id = id_on_success;
                    return W_SUCCESS;
                }
                default:
                {
                    return wp_token_add_error(
                        tokenizer,
                        WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                            "floating-point exponent"
                        )
                    );
                }
            }
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_UNTERMINATED("floating-point exponent")
            );
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR("floating-point exponent")
            );
        }
    }
}

static enum w_status
wp_token_scan_decimal_flt(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(
        (tokenizer->it[-1] >= '0' && tokenizer->it[-1] <= '9')
        || tokenizer->it[-1] == '.'
    );
    W_ASSERT(id != NULL);

    wp_token_consume_decimal_digits(tokenizer);
    switch (*tokenizer->it)
    {
        case 'e':
        {
            wp_token_advance_iter(tokenizer);
            return wp_token_scan_flt_exponent(
                tokenizer, id, W_TOKEN_ID_FLT_DECIMAL
            );
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            *id = W_TOKEN_ID_FLT_DECIMAL;
            return W_SUCCESS;
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                    "decimal floating-point literal"
                )
            );
        }
    }
}

static enum w_status
wp_token_scan_hexadecimal_flt(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->token_begin[0] == '0');
    W_ASSERT(tokenizer->token_begin[1] == 'x');
    W_ASSERT(tokenizer->it[-1] == '.');
    W_ASSERT(id != NULL);

    wp_token_consume_hexadecimal_digits(tokenizer);
    switch (*tokenizer->it)
    {
        case 'p':
        {
            wp_token_advance_iter(tokenizer);
            return wp_token_scan_flt_exponent(
                tokenizer, id, W_TOKEN_ID_FLT_HEXADECIMAL
            );
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            *id = W_TOKEN_ID_FLT_HEXADECIMAL;
            return W_SUCCESS;
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                    "hexadecimal floating-point literal"
                )
            );
        }
    }
}

// Token Scanning: Number Literals
// ---------------------------------------------------------------- //   O-(''Q)

static enum w_status
wp_token_scan_decimal_number(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(
        (tokenizer->it[-1] >= '0' && tokenizer->it[-1] <= '9')
        || tokenizer->it[-1] == '.'
    );
    W_ASSERT(id != NULL);

    wp_token_consume_decimal_digits(tokenizer);
    switch (*tokenizer->it)
    {
        case '.':
        {
            wp_token_advance_iter(tokenizer);
            return wp_token_scan_decimal_flt(tokenizer, id);
        }
        case 'e':
        {
            wp_token_advance_iter(tokenizer);
            return wp_token_scan_flt_exponent(
                tokenizer, id, W_TOKEN_ID_FLT_DECIMAL
            );
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            *id = W_TOKEN_ID_INT_DECIMAL;
            return W_SUCCESS;
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR("decimal number literal")
            );
        }
    }
}

static enum w_status
wp_token_scan_hexadecimal_number(
    struct w_tokenizer *tokenizer,
    enum w_token_id *id
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(tokenizer->it != NULL);
    W_ASSERT(tokenizer->it[-2] == '0');
    W_ASSERT(tokenizer->it[-1] == 'x');
    W_ASSERT(id != NULL);

    switch (*tokenizer->it)
    {
        case WP_TOKEN_CASE_HEXADECIMAL_DIGITS:
        {
            wp_token_advance_iter(tokenizer);
            wp_token_consume_hexadecimal_digits(tokenizer);
            switch (*tokenizer->it)
            {
                case '.':
                {
                    wp_token_advance_iter(tokenizer);
                    return wp_token_scan_hexadecimal_flt(tokenizer, id);
                }
                case 'p':
                {
                    wp_token_advance_iter(tokenizer);
                    return wp_token_scan_flt_exponent(
                        tokenizer, id, W_TOKEN_ID_FLT_HEXADECIMAL
                    );
                }
                case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                {
                    *id = W_TOKEN_ID_INT_HEXADECIMAL;
                    return W_SUCCESS;
                }
                default:
                {
                    return wp_token_add_error(
                        tokenizer,
                        WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                            "hexadecimal number literal"
                        )
                    );
                }
            }
        }
        case '.':
        {
            wp_token_advance_iter(tokenizer);
            return wp_token_scan_hexadecimal_flt(tokenizer, id);
        }
        case 'p':
        {
            return wp_token_add_error(
                tokenizer,
                "the exponent should always be preceded by "
                "an hexadecimal number"
            );
        }
        case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_UNTERMINATED(
                    "hexadecimal number literal"
                )
            );
        }
        default:
        {
            return wp_token_add_error(
                tokenizer,
                WP_TOKEN_GET_ERROR_MSG_INVALID_CHAR(
                    "hexadecimal number literal"
                )
            );
        }
    }
}

// Public API: Token ID
// ---------------------------------------------------------------- //   O-(''Q)

static const char *
WP_TOKEN_ID_LABELS[WP_TOKEN_ID_END] = {
    WP_TOKEN_ID_ENUM(W_DEFINE_ENUM_MEMBER_LABEL),
};

void
w_get_token_id_as_str(
    const char **str,
    enum w_token_id id
)
{
    W_ASSERT(str != NULL);
    W_ASSERT(id >= 0 && id < WP_TOKEN_ID_END);

    *str = WP_TOKEN_ID_LABELS[id];
}

// Public API: Tokenizer
// ---------------------------------------------------------------- //   O-(''Q)

enum w_status
w_tokenizer_create(
    struct w_tokenizer *tokenizer,
    const char *buf
)
{
    W_ASSERT(tokenizer != NULL);
    W_ASSERT(buf != NULL);

    tokenizer->buf = buf;
    tokenizer->it = buf;
    tokenizer->limit = &buf[wp_token_max_pos];
    tokenizer->line = 1;
    tokenizer->column = 1;
    tokenizer->error_count = 0;
    return W_SUCCESS;
}

void
w_tokenizer_destroy(
    struct w_tokenizer *tokenizer
)
{
    W_ASSERT(tokenizer != NULL);
}

enum w_status
w_tokenizer_advance(
    struct w_tokenizer *tokenizer,
    struct w_token *token
)
{
    enum w_status status;
    enum w_token_id id;

    W_ASSERT(tokenizer != NULL);
    W_ASSERT(token != NULL);

    status = W_SUCCESS;
    id = W_TOKEN_ID_INVALID;
    while (tokenizer->it < tokenizer->limit)
    {
        tokenizer->token_begin = tokenizer->it;
        switch (*tokenizer->it)
        {
            case ' ':
            {
                wp_token_advance_iter(tokenizer);
                break;
            }
            case '\n':
            {
                wp_token_advance_iter(tokenizer);
                ++tokenizer->line;
                tokenizer->column = 1;
                break;
            }
            case '0':
            {
                wp_token_advance_iter(tokenizer);
                switch (*tokenizer->it)
                {
                    case 'b':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_binary_int(tokenizer, &id);
                        goto exit;
                    }
                    case 'd':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_decimal_int(tokenizer, &id);
                        goto exit;
                    }
                    case 'o':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_octal_int(tokenizer, &id);
                        goto exit;
                    }
                    case 'x':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_hexadecimal_number(
                            tokenizer, &id
                        );
                        goto exit;
                    }
                    case WP_TOKEN_CASE_DECIMAL_DIGITS:
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_decimal_number(tokenizer, &id);
                        goto exit;
                    }
                    case '.':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_decimal_flt(tokenizer, &id);
                        goto exit;
                    }
                    case 'e':
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_flt_exponent(
                            tokenizer, &id, W_TOKEN_ID_FLT_DECIMAL
                        );
                        goto exit;
                    }
                    case WP_TOKEN_CASE_VALID_FOLLOW_UP_TO_NUMBER_LITERAL:
                    {
                        id = W_TOKEN_ID_INT_DECIMAL;
                        goto exit;
                    }
                    default:
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_decimal_number(tokenizer, &id);
                        goto exit;
                    }
                }
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                wp_token_advance_iter(tokenizer);
                status = wp_token_scan_decimal_number(tokenizer, &id);
                goto exit;
            }
            case '.':
            {
                wp_token_advance_iter(tokenizer);
                switch (*tokenizer->it)
                {
                    case WP_TOKEN_CASE_DECIMAL_DIGITS:
                    {
                        wp_token_advance_iter(tokenizer);
                        status = wp_token_scan_decimal_flt(tokenizer, &id);
                        goto exit;
                    }
                    default:
                    {
                        status = wp_token_add_error(
                            tokenizer, "unrecognized token"
                        );
                        goto exit;
                    }
                }
            }
            case '+':
            {
                wp_token_advance_iter(tokenizer);
                id = W_TOKEN_ID_OP_ADD;
                goto exit;
            }
            case '-':
            {
                wp_token_advance_iter(tokenizer);
                id = W_TOKEN_ID_OP_SUB;
                goto exit;
            }
            case '*':
            {
                wp_token_advance_iter(tokenizer);
                id = W_TOKEN_ID_OP_MUL;
                goto exit;
            }
            case '/':
            {
                wp_token_advance_iter(tokenizer);
                id = W_TOKEN_ID_OP_DIV;
                goto exit;
            }
            case '\0':
            {
                wp_token_advance_iter(tokenizer);
                id = W_TOKEN_ID_EOF;
                goto exit;
            }
            default:
            {
                status = wp_token_add_error(tokenizer, "unrecognized token");
                goto exit;
            }
        }
    }

    W_LOG_ERROR(
        "the input source code is larger than %" PRIu32" MB\n",
        wp_token_max_pos / (1 << 20)
    );
    status = W_ERROR_MAX_SIZE_EXCEEDED;

exit:
    token->pos = (uint32_t)(
        (uintptr_t)tokenizer->token_begin - (uintptr_t)tokenizer->buf
    );
    token->len = (uint32_t)(
        (uintptr_t)tokenizer->it - (uintptr_t)tokenizer->token_begin
    );
    token->id = id;
    return status;
}
