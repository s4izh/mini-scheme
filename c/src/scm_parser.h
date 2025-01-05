#ifndef __SCM_PARSER_H__
#define __SCM_PARSER_H__

#include "scm_lexer.h"
#include "scm_resources.h"
#include "ds.h"

typedef struct _scm_ast_sexpr_t scm_ast_sexpr_t;

DA_DEFINE(struct _scm_ast_sexpr_t*, da_scm_ast_sexpr_ptr);
DA_DEFINE(struct _scm_ast_sexpr_t, da_scm_ast_sexpr);

typedef enum {
    SCM_AST_ATOM,
    SCM_AST_QUOTE,
    // SCM_AST_QUASIQUOTE,
    SCM_AST_LIST,
    SCM_AST_ROOT,
} scm_ast_sexpr_type_t;

typedef struct {
    scm_token_t* token;
} scm_ast_atom_t;

typedef struct {
    scm_token_t* lparen;
    da_scm_ast_sexpr_ptr sexprs;
} scm_ast_list_t;

typedef struct {
    scm_token_t* quote;
    scm_ast_sexpr_t* sexpr;
} scm_ast_quote_t;

typedef struct {
    da_scm_ast_sexpr_ptr sexprs;
} scm_ast_root_t;

// sexpr ::= <atom>
//         |  '(' <sexpr>* ')'
struct _scm_ast_sexpr_t {
    scm_ast_sexpr_type_t type;
    union {
        scm_ast_atom_t atom;
        scm_ast_list_t list;
        scm_ast_quote_t quote;
        scm_ast_root_t root;
    } data;
};

// a program is just a list of sexpr
// program ::= <sexpr*>
typedef struct {
    da_scm_ast_sexpr_ptr sexprs;
} scm_program_t;

typedef struct {
    scm_ast_sexpr_t* root;
    da_token_ptr* tokens;
    u32 pos;
    scm_resources_t* resources;
} scm_parser_t;

void scm_parser_init(scm_parser_t* parser, scm_resources_t* resources);
scm_ast_sexpr_t* scm_parser_run(scm_parser_t* parser, da_token_ptr* tokens);

void scm_ast_sexpr_print(scm_ast_sexpr_t *sexpr);

#endif //  __SCM_PARSER_H__
