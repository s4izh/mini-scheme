#ifndef __SCM_RESOURCES_H__
#define __SCM_RESOURCES_H__

#include "blockpool.h"
#include "ds.h"

#include "scm_result.h"

DA_DEFINE(blockpool_t, da_blockpool);

// just one pool of each will be allocated at start
typedef struct {
    da_blockpool token_bp;
    da_blockpool sexpr_bp;
} scm_resources_t;

void* scm_resources_alloc_token(scm_resources_t* resources);
void* scm_resources_alloc_sexpr(scm_resources_t* resources);
void* scm_resources_alloc_binding(scm_resources_t* resources);
void* scm_resources_alloc_type(scm_resources_t* resources);

scm_result_t scm_resources_init(scm_resources_t* resources);
scm_result_t scm_resources_free(scm_resources_t* resources);

#endif // __SCM_RESOURCES_H__
