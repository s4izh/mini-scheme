#include "scm_types.h"
#include "scm_log.h"
#include "scm_result.h"
#include "sv.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

scm_result_t scm_types_fill_from_list(scm_type_t* type, scm_ast_sexpr_t* list_sexpr)
{
    assert(type != NULL);
    assert(list_sexpr != NULL);

    *type = (scm_type_t){
        .type = SCM_TYPE_LIST,
        .data.list.sexpr = list_sexpr,
    };

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

scm_result_t scm_types_fill_from_quote(scm_type_t* type, scm_ast_sexpr_t* quote_sexpr, scm_type_t* quoted_type)
{
    assert(type != NULL);
    assert(quote_sexpr != NULL);

    *type = (scm_type_t){
        .type = SCM_TYPE_QUOTED,
        .data.quoted.sexpr = quote_sexpr,
        .data.quoted.type = quoted_type,
    };

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
}

scm_result_t scm_types_fill_from_atom(scm_type_t* type, scm_ast_sexpr_t* atom_sexpr)
{
    assert(type != NULL);
    assert(atom_sexpr != NULL);

    scm_ast_atom_t* atom = &atom_sexpr->data.atom;

    assert(atom != NULL);

    switch (atom->token->type) {
        case SCM_TOKEN_LITERAL_NUMBER: {
            type->type = SCM_TYPE_NUMBER;
            type->data.number.value = sv_toi(&atom->token->sv);
            break;
        }
        case SCM_TOKEN_LITERAL_STRING: {
            type->type = SCM_TYPE_STRING;
            type->data.string.value = (char*)atom->token->sv.data;
            type->data.string.len = atom->token->sv.len;
            type->data.string.ref = true;
            break;
        }
        default:
            SCM_ERROR("this isn't an atom literal token type");
    }

    return SCM_RESULT_OK(SCM_OK_TYPE, type);
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
            printf("TODO: function print unimplemented\n");
            return;
        };
        case SCM_TYPE_SYMBOL: {
            sv_print(&type->data.symbol.sv);
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

bool scm_sv_to_cbool(string_view_t* sv)
{
    if (sv_equal(sv, &(string_view_t) { "#f", 2 }))
        return false;

    return true;
}
