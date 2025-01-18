#include <stdio.h>
#include "scm_log.h"
#include "scm_lexer.h"
#include "munit/munit.h"
#include "utils.h"

static void append_token(da_token_ptr* tokens, scm_resources_t* resources,
                        scm_token_type_t type, const char* text, u32 line)
{
    scm_token_t* token = scm_resources_alloc_token(resources);
    string_view_t sv = {text, (text != NULL ? strlen(text) : 0)};
    *token = (scm_token_t){type, sv, line};
    da_append(tokens, token);
}

#define APPEND_TOKEN(type, text, line) append_token(&expected_tokens, resources, type, text, line)

static void compare_tokens(scm_token_t* t1, scm_token_t* t2)
{
    munit_assert_int(t1->type, ==, t2->type);
    munit_assert_true(sv_equal(&t1->sv, &t2->sv));
    munit_assert_int(t1->line, ==, t2->line);
}

static void compare_da_tokens(da_token_ptr* tokens, da_token_ptr* expected_tokens)
{
    munit_assert_size(da_size(tokens), ==, da_size(expected_tokens));

    for (size_t i = 0; i < da_size(tokens); ++i) {
        // scm_token_print(da_at(tokens, i), false);
        // printf(" ");
        // scm_token_print(da_at(expected_tokens, i), false);
        // printf("\n");
        compare_tokens(da_at(tokens, i), da_at(expected_tokens, i));
    }
}

static void* test_setup(const MunitParameter params[], void* user_data) {
    scm_resources_t* resources = malloc(sizeof(scm_resources_t));
    scm_resources_init(resources);
    return resources;
}

static void test_tear_down(void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_resources_free(resources);
    free(resources);
}

static MunitResult test_lexer_suma(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "(+ 1 2)";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "+", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "1", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "2", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, NULL, 0);
    
    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));

    for (size_t i = 0; i < da_size(&tokens); ++i) {
        scm_token_t* t1 = da_at(&tokens, i);
        scm_token_t* t2 = da_at(&expected_tokens, i);
        munit_assert_int(t1->type, ==, t2->type);
        munit_assert_true(sv_equal(&t1->sv, &t2->sv));
        munit_assert_int(t1->line, ==, t2->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_string_literal(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "\"string\"";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_STRING, "\"string\"", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, NULL, 0);
    
    compare_da_tokens(&tokens, &expected_tokens);

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_empty_input(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, NULL, 0);

    compare_da_tokens(&tokens, &expected_tokens);

    da_free(&tokens);
    da_free(&expected_tokens);

    return MUNIT_OK;
}

/*

static MunitResult test_lexer_multiline_define(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "(define (multiline\n x\n y)\n  (+ x y))";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 0, 1, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 1, 6, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 8, 1, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 9, 9, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 20, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 22, 1, 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 23, 1, 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 27, 1, 3);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 29, 1, 3);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 31, 1, 3);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 32, 1, 3);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 33, 1, 3);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 33, 0, 3);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));

    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_nested_parens(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "((((((()))))))";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    for (int i = 0; i < 7; ++i) {
        append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, i, 1, 0);
    }
    for (int i = 0; i < 7; ++i) {
      append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 7 + i, 1, 0);
    }

    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 14, 0, 0);
    
    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));

    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }


    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_dot_identifier(const MunitParameter params[], void* fixture) {
     scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "a.b a.b.c .d";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 0, 3, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 4, 5, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 10, 2, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 12, 0, 0);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }


    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_multiple_quotes(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
     const char* src = "'''''``````,`,,,";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    for (int i = 0; i < 5; ++i)
    {
         append_token(&expected_tokens, resources, SCM_TOKEN_QUOTE, src, i, 1, 0);
    }
      for (int i = 5; i < 11; ++i)
    {
         append_token(&expected_tokens, resources, SCM_TOKEN_QUASIQUOTE, src, i, 1, 0);
    }
      for (int i = 11; i < 15; ++i)
    {
         append_token(&expected_tokens, resources, SCM_TOKEN_COMMA, src, i, 1, 0);
    }

    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 15, 0, 0);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_comments(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "; This is a single line comment\n(define x 10)\n; another comment\n;\n(define y 20)";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 31, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 32, 6, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 39, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, src, 41, 2, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 43, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 62, 1, 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 63, 6, 4);
        append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 70, 1, 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, src, 72, 2, 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 74, 1, 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 75, 0, 4);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
        for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_comments_and_code(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "(+ 1 ; this is a comment\n   2)\n(define x 3) ; this is another one";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

     da_token_ptr expected_tokens;
    da_init(&expected_tokens);

      append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 0, 1, 0);
        append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 1, 1, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, src, 3, 1, 0);
     append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, src, 34, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 35, 1, 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, src, 36, 1, 2);
       append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 37, 6, 2);
      append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, 44, 1, 2);
       append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, src, 46, 1, 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, src, 47, 1, 2);
     append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 67, 0, 2);
    

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data, da_at(&expected_tokens, i)->sv.data);
         munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}

static MunitResult test_lexer_special_chars(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "!@#$%^&*-=_+[]{}|;:'\",.<>?/\\~";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);
    
    for (size_t i = 0; i < strlen(src); i++) {
         append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, src, i, 1, 0);
    }
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, strlen(src), 0, 0);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(
            da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data,
            da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}
*/

static MunitResult test_lexer_factorial(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    char* src;
    file_to_string("tests/factorial.scm", &src);
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);

    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "define", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "factorial", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "n", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "if", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "=", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "n", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "0", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 1);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "*", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "n", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "factorial", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "-", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "n", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "1", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 2);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "factorial", 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "5", 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 4);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, NULL, 6);

    compare_da_tokens(&tokens, &expected_tokens);

    da_free(&tokens);
    da_free(&expected_tokens);
    free(src);
    return MUNIT_OK;
}

