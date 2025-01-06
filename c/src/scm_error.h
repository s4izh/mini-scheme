#ifndef __SCM_ERROR_H__
#define __SCM_ERROR_H__

#include "types.h"

typedef struct {
    enum {
        SCM_ERROR_GENERIC,
        SCM_ERROR_RESOURCE,
        SCM_ERROR_SYNTAX,
        SCM_ERROR_GRAMMAR,
        SCM_ERROR_SEMANTIC,
        SCM_ERROR_RUNTIME,
        SCM_ERROR_TODO,
        SCM_ERROR_NONE,
        SCM_ERROR_NUM_ERRORS,
    } type;
    char* msg;
} scm_error_t;

typedef struct {
    enum {
        SCM_RETURN_ERROR,
        SCM_RETURN_OK_GENERIC,
        SCM_RETURN_SEXPR,
        SCM_RETURN_TOKEN,
    } type;
    union {
        scm_error_t error;
        void* ptr;
        u32 u32;
    } data;
} scm_return_t;

void scm_error_print(scm_error_t* err);

#define SCM_RETURN_ERROR_GENERIC(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_GENERIC, msg } }
#define SCM_RETURN_ERROR_RESOURCE(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_RESOURCE, msg } }
#define SCM_RETURN_ERROR_SYNTAX(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_SYNTAX, msg } }
#define SCM_RETURN_ERROR_GRAMMAR(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_GRAMMAR, msg } }
#define SCM_RETURN_ERROR_SEMANTIC(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_SEMANTIC, msg } }
#define SCM_RETURN_ERROR_RUNTIME(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_RUNTIME, msg } }
#define SCM_RETURN_ERROR_TODO(msg) (scm_return_t) { SCM_RETURN_ERROR, { SCM_ERROR_TODO, msg } }
#define SCM_RETURN_OK() (scm_return_t) { SCM_RETURN_OK_GENERIC, { SCM_ERROR_NONE, NULL } }
#define SCM_RETURN_SEXPR(ptr) (scm_return_t) { SCM_RETURN_SEXPR, { SCM_ERROR_NONE, ptr } }
#define SCM_RETURN_TOKEN(ptr) (scm_return_t) { SCM_RETURN_TOKEN, { SCM_ERROR_NONE, ptr } }

#endif // !__SCM_ERROR_H__
