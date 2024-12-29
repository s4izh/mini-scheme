#ifndef __DS_H__
#define __DS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define DA_DEFINE(type, typename)       \
    typedef struct {                    \
        type* data;                     \
        size_t size;                    \
        size_t capacity;                \
    } typename                          \

#define da_init(array)         \
    do {                       \
        (array)->data = NULL;  \
        (array)->size = 0;     \
        (array)->capacity = 0; \
    } while (0)

#define da_init_with_preallocation(array, n)                  \
    do {                                                      \
        (array)->data = malloc((n) * sizeof(*(array)->data)); \
        (array)->size = 0;                                    \
        (array)->capacity = (n);                              \
    } while (0)

#define da_free(array)             \
    do {                           \
        if ((array)->data != NULL) \
            free((array)->data);   \
        (array)->data = NULL;      \
        (array)->size = 0;         \
        (array)->capacity = 0;     \
    } while (0)

#define da_append(array, value)                                                \
    do {                                                                       \
        if ((array)->size >= (array)->capacity) {                              \
            (array)->capacity = (array)->capacity ? (array)->capacity * 2 : 1; \
            (array)->data = realloc(                                           \
                (array)->data, (array)->capacity * sizeof(*(array)->data));    \
        }                                                                      \
        (array)->data[(array)->size++] = (value);                              \
    } while (0)

#define da_at(array, index) ((array)->data[index])

#define da_foreach(array, idx, value)                                       \
    for (idx = 0; idx < (array)->size && ((value) = (array)->data[idx], 1); \
         idx++)

/*
#define da_insert(array, index, value)                                         \
    do {                                                                       \
        if ((index) > (array)->size) {                                         \
            fprintf(stderr, "Index out of bounds\n");                          \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
        if ((array)->size >= (array)->capacity) {                              \
            (array)->capacity = (array)->capacity ? (array)->capacity * 2 : 1; \
            (array)->data = realloc((array)->data,                             \
                                    (array)->capacity * sizeof(*(array)->data));
\
        }                                                                      \
        for (size_t i = (array)->size; i > (index); --i) {                     \
            (array)->data[i] = (array)->data[i - 1];                           \
        }                                                                      \
        (array)->data[(index)] = (value);                                      \
        (array)->size++;                                                       \
    } while (0)

#define da_remove(array, index)                                                \
    do {                                                                       \
        if ((index) >= (array)->size) {                                        \
            fprintf(stderr, "Index out of bounds\n");                          \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
        for (size_t i = (index); i < (array)->size - 1; ++i) {                 \
            (array)->data[i] = (array)->data[i + 1];                           \
        }                                                                      \
        (array)->size--;                                                       \
    } while (0)
*/

/*
#define da_get(array, index)                                                   \
    (((index) < (array)->size) ? (array)->data[(index)] :                      \
    (fprintf(stderr, "Index out of bounds\n"), exit(EXIT_FAILURE), 0))

#define da_set(array, index, value)                                            \
    do {                                                                       \
        if ((index) >= (array)->size) {                                        \
            fprintf(stderr, "Index out of bounds\n");                          \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
        (array)->data[(index)] = (value);                                      \
    } while (0)
*/

#endif // __DS_H__
