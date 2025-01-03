#ifndef __SCM_BUILTIN_H__
#define __SCM_BUILTIN_H__

#include "types.h"

typedef enum {
    SCM_BUILTIN_DEFINE,
    SCM_BUILTIN_IF,
    SCM_BUILTIN_COND,
} scm_builtin_type_t;

typedef struct {
    scm_builtin_type_t type;
    void (*func)(void** args, u32 num);
} scm_builtin_t;

#endif // __SCM_BUILTIN_H__
