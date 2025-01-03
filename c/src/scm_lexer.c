#include "scm_lexer.h"
#include <stdio.h>
#include <unistd.h>
#include "nfa.h"

#define __STRING_VIEW_IMPLEMENTATION__
#include "sv.h"

static inline int is_digit(char c)
{
   return (c >= '0' && c <= '9');
}

static inline int is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static inline int is_identifier_char(char c)
{
    return (
        c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        is_digit(c));
}

static inline int is_paren(char c)
{
    return (c == '(' || c == ')');
}

static char scm_lexer_peek(scm_lexer_t* lx)
{
    if (lx->pos + 1 < lx->len)
        return lx->src[lx->pos + 1];
    return 0;
}

static u32 scm_lexer_epsilon_cb(nfa_engine_t* nfa, void* user_data)
{
    scm_lexer_t* lx = (scm_lexer_t*)user_data;

    switch (nfa->state_current) {
        case SCM_LEXER_NFA_QUOTE_0:
        {
            char c = scm_lexer_peek(lx);
            if (is_paren(c) || c == '\"')
            {
                return SCM_LEXER_NFA_QUOTE_0;
            }
        }
        break;
        case SCM_LEXER_NFA_QUASIQUOTE_0:
        {
            char c = scm_lexer_peek(lx);
            if (is_paren(c) || c == '\"')
            {
                return SCM_LEXER_NFA_QUASIQUOTE_A;
            }
        }
        break;
        case SCM_LEXER_NFA_LITERAL_NUMBER_0:
        {
            char c = scm_lexer_peek(lx);
            if (is_paren(c) || c == '\"' || is_whitespace(c))
            {
                return SCM_LEXER_NFA_LITERAL_NUMBER_A;
            }
        }
        break;
        case SCM_LEXER_NFA_IDENTIFIER_0:
        {
            char c = scm_lexer_peek(lx);
            if (is_paren(c) || c == '\"' || is_whitespace(c))
            {
                return SCM_LEXER_NFA_IDENTIFIER_A;
            }
        }
        break;
    }
    return nfa->state_current;
}

static const char* token_type_to_str(scm_token_type_t token)
{
    // clang-format off
    switch (token)
    {
        case SCM_TOKEN_LPAREN:        return "LPAREN";
        case SCM_TOKEN_RPAREN:        return "RPAREN";
        // case SCM_TOKEN_IF:            return "IF";
        // case SCM_TOKEN_COND:          return "COND";
        // case SCM_TOKEN_LET:           return "LET";
        // case SCM_TOKEN_DEFINE:        return "DEFINE";
        case SCM_TOKEN_IDENTIFIER:    return "IDENTIFIER";
        case SCM_TOKEN_LITERAL_NUMBER:return "LITERAL_NUMBER";
        case SCM_TOKEN_LITERAL_STRING:return "LITERAL_STRING";
        case SCM_TOKEN_QUOTE:         return "QUOTE";
        case SCM_TOKEN_QUASIQUOTE:    return "QUASIQUOTE";
        case SCM_TOKEN_COMMA:         return "COMMA";
        case SCM_TOKEN_ATSIGN:        return "ATSIGN";
        case SCM_TOKEN_EOF:           return "EOF";
        case SCM_TOKEN_UNKNOWN:       return "UNKNOWN";
        default:                      return "DEFAULT";
    }
    // clang-format on
}

static const char* nfa_state_to_str(scm_lexer_nfa_state_t state)
{
    switch (state) {
        case SCM_LEXER_NFA_START:                return "SCM_LEXER_NFA_START";
        case SCM_LEXER_NFA_QUASIQUOTE_0:         return "SCM_LEXER_NFA_QUASIQUOTE_0";
        case SCM_LEXER_NFA_QUASIQUOTE_A:         return "SCM_LEXER_NFA_QUASIQUOTE_A";
        case SCM_LEXER_NFA_QUOTE_0:              return "SCM_LEXER_NFA_QUOTE_0";
        case SCM_LEXER_NFA_QUOTE_A:              return "SCM_LEXER_NFA_QUOTE_A";
        case SCM_LEXER_NFA_LP_A:                 return "SCM_LEXER_NFA_LP_A";
        case SCM_LEXER_NFA_RP_A:                 return "SCM_LEXER_NFA_RP_A";
        case SCM_LEXER_NFA_IDENTIFIER_0:         return "SCM_LEXER_NFA_IDENTIFIER_0";
        case SCM_LEXER_NFA_IDENTIFIER_A:         return "SCM_LEXER_NFA_IDENTIFIER_A";
        case SCM_LEXER_NFA_LITERAL_NUMBER_0:     return "SCM_LEXER_NFA_LITERAL_NUMBER_0";
        case SCM_LEXER_NFA_LITERAL_NUMBER_A:     return "SCM_LEXER_NFA_LITERAL_NUMBER_A";
        case SCM_LEXER_NFA_LITERAL_STRING_0:     return "SCM_LEXER_NFA_LITERAL_STRING_0";
        case SCM_LEXER_NFA_LITERAL_STRING_A:     return "SCM_LEXER_NFA_LITERAL_STRING_A";
        case SCM_LEXER_NFA_INVALID_R:            return "SCM_LEXER_NFA_INVALID_R";
        case SCM_LEXER_NFA_NUM_STATES:           return "SCM_LEXER_NFA_NUM_STATES";
        default:                                 return "UNKNOWN_STATE";
    }
}

