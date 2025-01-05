#include "scm_error.h"

#include <stdio.h>

void scm_error_print(scm_error_t* err) {
    printf("ERROR: ");
    switch (err->type) {
        case SCM_ERROR_NONE:
            printf("No error");
            break;
        case SCM_ERROR_GENERIC:
            printf("Generic error");
            break;
        case SCM_ERROR_RESOURCE:
            printf("Generic error");
            break;
        case SCM_ERROR_SYNTAX:
            printf("Syntax error");
            break;
        case SCM_ERROR_SEMANTIC:
            printf("Semantic error");
            break;
        case SCM_ERROR_RUNTIME:
            printf("Runtime error");
            break;
        case SCM_ERROR_TODO:
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

