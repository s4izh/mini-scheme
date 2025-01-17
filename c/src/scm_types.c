#include "scm_types.h"

scm_type_t* scm_types_list_fill(scm_type_t* type, scm_ast_sexpr_t* list)
{
    *type = (scm_type_t){
        .type = SCM_TYPE_LIST,
        .data.list.sexpr = list,
    };

    return type;
}
