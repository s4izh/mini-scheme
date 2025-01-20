#ifndef __SCM_LEXER_H__
#define __SCM_LEXER_H__

#include "scm_resources.h"
#include "types.h"
#include "nfa.h"
#include "rc.h"

typedef struct _scm_token_t scm_token_t;
typedef struct _scm_lexer_t scm_lexer_t;

typedef enum {
    SCM_TOKEN_LPAREN = 0,
    SCM_TOKEN_RPAREN,

    // ATOMS
    SCM_TOKEN_IDENTIFIER,
    SCM_TOKEN_LITERAL_NUMBER,
    SCM_TOKEN_LITERAL_STRING,

    SCM_TOKEN_QUOTE,
    SCM_TOKEN_QUASIQUOTE,
    SCM_TOKEN_COMMA,
    SCM_TOKEN_ATSIGN,
    SCM_TOKEN_EOF,
    SCM_TOKEN_UNKNOWN,
} scm_token_type_t;

struct _scm_token_t {
    scm_token_type_t type;
    string_view_t sv;
    u32 line;
    RC_DECLARE;
};

void scm_token_print(scm_token_t* token, bool only_token_type);

typedef enum {
    SCM_LEXER_NFA_START = 0,
    SCM_LEXER_NFA_QUASIQUOTE_0,
    SCM_LEXER_NFA_QUASIQUOTE_A,
    SCM_LEXER_NFA_QUOTE_0,
    SCM_LEXER_NFA_QUOTE_A,
    SCM_LEXER_NFA_LP_A,
    SCM_LEXER_NFA_RP_A,
    SCM_LEXER_NFA_IDENTIFIER_0,
    SCM_LEXER_NFA_IDENTIFIER_A,
    SCM_LEXER_NFA_LITERAL_NUMBER_0,
    SCM_LEXER_NFA_LITERAL_NUMBER_A,
    SCM_LEXER_NFA_LITERAL_STRING_0,
    SCM_LEXER_NFA_LITERAL_STRING_1,
    SCM_LEXER_NFA_LITERAL_STRING_A,
    SCM_LEXER_NFA_INVALID_R,
    SCM_LEXER_NFA_NUM_STATES,
} scm_lexer_nfa_state_t;

struct _scm_lexer_t {
    const char* filename;
    const char* src;
    u32 len;
    u32 pos;
    u32 line;
    bool has_error;
    nfa_engine_t nfa;
    scm_resources_t* resources;
};

void scm_lexer_init(scm_lexer_t* lexer, scm_resources_t* resources);
void scm_lexer_set_source(scm_lexer_t* lexer, const char* filename, const char* src, u32 len);
scm_token_t* scm_lexer_next_token(scm_lexer_t* lexer);

// for the future parser
// const char *special_forms[] = {
//     "define", "if", "let", "lambda", "quote", "cond"
// };
// int is_special_form(const char *value) {
//     for (size_t i = 0; i < special_forms_count; i++) {
//         if (strcmp(value, special_forms[i]) == 0) {
//             return 1;
//         }
//     }
//     return 0;
// }


#define SCM_SYMBOLS_NO_SPECIAL "!@#$%^&*-_=+[]{}|;:,.<>?/\\~"
#define SCM_LETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define SCM_NUMBERS "0123456789"
#define SCM_LETTERS_NUMBERS SCM_LETTERS SCM_NUMBERS

#define SCM_IDENTIFIERS_BASE SCM_SYMBOLS_NO_SPECIAL SCM_LETTERS
#define SCM_IDENTIFIERS_ALL SCM_SYMBOLS_NO_SPECIAL SCM_LETTERS SCM_NUMBERS

// le aposta el "\\"
#define SCM_LITERAL_STRING_SYMBOLS "!@#$%^&*-_=+[]{}|;:,.<>?/~'`()"
#define SCM_LITERAL_STRING_ESCAPE "\\"
#define SCM_LITERAL_STRING SCM_LITERAL_STRING_SYMBOLS SCM_LETTERS SCM_NUMBERS

#define SCM_LITERAL_STRING_SYMBOLS_VALID_ESCAPED SCM_LITERAL_STRING_SYMBOLS "'`()\\\""
#define SCM_LITERAL_STRING_VALID_ESCAPED SCM_LITERAL_STRING_SYMBOLS_VALID_ESCAPED SCM_LETTERS SCM_NUMBERS

#define SCM_WHITESPACE " \t\n\v\f\r"
#define SCM_QUOTATION "\""
#define SCM_QUOTE "\'"
#define SCM_QUASIQUOTE "`"
#define SCM_LPAREN "("
#define SCM_RPAREN ")"
#define SCM_EOS "\0"

    // const char ALL_CHARACTERS[] = 
    //     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    //     "abcdefghijklmnopqrstuvwxyz"
    //     "0123456789"
    //     "!@#$%^&*-_=+[]{}|;:'\",.<>?/\\`~()";

// #define SCM_LEXER_SLEEP_ENABLE 1
#ifdef SCM_LEXER_SLEEP_ENABLE
#define SCM_LEXER_SLEEP usleep(300000);
#else
#define SCM_LEXER_SLEEP ;
#endif

DA_DEFINE(scm_token_t*, da_token_ptr);  

#endif // __SCM_LEXER_H__
