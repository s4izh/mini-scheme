#ifndef __SCM_TYPES_H__
#define __SCM_TYPES_H__

#include "rc.h"
#include "types.h"
#include "scm_parser.h"

typedef struct _scm_type_bool_t scm_type_bool_t;
typedef struct _scm_type_num_t scm_type_num_t;
typedef struct _scm_type_str_t scm_type_str_t;
typedef struct _scm_type_function_t scm_type_function_t;
typedef struct _scm_type_function_builtin_t scm_type_function_builtin_t;
typedef struct _scm_type_function_sexpr_t scm_type_function_sexpr_t;
typedef struct _scm_type_list_t scm_type_list_t;
typedef struct _scm_type_t scm_type_t;

typedef scm_result_t (*scm_builtin_func_t) (void*, scm_type_function_builtin_t*, da_scm_ast_sexpr_ptr*);

struct _scm_type_bool_t {
    bool value;
};

struct _scm_type_num_t {
    f64 value;
};

struct _scm_type_str_t {
    char* value;
    int len;
    bool ref;
};

struct _scm_type_list_t {
    scm_ast_sexpr_t* sexpr;
};

struct _scm_type_quoted_list_t {
    scm_ast_sexpr_t* sexpr;
};

struct _scm_type_function_sexpr_t {
    da_token_ptr arg_identifiers;
    scm_ast_sexpr_t* sexpr;
};

typedef enum {
    CMP_LT,
    CMP_LEQ,
    CMP_GT,
    CMP_GEQ,
    CMP_EQ,
    CMP_NONE,
} cmp_op_t;

typedef enum {
    OP_SUM,
    OP_SUB,
    OP_MUL,
    OP_NONE,
} num_op_t;

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
        SCM_TYPE_BOOLEAN,
        SCM_TYPE_NUMBER,
        SCM_TYPE_STRING,
        SCM_TYPE_LIST,
        SCM_TYPE_FUNCTION,
    } type;
    union {
        scm_type_bool_t boolean;
        scm_type_num_t number;
        scm_type_str_t string;
        scm_type_list_t list;
        scm_type_function_t function;
    } data;

    RC_DECLARE;
};

void scm_types_fill_from_list(scm_type_t* type, scm_ast_sexpr_t* list_sexpr);

void scm_types_fill_from_atom_literal(scm_type_t* type, scm_ast_sexpr_t* atom_sexpr);

void scm_types_print(scm_type_t* type);

bool scm_type_to_cbool(scm_type_t* type);

#endif // !__SCM_TYPES_H__
