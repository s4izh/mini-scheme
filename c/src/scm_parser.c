#include "scm_parser.h"
#include "scm_lexer.h"
#include "scm_resources.h"
#include "ds.h"

#define TOKEN_CURRENT(parser) (da_at(parser->tokens, parser->pos))
#define TOKEN_CURRENT_TYPE(parser) ((da_at(parser->tokens, parser->pos))->type)
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
    scm_token_print(da_at(parser->tokens, parser->pos), false);
}

static inline void scm_parser_append_sexpr_to_list(
    scm_ast_sexpr_t* list, scm_ast_sexpr_t* sexpr)
{
    if (list->type == SCM_AST_LIST) {
        da_append(&list->data.list.sexprs, sexpr);
    } else
        printf("Tried to append to an ATOM\n");
}

static scm_ast_sexpr_t* scm_parser_create_list(
    scm_parser_t* parser, scm_token_t* lparen)
{
    scm_ast_sexpr_t* list = scm_resources_alloc_sexpr(parser->resources);
    *list = (scm_ast_sexpr_t){
        .type = SCM_AST_LIST,
        .data = {
            .list = {
                .lparen = lparen,
                .sexprs = (da_scm_ast_sexpr_ptr){NULL, 0, 0},
            }}};
    return list;
}

static scm_ast_sexpr_t* scm_parser_create_atom(scm_parser_t* parser)
{
    scm_ast_sexpr_t* atom = scm_resources_alloc_sexpr(parser->resources);
    *atom = (scm_ast_sexpr_t){
        .type = SCM_AST_ATOM,
        .data = {.atom = {.token = TOKEN_CURRENT(parser)}}};
    return atom;
}

static scm_ast_sexpr_t* scm_parser_create_quote(scm_parser_t* parser)
{
    scm_ast_sexpr_t* quote = scm_resources_alloc_sexpr(parser->resources);
    *quote = (scm_ast_sexpr_t){
        .type = SCM_AST_QUOTE,
        .data = {.quote = {.quote = TOKEN_CURRENT(parser)}}};
    return quote;
}

static scm_ast_sexpr_t* scm_parser_create_root(scm_parser_t* parser)
{
    scm_ast_sexpr_t* root = scm_resources_alloc_sexpr(parser->resources);
    *root = (scm_ast_sexpr_t){
        .type = SCM_AST_ROOT,
        .data = {
            .root = {
                .sexprs = (da_scm_ast_sexpr_ptr){NULL, 0, 0},
            }}};
    return root;
}

// static scm_ast_sexpr_t* scm_parser_create_quasiquote(scm_parser_t* parser)
// {
//     scm_ast_sexpr_t* quote = malloc(sizeof(scm_ast_sexpr_t));
//     *quote = (scm_ast_sexpr_t) {
//         .type = SCM_AST_QUASIQUOTE,
//         .data = { .quote = { .quote = TOKEN_CURRENT(parser) } }
//     };
//     return quote;
// }

static scm_ast_sexpr_t* scm_parse_s_expression(scm_parser_t* parser)
{
    // scm_parser_print_current_token(parser);
    // printf("\n");

    // printf("parsing sexpr...\n");

    switch (TOKEN_CURRENT_TYPE(parser)) {
        case SCM_TOKEN_IDENTIFIER:
        case SCM_TOKEN_LITERAL_NUMBER:
        case SCM_TOKEN_LITERAL_STRING: {
            scm_ast_sexpr_t* atom = scm_parser_create_atom(parser);
            return atom;
        };
        case SCM_TOKEN_QUOTE: {
            scm_ast_sexpr_t* quote = scm_parser_create_quote(parser);
            scm_parser_token_step_forward(parser);
            quote->data.quote.sexpr = scm_parse_s_expression(parser);
            return quote;
        };
        // case SCM_TOKEN_QUASIQUOTE: {
        //     scm_ast_sexpr_t* quasiquote = scm_parser_create_quote(parser);
        //     scm_parser_token_step_forward(parser);
        //     quasiquote->data.quote.sexpr = scm_parse_s_expression(parser);
        //     return quasiquote;
        // };
        case SCM_TOKEN_LPAREN: {
            scm_ast_sexpr_t* list =
                scm_parser_create_list(parser, TOKEN_CURRENT(parser));

            scm_parser_token_step_forward(parser);

            while (TOKEN_CURRENT_TYPE(parser) != SCM_TOKEN_RPAREN) {
                scm_ast_sexpr_t* sexpr = scm_parse_s_expression(parser);
                if (!sexpr) {
                    printf("ERR: parsing s_expr\n");
                }
                scm_parser_append_sexpr_to_list(list, sexpr);

                scm_parser_token_step_forward(parser);
            }
            return list;
        };
        default: {
            printf("ERR: unhandled token for the parser\n");
            sleep(1);
        };
    }

    return NULL;
}

