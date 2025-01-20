#include "scm_runtime.h"
#include "rc.h"
#include "scm_lexer.h"
#include "scm_parser.h"
#include "ds.h"
#include "scm_resources.h"
#include "scm_result.h"
#include "scm_types.h"
#include "scm_log.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static scm_binding_t* scm_runtime_create_binding(scm_runtime_t* runtime)
{
    return malloc(sizeof(scm_binding_t));
}

static inline scm_result_t scm_builtin_define_constant(scm_runtime_t* runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    scm_result_t res;
    u32 num_args = da_size(sexprs) - 1;
    scm_binding_t* binding = NULL;

    binding = scm_runtime_create_binding(runtime);

    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    // just one sexpr can be evaluated as the constant
    if (num_args > 2) {
        err = "define constant takes the result of evaluating just one expr";
        res = SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), NULL);
        goto error;
    }

    scm_ast_sexpr_t* second = da_at(sexprs, 2);

    if (second->type == SCM_AST_ATOM 
        && second->data.atom.token->type != SCM_TOKEN_IDENTIFIER) {
        SCM_DEBUG("constant from an atom");
        scm_type_t* type = scm_resources_alloc_type(runtime->resources);
        scm_types_fill_from_atom_literal(type, second);
        binding->token = first->data.atom.token;
        binding->type = type;
    } else {
        res = scm_runtime_eval(runtime, second);
        if (SCM_RESULT_IS_ERR(res)) {
            goto error;
        }
        if (res.data.ok.type == SCM_OK_VOID) {
            err = "should evaluate to a type";
            res = SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), NULL);
            goto error;
        }
        binding->token = first->data.atom.token;
        binding->type = res.data.ok.data;
    }
    scm_runtime_binding_add(runtime, binding);
    return SCM_RESULT_OK(SCM_OK_VOID, NULL);

error:
    RC_UNREF(binding);
    return res;
}

