#include "scm_lexer.h"
#include "scm_parser.h"
#include "scm_result.h"
#include "scm_runtime.h"
#include "scm_log.h"
#include "ds.h"
#include "utils.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#ifndef TEST_MODE

static void usage(char* name)
{
    printf("Usage: %s [file.scm]\n", name);
}

static scm_result_t scm_evaluate_source(
    scm_runtime_t* runtime, scm_lexer_t* lexer, scm_parser_t* parser,
    const char* filename, const char* src)
{
    scm_lexer_set_source(lexer, filename, (const char*)src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    printf("raw source code:\n%s\n", src);

    SCM_DEBUG("lexing start");

    scm_token_t* token;
    while ((token = scm_lexer_next_token(lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
        // printf("generated: ");
        // scm_print_token(&token, true);
        // printf("\n");
    }
    da_append(&tokens, token);

    SCM_DEBUG("lexing end");

    for (u32 i = 0; i < da_size(&tokens); ++i) {
        scm_token_print(da_at(&tokens, i), true);
    }
    printf("\n");

    SCM_DEBUG("parsing start");

    scm_ast_sexpr_t* root = scm_parser_run(parser, &tokens);

    SCM_DEBUG("parsing end");

    printf("full tree:\n");
    scm_ast_sexpr_print(root);

    da_free(&tokens);

    printf("\n");

    return scm_runtime_eval(runtime, root);
    // if (SCM_RESULT_IS_ERR(res)) {
    //     scm_err_print(&res.data.err);
    // } else {
    //     scm_ok_print(&res.data.ok);
    // }

    // return SCM_RESULT_OK(SCM_OK_VOID, NULL);
}

#define HISTORY_FILE ".mscm_hist"
#define HISTORY_MAX_SIZE 1000
// #define HISTORY_FILE getenv("HOME") ? strcat(getenv("HOME"),
// "/.cache/mscm/history") : ".mscm_hist"

static void repl()
{
    scm_result_t res;

    read_history(HISTORY_FILE);
    stifle_history(HISTORY_MAX_SIZE);

    scm_resources_t resources = {0};
    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    res = scm_resources_init(&resources);
    if (SCM_RESULT_IS_ERR(res)) {
        scm_result_print(&res);
        return;
    }
    scm_lexer_init(&lexer, &resources);
    scm_parser_init(&parser, &resources);
    scm_runtime_init(&runtime, &resources, SCM_RUNTIME_MODE_REPL);

    while (true) {
        char* line = readline("mscm> ");
        if (line == NULL || strcmp(line, "exit") == 0)
            break;

        if (*line) {
            add_history(line);
            res = scm_evaluate_source(&runtime, &lexer, &parser, "repl", line);
            if (SCM_RESULT_IS_ERR(res)) {
                // in case we want to do anything else
                scm_result_print(&res);
            } else {
                scm_result_print(&res);
            }
        }
        // free(line);
    }

    write_history(HISTORY_FILE);
}

static int evaluate_file(const char* filename)
{
    int ret;
    char* src = NULL;
    scm_result_t res;

    ret = file_to_string(filename, &src);
    if (ret != 0) {
        return ret;
    }

    scm_resources_t resources = {0};
    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    res = scm_resources_init(&resources);
    if (SCM_RESULT_IS_ERR(res)) {
        scm_result_print(&res);
        return EXIT_FAILURE;
    }
    scm_lexer_init(&lexer, &resources);
    scm_parser_init(&parser, &resources);
    scm_runtime_init(&runtime, &resources, SCM_RUNTIME_MODE_FILE);

    res = scm_evaluate_source(&runtime, &lexer, &parser, filename, src);
    if (SCM_RESULT_IS_ERR(res)) {
        scm_result_print(&res);
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    scm_log_init();
    scm_log_set_level(LOG_DEBUG);
    scm_log_timestamp_mode(true);

    if (argc == 1) {
        repl();
        return EXIT_SUCCESS;
    } else if (argc == 2) {
        return evaluate_file(argv[1]);
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
}
#endif
