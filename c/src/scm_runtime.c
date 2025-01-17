#include "scm_runtime.h"
#include "scm_parser.h"
#include "ds.h"
#include "scm_resources.h"
#include "scm_result.h"
#include "scm_types.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static scm_binding_t* scm_runtime_create_binding(scm_runtime_t* runtime)
{
    return malloc(sizeof(scm_binding_t));
}

static void scm_runtime_destroy_binding(scm_binding_t* binding)
{
    free(binding);
}

static scm_result_t scm_builtin_define(
    void* _runtime, void** args, u32 num_args)
{
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;

    scm_ast_sexpr_t* first =
        (scm_ast_sexpr_t*)((uintptr_t)args) + (sizeof(scm_ast_sexpr_t) * 0);

    if (first->type == SCM_AST_ATOM) {
        // defining a constant
        scm_binding_t* binding = scm_runtime_create_binding(runtime);

        // just one sexpr can be evaluated as the constant
        if (num_args > 2) {
            char* err = "constant takes the result of evaluating just one expr";
            return SCM_RESULT_ERR_FROM_TOKEN(
                SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), NULL);
        }
        scm_ast_sexpr_t* second =
            (scm_ast_sexpr_t*)((uintptr_t)args) + (sizeof(scm_ast_sexpr_t) * 1);
        // binding->internal.value.constant.sexpr = second;
        return SCM_RESULT_OK(SCM_OK_VOID, NULL);

    } else if (first->type == SCM_AST_LIST) {
        // defining a function
        if (da_size(&first->data.list.sexprs) == 1) {
            // takes no args
        } else {
            // takes args
        }
        return SCM_RESULT_OK(SCM_OK_VOID, NULL);

    } else {
        char* err;
        asprintf(
            &err,
            "not expecting a %s after define, expecting an identifier or a list",
            scm_ast_sexpr_type_to_str(first));
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err, scm_ast_sexpr_token(first), free);
    }
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
    void* _runtime, void** args, u32 num_args)
{
    scm_result_t res;
    scm_runtime_t* runtime = (scm_runtime_t*)_runtime;

    scm_ast_sexpr_t* first =
        (scm_ast_sexpr_t*)((uintptr_t)args) + (sizeof(scm_ast_sexpr_t) * 0);

    i32 sum = 0;

    for (u32 i = 0; i < num_args; ++i) {
        scm_ast_sexpr_t* sexpr_i =
            (scm_ast_sexpr_t*)((uintptr_t)args) + (sizeof(scm_ast_sexpr_t) * i);
        // scm_runtime_result_t res_i = scm_runtime_eval(runtime, sexpr_i);
        // if (res_i.has_error)
        //     return res_i;

        // check if type is comsum patible
        // if (res_i.data.internal.value.constant.type != SCM_TYPE_NUM) {
        //     result_set_error(
        //         res, "evaluating +, arg %i has incompatible type %s", i,
        //         "type_to_str");
        //     return res;
        // }
        // sum += res.data.internal.value.constant.data.num;
    }

    if (first->type == SCM_AST_ATOM) {
    } else if (first->type == SCM_AST_LIST) {
    }

    return res;
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

    binding->type.type = SCM_TYPE_FUNCTION;
    binding->token = fake_token;

    binding->type.data.function = (scm_type_function_t){
        .type = SCM_TYPE_FUNCTION_BUILTIN,
        .min_args = min_args,
        .max_args = max_args,
        .func.builtin.func = func,
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
    // scm_runtime_add_builtin_binding(runtime, "+", &scm_builtin_op_sum, 0, -1);
    // clang-format on
}

scm_binding_t* scm_runtime_binding_lookup(
    scm_runtime_t* runtime, const char* name, u32 size)
{
    // we start looking from the last environment created
    // (the most nested one)
    for (i32 i = da_size(&runtime->environments) - 1; i >= 0; i--) {
        scm_environment_t* env = &da_at(&runtime->environments, i);
        for (u32 j = 0; j < da_size(&env->bindings); j++) {
            scm_binding_t* binding = da_at(&env->bindings, j);
            if (binding->token->sv.len != size)
                continue;
            if (strncmp(binding->token->sv.data, name, size) == 0)
                return binding;
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
    scm_runtime_t* runtime, scm_ast_sexpr_t* atom)
{
    assert(atom != NULL);
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
            printf("TODO!!!!!!!!!!");
        };
        case SCM_AST_LIST: {
            scm_type_t* type = scm_resources_alloc_type(runtime->resources);
            return SCM_RESULT_OK(SCM_OK_TYPE, scm_types_list_fill(type, sexpr));
        };
        default:
            __builtin_unreachable();
    }
}

static scm_result_t scm_runtime_eval_ast_list(
    scm_runtime_t* runtime, scm_ast_sexpr_t* list)
{
    char* err;

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

    string_view_t* id = &first->data.atom.token->sv;

    scm_binding_t* binding =
        scm_runtime_binding_lookup(runtime, id->data, id->len);

    if (binding == NULL) {
        asprintf(&err, "undefined symbol %.*s", (int)id->len, id->data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    if (binding->type.type == SCM_TYPE_FUNCTION) {
        asprintf(&err, "can't evaluate a non function type '%.*s'", (int)id->len, id->data);
        return SCM_RESULT_ERR_FROM_TOKEN(
            SCM_ERR_SEMANTIC, err,
            scm_ast_sexpr_token(list), free);
    }

    printf("found %.*s\n", (int)id->len, id->data);

    return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

static scm_result_t scm_runtime_eval_ast_root(
    scm_runtime_t* runtime, scm_ast_sexpr_t* root)
{
    assert(root != NULL);

    // scm_runtime_result_t res = result();
    scm_result_t res;

    for (u32 i = 0; i <= da_size(&root->data.root.sexprs); ++i) {
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

    printf("evaluating:\n");
    scm_ast_sexpr_print(sexpr);

    switch (sexpr->type) {
        case SCM_AST_ATOM:
            return scm_runtime_eval_ast_atom(runtime, sexpr);
        case SCM_AST_QUOTE:
            return scm_runtime_eval_ast_quote(runtime, sexpr);
        case SCM_AST_LIST:
            return scm_runtime_eval_ast_list(runtime, sexpr);
    }
}

void scm_type_print(scm_type_t* type)
{
    printf("TODO\n");
}