static inline scm_result_t scm_builtin_define_function(scm_runtime_t* runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    scm_result_t res;
    u32 num_args = da_size(sexprs) - 1;
    scm_binding_t* binding = NULL;

    binding = scm_runtime_create_binding(runtime);

    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    if ((da_size(sexprs) - 1) < 2) {
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC,
            "function should at least provide a expression"
            "for the name and arguments and another "
            "for the function itself",
            scm_ast_sexpr_token(first), NULL);
    }

    scm_ast_sexpr_t* second = da_at(sexprs, 2);

    if (da_size(&first->data.list.sexprs) == 1) {
        SCM_DEBUG("registering a non args function");
        da_scm_ast_sexpr_ptr name_args_sexpr = first->data.list.sexprs;
        scm_ast_sexpr_t* name = da_at(&name_args_sexpr, 0);

        if (name->type != SCM_AST_ATOM)
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                "function name should be an atom",
                scm_ast_sexpr_token(name), NULL);

        if (name->data.atom.token->type != SCM_TOKEN_IDENTIFIER)
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                "function name should be an identifier",
                scm_ast_sexpr_token(name), NULL);

        scm_ast_sexpr_t* args = da_at(&name_args_sexpr, 2);

        scm_type_t* type = scm_resources_alloc_type(runtime->resources);

        type->type = SCM_TYPE_FUNCTION;
        type->data.function.type = SCM_TYPE_FUNCTION_SEXPR;

        type->data.function = (scm_type_function_t) {
            .type = SCM_TYPE_FUNCTION_SEXPR,
            .min_args = 0,
            .max_args = -1,
            .data.function.sexpr = second,
            .data.function.arg_identifiers = (da_token_ptr) { NULL, 0, 0},
        };

        // scm_ast_sexpr_print(second);
        // printf("\n");

        // RC_REF(name->data.atom.token);
        binding->token = name->data.atom.token;
        binding->type = type;
        scm_runtime_binding_add(runtime, binding);

        return SCM_RESULT_OK(SCM_OK_VOID, NULL);

    } else {
        SCM_DEBUG("registering an args function");
        da_scm_ast_sexpr_ptr name_args_sexpr = first->data.list.sexprs;
        scm_ast_sexpr_t* name = da_at(&name_args_sexpr, 0);

        if (name->type != SCM_AST_ATOM)
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                "function name should be an atom",
                scm_ast_sexpr_token(name), NULL);

        if (name->data.atom.token->type != SCM_TOKEN_IDENTIFIER)
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                "function name should be an identifier",
                scm_ast_sexpr_token(name), NULL);

        string_view_t* func_name = &name->data.atom.token->sv;

        da_token_ptr arg_identifiers;
        da_init(&arg_identifiers);

        // scm_ast_sexpr_print(first);

        for (u32 i = 1; i < da_size(&name_args_sexpr); ++i)
        {
            scm_ast_sexpr_t* arg = da_at(&name_args_sexpr, i);
            if (arg->type != SCM_AST_ATOM)
                return SCM_RESULT_ERR_FROM_TOKEN(
                        SCM_ERR_SEMANTIC,
                        "function arg should be an atom",
                        scm_ast_sexpr_token(name), NULL);

            if (arg->data.atom.token->type != SCM_TOKEN_IDENTIFIER)
                return SCM_RESULT_ERR_FROM_TOKEN(
                        SCM_ERR_SEMANTIC,
                        "function arg should be an identifier",
                        scm_ast_sexpr_token(name), NULL);

            string_view_t* arg_name = &arg->data.atom.token->sv;

            // RC_REF(arg->data.atom.token);
            da_append(&arg_identifiers, arg->data.atom.token);

            SCM_DEBUG("function: %.*s, registered argument: %.*s",
                (int)func_name->len, func_name->data,
                (int)arg_name->len, arg_name->data);
        }

        scm_type_t* type = scm_resources_alloc_type(runtime->resources);

        type->type = SCM_TYPE_FUNCTION;
        type->data.function.type = SCM_TYPE_FUNCTION_SEXPR;

        type->data.function = (scm_type_function_t) {
            .type = SCM_TYPE_FUNCTION_SEXPR,
            .min_args = 0,
            .max_args = -1,
            .data.function.sexpr = second,
            .data.function.arg_identifiers = arg_identifiers,
        };

        // RC_REF(name->data.atom.token);
        binding->token = name->data.atom.token;
        binding->type = type;

        scm_runtime_binding_add(runtime, binding);

        return SCM_RESULT_OK(SCM_OK_VOID, NULL);

        // takes args
    }
    return SCM_RESULT_ERR(SCM_ERR_TODO, "function definition unimplemented", NULL, 0, 0, NULL)

error:
    RC_UNREF(binding);
    return res;
}

static scm_result_t scm_builtin_define(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    scm_result_t res;
    // scm_binding_t* binding = NULL;

    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    if (first->type == SCM_AST_ATOM) {
        SCM_DEBUG("defining a constant");
        return scm_builtin_define_constant(runtime, sexprs);
    } else if (first->type == SCM_AST_LIST) {
        SCM_DEBUG("defining a function");
        return scm_builtin_define_function(runtime, sexprs);
    } else {
        asprintf(
            &err,
            "not expecting a %s after define, expecting an identifier or a list",
            scm_ast_sexpr_type_to_str(first));
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), free);
    }
    return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

static scm_result_t scm_builtin_if(scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    scm_result_t res;
    char* err;

    scm_ast_sexpr_t* cond = da_at(sexprs, 1);
    scm_ast_sexpr_t* true_cond = da_at(sexprs, 2);
    scm_ast_sexpr_t* false_cond = NULL;

    if (da_size(sexprs) == 4)
        false_cond = da_at(sexprs, 3);

    res = scm_runtime_eval(runtime, cond);
    if (SCM_RESULT_IS_ERR(res))
        return res;

    if (res.data.ok.type == SCM_OK_VOID) {
        err = "should evaluate to a type";
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(cond), NULL);
    }

    scm_type_t* type = res.data.ok.data;

    if (scm_type_to_cbool(type)) {
        return scm_runtime_eval(runtime, true_cond);
    } else if (false_cond != NULL) {
        return scm_runtime_eval(runtime, false_cond);
    }

    return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

