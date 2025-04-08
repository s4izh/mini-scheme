#ifndef __RC_H__
#define __RC_H__

#include "types.h"

struct rc_t {
    u32 references;
    void (*free)(void*);
};

#define RC_DECLARE struct rc_t __ref_count

#define RC_INIT(obj, free_fn)              \
    do {                                   \
        (obj)->__ref_count.references = 1; \
    } while (0)

#define RC_REF(obj) ((obj)->__ref_count.references++)

#define RC_UNREF(obj)                                     \
    if (obj != NULL) {                                    \
        do {                                              \
            if (--((obj)->__ref_count.references) == 0) { \
                if ((obj)->__ref_count.free) {            \
                    (obj)->__ref_count.free(obj);         \
                }                                         \
            }                                             \
        } while (0);                                      \
    }

#endif // !__RC_H__
