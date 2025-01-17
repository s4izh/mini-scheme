#include <stdlib.h>
#include <stdio.h>

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

