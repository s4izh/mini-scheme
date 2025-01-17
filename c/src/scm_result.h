#ifndef __SCM_RESULT_H__
#define __SCM_RESULT_H__

#include "types.h"

typedef struct _scm_ok_t scm_ok_t;
typedef struct _scm_err_t scm_err_t;
typedef struct _scm_result_t scm_result_t;

struct _scm_ok_t {
    enum {
        SCM_OK_VOID,
        SCM_OK_TYPE,
    } type;

    // data is a void pointer so we don't have circular includes,
    // other modules import this one in order to declare the return types,
    // concrete types are defined in the implementation
    void* data;
};

struct _scm_err_t {
    enum {
        SCM_ERR_GENERIC,
        SCM_ERR_RESOURCE,
        SCM_ERR_SYNTAX,
        SCM_ERR_GRAMMAR,
        SCM_ERR_SEMANTIC,
        SCM_ERR_RUNTIME,
        SCM_ERR_TODO,
        SCM_ERR_NONE,
        SCM_ERR_NUM_ERRORS,
    } type;

    // error message
    char* msg;

    // source code file reference of the error
    char* src;
    u32 line;
    u32 col;

    // reference to the token it caused the error
    void* token;

    // callback to free the message if needed
    void (*free_msg_cb)(void*);
};

struct _scm_result_t {
    enum {
        SCM_RESULT_OK,
        SCM_RESULT_ERR,
    } type;
    union {
        scm_ok_t ok;
        scm_err_t err;
    } data;
};

#define SCM_RESULT_OK(ok_type, _data) \
    ((scm_result_t){                  \
        .type = SCM_RESULT_OK,        \
        .data.ok = (scm_ok_t){.type = ok_type, .data = _data}})

#define SCM_RESULT_ERR(err_type, _msg, _src, _line, _col, _free) \
    ((scm_result_t){                                             \
        .type = SCM_RESULT_ERR,                                  \
        .data.err = (scm_err_t){                                 \
            .type = err_type,                                    \
            .msg = _msg,                                         \
            .src = _src,                                         \
            .line = _line,                                       \
            .col = _col,                                         \
            .free_msg_cb = _free}});

#define SCM_RESULT_ERR_FROM_TOKEN(err_type, _msg, _token, _free) \
    ((scm_result_t){                                             \
        .type = SCM_RESULT_ERR,                                  \
        .data.err = (scm_err_t){                                 \
            .type = err_type,                                    \
            .msg = _msg,                                         \
            .token = _token,                                     \
            .line = 0,                                           \
            .col = 0,                                            \
            .free_msg_cb = _free}});

#define SCM_RESULT_IS_OK(result) ((result).type == SCM_RESULT_OK)
#define SCM_RESULT_IS_ERR(result) ((result).type == SCM_RESULT_ERR)

#define SCM_RESULT_OK_DATA(result) ((result).data.ok)
#define SCM_RESULT_ERR_DATA(result) ((result).data.err)

void scm_err_print(scm_err_t* err);
void scm_ok_print(scm_ok_t* ok);

void scm_result_print(scm_result_t* res);

#endif // !__SCM_RESULT_H__
