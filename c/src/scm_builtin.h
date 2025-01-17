#ifndef __SCM_BUILTIN_H__
#define __SCM_BUILTIN_H__

#include "types.h"

typedef void (*scm_builtin_func_t) (void*, void** args, u32 num);

typedef struct {
    u32 min_args;
    u32 max_args;
    scm_builtin_func_t func;
} scm_builtin_t;

#endif // __SCM_BUILTIN_H__
