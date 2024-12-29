#include "scm_lexer.h"
#include "ds.h"

#include <stdlib.h>
#include <stdio.h>

int file_to_string(char* filename, char** buffer)
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

void usage(char *name) {
    printf("Usage: %s <file.scm>\n", name);
}

DA_DEFINE(scm_token_t, da_token);  

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int res; 
    char* filename = argv[1];
    char* src = NULL;

    res = file_to_string(filename, &src);
    if (res != 0) {
        return res;
    }

    scm_lexer_t lexer = {0};

    scm_lexer_init(&lexer, (const char*)src, strlen(src));

    da_token tokens;
    da_init(&tokens);

    printf("raw source code:\n%s", src);

    scm_token_t token;
    while ((token = scm_lexer_next_token(&lexer)).type != SCM_TOKEN_UNKNOWN) {
        da_append(&tokens, token);
        scm_print_token(&token);
        printf(" ");
    }
    printf("\n");

    da_free(&tokens);

    return 0;
}
