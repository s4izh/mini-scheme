#include "scm_runtime.h"
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

static void scm_runtime_destroy_binding(scm_runtime_t* runtime, scm_binding_t* binding)
{
    free(binding);
}

static scm_result_t scm_builtin_define(
    scm_runtime_t* runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    scm_result_t res;
    scm_binding_t* binding = NULL;

    u32 num_args = da_size(sexprs) - 1;
    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    if (first->type == SCM_AST_ATOM) {
        SCM_DEBUG("defining a constant");
        // defining a constant
        binding = scm_runtime_create_binding(runtime);

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
            scm_types_from_atom(type, second);
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
    } else if (first->type == SCM_AST_LIST) {
        // defining a function
        if (da_size(&first->data.list.sexprs) == 1) {
            // takes no args
        } else {
            // takes args
        }
        res = SCM_RESULT_ERR(
            SCM_ERR_TODO, "function definition unimplemented", NULL, 0, 0, NULL)
        goto error;
    } else {
        asprintf(
            &err,
            "not expecting a %s after define, expecting an identifier or a list",
            scm_ast_sexpr_type_to_str(first));

        res = SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), free);
        goto error;
    }
    return SCM_RESULT_OK(SCM_OK_VOID, NULL);

error:
    if (binding != NULL)
        scm_runtime_destroy_binding(runtime, binding);
    return res;
}

static void scm_builtin_if(void* _runtime, void** args, u32 num)
{
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;
}

static void scm_builtin_let(void* _runtime, void** args, u32 num)
{
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;
}

static void scm_builtin_cond(void* _runtime, void** args, u32 num)
{
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;
}

static scm_result_t scm_builtin_op_sum(
    scm_runtime_t *runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    u32 num_args = da_size(sexprs) - 1;
    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    f64 sum = 0.0;

    if (first->type == SCM_AST_ATOM) {
    } else if (first->type == SCM_AST_LIST) {
    }

    // start at 1 to skip the call
    for (u32 i = 1; i < da_size(sexprs); ++i) {
        scm_ast_sexpr_t* sexpr = da_at(sexprs, i);
        scm_result_t res = scm_runtime_eval(runtime, sexpr);
        if (SCM_RESULT_IS_ERR(res))
            return res;

        if (res.data.ok.type != SCM_OK_TYPE) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to num type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }

        scm_type_t* type = res.data.ok.data;
        if (type->type != SCM_TYPE_NUM) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to num type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }
        sum += type->data.num.num;
    }
    scm_type_t* type = scm_resources_alloc_type(runtime->resources);
    type->type = SCM_TYPE_NUM;
    type->data.num.num = sum;

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

static scm_result_t scm_builtin_op_sub(
    scm_runtime_t *runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    u32 num_args = da_size(sexprs) - 1;
    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    if (first->type == SCM_AST_ATOM) {
    } else if (first->type == SCM_AST_LIST) {
    }

    f64 sub = 0.0;

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
        if (type->type != SCM_TYPE_NUM) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to int type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }
        if (i == 1) sub = type->data.num.num;
        else sub -= type->data.num.num;
    }
    scm_type_t* type = scm_resources_alloc_type(runtime->resources);
    type->type = SCM_TYPE_NUM;
    type->data.num.num = sub;

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

static scm_result_t scm_builtin_op_mul(
    scm_runtime_t *runtime, da_scm_ast_sexpr_ptr* sexprs)
{
    char* err;
    u32 num_args = da_size(sexprs) - 1;
    scm_ast_sexpr_t* first = da_at(sexprs, 1);

    if (first->type == SCM_AST_ATOM) {
    } else if (first->type == SCM_AST_LIST) {
    }

    f64 mul = 1.0;

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
        if (type->type != SCM_TYPE_NUM) {
            return SCM_RESULT_ERR_FROM_TOKEN(SCM_ERR_SEMANTIC,
                    "should evaluate to int type",
                    scm_ast_sexpr_token(sexpr), NULL);
        }
        mul *= type->data.num.num;
    }
    scm_type_t* type = scm_resources_alloc_type(runtime->resources);
    type->type = SCM_TYPE_NUM;
    type->data.num.num = mul;

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
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
    scm_runtime_push_environment(runtime);

    // clang-format off
    scm_runtime_add_builtin_binding(runtime, "define", &scm_builtin_define, 2, -1);
    // scm_runtime_add_builtin_binding(runtime, "if", &scm_builtin_if, 2, 3);
    // scm_runtime_add_builtin_binding(runtime, "let", &scm_builtin_define, 2, -1);
    // scm_runtime_add_builtin_binding(runtime, "cond", &scm_builtin_define, 2, -1);
    scm_runtime_add_builtin_binding(runtime, "+", &scm_builtin_op_sum, 0, -1);
    scm_runtime_add_builtin_binding(runtime, "-", &scm_builtin_op_sub, 1, -1);
    scm_runtime_add_builtin_binding(runtime, "*", &scm_builtin_op_mul, 0, -1);
    // clang-format on
}