static scm_token_t scm_create_token(
    scm_lexer_t* lx, scm_token_type_t type, u32 pos_start, u32 pos_end)
{
    string_view_t sv = {lx->src + pos_start, pos_end - pos_start + 1};
    return (scm_token_t){type, sv, lx->line};
}

static scm_token_t scm_generate_token(
    scm_lexer_t* lx, scm_lexer_nfa_state_t nfa_state, u32 pos_start, u32 pos_end)
{
    switch (nfa_state) {
        case SCM_LEXER_NFA_QUASIQUOTE_A:
            return scm_create_token(lx, SCM_TOKEN_QUASIQUOTE, pos_start, pos_end);

        case SCM_LEXER_NFA_QUOTE_A:
            return scm_create_token(lx, SCM_TOKEN_QUOTE, pos_start, pos_end);

        case SCM_LEXER_NFA_LP_A:
            return scm_create_token(lx, SCM_TOKEN_LPAREN, pos_start, pos_end);

        case SCM_LEXER_NFA_RP_A:
            return scm_create_token(lx, SCM_TOKEN_RPAREN, pos_start, pos_end);

        case SCM_LEXER_NFA_IDENTIFIER_A:
            return scm_create_token(lx, SCM_TOKEN_IDENTIFIER, pos_start, pos_end);

        case SCM_LEXER_NFA_LITERAL_NUMBER_A:
            return scm_create_token(lx, SCM_TOKEN_LITERAL_NUMBER, pos_start, pos_end);

        case SCM_LEXER_NFA_LITERAL_STRING_A:
            return scm_create_token(lx, SCM_TOKEN_LITERAL_STRING, pos_start, pos_end);

        default:
            return scm_create_token(lx, SCM_TOKEN_UNKNOWN, 0, pos_end);
    }
}

scm_token_t scm_lexer_next_token(scm_lexer_t* lx)
{
    u32 pos_start;
    u32 pos_end;
    nfa_conclusion_t conclusion;

    while (is_whitespace(lx->src[lx->pos])) {
        if (lx->src[lx->pos] == '\n') {
            lx->line++;
        }
        lx->pos++;
    }

    if (lx->pos >= lx->len) {
        return scm_create_token(lx, SCM_TOKEN_EOF, lx->pos, lx->pos);
    }

    nfa_restart(&lx->nfa, SCM_LEXER_NFA_START);

    pos_start = lx->pos;

    conclusion = nfa_process(&lx->nfa, lx->src[lx->pos]);
    while (conclusion == NFA_CONTINUE) {
        SCM_LEXER_SLEEP;
        lx->pos++;
        conclusion = nfa_process(&lx->nfa, lx->src[lx->pos]);
    }
    SCM_LEXER_SLEEP;

    pos_end = lx->pos;

    lx->pos++;

    switch (conclusion) {
        case NFA_ACCEPT:
            return scm_generate_token(lx, lx->nfa.state_current, pos_start, pos_end);
        case NFA_REJECT:
        default:
            return scm_create_token(lx, SCM_TOKEN_UNKNOWN, pos_start, pos_end);
    }
}

void scm_token_print(scm_token_t* tkn, bool only_token_type)
{
    if (tkn == NULL) {
        printf("Null token\n");
        return;
    }
    printf("%s ", token_type_to_str(tkn->type));

    if (!only_token_type)
        sv_print(&tkn->sv);
}

