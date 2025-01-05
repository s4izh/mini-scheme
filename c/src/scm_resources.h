#ifndef __SCM_RESOURCES_H__
#define __SCM_RESOURCES_H__

#include "blockpool.h"
#include "ds.h"

DA_DEFINE(blockpool_t, da_blockpool);

// just one pool of each will be allocated at start
typedef struct {
    da_blockpool tokens;
    da_blockpool sexprs;
} scm_resources_t;

void* scm_resources_allocate_token(scm_resources_t* resources);
void* scm_resources_allocate_sexpr(scm_resources_t* resources);

void scm_resources_init(scm_resources_t* resources);

#endif // __SCM_RESOURCES_H__
