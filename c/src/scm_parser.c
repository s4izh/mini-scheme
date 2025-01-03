#include "scm_parser.h"
#include "scm_lexer.h"
#include "sv.h"

#define TOKEN_CURRENT(parser) (&da_at(parser->tokens, parser->pos))
#define TOKEN_CURRENT_TYPE(parser) ((&da_at(parser->tokens, parser->pos))->type)
#define TOKEN_ADVANCE(parser) (++parser->pos)

static inline void scm_parser_token_step_forward(scm_parser_t* parser)
{
    ++parser->pos;
}

static inline void scm_parser_token_step_back(scm_parser_t* parser)
{
    --parser->pos;
}

static inline void scm_parser_print_current_token(scm_parser_t* parser)
{
    scm_token_print(&da_at(parser->tokens, parser->pos), true);
}

static inline void scm_parser_append_sexpr_to_list(scm_ast_sexpr_t* list, scm_ast_sexpr_t* sexpr)
{
    if (list->type == SCM_AST_LIST) {
        da_append(&list->data.list.sexprs, sexpr); 
    }
    else
        printf("Tried to append to an ATOM\n");

}

static scm_ast_sexpr_t* scm_parser_create_list(scm_parser_t* parser, scm_token_t* lparen)
{
    // da_append(&parser->sexprs, ((scm_ast_sexpr_t) {
    //             .type = SCM_AST_LIST,
    //             .data = {
    //                 .list = {
    //                     .lparen = lparen,
    //                     .sexprs = (da_scm_ast_sexpr_ptr) { NULL, 0, 0 },
    //                 }
    //             }
    // }));
    // return &da_at(&parser->sexprs, da_size(&parser->sexprs) - 1);

    scm_ast_sexpr_t* list = malloc(sizeof(scm_ast_sexpr_t));
    *list = (scm_ast_sexpr_t) {
        .type = SCM_AST_LIST,
        .data = {
            .list = {
                .lparen = lparen,
                .sexprs = (da_scm_ast_sexpr_ptr) { NULL, 0, 0 },
            }
        }
    };

    return list;
}

static scm_ast_sexpr_t* scm_parser_create_atom(scm_parser_t* parser)
{
    // da_append(&parser->sexprs, ((scm_ast_sexpr_t) {
    //             .type = SCM_AST_ATOM,
    //             .data = { .atom = { .token = TOKEN_CURRENT(parser) } }
    // }));
    // return &da_at(&parser->sexprs, da_size(&parser->sexprs) - 1);

    scm_ast_sexpr_t* atom = malloc(sizeof(scm_ast_sexpr_t));
    *atom = (scm_ast_sexpr_t) {
        .type = SCM_AST_ATOM,
        .data = { .atom = { .token = TOKEN_CURRENT(parser) } }
    };

    return atom;
}

// static int scm_append_to_list(scm_ast_list_t* list, )
// {
//     da_append(&list->children, );
// }

static scm_ast_sexpr_t* scm_parse_s_expression(scm_parser_t* parser)
{
    // scm_parser_print_current_token(parser);
    // printf("\n");

    // printf("parsing sexpr...\n");

    if (TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_IDENTIFIER ||
        TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_LITERAL_NUMBER ||
        TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_LITERAL_STRING) {

        // printf("Creating atom for: ");
        // scm_parser_print_current_token(parser);
        // printf("\n");

        scm_ast_sexpr_t* atom = scm_parser_create_atom(parser);

        return atom;
    }
    else if (TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_LPAREN) {
        scm_ast_sexpr_t* list = scm_parser_create_list(parser, TOKEN_CURRENT(parser));

        scm_parser_token_step_forward(parser);

        while (TOKEN_CURRENT_TYPE(parser) != SCM_TOKEN_RPAREN)
        {
            scm_ast_sexpr_t* sexpr = scm_parse_s_expression(parser);
            if (!sexpr) {
                printf("ERROR: parsing s_expr\n");
            }
            scm_parser_append_sexpr_to_list(list, sexpr);

            scm_parser_token_step_forward(parser);
        }

        return list;
    }
    else {
        printf("ERROR: unhandled token for the parser");
    }

    return NULL;
}

scm_ast_sexpr_t* scm_parser_run(scm_parser_t* parser, da_token* tokens)
{
    parser->root = NULL;
    parser->tokens = tokens;
    parser->pos = 0;

    parser->root = scm_parse_s_expression(parser);

    if (TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_EOF) {
        printf("rarete, la última posición tendría que ser un token EOF");
    }

    return parser->root;
}

static void scm_print_indent(int indent_level)
{
    for (int i = 0; i < indent_level; i++) {
        printf("  ");
    }
}

static void scm_pretty_print_sexpr_rec(scm_ast_sexpr_t* sexpr, int indent_level)
{
    if (!sexpr) return;

    switch (sexpr->type) {
        case SCM_AST_ATOM: {
            // scm_token_print(sexpr->data.atom.token, false);
            sv_print(&sexpr->data.atom.token->sv);
            break;
        }
        case SCM_AST_LIST: {
           printf("(");
           for (u32 i = 0; i < sexpr->data.list.sexprs.size; i++) {
               if (i > 0) {
                   printf(" ");
               }
               scm_ast_sexpr_t* child = sexpr->data.list.sexprs.data[i];
               if (child->type == SCM_AST_LIST) {
                   printf("\n");
                   scm_print_indent(indent_level + 1);
                   scm_pretty_print_sexpr_rec(child, indent_level + 1);
               } else {
                   scm_pretty_print_sexpr_rec(child, indent_level);
               }
           }
           printf(")");
           break;
       }
        default:
            scm_print_indent(indent_level);
            printf("Unknown s-expression type\n");
    }
}

void scm_pretty_print_sexpr(scm_ast_sexpr_t *sexpr) 
{
    scm_pretty_print_sexpr_rec(sexpr, 0); 
}

static void scm_pretty_print_sexpr_extra_rec(scm_ast_sexpr_t* sexpr, int indent_level)
{
    if (!sexpr) return;

    switch (sexpr->type) {
        case SCM_AST_ATOM: {
            scm_token_print(sexpr->data.atom.token, false);
            // sv_print(&sexpr->data.atom.token->sv);
            break;
        }
        case SCM_AST_LIST: {
           printf("(");
           for (u32 i = 0; i < sexpr->data.list.sexprs.size; i++) {
               if (i > 0) {
                   printf(" ");
               }
               scm_ast_sexpr_t* child = sexpr->data.list.sexprs.data[i];
               if (child->type == SCM_AST_LIST) {
                   if (i > 0) {
                       printf("\n");
                       scm_print_indent(indent_level + 1);
                   }
                   scm_pretty_print_sexpr_extra_rec(child, indent_level + 1);
               } else {
                   if (i > 0) {
                       printf("\n");
                       scm_print_indent(indent_level + 1);
                   }
                   scm_pretty_print_sexpr_extra_rec(child, indent_level);
               }
           }
           printf(")");
           break;
       }
        default:
            scm_print_indent(indent_level);
            printf("Unknown s-expression type\n");
    }
}

void scm_pretty_print_sexpr_extra(scm_ast_sexpr_t *sexpr) 
{
    scm_pretty_print_sexpr_extra_rec(sexpr, 0); 
}
