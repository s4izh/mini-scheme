#include <stdio.h>
#include "scm_log.h"
#include "scm_result.h"
#include "scm_runtime.h"
#include "scm_lexer.h"
#include "munit/munit.h"
#include "utils.h"

struct math_test_case {
    const char* src;
    f64 expected_result;
};

struct cmp_test_case {
    const char* src;
    bool expected_result;
};

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


// static void* test_setup(const MunitParameter params[], void* user_data) {
//     lexer_test_helper* helper = malloc(sizeof(lexer_test_helper));
//     scm_resources_init(&helper->resources);
//     scm_lexer_init(&helper->lexer, &helper->resources);
//     return helper;
// }

// static void test_tear_down(void* fixture) {
//     lexer_test_helper* helper = (lexer_test_helper*)fixture;
//     scm_resources_free(&helper->resources);
//     free(helper);
// }

static scm_result_t scm_evaluate_source(
    scm_runtime_t* runtime, scm_lexer_t* lexer, scm_parser_t* parser,
    const char* filename, const char* src)
{
    scm_lexer_set_source(lexer, filename, (const char*)src, strlen(src));

    da_token_ptr tokens;
    da_init(&tokens);

    SCM_DEBUG("raw source code:\n%s\n", src);

    SCM_DEBUG("lexing start");

    scm_token_t* token;
    while ((token = scm_lexer_next_token(lexer))->type != SCM_TOKEN_EOF) {
        da_append(&tokens, token);
        // printf("generated: ");
        // scm_print_token(&token, true);
        // printf("\n");
    }
    da_append(&tokens, token);

    scm_ast_sexpr_t* root = scm_parser_run(parser, &tokens);

    da_free(&tokens);

    return scm_runtime_eval(runtime, root);
}

static MunitResult test_runtime_suma(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(+ 1 2)";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 3);

    return MUNIT_OK;
}

static MunitResult test_runtime_resta(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(- 1 2)";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, -1);

    return MUNIT_OK;
}

static MunitResult test_runtime_mul(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(* 1 20 25)";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 500);

    return MUNIT_OK;
}

static MunitResult test_runtime_suma_resta_mul(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(+ (- 5 200) (* 15 80))";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 1005);

    return MUNIT_OK;
}

static MunitResult test_runtime_suma_resta_mul_negative(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(+ (- -5 200) (* -15 80))";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 1005);

    return MUNIT_OK;
}

static MunitResult test_runtime_cmp(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = "(+ (- -5 200) (* -15 80))";

    scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    scm_type_t* type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 1005);

    return MUNIT_OK;
}

static MunitResult test_runtime_define_constant(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    const char* src = NULL;
    scm_type_t* type = NULL;
    scm_result_t res;

    src = "(define x 1) x";
    res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 1);

    src = "(define x (* 10 5 6)) (* 10 5 x)";
    res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
    munit_assert_false(SCM_RESULT_IS_ERR(res));
    munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

    type = res.data.ok.data;
    munit_assert_true(type->type == SCM_TYPE_NUMBER);
    munit_assert_double(type->data.number.value, ==, 15000);

    return MUNIT_OK;
}

static MunitResult test_runtime_define_function(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    char* factorial_src;

    file_to_string("tests/factorial.scm", &factorial_src);

    struct math_test_case test_cases[] = {
        {"(define (suma x y) (+ x y)) (suma 100 (* 2 2))", 104},
        {factorial_src, 120},
        {"(define (suma x y) (+ x y)) (suma 100 (* 2 2))", 104},
        {"(define (suma x y) (+ x y)) (suma 100 (* 2 2))", 104},
        {"(define (suma x y) (+ x y)) (suma 100 (* 2 2))", 104},
        {NULL, 0}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        double expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_NUMBER);
        munit_assert_double(type->data.number.value, ==, expected_result);
    }

    free(factorial_src);

    return MUNIT_OK;
}

static MunitResult 
test_runtime_operators_cmp(const MunitParameter params[], void* fixture) {
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    struct cmp_test_case test_cases[] = {
        {"(< 3 5)", true},
        {"(< 5 3)", false},
        {"(<= 3 3)", true},
        {"(<= 5 3)", false},
        {"(> 10 2)", true},
        {"(> 2 10)", false},
        {"(>= 4 4)", true},
        {"(>= 3 4)", false},
        {"(= 7 7)", true},
        {"(= 7 8)", false},
        {NULL, false}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        bool expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_BOOLEAN);
        munit_assert_int(type->data.boolean.value, ==, expected_result);
    }

    return MUNIT_OK;
}

