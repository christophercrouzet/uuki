#include <rexo.h>

#include <uuki/base/macros.h>
#include <uuki/base/status.h>
#include <uuki/base/platform.h>
#include <uuki/lang/token.h>

#include <stddef.h>

#define TEST_TOKEN_SCANNING(expr, expected_id, expected_error_msg)             \
    do                                                                         \
    {                                                                          \
        enum w_status status;                                                  \
        struct w_tokenizer tokenizer;                                          \
        struct w_token token;                                                  \
                                                                               \
        status = w_tokenizer_create(&tokenizer, (expr));                       \
        RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);                               \
                                                                               \
        status = w_tokenizer_advance(&tokenizer, &token);                      \
        RX_INT_REQUIRE_EQUAL(status, W_SUCCESS);                               \
        RX_INT_REQUIRE_EQUAL(token.id, (expected_id));                         \
                                                                               \
        if (token.id != W_TOKEN_ID_INVALID)                                    \
        {                                                                      \
            RX_STR_REQUIRE_EQUAL(&tokenizer.buf[token.pos], (expr));           \
        }                                                                      \
                                                                               \
        if (tokenizer.error_count == 0)                                        \
        {                                                                      \
            RX_PTR_REQUIRE_EQUAL((expected_error_msg), NULL);                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            RX_UINT_REQUIRE_EQUAL(tokenizer.error_count, 1);                   \
            RX_STR_REQUIRE_EQUAL(                                              \
                tokenizer.errors[0].msg, (expected_error_msg)                  \
            );                                                                 \
        }                                                                      \
                                                                               \
        w_tokenizer_destroy(&tokenizer);                                       \
    }                                                                          \
    while (0)                                                                  \

RX_TEST_CASE(tokenizer, scan_valid_number_literals)
{
    // Binary integer literals.
    TEST_TOKEN_SCANNING(
        "0b1", 
        W_TOKEN_ID_INT_BINARY,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0b0", 
        W_TOKEN_ID_INT_BINARY,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0b01",
        W_TOKEN_ID_INT_BINARY,
        NULL
    );

    // Octal integer literals.
    TEST_TOKEN_SCANNING(
        "0o1",       
        W_TOKEN_ID_INT_OCTAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0o0",       
        W_TOKEN_ID_INT_OCTAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0o01234567",
        W_TOKEN_ID_INT_OCTAL,
        NULL
    );

    // Decimal integer literals.
    TEST_TOKEN_SCANNING(
        "1",         
        W_TOKEN_ID_INT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0",         
        W_TOKEN_ID_INT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "1234567890",
        W_TOKEN_ID_INT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0d123",
        W_TOKEN_ID_INT_DECIMAL,
        NULL
    );

    // Hexadecimal integer literals.
    TEST_TOKEN_SCANNING(
        "0x1",             
        W_TOKEN_ID_INT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x0",             
        W_TOKEN_ID_INT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x12345678ABCDEF",
        W_TOKEN_ID_INT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0xDECAFFFF",      
        W_TOKEN_ID_INT_HEXADECIMAL,
        NULL
    );

    // Decimal floating-point literals.
    TEST_TOKEN_SCANNING(
        "0.",     
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        ".0",     
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "1.23",   
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "1.2e3",  
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0.1e+23",
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0.1e-23",
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "5.e1",   
        W_TOKEN_ID_FLT_DECIMAL,
        NULL
    );

    // Hexadecimal floating-point literals.
    TEST_TOKEN_SCANNING(
        "0x0p0",       
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x123EFp-345",
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0xABCD.p+0",  
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x1.ABCp+123",
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x1.",        
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x.0",        
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x.p2",       
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x.123p2",    
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0xABCp8",     
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x0p-1",      
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x1.p0",      
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0xF.p-1",     
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0x0.123p-1",  
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
    TEST_TOKEN_SCANNING(
        "0xA.Bp10",    
        W_TOKEN_ID_FLT_HEXADECIMAL,
        NULL
    );
}

RX_TEST_CASE(tokenizer, scan_invalid_number_literals)
{
    // Binary integer literals.
    TEST_TOKEN_SCANNING(
        "0b",
        W_TOKEN_ID_INVALID,
        "unterminated binary integer literal"
    );
    TEST_TOKEN_SCANNING(
        "0b2",
        W_TOKEN_ID_INVALID,
        "not a valid binary integer literal character"
    );
    TEST_TOKEN_SCANNING(
        "0bZ",
        W_TOKEN_ID_INVALID,
        "not a valid binary integer literal character"
    );

    // Octal integer literals.
    TEST_TOKEN_SCANNING(
        "0o",
        W_TOKEN_ID_INVALID,
        "unterminated octal integer literal"
    );
    TEST_TOKEN_SCANNING(
        "0o8",
        W_TOKEN_ID_INVALID,
        "not a valid octal integer literal character"
    );
    TEST_TOKEN_SCANNING(
        "0oZ",
        W_TOKEN_ID_INVALID,
        "not a valid octal integer literal character"
    );

    // Decimal integer literals.
    TEST_TOKEN_SCANNING(
        "0d",
        W_TOKEN_ID_INVALID,
        "unterminated decimal integer literal"
    );
    TEST_TOKEN_SCANNING(
        "0dA",
        W_TOKEN_ID_INVALID,
        "not a valid decimal integer literal character"
    );

    // Hexadecimal integer literals.
    TEST_TOKEN_SCANNING(
        "0x",
        W_TOKEN_ID_INVALID,
        "unterminated hexadecimal number literal"
    );
    TEST_TOKEN_SCANNING(
        "0xG",
        W_TOKEN_ID_INVALID,
        "not a valid hexadecimal number literal character"
    );

    // Decimal floating-point literals.
    TEST_TOKEN_SCANNING(
        "1.23e",
        W_TOKEN_ID_INVALID,
        "unterminated floating-point exponent"
    );
    TEST_TOKEN_SCANNING(
        "1.23e+",
        W_TOKEN_ID_INVALID,
        "unterminated floating-point exponent"
    );
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