static scm_result_t scm_builtin_let(scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    scm_result_t res;
    char* err;
    scm_environment_t env = (scm_environment_t){.bindings = {NULL, 0, 0}};

    scm_ast_sexpr_t* bindings = da_at(sexprs, 1);

    if (bindings->type != SCM_AST_LIST) {
        err = "should be a list of bindings";
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_GRAMMAR, err, scm_ast_sexpr_token(bindings), NULL);
    }

    for (u32 i = 0; i < da_size(&bindings->data.list.sexprs); ++i) {
        scm_ast_sexpr_t* binding_sexpr = da_at(&bindings->data.list.sexprs, i);
        if (binding_sexpr->type != SCM_AST_LIST) {
            err = "a binding should be a list";
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_GRAMMAR, err, scm_ast_sexpr_token(bindings), NULL);
        }

        if (da_size(&binding_sexpr->data.list.sexprs) != 2) {
            err = "a binding should be a list of size 2";
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_GRAMMAR, err, scm_ast_sexpr_token(bindings), NULL);
        }
        scm_ast_sexpr_t* binding_name = da_at(&binding_sexpr->data.list.sexprs, 0);
        if (binding_name->type != SCM_AST_ATOM &&
            binding_name->data.atom.token->type != SCM_TOKEN_IDENTIFIER) {
            err = "first element of the binding must be an identifier";
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_GRAMMAR, err, scm_ast_sexpr_token(bindings), NULL);
        }

        scm_ast_sexpr_t* binding_expr = da_at(&binding_sexpr->data.list.sexprs, 1);

        res = scm_runtime_eval(runtime, binding_expr);
        if (SCM_RESULT_IS_ERR(res))
            return res;

        if (res.data.ok.type != SCM_OK_TYPE) {
            err = "binding should evaluate to a type";
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_GRAMMAR, err, scm_ast_sexpr_token(bindings), NULL);
        }

        scm_binding_t* binding = scm_runtime_create_binding(runtime);
        binding->token = binding_name->data.atom.token;
        binding->type  = res.data.ok.data;

        scm_environment_binding_add(&env, binding);
    }

    scm_runtime_push_environment(runtime, env);

    for (u32 i = 2; i < da_size(sexprs); ++i) {
        res = scm_runtime_eval(runtime, da_at(sexprs, i));
        if (SCM_RESULT_IS_ERR(res))
            return res;
    }

    scm_runtime_pop_environment(runtime);

    return res;
}

static void scm_builtin_cond(void* _runtime, void** args, u32 num)
{
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;
}

static inline scm_result_t scm_builtin_op_cmp(
    scm_runtime_t *runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs, cmp_op_t cmp)
{
    char* err;
    u32 num_args = da_size(sexprs) - 1;
    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    bool cmp_result = true;
    f64 cmp_value;

    // start at 1 skip the call and the first number
    for (u32 i = 1; i < da_size(sexprs); ++i) {
        scm_ast_sexpr_t* sexpr = da_at(sexprs, i);
        scm_result_t res = scm_runtime_eval(runtime, sexpr);
        if (SCM_RESULT_IS_ERR(res))
            return res;

        if (res.data.ok.type != SCM_OK_TYPE) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to int type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }

        scm_type_t* type = res.data.ok.data;
        if (type->type != SCM_TYPE_NUMBER) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to int type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }

        if (i == 1) {
            cmp_value = type->data.number.value;
        } else {
            if (cmp == CMP_LT && cmp_value >= type->data.number.value) {
                cmp_result = false;
            } else if (cmp == CMP_LEQ && cmp_value > type->data.number.value) {
                cmp_result = false;
            } else if (cmp == CMP_GT && cmp_value <= type->data.number.value) {
                cmp_result = false;
            } else if (cmp == CMP_GEQ && cmp_value < type->data.number.value) {
                cmp_result = false;
            } else if (cmp == CMP_EQ && cmp_value != type->data.number.value) {
                cmp_result = false;
            }
        }

        if (cmp_result == false)
            break;
    }
    scm_type_t* type = scm_resources_alloc_type(runtime->resources);
    type->type = SCM_TYPE_BOOLEAN;
    type->data.boolean.value = cmp_result;

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

