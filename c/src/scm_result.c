#include "scm_result.h"

#include <stdio.h>

void scm_err_print(scm_err_t* err) {
    printf("ERR: ");
    switch (err->type) {
        case SCM_ERR_NONE:
            printf("No error");
            break;
        case SCM_ERR_GENERIC:
            printf("Generic error");
            break;
        case SCM_ERR_RESOURCE:
            printf("Generic error");
            break;
        case SCM_ERR_SYNTAX:
            printf("Syntax error");
            break;
        case SCM_ERR_SEMANTIC:
            printf("Semantic error");
            break;
        case SCM_ERR_RUNTIME:
            printf("Runtime error");
            break;
        case SCM_ERR_TODO:
            printf("TODO error");
            break;
        default:
            printf("Unknown error type\n");
            break;
    }
    /* printf("\n"); */
    if (err->msg != NULL)
        printf(" -> %s", err->msg);

    printf("\n");
}

void scm_ok_print(scm_ok_t* ok)
{
    switch(ok->type) {
        case SCM_OK_VOID:
            return;
        case SCM_OK_TYPE:
            scm_types_print(ok->data);
            return;
    }
}

void scm_result_print(scm_result_t* res)
{
    if (res->type == SCM_RESULT_OK)
        scm_ok_print(&res->data.ok);
    else
        scm_err_print(&res->data.err);
}
