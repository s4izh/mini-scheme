#ifndef __SCM_ERROR_H__
#define __SCM_ERROR_H__

typedef struct {
    enum {
        SCM_ERROR_GENERIC,
        SCM_ERROR_RESOURCE,
        SCM_ERROR_SYNTAX,
        SCM_ERROR_GRAMMAR,
        SCM_ERROR_SEMANTIC,
        SCM_ERROR_RUNTIME,
        SCM_ERROR_TODO,
        SCM_ERROR_NONE,
        SCM_ERROR_NUM_ERRORS,
    } type;
    char* msg;
} scm_error_t;

void scm_error_print(scm_error_t* err);

#endif // !__SCM_ERROR_H__
