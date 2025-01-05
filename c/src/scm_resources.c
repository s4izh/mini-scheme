#include "scm_resources.h"
#include "blockpool.h"
#include "ds.h"
#include "scm_lexer.h"
#include "scm_parser.h"

#include <stdlib.h>

#define TOKENS_IN_POOL 1000
#define SEXPRS_IN_POOL 1000

void* scm_resources_allocate_token(scm_resources_t* resources)
{
    scm_token_t* token = NULL;
    void* tokens_data = NULL;
    bool* tokens_bool = NULL;

    for (u32 i = 0; i < da_size(&resources->tokens); ++i) {
        token = blockpool_alloc(&da_at(&resources->tokens, i));
        if (token != NULL)
            break;
    }

    if (token == NULL) {
        tokens_data = malloc(TOKENS_IN_POOL * sizeof(scm_token_t));
        tokens_bool = malloc(TOKENS_IN_POOL * sizeof(bool));
        if (tokens_data == NULL || tokens_bool == NULL)
            goto error;

        da_init(&resources->tokens);
        da_append(&resources->tokens, ((blockpool_t) { tokens_data, tokens_bool, TOKENS_IN_POOL, sizeof(scm_token_t) }));

        token = blockpool_alloc(&da_at(&resources->tokens, da_size(&resources->tokens - 1)));
    }
    return token;

error:
    if (tokens_data != NULL)
        free(tokens_data);

    if (tokens_bool != NULL)
        free(tokens_bool);

    return NULL;
}

void* scm_resources_allocate_sexpr(scm_resources_t* resources)
{
    scm_ast_sexpr_t* sexpr = NULL;
    void* sexprs_data = NULL;
    bool* sexprs_bool = NULL;

    for (u32 i = 0; i < da_size(&resources->sexprs); ++i) {
        sexpr = blockpool_alloc(&da_at(&resources->sexprs, i));
        if (sexpr != NULL)
            break;
    }
    if (sexpr == NULL) {
        void* sexprs_data = malloc(SEXPRS_IN_POOL * sizeof(scm_ast_sexpr_t));
        bool* sexprs_bool = malloc(SEXPRS_IN_POOL * sizeof(bool));
        if (sexprs_data == NULL || sexprs_bool == NULL)
            goto error;

        da_init(&resources->sexprs);
        da_append(&resources->sexprs, ((blockpool_t) { sexprs_data, sexprs_bool, SEXPRS_IN_POOL, sizeof(scm_ast_sexpr_t) }));

        sexpr = blockpool_alloc(&da_at(&resources->tokens, da_size(&resources->sexprs - 1)));
    }
    return sexpr;

error:
    if (sexprs_data != NULL)
        free(sexprs_data);

    if (sexprs_bool != NULL)
        free(sexprs_bool);

    return NULL;
}

void scm_resources_init(scm_resources_t* resources)
{
    void* tokens_data = malloc(TOKENS_IN_POOL * sizeof(scm_token_t));
    bool* tokens_bool = malloc(TOKENS_IN_POOL * sizeof(bool));

    da_init(&resources->tokens);
    da_append(&resources->tokens, ((blockpool_t) { tokens_data, tokens_bool, TOKENS_IN_POOL, sizeof(scm_token_t) }));

    void* sexprs_data = malloc(SEXPRS_IN_POOL * sizeof(scm_ast_sexpr_t));
    bool* sexprs_bool = malloc(SEXPRS_IN_POOL * sizeof(bool));

    da_init(&resources->sexprs);
    da_append(&resources->sexprs, ((blockpool_t) { sexprs_data, sexprs_bool, SEXPRS_IN_POOL, sizeof(scm_ast_sexpr_t) }));

    if (tokens_data == NULL || tokens_bool == NULL || sexprs_data == NULL || sexprs_bool == NULL)
        goto error;
    else
        return;

error:
    if (tokens_data != NULL)
        free(tokens_data);

    if (tokens_bool != NULL)
        free(tokens_bool);

    if (sexprs_data != NULL)
        free(sexprs_data);

    if (sexprs_bool != NULL)
        free(sexprs_bool);

    return;
}
