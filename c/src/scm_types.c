#include "scm_types.h"
#include "scm_log.h"

#include <assert.h>
#include <math.h>

void scm_types_list_fill(scm_type_t* type, scm_ast_sexpr_t* list_sexpr)
{
    assert(type != NULL);
    assert(list_sexpr != NULL);

    *type = (scm_type_t){
        .type = SCM_TYPE_LIST,
        .data.list.sexpr = list_sexpr,
    };
}

void scm_types_from_atom(scm_type_t* type, scm_ast_sexpr_t* atom_sexpr)
{
    assert(type != NULL);
    assert(atom_sexpr != NULL);

    scm_ast_atom_t* atom = &atom_sexpr->data.atom;

    assert(atom != NULL);

    switch (atom->token->type) {
        case SCM_TOKEN_IDENTIFIER:
            return;
        case SCM_TOKEN_LITERAL_NUMBER: {
            type->type = SCM_TYPE_NUM;
            type->data.num.num = sv_toi(&atom->token->sv);
            return;
        }
        case SCM_TOKEN_LITERAL_STRING: {
            type->type = SCM_TYPE_STR;
            type->data.str.str = atom->token->sv.data;
            type->data.str.len = atom->token->sv.len;
            type->data.str.ref = true;
            return;
        }
        default:
            SCM_ERROR("this isn't an atom token type");
            return;
    }
}

static void print_num(f64 num) {
    if (fabs(num - (int)num) < 1e-6) {
        printf("%.0f", num);
    } else {
        printf("%f", num);
    }
}

void scm_types_print(scm_type_t* type)
{
    switch (type->type) {
        case SCM_TYPE_NUM: {
            print_num(type->data.num.num);
            return;
        };
        case SCM_TYPE_STR: {
            string_view_t sv = {type->data.str.str, type->data.str.len};
            sv_print(&sv);
            return;
        };
        case SCM_TYPE_LIST: {
            return;
        };
        case SCM_TYPE_FUNCTION: {
            return;
        };
    }
}