void scm_lexer_init(scm_lexer_t* lx, const char* filename, const char* src, u32 len)
{
    lx->filename = filename;
    lx->src = src;
    lx->len = len;

    lx->pos = 0;
    lx->line = 0;
    lx->has_error = 0;

    if (da_size(&lx->nfa.states) == SCM_LEXER_NFA_NUM_STATES)
    {
        printf("nfa already initialized\n");
        return;
    }

    nfa_init(&lx->nfa, SCM_LEXER_NFA_NUM_STATES, &nfa_state_to_str, lx);

    // STATES
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_START, NFA_CONTINUE, NULL, NULL);

    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_QUOTE_0, NFA_CONTINUE, NULL, &scm_lexer_epsilon_cb);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_QUASIQUOTE_0, NFA_CONTINUE, NULL, &scm_lexer_epsilon_cb);

    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_LP_A, NFA_ACCEPT, NULL, NULL);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_RP_A, NFA_ACCEPT, NULL, NULL);

    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_IDENTIFIER_0, NFA_CONTINUE, NULL, &scm_lexer_epsilon_cb);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_LITERAL_NUMBER_0, NFA_CONTINUE, NULL, &scm_lexer_epsilon_cb);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_0, NFA_CONTINUE, NULL, NULL);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_1, NFA_CONTINUE, NULL, NULL);

    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_QUOTE_A, NFA_ACCEPT, NULL, NULL);
    nfa_configure_state(&lx->nfa, SCM_LEXER_NFA_QUASIQUOTE_A, NFA_ACCEPT, NULL, NULL);

    // TRANSITIONS
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, "'", SCM_LEXER_NFA_QUOTE_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, "(", SCM_LEXER_NFA_LP_A);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, ")", SCM_LEXER_NFA_RP_A);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, "`", SCM_LEXER_NFA_QUASIQUOTE_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, SCM_IDENTIFIERS_BASE, SCM_LEXER_NFA_IDENTIFIER_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, SCM_NUMBERS, SCM_LEXER_NFA_LITERAL_NUMBER_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_START, SCM_NUMBERS, SCM_LEXER_NFA_LITERAL_STRING_0);

    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_IDENTIFIER_0, SCM_IDENTIFIERS_ALL, SCM_LEXER_NFA_IDENTIFIER_0);

    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_NUMBER_0, SCM_LETTERS, SCM_LEXER_NFA_IDENTIFIER_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_NUMBER_0, SCM_NUMBERS, SCM_LEXER_NFA_LITERAL_NUMBER_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_NUMBER_0, SCM_WHITESPACE, SCM_LEXER_NFA_LITERAL_NUMBER_A);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_0, SCM_EOS, SCM_LEXER_NFA_INVALID_R);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_0, SCM_LITERAL_STRING, SCM_LEXER_NFA_LITERAL_STRING_0);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_0, SCM_LITERAL_STRING_ESCAPE, SCM_LEXER_NFA_LITERAL_STRING_1);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_0, SCM_QUOTATION, SCM_LEXER_NFA_LITERAL_STRING_A);

    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_LITERAL_STRING_1, SCM_LITERAL_STRING_VALID_ESCAPED, SCM_LEXER_NFA_LITERAL_STRING_1);

    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_QUASIQUOTE_0, SCM_LETTERS_NUMBERS, SCM_LEXER_NFA_QUASIQUOTE_A);
    // nfa_add_transition(&lx->nfa, SCM_LEXER_NFA_QUOTE_0, SCM_LETTERS_NUMBERS, SCM_LEXER_NFA_QUOTE_A);

    nfa_generated_register_transitions(&lx->nfa);

    // printf("added %d states\n", SCM_LEXER_NFA_NUM_STATES);
}

/*
static scm_token_t lex_number(scm_lexer_t* lx)
{
    u32 pos_start = lx->pos;
    while (lx->pos < lx->len && is_digit(lx->src[lx->pos])) {
        lx->pos++;
    }
    return scm_create_token(lx, SCM_TOKEN_LITERAL_NUMBER, lx->pos - pos_start);
}

static scm_token_t lex_identifier(scm_lexer_t* lx)
{
    u32 pos_start = lx->pos;
    while (lx->pos < lx->len && is_identifier_char(lx->src[lx->pos])) {
        lx->pos++;
    }
    return scm_create_token(lx, SCM_TOKEN_IDENTIFIER, lx->pos - pos_start);
}

static scm_token_t lex_lparen(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_LPAREN, 1);
}

static scm_token_t lex_rparen(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_RPAREN, 1);
}

static scm_token_t lex_quote(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_QUOTE, 1);
}

static scm_token_t lex_quasiquote(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_QUASIQUOTE, 1);
}

static scm_token_t lex_comma(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_COMMA, 1);
}

static scm_token_t lex_atsign(scm_lexer_t* lx)
{
    lx->pos++;
    return scm_create_token(lx, SCM_TOKEN_ATSIGN, 1);
}
*/
