#ifndef __TESTS_H__
#define __TESTS_H__

#include "munit/munit.h"

int run_test_suites(void);

extern const MunitSuite scm_lexer_test_suite;
extern const MunitSuite scm_runtime_test_suite;

#endif // __TESTS_H__
