#include "scm_lexer.h"
#include <stdio.h>
#include "nfa.h"

#define __STRING_VIEW_IMPLEMENTATION__
#include "sv.h"

void scm_lexer_init(scm_lexer_t* lx, const char* src, u32 len)
{
    lx->src = src;
    lx->len = len;

    lx->pos = 0;
    lx->line = 0;
    lx->has_error = 0;

    nfa_init(&lx->nfa, SCM_LEXER_NFA_NUM_STATES);
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
        case SCM_TOKEN_NUMBER_LITERAL:return "NUMBER_LITERAL";
        case SCM_TOKEN_STRING_LITERAL:return "STRING_LITERAL";
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

static int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static int is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static int is_identifier_char(char c)
{
    return (
        c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        is_digit(c));
}

static scm_token_t scm_create_token(
    scm_lexer_t* lx, scm_token_type_t type, u32 len)
{
    string_view_t sv = {lx->src + lx->pos, len};
    lx->pos += len;
    return (scm_token_t){type, sv, lx->line};
}

static scm_token_t lex_number(scm_lexer_t* lx)
{
    u32 start_pos = lx->pos;
    while (lx->pos < lx->len && is_digit(lx->src[lx->pos])) {
        lx->pos++;
    }
    return scm_create_token(lx, SCM_TOKEN_NUMBER_LITERAL, lx->pos - start_pos);
}

static scm_token_t lex_identifier(scm_lexer_t* lx)
{
    u32 start_pos = lx->pos;
    while (lx->pos < lx->len && is_identifier_char(lx->src[lx->pos])) {
        lx->pos++;
    }
    return scm_create_token(lx, SCM_TOKEN_IDENTIFIER, lx->pos - start_pos);
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

scm_token_t scm_lexer_next_token(scm_lexer_t* lx)
{
    while (is_whitespace(lx->src[lx->pos])) {
        if (lx->src[lx->pos] == '\n') {
            lx->line++;
        }
        lx->pos++;
    }

    u32 start_pos = lx->pos;

#if 0
    nfa_restart(&lx->nfa);

    while (true)
    {
        nfa_process(&lx->nfa, lx->src[lx->pos]);
        if (nfa_ended(&lx->nfa))
        {
            break;
        }
        lx->pos++;
    }

    switch (nfa_state(&lx->nfa))
    {
        case NFA_STATE_ACCEPT:
        {

        }
        break;
        case NFA_STATE_REJECT:
        {

        }
        break;
    }
#endif

    if (lx->pos >= lx->len) {
        return scm_create_token(lx, SCM_TOKEN_EOF, 0);
    }

    // always create tokens for ) and (
    // since they can't be used in identifiers
    char current_char = lx->src[lx->pos];
    if (current_char == '(') {
        return scm_create_token(lx, SCM_TOKEN_LPAREN, 1);
    } else if (current_char == ')') {
        return scm_create_token(lx, SCM_TOKEN_RPAREN, 1);
    }

    // any other character can be part of an identifier
    // including numbers

    lx->has_error = 1;
    return scm_create_token(lx, SCM_TOKEN_UNKNOWN, 1);
}

void scm_print_token(scm_token_t* tkn)
{
    if (tkn == NULL) {
        printf("Null token\n");
        return;
    }
    printf("%s ", token_type_to_str(tkn->type));
    sv_print(&tkn->sv);
    // printf("\n");
}
