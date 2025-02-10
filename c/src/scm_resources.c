#include "rc.h"
#include "scm_lexer.h"
#include "scm_parser.h"
#include "scm_resources.h"
#include "scm_result.h"
#include "scm_runtime.h"
#include "scm_types.h"
#include "scm_log.h"

#include "blockpool.h"
#include "ds.h"

#include <stdlib.h>

#define TOKENS_IN_POOL 1000
#define SEXPRS_IN_POOL 1000

void* scm_resources_alloc_token(scm_resources_t* resources)
{
    scm_token_t* token = NULL;
    void* tokens_data = NULL;
    bool* tokens_bool = NULL;

    for (u32 i = 0; i < da_size(&resources->token_bp); ++i) {
        token = blockpool_alloc(&da_at(&resources->token_bp, i));
        if (token != NULL)
            break;
    }

    if (token == NULL) {
        tokens_data = malloc(TOKENS_IN_POOL * sizeof(scm_token_t));
        tokens_bool = malloc(TOKENS_IN_POOL * sizeof(bool));
        if (tokens_data == NULL || tokens_bool == NULL)
            goto error;

        da_init(&resources->token_bp);
        da_append(&resources->token_bp, ((blockpool_t) { tokens_data, tokens_bool, TOKENS_IN_POOL, sizeof(scm_token_t) }));

        token = blockpool_alloc(&da_at(&resources->token_bp, da_size(&resources->token_bp - 1)));
    }

    return token;

error:
    if (tokens_data != NULL)
        free(tokens_data);

    if (tokens_bool != NULL)
        free(tokens_bool);

    return NULL;
}

void* scm_resources_alloc_sexpr(scm_resources_t* resources)
{
    scm_ast_sexpr_t* sexpr = NULL;
    void* sexprs_data = NULL;
    bool* sexprs_bool = NULL;

    for (u32 i = 0; i < da_size(&resources->sexpr_bp); ++i) {
        sexpr = blockpool_alloc(&da_at(&resources->sexpr_bp, i));
        if (sexpr != NULL)
            break;
    }
    if (sexpr == NULL) {
        void* sexprs_data = malloc(SEXPRS_IN_POOL * sizeof(scm_ast_sexpr_t));
        bool* sexprs_bool = malloc(SEXPRS_IN_POOL * sizeof(bool));
        if (sexprs_data == NULL || sexprs_bool == NULL)
            goto error;

        da_init(&resources->sexpr_bp);
        da_append(&resources->sexpr_bp, ((blockpool_t) { sexprs_data, sexprs_bool, SEXPRS_IN_POOL, sizeof(scm_ast_sexpr_t) }));

        sexpr = blockpool_alloc(&da_at(&resources->token_bp, da_size(&resources->sexpr_bp - 1)));
    }
    return sexpr;

error:
    if (sexprs_data != NULL)
        free(sexprs_data);

    if (sexprs_bool != NULL)
        free(sexprs_bool);

    return NULL;
}

void* scm_resources_alloc_binding(scm_resources_t* resources)
{
    scm_binding_t* binding = malloc(sizeof(scm_binding_t));
    if (binding == NULL)
        return NULL;

    RC_INIT(binding, free);
    return binding;
}

void* scm_resources_alloc_type(scm_resources_t* resources)
{
    scm_type_t* type = malloc(sizeof(scm_type_t));
    if (type == NULL)
        return NULL;

    RC_INIT(type, free);
    return type;
}

void scm_resources_free_binding(scm_resources_t* resources, void* binding)
{
    if (binding != NULL)
        free(binding);
}

void scm_resources_free_type(scm_resources_t* resources, void* type)
{
    if (type != NULL)
        free(type);
}

void scm_resources_free_token(scm_resources_t* resources, void* token)
{
}

void scm_resources_free_sexpr(scm_resources_t* resources, void* sexpr)
{
}

scm_result_t scm_resources_init(scm_resources_t* resources)
{
    void* tokens_data = malloc(TOKENS_IN_POOL * sizeof(scm_token_t));
    bool* tokens_bool = malloc(TOKENS_IN_POOL * sizeof(bool));

    da_init(&resources->token_bp);
    da_append(&resources->token_bp, (blockpool_t) {0});

    blockpool_t* token_bp = &da_at(&resources->token_bp, 0);
    blockpool_init(token_bp, tokens_data, tokens_bool, TOKENS_IN_POOL, sizeof(scm_token_t));

    void* sexprs_data = malloc(SEXPRS_IN_POOL * sizeof(scm_ast_sexpr_t));
    bool* sexprs_bool = malloc(SEXPRS_IN_POOL * sizeof(bool));

    da_init(&resources->sexpr_bp);
    da_append(&resources->sexpr_bp, ((blockpool_t) { sexprs_data, sexprs_bool, SEXPRS_IN_POOL, sizeof(scm_ast_sexpr_t) }));

    blockpool_t* sexpr_bp = &da_at(&resources->sexpr_bp, 0);
    blockpool_init(sexpr_bp, sexprs_data, sexprs_bool, SEXPRS_IN_POOL, sizeof(scm_ast_sexpr_t));

    if (tokens_data == NULL || tokens_bool == NULL || sexprs_data == NULL || sexprs_bool == NULL)
        goto error;

    return SCM_RESULT_OK(SCM_OK_VOID, NULL);

error:
    if (tokens_data != NULL)
        free(tokens_data);

    if (tokens_bool != NULL)
        free(tokens_bool);

    if (sexprs_data != NULL)
        free(sexprs_data);

    if (sexprs_bool != NULL)
        free(sexprs_bool);

    da_free(&resources->token_bp);
    da_free(&resources->sexpr_bp);

    char* err = "could not allocate memory for resources";
    return SCM_RESULT_ERR(SCM_ERR_RESOURCE, err, NULL, 0, 0, NULL);
}

scm_result_t scm_resources_free(scm_resources_t* resources)
{
    da_free(&resources->token_bp);
    da_free(&resources->sexpr_bp);
}