scm_binding_t* scm_runtime_binding_lookup(
    scm_runtime_t* runtime, const char* name, u32 size)
{
    string_view_t lookfor = {name, size};
    // we start looking from the last environment created
    // (the most nested one)
    for (i32 i = da_size(&runtime->environments) - 1; i >= 0; i--) {
        scm_environment_t* env = &da_at(&runtime->environments, i);
        for (u32 j = 0; j < da_size(&env->bindings); j++) {
            scm_binding_t* binding = da_at(&env->bindings, j);
            if (sv_equal(&binding->token->sv, &lookfor)) {
                return binding;
            }
        }
    }
    return NULL;
}

void scm_runtime_push_environment(scm_runtime_t* runtime)
{
    da_append(
        &runtime->environments,
        ((scm_environment_t){.bindings = {NULL, 0, 0}}));

    // this is bad
    // da_init(&da_at(&runtime->environments, da_size(&runtime->environments) -
    // 1).bindings);
}

void scm_runtime_binding_add(scm_runtime_t* runtime, scm_binding_t* binding)
{
    u32 last = da_size(&runtime->environments) - 1;
    da_binding_ptr* bindings = &da_at(&runtime->environments, last);
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
            string_view_t* idenfifier = &atom->token->sv;
            scm_binding_t* binding = scm_runtime_binding_lookup(runtime, idenfifier->data, idenfifier->len);

            if (binding == NULL) {
                asprintf(&err, "undefined symbol %.*s", (int)idenfifier->len, idenfifier->data);
                return SCM_RESULT_ERR_FROM_TOKEN(
                    SCM_ERR_SEMANTIC, err,
                    scm_ast_sexpr_token(atom_sexpr), free);
            }

            return SCM_RESULT_OK(SCM_OK_TYPE, binding->type);
        }
        case SCM_TOKEN_LITERAL_NUMBER: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            type->type = SCM_TYPE_NUM;
            type->data.num.num = sv_toi(&atom->token->sv);
            return SCM_RESULT_OK(SCM_OK_TYPE, type);
        }
        case SCM_TOKEN_LITERAL_STRING: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            type->type = SCM_TYPE_STR;
            type->data.str.str = (char*)atom->token->sv.data;
            type->data.str.len = atom->token->sv.len;
            type->data.str.ref = true;
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
            scm_types_list_fill(type, sexpr);
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
            SCM_ERR_SEMANTIC, "can't evaluate a non identifer function",
            scm_ast_sexpr_token(list), NULL);
    }

    string_view_t* idenfifier = &first->data.atom.token->sv;

    scm_binding_t* binding =
        scm_runtime_binding_lookup(runtime, idenfifier->data, idenfifier->len);

    if (binding == NULL) {
        asprintf(&err, "undefined symbol %.*s", (int)idenfifier->len, idenfifier->data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    if (binding->type->type != SCM_TYPE_FUNCTION) {
        asprintf(&err, "can't evaluate a non function type '%.*s'", (int)idenfifier->len, idenfifier->data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    SCM_DEBUG("found %.*s", (int)idenfifier->len, idenfifier->data);

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

    if (function->type == SCM_TYPE_FUNCTION_BUILTIN)
    {
        return function->data.builtin.func(runtime, sexprs);
    }
    else if (function->type == SCM_TYPE_FUNCTION_SEXPR)
    {
    }
    else
    {
        return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC,
                "unknown function type",
                scm_ast_sexpr_token(first), NULL);
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
