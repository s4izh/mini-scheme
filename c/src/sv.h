#ifndef __STRING_VIEW_H__
#define __STRING_VIEW_H__

#include <stddef.h>

typedef struct {
    const char* data;
    size_t len;
} string_view_t;

#define sv_empty(sv) ((sv)->len == 0)
#define sv_len(sv) ((sv)->len)
#define sv_data(sv) ((sv)->data)
#define sv_at(sv, index) ((sv)->data[index])
#define sv_format(sv) (int)(sv)->len, (sv)->data

void sv_init(string_view_t* sv);
void sv_init_with_str(string_view_t* sv, const char* str);
void sv_init_with_str_and_len(string_view_t* sv, const char* str, size_t len);
void sv_free(string_view_t* sv);
string_view_t sv_substr(const string_view_t* sv, size_t start, size_t end);
int sv_equal(const string_view_t* sv1, const string_view_t* sv2);
void sv_print(const string_view_t* sv);
int sv_toi(const string_view_t* sv);

#endif // __STRING_VIEW_H__