static inline scm_result_t scm_builtin_op_math(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs, num_op_t op)
{
    u32 num_args = da_size(sexprs) - 1;
    // if (num_args == 0) {
    //     return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
    //             "requires at least one argument",
    //             scm_ast_sexpr_token(da_at(sexprs, 0)), NULL);
    // }

    f64 result = (op == OP_SUM) ? 0.0 :
                 (op == OP_MUL) ? 1.0 : 0.0;

    for (u32 i = 1; i < da_size(sexprs); ++i) {
        scm_ast_sexpr_t* sexpr = da_at(sexprs, i);
        scm_result_t res = scm_runtime_eval(runtime, sexpr);
        if (SCM_RESULT_IS_ERR(res))
            return res;

        if (res.data.ok.type != SCM_OK_TYPE) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to a number type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }

        scm_type_t* type = res.data.ok.data;
        if (type->type != SCM_TYPE_NUMBER) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to a number type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }

        f64 value = type->data.number.value;

        if (op == OP_SUM) {
            result += value;
        } else if (op == OP_SUB) {
            result = (i == 1) ? value : (result - value);
            if (num_args == 1) {
                result = -value;
            }
        } else if (op == OP_MUL) {
            result *= value;
        }
    }

    scm_type_t* type = scm_resources_alloc_type(runtime->resources);
    type->type = SCM_TYPE_NUMBER;
    type->data.number.value = result;

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

static scm_result_t scm_builtin_op_sum(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_math(runtime, func, sexprs, OP_SUM);
}

static scm_result_t scm_builtin_op_sub(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_math(runtime, func, sexprs, OP_SUB);
}

static scm_result_t scm_builtin_op_mul(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_math(runtime, func, sexprs, OP_MUL);
}


static scm_result_t scm_builtin_op_lt(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_cmp(runtime, func, sexprs, CMP_LT);
}

static scm_result_t scm_builtin_op_leq(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_cmp(runtime, func, sexprs, CMP_LEQ);
}

static scm_result_t scm_builtin_op_gt(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_cmp(runtime, func, sexprs, CMP_GT);
}

static scm_result_t scm_builtin_op_geq(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_cmp(runtime, func, sexprs, CMP_GEQ);
}

static scm_result_t scm_builtin_op_eq(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    return scm_builtin_op_cmp(runtime, func, sexprs, CMP_EQ);
}

static scm_result_t scm_builtin_list_car(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
    scm_ast_sexpr_t* sexpr = da_at(sexprs, 0);

    if (sexpr->type != SCM_AST_LIST) {
        return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                "only can car on a list",
                scm_ast_sexpr_token(sexpr), NULL);
    }

    scm_type_t* type = scm_resources_alloc_type(runtime->resources);

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

static scm_result_t scm_builtin_list_cdr(
    scm_runtime_t* runtime, scm_type_function_builtin_t* func, da_scm_ast_sexpr_ptr* sexprs)
{
}

static void scm_runtime_add_builtin_binding(
    scm_runtime_t* runtime, const char* name, scm_builtin_func_t func,
    u32 min_args, u32 max_args)
{
    scm_environment_t* env = &da_at(&runtime->environments, 0);
    scm_binding_t* binding = scm_resources_alloc_binding(runtime->resources);

    scm_token_t* fake_token = scm_resources_alloc_token(runtime->resources);
    *fake_token = (scm_token_t) {
        .type = SCM_TOKEN_IDENTIFIER,
        .sv = (string_view_t) {
            name,
            strlen(name),
        },
        .line = -1,
    };

    scm_type_t* type = scm_resources_alloc_type(runtime->resources);

    binding->type = type;
    binding->type->type = SCM_TYPE_FUNCTION;
    binding->token = fake_token;

    binding->type->data.function = (scm_type_function_t){
        .type = SCM_TYPE_FUNCTION_BUILTIN,
        .min_args = min_args,
        .max_args = max_args,
        .data.builtin.func = func,
    };

    da_append(&env->bindings, binding);
}