static MunitResult test_lexer_random1(const MunitParameter params[], void* fixture) {
     scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "(  ) (  1) '  abc";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LPAREN, "(", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_NUMBER, "1", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_RPAREN, ")", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_QUOTE, "'", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_IDENTIFIER, "abc", 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, NULL, 0);

    compare_da_tokens(&tokens, &expected_tokens);

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}
/*

static MunitResult test_lexer_string_edge(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;
    scm_lexer_t lexer;

    scm_lexer_init(&lexer, resources);
    const char* src = "\"\\\\\" \"\\\"\" \"\\\\\"";
    scm_lexer_set_source(&lexer, "test", src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    scm_token_t* token;
    while ((token = scm_lexer_next_token(&lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
    }
    da_append(&tokens, token);

    da_token_ptr expected_tokens;
    da_init(&expected_tokens);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_STRING, src, 0, 4, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_STRING, src, 5, 4, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_EOF, src, 14, 0, 0);
    append_token(&expected_tokens, resources, SCM_TOKEN_LITERAL_STRING, src, 10, 4, 0);

    munit_assert_size(da_size(&tokens), ==, da_size(&expected_tokens));
    for (size_t i = 0; i < da_size(&tokens); ++i) {
        munit_assert_int(da_at(&tokens, i)->type, ==, da_at(&expected_tokens, i)->type);
        munit_assert_size(da_at(&tokens, i)->sv.len, ==, da_at(&expected_tokens, i)->sv.len);
        munit_assert_memory_equal(
            da_at(&tokens, i)->sv.len, da_at(&tokens, i)->sv.data,
            da_at(&expected_tokens, i)->sv.data);
        munit_assert_int(da_at(&tokens, i)->line, ==, da_at(&expected_tokens, i)->line);
    }

    da_free(&tokens);
    da_free(&expected_tokens);
    return MUNIT_OK;
}
*/

#define basic_test(name, func) \
    {(char*)name, func, test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL}

static MunitTest scm_lexer_suite_tests[] = {
    basic_test("/empty", test_lexer_empty_input),
    basic_test("/random1", test_lexer_random1),
    basic_test("/suma", test_lexer_suma),
    basic_test("/string-literal", test_lexer_string_literal),
    basic_test("/factorial", test_lexer_factorial),
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite scm_lexer_test_suite = {
  (char*) "/lexer",
  scm_lexer_suite_tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};