static MunitResult test_runtime_operators_math(const MunitParameter params[], void* fixture)
{
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    struct math_test_case test_cases[] = {
        {"(+ 3 5)", 8},
        {"(- 10 4)", 6},
        {"(- 5 10)", -5},
        {"(* 4 3)", 12},
        // {"(+ -3 7)", 4},
        // {"(* -3 6)", -18},
        // {"(/ 10 2)", 5},
        // {"(/ 5 2)", 2.5},
        {NULL, 0}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        double expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_NUMBER);
        munit_assert_double(type->data.number.value, ==, expected_result);
    }

    return MUNIT_OK;
}

static MunitResult test_runtime_operators_math_complex(const MunitParameter params[], void* fixture)
{
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    struct math_test_case test_cases[] = {
        {"(+ 3 5 (* 1 2 3 4) (- 1 10))", 23},
        {"(* 3 5 (- 1 20 3 4) (* 1 10))", -3900},
        {NULL, 0}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        f64 expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_NUMBER);
        munit_assert_double(type->data.number.value, ==, expected_result);
    }

    return MUNIT_OK;
}

static MunitResult test_runtime_if_basic(const MunitParameter params[], void* fixture)
{
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    struct math_test_case test_cases[] = {
        {"(if 1 23)", 23},
        {"(if '() 23 1)", 1},
        {"(if (= 4 4) 18)", 18},
        {"(if (= 4 18) 18 (+ 2 4 5))", 11},
        {NULL, 0}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        f64 expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_NUMBER);
        munit_assert_double(type->data.number.value, ==, expected_result);
    }

    return MUNIT_OK;
}

static MunitResult test_runtime_let_basic(const MunitParameter params[], void* fixture)
{
    scm_resources_t* resources = (scm_resources_t*)fixture;

    scm_lexer_t lexer = {0};
    scm_parser_t parser = {0};
    scm_runtime_t runtime = {0};

    scm_lexer_init(&lexer, resources);
    scm_parser_init(&parser, resources);
    scm_runtime_init(&runtime, resources, SCM_RUNTIME_MODE_REPL);

    struct math_test_case test_cases[] = {
        {"(let () 1)", 1},
        {"(let ((x 15000)) 1 2 3 4 x)", 15000},
        {"(let ((x 15) (y 30)) (+ x y))", 45},
        {NULL, 0}
    };

    for (int i = 0; test_cases[i].src != NULL; ++i) {
        const char* src = test_cases[i].src;
        f64 expected_result = test_cases[i].expected_result;

        scm_result_t res = scm_evaluate_source(&runtime, &lexer, &parser, "test.c", src);
        munit_assert_false(SCM_RESULT_IS_ERR(res));
        munit_assert_true(res.data.ok.type == SCM_OK_TYPE);

        scm_type_t* type = res.data.ok.data;
        munit_assert_true(type->type == SCM_TYPE_NUMBER);
        munit_assert_double(type->data.number.value, ==, expected_result);
    }

    return MUNIT_OK;
}

#define basic_test(name, func) \
    {(char*)name, func, test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL}

static MunitTest scm_runtime_operators_suite_tests[] = {
    basic_test("/cmp",  test_runtime_operators_cmp),
    basic_test("/math", test_runtime_operators_math),
    basic_test("/math-complex", test_runtime_operators_math_complex),
    // basic_test("/let", test_runtime_let),
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static MunitTest scm_runtime_define_suite_tests[] = {
    basic_test("/constant", test_runtime_define_constant),
    basic_test("/function", test_runtime_define_function),
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite scm_runtime_define_test_suite = {
    (char*) "/define",
    scm_runtime_define_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

static MunitTest scm_runtime_let_suite_tests[] = {
    basic_test("/basic", test_runtime_let_basic),
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite scm_runtime_let_test_suite = {
    (char*) "/let",
    scm_runtime_let_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

static MunitTest scm_runtime_if_suite_tests[] = {
    basic_test("/basic", test_runtime_if_basic),
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

const MunitSuite scm_runtime_if_test_suite = {
    (char*) "/if",
    scm_runtime_if_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

const MunitSuite scm_runtime_operators_test_suite = {
    (char*) "/ops",
    scm_runtime_operators_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

const MunitSuite mscm_runtime_suites[] = {
    scm_runtime_operators_test_suite,
    scm_runtime_define_test_suite,
    scm_runtime_if_test_suite,
    scm_runtime_let_test_suite,
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
};

const MunitSuite scm_runtime_test_suite = {
    (char*) "/runtime",
    NULL,
    (MunitSuite*)mscm_runtime_suites,
    1,
    MUNIT_SUITE_OPTION_NONE
};
