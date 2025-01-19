#ifndef __SCM_TYPES_H__
#define __SCM_TYPES_H__

#include "types.h"
#include "scm_parser.h"

typedef struct _scm_type_num_t scm_type_num_t;
typedef struct _scm_type_str_t scm_type_str_t;
typedef struct _scm_type_function_t scm_type_function_t;
typedef struct _scm_type_function_builtin_t scm_type_function_builtin_t;
typedef struct _scm_type_function_sexpr_t scm_type_function_sexpr_t;
typedef struct _scm_type_list_t scm_type_list_t;
typedef struct _scm_type_t scm_type_t;

typedef scm_result_t (*scm_builtin_func_t) (void*, da_scm_ast_sexpr_ptr*);

struct _scm_type_num_t {
    f64 num;
};

struct _scm_type_str_t {
    char* str;
    int len;
    bool ref;
};

struct _scm_type_list_t {
    scm_ast_sexpr_t* sexpr;
};

struct _scm_type_function_sexpr_t {
    da_token_ptr arg_identifiers;
    scm_ast_sexpr_t* sexpr;
};

struct _scm_type_function_builtin_t {
    scm_builtin_func_t func;
};

struct _scm_type_function_t {
    enum {
        SCM_TYPE_FUNCTION_BUILTIN,
        SCM_TYPE_FUNCTION_SEXPR,
    } type;

    u32 min_args;
    u32 max_args;

    union {
        scm_type_function_builtin_t builtin;
        scm_type_function_sexpr_t function;
    } data;
};

struct _scm_type_t {
    enum {
        SCM_TYPE_NUM,
        SCM_TYPE_STR,
        SCM_TYPE_LIST,
        SCM_TYPE_FUNCTION,
    } type;
    union {
        scm_type_num_t num;
        scm_type_str_t str;
        scm_type_list_t list;
        scm_type_function_t function;
    } data;
};

void scm_types_list_fill(scm_type_t* type, scm_ast_sexpr_t* list_sexpr);

void scm_types_from_atom(scm_type_t* type, scm_ast_sexpr_t* atom_sexpr);

void scm_types_print(scm_type_t* type);

#endif // !__SCM_TYPES_H__
