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
    char *arg_names[SCM_RUNTIME_MAX_BINDING_NAME_SIZE];
    u32  args;
    scm_ast_sexpr_t* sexpr;
} scm_runtime_function_t;

typedef struct {
    scm_ast_sexpr_t* sexpr;
} scm_runtime_list_t;

typedef struct {
    scm_ast_sexpr_t* sexpr;
} scm_runtime_constant_t;

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

typedef struct {
    da_binding bindings;
} scm_environment_t;

typedef enum {
    SCM_RUNTIME_MODE_REPL,
    SCM_RUNTIME_MODE_FILE,
} scm_runtime_mode_t;

DA_DEFINE(scm_environment_t, da_environment);

typedef struct {
    da_environment environments;
    scm_runtime_mode_t mode;
    scm_resources_t* resources;
} scm_runtime_t;

void scm_runtime_init(scm_runtime_t* runtime, scm_resources_t* resources, scm_runtime_mode_t mode);

scm_binding_t* scm_runtime_lookup_binding(scm_runtime_t* runtime, const char* name);

void scm_runtime_push_environment(scm_runtime_t* runtime);

void scm_runtime_pop_environment(scm_runtime_t* runtime);

void scm_binding_free(scm_binding_t* binding);

#endif // __SCM_RUNTIME_H__
