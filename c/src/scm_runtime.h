#ifndef __SCM_RUNTIME_H__
#define __SCM_RUNTIME_H__

#include "ds.h"
#include "scm_parser.h"
#include "scm_types.h"

typedef struct _scm_binding_t scm_binding_t;
typedef struct _scm_environment_t scm_environment_t;
typedef struct _scm_runtime_t scm_runtime_t;

DA_DEFINE(scm_environment_t, da_environment);
DA_DEFINE(scm_binding_t*, da_binding_ptr);

struct _scm_binding_t {
    scm_token_t* token;
    scm_type_t type;
};

struct _scm_environment_t {
    da_binding_ptr bindings;
};

typedef enum {
    SCM_RUNTIME_MODE_REPL,
    SCM_RUNTIME_MODE_FILE,
} scm_runtime_mode_t;

struct _scm_runtime_t {
    scm_runtime_mode_t mode;
    da_environment environments;
    scm_resources_t* resources;
};

void scm_runtime_init(scm_runtime_t* runtime, scm_resources_t* resources, scm_runtime_mode_t mode);

scm_binding_t* scm_runtime_binding_lookup(scm_runtime_t* runtime, const char* name, u32 size);

void scm_runtime_binding_add(scm_runtime_t* runtime, scm_binding_t* binding);

void scm_runtime_push_environment(scm_runtime_t* runtime);

void scm_runtime_pop_environment(scm_runtime_t* runtime);

void scm_binding_free(scm_binding_t* binding);

scm_result_t scm_runtime_eval(scm_runtime_t* runtime, scm_ast_sexpr_t* sexpr);

void scm_type_print(scm_type_t* type);


#endif // __SCM_RUNTIME_H__