void scm_runtime_init(
    scm_runtime_t* runtime, scm_resources_t* resources, scm_runtime_mode_t mode)
{
    runtime->resources = resources;
    runtime->mode = mode;

    da_init(&runtime->environments);
    scm_runtime_push_environment(runtime, (scm_environment_t){.bindings = {NULL, 0, 0}});

    // clang-format off
    scm_runtime_add_builtin_binding(runtime, "define", (scm_builtin_func_t)&scm_builtin_define, 2, -1);
    scm_runtime_add_builtin_binding(runtime, "if", (scm_builtin_func_t)&scm_builtin_if, 2, 3);
    scm_runtime_add_builtin_binding(runtime, "let",(scm_builtin_func_t)&scm_builtin_let, 2, -1);
    // scm_runtime_add_builtin_binding(runtime, "cond", &scm_builtin_define, 2, -1);
    scm_runtime_add_builtin_binding(runtime, "+", (scm_builtin_func_t)&scm_builtin_op_sum, 0, -1);
    scm_runtime_add_builtin_binding(runtime, "-", (scm_builtin_func_t)&scm_builtin_op_sub, 1, -1);
    scm_runtime_add_builtin_binding(runtime, "*", (scm_builtin_func_t)&scm_builtin_op_mul, 0, -1);

    scm_runtime_add_builtin_binding(runtime, "<", (scm_builtin_func_t)&scm_builtin_op_lt, 0, -1);
    scm_runtime_add_builtin_binding(runtime, "<=", (scm_builtin_func_t)&scm_builtin_op_leq, 0, -1);
    scm_runtime_add_builtin_binding(runtime, ">", (scm_builtin_func_t)&scm_builtin_op_gt, 0, -1);
    scm_runtime_add_builtin_binding(runtime, ">=", (scm_builtin_func_t)&scm_builtin_op_geq, 0, -1);
    scm_runtime_add_builtin_binding(runtime, "=", (scm_builtin_func_t)&scm_builtin_op_eq, 0, -1);

    scm_runtime_add_builtin_binding(runtime, "car", (scm_builtin_func_t)&scm_builtin_list_car, 1, 1);
    scm_runtime_add_builtin_binding(runtime, "cdr", (scm_builtin_func_t)&scm_builtin_list_cdr, 1, 1);
    // clang-format on
}

scm_binding_t* scm_runtime_binding_lookup(
    scm_runtime_t* runtime, string_view_t identifier)
{
    // we start looking from the last environment created
    // (the most nested one)
    for (i32 i = da_size(&runtime->environments) - 1; i >= 0; i--) {
        scm_environment_t* env = &da_at(&runtime->environments, i);
        for (u32 j = 0; j < da_size(&env->bindings); j++) {
            scm_binding_t* binding = da_at(&env->bindings, j);
            if (sv_equal(&binding->token->sv, &identifier)) {
                return binding;
            }
        }
    }
    return NULL;
}

void scm_runtime_push_environment(scm_runtime_t* runtime, scm_environment_t environment)
{
    da_append(
        &runtime->environments,
        environment);

    // ((scm_environment_t){.bindings = {NULL, 0, 0}}));
}

void scm_runtime_binding_add(scm_runtime_t* runtime, scm_binding_t* binding)
{
    u32 last = da_size(&runtime->environments) - 1;
    scm_environment_t* env = &da_at(&runtime->environments, last);
    scm_environment_binding_add(env, binding);
}

void scm_environment_binding_add(scm_environment_t* env, scm_binding_t* binding)
{
    da_binding_ptr* bindings = &env->bindings;
    for (u32 i = 0; i < da_size(bindings); ++i) {
        scm_binding_t* binded_binding = da_at(bindings, i);
        if (sv_equal(&binded_binding->token->sv, &binding->token->sv)) {
            // TODO: destroy the binding
            da_at(bindings, i) = binding;
            SCM_DEBUG("overwritten binding %.*s", sv_format(&binding->token->sv));
            return;
        }
    }
    da_append(bindings, binding);
    SCM_DEBUG("appended binding to %p", bindings);
}

