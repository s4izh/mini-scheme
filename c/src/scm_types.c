#include "scm_types.h"
#include "scm_log.h"

#include <assert.h>
#include <math.h>

void scm_types_fill_from_list(scm_type_t* type, scm_ast_sexpr_t* list_sexpr)
{
    assert(type != NULL);
    assert(list_sexpr != NULL);

    *type = (scm_type_t){
        .type = SCM_TYPE_LIST,
        .data.list.sexpr = list_sexpr,
    };
}

void scm_types_fill_from_atom_literal(scm_type_t* type, scm_ast_sexpr_t* atom_sexpr)
{
    assert(type != NULL);
    assert(atom_sexpr != NULL);

    scm_ast_atom_t* atom = &atom_sexpr->data.atom;

    assert(atom != NULL);

    switch (atom->token->type) {
        case SCM_TOKEN_LITERAL_NUMBER: {
            type->type = SCM_TYPE_NUMBER;
            type->data.number.value = sv_toi(&atom->token->sv);
            return;
        }
        case SCM_TOKEN_LITERAL_STRING: {
            type->type = SCM_TYPE_STRING;
            type->data.string.value = (char*)atom->token->sv.data;
            type->data.string.len = atom->token->sv.len;
            type->data.string.ref = true;
            return;
        }
        default:
            SCM_ERROR("this isn't an atom literal token type");
            return;
    }
}

static void print_num(f64 num)
{
    if (fabs(num - (int)num) < 1e-6) {
        printf("%.0f", num);
    } else {
        printf("%f", num);
    }
}

static void print_bool(bool value)
{
    printf("%s", value ? "#t" : "#f");
}

void scm_types_print(scm_type_t* type)
{
    switch (type->type) {
        case SCM_TYPE_BOOLEAN: {
            print_bool(type->data.boolean.value);
            return;
        };
        case SCM_TYPE_NUMBER: {
            print_num(type->data.number.value);
            return;
        };
        case SCM_TYPE_STRING: {
            string_view_t sv = {type->data.string.value, type->data.string.len};
            sv_print(&sv);
            return;
        };
        case SCM_TYPE_LIST: {
            scm_ast_sexpr_print(type->data.list.sexpr);
            return;
        };
        case SCM_TYPE_FUNCTION: {
            return;
        };
    }
}

bool scm_type_to_cbool(scm_type_t* type)
{
    if (type->type == SCM_TYPE_BOOLEAN)
        return type->data.boolean.value;
    else if (type->type == SCM_TYPE_LIST)
        return (da_size(&type->data.list.sexpr->data.list.sexprs) >= 1);

    return true;
}
