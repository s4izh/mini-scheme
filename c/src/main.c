#include "scm_lexer.h"
#include "scm_parser.h"
#include "scm_runtime.h"
#include "ds.h"

#include <stdlib.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

int file_to_string(const char* filename, char** buffer)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("error_t: could not open file %s\n", filename);
        return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* tmp;
    tmp = malloc(length + 1);
    if (tmp == NULL) {
        printf("error_t: could not allocate memory\n");
        return EXIT_FAILURE;
    }
    int read = fread(tmp, 1, length, file);
    if (read != length) {
        printf("error_t: could not read file %s\n", filename);
        return EXIT_FAILURE;
    }
    tmp[length] = '\0';
    *buffer = tmp;
    fclose(file);
    return 0;
}

static void usage(char* name)
{
    printf("Usage: %s [file.scm]\n", name);
}

static bool evaluate_source(const char* filename, const char* src)
{
    scm_lexer_t lexer = {0};

    scm_lexer_init(&lexer, filename, (const char*)src, strlen(src));

    da_token tokens;
    da_init(&tokens);

    printf("raw source code:\n%s\n", src);

    scm_token_t token;
    while ((token = scm_lexer_next_token(&lexer)).type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
        // printf("generated: ");
        // scm_print_token(&token, true);
        // printf("\n");
    }
    da_append(&tokens, token);

    for (u32 i = 0; i < da_size(&tokens); ++i) {
        scm_token_print(&da_at(&tokens, i), true);
    }

    scm_parser_t parser = {0};

    printf("\n");
    printf("parsing...\n");

    scm_ast_sexpr_t* root = scm_parser_run(&parser, &tokens);

    printf("\n");
    printf("parse tree:\n");

    scm_pretty_print_sexpr(root);

    printf("\n");
    printf("\n");
    printf("parse tree extra:\n");
    scm_pretty_print_sexpr_extra(root);

    da_free(&tokens);

    printf("\n");

    return true;
}

#define HISTORY_FILE ".mscm_hist"
#define HISTORY_MAX_SIZE 1000
// #define HISTORY_FILE getenv("HOME") ? strcat(getenv("HOME"),
// "/.cache/mscm/history") : ".mscm_hist"

static void repl()
{
    read_history(HISTORY_FILE);
    stifle_history(HISTORY_MAX_SIZE);

    scm_runtime_t runtime = {0};
    scm_runtime_init(&runtime);

    while (true) {
        char* line = readline("mscm> ");
        if (line == NULL || strcmp(line, "exit") == 0)
            break;

        if (*line) {
            add_history(line);
            evaluate_source("repl", line);
        }
        free(line);
    }

    write_history(HISTORY_FILE);
}

static bool evaluate_file(const char* filename)
{
    int res;
    char* src = NULL;

    res = file_to_string(filename, &src);
    if (res != 0) {
        return res;
    }

    evaluate_source(filename, src);

    return true;
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        if (!evaluate_file(argv[1]))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    } else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    return 0;
}
