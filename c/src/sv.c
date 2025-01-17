#include "sv.h"

#include <string.h>
#include <stdio.h>

inline void sv_init(string_view_t* sv) {
    sv->data = NULL;
    sv->len = 0;
}

inline void sv_init_with_str(string_view_t* sv, const char* str) {
    sv->data = str;
    sv->len = strlen(str);
}

inline void sv_init_with_str_and_len(string_view_t* sv, const char* str, size_t len) {
    sv->data = str;
    sv->len = len;
}

inline void sv_free(string_view_t* sv) {
    sv->data = NULL;
    sv->len = 0;
}

inline string_view_t sv_substr(const string_view_t* sv, size_t start, size_t end) {
    if (start > end || end > sv->len) {
        return (string_view_t){NULL, 0};
    }
    return (string_view_t){sv->data + start, end - start};
}

inline int sv_equal(const string_view_t* sv1, const string_view_t* sv2) {
    if (sv1->len != sv2->len) {
        return 0;
    }
    if (sv1->data == NULL && sv2->data == NULL) return 1;
    return strncmp(sv1->data, sv2->data, sv1->len) == 0;
}

inline void sv_print(const string_view_t* sv) {
    if (sv->data == NULL || sv->len == 0) {
        printf("(empty)");
        return;
    }
    printf("%.*s", (int)sv->len, sv->data);
}
