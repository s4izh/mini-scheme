#include "sv.h"

inline void sv_init(string_view_t* sv) {
    sv->data = NULL;
    sv->size = 0;
}

inline void sv_init_with_str(string_view_t* sv, const char* str) {
    sv->data = str;
    sv->size = strlen(str);
}

inline void sv_init_with_str_and_size(string_view_t* sv, const char* str, size_t size) {
    sv->data = str;
    sv->size = size;
}

inline void sv_free(string_view_t* sv) {
    sv->data = NULL;
    sv->size = 0;
}

inline string_view_t sv_substr(const string_view_t* sv, size_t start, size_t end) {
    if (start > end || end > sv->size) {
        return (string_view_t){NULL, 0};
    }
    return (string_view_t){sv->data + start, end - start};
}

inline int sv_compare(const string_view_t* sv1, const string_view_t* sv2) {
    if (sv1->size != sv2->size) {
        return 0;
    }
    return strncmp(sv1->data, sv2->data, sv1->size) == 0;
}

inline void sv_print(const string_view_t* sv) {
    if (sv->data == NULL || sv->size == 0) {
        printf("(empty)");
        return;
    }
    printf("%.*s", (int)sv->size, sv->data);
}