void scm_binding_free(scm_binding_t* binding)
{
    // switch (binding->internal.type) {
    //     case SCM_BINDING_FUNCTION:
    //         // free(binding->data.function.sexpr);
    //         break;
    //     case SCM_BINDING_LIST:
    //         // free(binding->data.list.sexpr);
    //         break;
    //     case SCM_BINDING_BUILTIN:
    //         break;
    // }
}

void scm_runtime_pop_environment(scm_runtime_t* runtime)
{
    scm_environment_t* env =
        &da_at(&runtime->environments, da_size(&runtime->environments) - 1);
    for (u32 i = 0; i < da_size(&env->bindings); i++) {
        scm_binding_t* binding = da_at(&env->bindings, i);
        scm_binding_free(binding);
    }
    da_free(&env->bindings);
    da_remove_last(&runtime->environments);
}

static scm_result_t scm_runtime_eval_ast_atom(
    scm_runtime_t* runtime, scm_ast_sexpr_t* atom_sexpr)
{
    assert(atom_sexpr != NULL);
    char* err;
    scm_ast_atom_t* atom = &atom_sexpr->data.atom;
    assert(atom != NULL);

    switch (atom->token->type) {
        case SCM_TOKEN_IDENTIFIER: {
            string_view_t identifier = atom->token->sv;
            scm_binding_t* binding = scm_runtime_binding_lookup(runtime, identifier);

            if (binding == NULL) {
                asprintf(&err, "undefined symbol %.*s", (int)identifier.len, identifier.data);
                return SCM_RESULT_ERR_FROM_TOKEN(
                    SCM_ERR_SEMANTIC, err,
                    scm_ast_sexpr_token(atom_sexpr), free);
            }

            return SCM_RESULT_OK(SCM_OK_TYPE, binding->type);
        }
        case SCM_TOKEN_LITERAL_NUMBER: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            type->type = SCM_TYPE_NUMBER;
            type->data.number.value = sv_toi(&atom->token->sv);
            return SCM_RESULT_OK(SCM_OK_TYPE, type);
        }
        case SCM_TOKEN_LITERAL_STRING: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            type->type = SCM_TYPE_STRING;
            type->data.string.value = (char*)atom->token->sv.data;
            type->data.string.len = atom->token->sv.len;
            type->data.string.ref = true;
            return SCM_RESULT_OK(SCM_OK_TYPE, type);
        }
        default:
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC, "evaluating a non atom expresion as atom",
                scm_ast_sexpr_token(atom_sexpr), NULL);
    }

    return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

static scm_result_t scm_runtime_eval_ast_quote(
    scm_runtime_t* runtime, scm_ast_sexpr_t* quote)
{
    assert(quote != NULL);
    assert(quote->data.quote.sexpr != NULL);

    scm_ast_sexpr_t* sexpr = quote->data.quote.sexpr;
    assert(sexpr != NULL);

    scm_token_t* token = quote->data.quote.quote;

    switch (sexpr->type) {
        case SCM_AST_QUOTE: {
            return SCM_RESULT_ERR(
                SCM_ERR_TODO, "eval quote", NULL, token->line, 0, NULL)
        };
        case SCM_AST_ATOM: {
            printf("TODO!!!!!!!!!!\n");
        };
        case SCM_AST_LIST: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            scm_types_fill_from_list(type, sexpr);
            return SCM_RESULT_OK(SCM_OK_TYPE, type);
        };
        default:
            __builtin_unreachable();
    }
}

