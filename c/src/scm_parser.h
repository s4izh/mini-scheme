#ifndef __SCM_PARSER_H__
#define __SCM_PARSER_H__

#include "scm_lexer.h"
#include "ds.h"

typedef struct _scm_ast_sexpr_t scm_ast_sexpr_t;

DA_DEFINE(struct _scm_ast_sexpr_t*, da_scm_ast_sexpr_ptr);
DA_DEFINE(struct _scm_ast_sexpr_t, da_scm_ast_sexpr);

typedef enum {
    SCM_AST_ATOM,
    SCM_AST_LIST,
} scm_ast_sexpr_type_t;

typedef struct {
    scm_token_t* token;
} scm_ast_atom_t;

typedef struct {
    scm_token_t* lparen;
    da_scm_ast_sexpr_ptr sexprs;
} scm_ast_list_t;

// s-expr
struct _scm_ast_sexpr_t {
    scm_ast_sexpr_type_t type;
    union {
        scm_ast_atom_t atom;
        scm_ast_list_t list;
    } data;
};

typedef struct {
    scm_ast_sexpr_t* root;
    da_token* tokens;
    u32 pos;
    da_scm_ast_sexpr sexprs; // linearly allocated sexpr
} scm_parser_t;

scm_ast_sexpr_t* scm_parser_run(scm_parser_t* parser, da_token* tokens);

void scm_pretty_print_sexpr(scm_ast_sexpr_t *sexpr);
void scm_pretty_print_sexpr_extra(scm_ast_sexpr_t *sexpr);

#endif //  __SCM_PARSER_H__
