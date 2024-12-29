#ifndef __SCM_LEXER_H__
#define __SCM_LEXER_H__

#include "types.h"
#include "nfa.h"

typedef enum {
    SCM_TOKEN_LPAREN,
    SCM_TOKEN_RPAREN,
    // SCM_TOKEN_IF,
    // SCM_TOKEN_COND,
    // SCM_TOKEN_LET,
    // SCM_TOKEN_DEFINE,
    SCM_TOKEN_IDENTIFIER,
    SCM_TOKEN_NUMBER_LITERAL,
    SCM_TOKEN_STRING_LITERAL,
    SCM_TOKEN_QUOTE,
    SCM_TOKEN_QUASIQUOTE,
    SCM_TOKEN_COMMA,
    SCM_TOKEN_ATSIGN,
    SCM_TOKEN_EOF,
    SCM_TOKEN_UNKNOWN,
} scm_token_type_t;

typedef struct {
    scm_token_type_t type;
    string_view_t sv;
    u32 line;
} scm_token_t;

void scm_print_token(scm_token_t* tkn);

typedef enum {
    SCM_LEXER_NFA_STATE_START = 0,
    SCM_LEXER_NFA_STATE_QUASIQUOTE_0,
    SCM_LEXER_NFA_STATE_QUASIQUOTE_A,
    SCM_LEXER_NFA_STATE_QUOTE_0,
    SCM_LEXER_NFA_STATE_QUOTE_A,
    SCM_LEXER_NFA_STATE_LP_A,
    SCM_LEXER_NFA_STATE_RP_A,
    SCM_LEXER_NFA_STATE_IDENTIFIER_0,
    SCM_LEXER_NFA_STATE_IDENTIFIER_A,
    SCM_LEXER_NFA_STATE_LITERAL_NUMBER_0,
    SCM_LEXER_NFA_STATE_LITERAL_NUMBER_A,
    SCM_LEXER_NFA_STATE_STRING_NUMBER_0,
    SCM_LEXER_NFA_STATE_STRING_NUMBER_A,
    SCM_LEXER_NFA_NUM_STATES,
} scm_lexer_nfa_state_t;

typedef struct {
    const char* src;
    u32 len;
    u32 pos;
    u32 line;
    bool has_error;
    // scm_lexer_dfa_state_t state;
    nfa_engine_t nfa;
} scm_lexer_t;

void scm_lexer_init(scm_lexer_t* lx, const char* src, u32 len);
scm_token_t scm_lexer_next_token(scm_lexer_t* lx);

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

#endif // __SCM_LEXER_H__