void scm_parser_init(scm_parser_t* parser, scm_resources_t* resources)
{
    parser->resources = resources;
}

scm_ast_sexpr_t* scm_parser_run(scm_parser_t* parser, da_token_ptr* tokens)
{
    parser->tokens = tokens;
    parser->pos = 0;

    scm_ast_sexpr_t* root = scm_parser_create_root(parser);

    while (TOKEN_CURRENT_TYPE(parser) != SCM_TOKEN_EOF) {
        scm_ast_sexpr_t* sexpr = scm_parse_s_expression(parser);
        if (sexpr == NULL)
            printf("null sexpr\n");

        da_append(&(root->data.root.sexprs), sexpr);

        // if (TOKEN_CURRENT_TYPE(parser) == SCM_TOKEN_RPAREN)
        scm_parser_token_step_forward(parser);
    }

    if (TOKEN_CURRENT_TYPE(parser) != SCM_TOKEN_EOF) {
        printf("rarete, la última posición tendría que ser un token EOF\n");
        scm_token_print(TOKEN_CURRENT(parser), false);
    }

    return root;
}

const char* scm_ast_sexpr_type_to_str(scm_ast_sexpr_t* sexpr)
{
    switch (sexpr->type) {
        case SCM_AST_ATOM:
            return "atom";
        case SCM_AST_QUOTE:
            return "quote";
        // case SCM_AST_QUASIQUOTE: return "quasiquote";
        case SCM_AST_LIST:
            return "list";
        case SCM_AST_ROOT:
            return "root";
    }
    return "unknown";
}

static void scm_print_indent(int indent_level)
{
    for (int i = 0; i < indent_level; i++) {
        printf("  ");
    }
}

static void scm_ast_sexpr_print_rec(scm_ast_sexpr_t* sexpr, int indent_level)
{
    // printf("indent level %d: ", indent_level);
    if (!sexpr)
        return;

    switch (sexpr->type) {
        case SCM_AST_ATOM: {
            printf("(%s ", scm_ast_sexpr_type_to_str(sexpr));
            scm_token_print(sexpr->data.atom.token, false);
            printf(")");
            break;
        }
        case SCM_AST_QUOTE: {
            printf("(%s \n", scm_ast_sexpr_type_to_str(sexpr));
            scm_print_indent(indent_level + 1);
            scm_ast_sexpr_print_rec(sexpr->data.quote.sexpr, indent_level + 1);
            break;
        };
        case SCM_AST_ROOT:
        case SCM_AST_LIST: {
            printf("(%s \n", scm_ast_sexpr_type_to_str(sexpr));
            scm_print_indent(indent_level + 1);
            for (u32 i = 0; i < sexpr->data.list.sexprs.size; i++) {
                if (i > 0) {
                    printf(" ");
                }
                scm_ast_sexpr_t* child;
                if (sexpr->type == SCM_AST_ROOT)
                    child = da_at(&sexpr->data.root.sexprs, i);
                else
                    child = da_at(&sexpr->data.list.sexprs, i);

                if (child->type == SCM_AST_LIST) {
                    if (i > 0) {
                        printf("\n");
                        scm_print_indent(indent_level + 1);
                    }
                    scm_ast_sexpr_print_rec(child, indent_level + 1);
                } else {
                    if (i > 0) {
                        printf("\n");
                        scm_print_indent(indent_level + 1);
                    }
                    scm_ast_sexpr_print_rec(child, indent_level + 1);
                }
            }
            printf(")");
            break;
        };
        default:
            printf("Unknown s-expression type\n");
    }
}

void scm_ast_sexpr_print(scm_ast_sexpr_t* sexpr)
{
    scm_ast_sexpr_print_rec(sexpr, 0);
    printf("\n");
}

scm_token_t* scm_ast_sexpr_token(scm_ast_sexpr_t* sexpr)
{
    switch (sexpr->type) {
        case SCM_AST_ATOM:
            return sexpr->data.atom.token;
        case SCM_AST_QUOTE:
            return sexpr->data.quote.quote;
        case SCM_AST_LIST:
            return sexpr->data.list.lparen;
        case SCM_AST_ROOT:
            return NULL;
    }
}