static scm_result_t scm_runtime_eval_ast_list(
    scm_runtime_t* runtime, scm_ast_sexpr_t* list)
{
    char* err = NULL;
    scm_result_t res;

    assert(list != NULL);

    da_scm_ast_sexpr_ptr* sexprs = &list->data.list.sexprs;

    if (da_size(sexprs) == 0) {
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, "can't evaluate empty list",
            scm_ast_sexpr_token(list), NULL);
    }

    scm_ast_sexpr_t* first = da_at(sexprs, 0);

    // TODO lambdas
    // if (first->type == SCM_AST_LIST) {
    // }

    if (first->type != SCM_AST_ATOM &&
        first->data.atom.token->type != SCM_TOKEN_IDENTIFIER) {
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, "can't evaluate a non identifier function",
            scm_ast_sexpr_token(list), NULL);
    }

    string_view_t identifier = first->data.atom.token->sv;

    scm_binding_t* binding = scm_runtime_binding_lookup(runtime, identifier);

    if (binding == NULL) {
        asprintf(&err, "undefined symbol %.*s", (int)identifier.len, identifier.data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    if (binding->type->type != SCM_TYPE_FUNCTION) {
        asprintf(&err, "can't evaluate a non function type '%.*s'", (int)identifier.len, identifier.data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    SCM_DEBUG("found %.*s", (int)identifier.len, identifier.data);

    scm_type_function_t* function = &binding->type->data.function;
    u32 args = da_size(sexprs) - 1;

    if (args < function->min_args)
        asprintf(&err, "args provided %d, min_args = %d", args, function->min_args);

    if (function->max_args != -1 && args > function->max_args)
        asprintf(&err, "args provided %d, max_args = %d", args, function->max_args);

    if (err != NULL) {
        return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                err,
                scm_ast_sexpr_token(first), free);
    }

    if (function->type == SCM_TYPE_FUNCTION_BUILTIN) {
        return function->data.builtin.func(runtime, &function->data.builtin, sexprs);
    } else if (function->type == SCM_TYPE_FUNCTION_SEXPR) {

        da_token_ptr* args = &function->data.function.arg_identifiers;

        scm_environment_t env = (scm_environment_t){.bindings = {NULL, 0, 0}};

        for (u32 i = 0; i < da_size(args); ++i)
        {
            scm_binding_t* binding = scm_resources_alloc_binding(runtime->resources);
            binding->token = da_at(args, i);

            scm_ast_sexpr_t* sexpr = da_at(sexprs, i + 1);

            res = scm_runtime_eval(runtime, sexpr);
            if (SCM_RESULT_IS_ERR(res)) {
                RC_UNREF(binding);
                scm_runtime_pop_environment(runtime);
                return res;
            }
            if (res.data.ok.type == SCM_OK_VOID) {
                RC_UNREF(binding);
                scm_runtime_pop_environment(runtime);
                err = "should evaluate to a type";
                return SCM_RESULT_ERR_FROM_TOKEN(
                        SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(sexpr), NULL);
            }
            binding->type = res.data.ok.data;

            SCM_DEBUG(
                "creating call stack, adding %.*s",
                (int)binding->token->sv.len,
                binding->token->sv.data);

            scm_environment_binding_add(&env, binding);
        }

        scm_runtime_push_environment(runtime, env);

        res = scm_runtime_eval(runtime, function->data.function.sexpr);

        scm_runtime_pop_environment(runtime);

        return res;

    } else {
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, "unknown function type", scm_ast_sexpr_token(first), NULL);
    }

    return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

static scm_result_t scm_runtime_eval_ast_root(
    scm_runtime_t* runtime, scm_ast_sexpr_t* root)
{
    assert(root != NULL);

    scm_result_t res;

    for (u32 i = 0; i < da_size(&root->data.root.sexprs); ++i) {
        res = scm_runtime_eval(runtime, da_at(&root->data.root.sexprs, i));
        if (SCM_RESULT_IS_ERR(res))
            return res;
    }

    return res;
}

scm_result_t scm_runtime_eval(
    scm_runtime_t* runtime, scm_ast_sexpr_t* sexpr)
{
    assert(sexpr != NULL);

    SCM_LOG(LOG_DEBUG, "evaluating:");
    SCM_LOG_IF(LOG_DEBUG, scm_ast_sexpr_print(sexpr));

    switch (sexpr->type) {
        case SCM_AST_ATOM:
            return scm_runtime_eval_ast_atom(runtime, sexpr);
        case SCM_AST_QUOTE:
            return scm_runtime_eval_ast_quote(runtime, sexpr);
        case SCM_AST_LIST:
            return scm_runtime_eval_ast_list(runtime, sexpr);
        case SCM_AST_ROOT:
            return scm_runtime_eval_ast_root(runtime, sexpr);
    }
}

void scm_type_print(scm_type_t* type)
{
    printf("TODO\n");
}
