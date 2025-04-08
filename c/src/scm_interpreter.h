#ifndef __SCM_INTERPRETER_H__
#define __SCM_INTERPRETER_H__

#include "scm_resources.h"
#include "scm_lexer.h"
#include "scm_parser.h"
#include "scm_runtime.h"
#include "scm_result.h"

typedef struct _scm_interpreter_t scm_interpreter_t;

struct _scm_interpreter_t {
    scm_resources_t resources;
    scm_lexer_t lexer;
    scm_parser_t parser;
    scm_runtime_t runtime;
};

/**
 * @brief Initializes the interpreter.
 *
 * This function initializes the interpreter by setting up its internal
 * components, including resources, lexer, parser, and runtime.
 *
 * @param[in,out] interpreter pointer to the interpreter to initialize.
 * @return `scm_result_t` indicating success or failure of initialization.
 */
scm_result_t scm_interpreter_init(scm_interpreter_t* interpreter);

/**
 * @brief Evaluates mscm code using the interpreter.
 *
 * This function takes Scheme source code as input and evaluates it within
 * the context of the interpreter.
 *
 * @param[in,out] interpreter pointer.
 * @param[in] src pointer to the source code string to evaluate.
 * @param[in] len length of the source code string.
 * @param[in] free_src_cb callback to free the source code string, set to NULL if doesn't need to be freed
 * @return `scm_result_t` indicating the result of the evaluation.
 */
scm_result_t scm_interpreter_eval(scm_interpreter_t* interpreter, char* src, u32 len, void* free_src_cb);

#endif // __SCM_INTERPRETER_H__
