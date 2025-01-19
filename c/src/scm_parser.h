#ifndef __SCM_PARSER_H__
#define __SCM_PARSER_H__

#include "scm_lexer.h"
#include "scm_resources.h"
#include "ds.h"

typedef struct _scm_ast_sexpr_t scm_ast_sexpr_t;
typedef struct _scm_ast_atom_t scm_ast_atom_t;
typedef struct _scm_ast_list_t scm_ast_list_t;
typedef struct _scm_ast_quote_t scm_ast_quote_t;
typedef struct _scm_ast_root_t scm_ast_root_t;
typedef struct _scm_parser_t scm_parser_t;

DA_DEFINE(struct _scm_ast_sexpr_t*, da_scm_ast_sexpr_ptr);

struct _scm_ast_atom_t {
    scm_token_t* token;
};

struct _scm_ast_list_t {
    scm_token_t* lparen;
    da_scm_ast_sexpr_ptr sexprs;
};

struct _scm_ast_quote_t {
    scm_token_t* quote;
    scm_ast_sexpr_t* sexpr;
};

struct _scm_ast_root_t {
    da_scm_ast_sexpr_ptr sexprs;
};

// sexpr ::= <atom>
//         |  ( <sexpr>* )
//         | '( <sexpr>* )
struct _scm_ast_sexpr_t {
    enum {
        SCM_AST_ATOM,
        SCM_AST_QUOTE,
        // SCM_AST_QUASIQUOTE,
        SCM_AST_LIST,
        SCM_AST_ROOT
    } type;

    union {
        scm_ast_atom_t atom;
        scm_ast_quote_t quote;
        scm_ast_list_t list;
        scm_ast_root_t root;
    } data;
};

struct _scm_parser_t {
    scm_ast_sexpr_t* root;
    da_token_ptr* tokens;
    u32 pos;
    scm_resources_t* resources;
};

void scm_parser_init(scm_parser_t* parser, scm_resources_t* resources);

scm_ast_sexpr_t* scm_parser_run(scm_parser_t* parser, da_token_ptr* tokens);

void scm_ast_sexpr_print_rec(scm_ast_sexpr_t* sexpr, int indent_level);
void scm_ast_sexpr_print(scm_ast_sexpr_t* sexpr);

const char* scm_ast_sexpr_type_to_str(scm_ast_sexpr_t* sexpr);

scm_token_t* scm_ast_sexpr_token(scm_ast_sexpr_t* sexpr);

#endif //  __SCM_PARSER_H__
