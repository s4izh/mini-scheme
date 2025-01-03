#ifndef __SCM_RUNTIME_H__
#define __SCM_RUNTIME_H__

#include "ds.h"
#include "scm_parser.h"
#include "scm_builtin.h"

#define SCM_RUNTIME_MAX_BINDING_NAME_SIZE 16

typedef enum {
    SCM_BINDING_FUNCTION,
    SCM_BINDING_LIST,
    SCM_BINDING_BUILTIN,
} scm_binding_type_t;

typedef struct {
    char *arg_names[SCM_BINDING_MAX_BINDING_SIZE];
    u32  args;
    scm_ast_sexpr_t* sexpr;
} scm_runtime_function_t;

typedef struct {
    scm_binding_type_t type;
    char name[SCM_RUNTIME_MAX_BINDING_NAME_SIZE];

    u32 min_args;
    u32 max_args;

    union {
        scm_runtime_constant_t constant;
        scm_runtime_function_t function;
        scm_runtime_list_t list;
        scm_builtin_t builtin;
    } data;

} scm_binding_t;

DA_DEFINE(scm_binding_t, da_binding);

typedef struct _scm_environment_t scm_environment_t;

struct _scm_environment_t {
    scm_environment_t* parent;
    da_binding bindings;
};

typedef struct {
    scm_lexer_t lexer;
    scm_parser_t parser;
    scm_environment_t env;
} scm_runtime_t;

void scm_runtime_init(scm_runtime_t* runtime);

#endif // __SCM_RUNTIME_H__
